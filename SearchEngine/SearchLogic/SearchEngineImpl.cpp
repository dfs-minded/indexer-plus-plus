// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "SearchEngineImpl.h"

#include <algorithm>
#include <cassert>
#include <iterator>

#include "AsyncLog.h"
#include "FileInfo.h"
#include "HelperCommon.h"
#include "Log.h"
#include "OneThreadLog.h"
#include "SearchQuery.h"

#include "Merger.h"
#include "SearchEngine.h"
#include "SortingProperty.h"

using namespace std;

SearchEngineImpl::SearchEngineImpl(SearchEngine* interface_backlink, SearchResultObserver* result_observer,
                                   bool listen_index_change)
    : interface_class_(interface_backlink),
      query_search_res_outdated_(false),
      index_outdated_(false),
      sort_outdated_(false),
      p_search_result_(make_shared<SearchResult>()),
      u_tmp_search_result_(make_unique<SearchResult>()),
      last_query_(new SearchQuery()),
      file_infos_filter_(make_unique<FileInfosFilter>()),
      last_sort_prop_(SortingProperty::SORT_NAME),
      last_sort_direction_(1),
      sorter_(last_sort_prop_, last_sort_direction_),
      result_observer_(result_observer),
      mtx_(new mutex()),
      conditional_var_(new condition_variable()) {

    GET_LOGGER
    logger_->Debug(METHOD_METADATA + L"SearchEngine_ctor");
    indices_container_ = &IndexManagersContainer::Instance();

    if (listen_index_change) {
        indices_container_->RegisterIndexChangeObserver(interface_class_);
    }

    file_infos_filter_->ResetQuery(move(last_query_));

#ifndef SINGLE_THREAD

    logger_->Debug(METHOD_METADATA + L"SearchEngine. Constructor called, starting new worker thread.");

    worker_thread_ = new thread(&SearchEngineImpl::SearchWorker, this);
    HelperCommon::SetThreadName(worker_thread_, "SearchWorker ");
    worker_thread_->detach();

#endif
}

SearchEngineImpl::~SearchEngineImpl() {
    indices_container_->UnregisterIndexChangeObserver(interface_class_);

#ifndef SINGLE_THREAD
    delete mtx_;
    delete conditional_var_;
    delete worker_thread_;
#endif
}

// Called from other then SearchWorker thread (UI, CMD thread). Synchronous version of SearchAsync function.
pSearchResult SearchEngineImpl::Search(uSearchQuery query) {
    last_query_ = move(query);

    ProcessNewSearchQuery();
    Sort(u_tmp_search_result_->Files.get());

    p_search_result_.reset(u_tmp_search_result_.release());
    return p_search_result_;
}

// Called from other then SearchWorker thread (UI thread).
void SearchEngineImpl::SearchAsync(uSearchQuery query) {
    logger_->Debug(METHOD_METADATA + L"New Search Query arrived:" + move(SerializeQuery(*query)));

    UNIQUE_LOCK

    last_query_                = move(query);
    query_search_res_outdated_ = true;

    UNLOCK_AND_NOTIFY_ONE
}

// Do not need to lock here, this function called from already locked block.
bool SearchEngineImpl::ReadyToProcessSearch() const {
    auto res = query_search_res_outdated_ || sort_outdated_ || index_outdated_;

    logger_->Debug(METHOD_METADATA + (res ? L"True" : L"False"));
    return res;
}

void SearchEngineImpl::SearchWorker() {
#ifdef WIN32
    // Needed for calls to COM.
    HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr))
        logger_->Error(METHOD_METADATA + L"Worker thread after FAIL CoInitializeEx. Res = " +
                       HelperCommon::GetLastErrorString());
#endif

#ifdef SINGLE_THREAD
    if (!ReadyToProcessSearch()) return;
#else

    while (true)
#endif  // SINGLE_THREAD
    {
        logger_->Debug(METHOD_METADATA + L"Entered section before thread will wait.");

        UNIQUE_LOCK

#ifndef SINGLE_THREAD
        conditional_var_->wait(locker_, [this]() { return ReadyToProcessSearch(); });
#endif

        UNLOCK

        logger_->Debug(METHOD_METADATA + L"Worker thread awakened.");
        bool is_new_query = query_search_res_outdated_;

        // Process one change at a time, in the priority: new search query, new sorting property or direction,
        // indices changes.
        if (query_search_res_outdated_) {

			logger_->Debug(L"Started processing 'query search result outdated'.");

            UNIQUE_LOCK
            sorter_.ResetSortingProperties(last_sort_prop_, last_sort_direction_);
            UNLOCK

            ProcessNewSearchQuery();

        } else if (sort_outdated_) {

			logger_->Debug(L"Started processing 'sort outdated.'");

            UNIQUE_LOCK
            sorter_.ResetSortingProperties(last_sort_prop_, last_sort_direction_);
            UNLOCK

            auto mgrs = indices_container_->GetAllIndexManagers();
            for (const auto* mgr : mgrs)
                mgr->GetIndex()->LockData();

            // Do not need to lock |index_outdated_|. It can be changed only by invoking OnIndexChanged, but it could
            // not be invoked because indices are locked now and no index changes can be handled by IndexManager.
            if (index_outdated_) {

				logger_->Debug(L"Started processing 'index outdated.'");

                ProcessIndexChanged();  // |u_tmp_search_result_| will be created by ProcessIndexChanged.

            } else {  // Form new |u_tmp_search_result_|.

                u_tmp_search_result_ = make_unique<SearchResult>();

                // Copy primary search result into tmp.
                u_tmp_search_result_->Files->insert(u_tmp_search_result_->Files->begin(),
                                                    p_search_result_->Files->begin(), p_search_result_->Files->end());
            }

            Sort(u_tmp_search_result_->Files.get());

            for (const auto* mgr : mgrs)
                mgr->GetIndex()->UnlockData();

        } else if (index_outdated_) {
            ProcessIndexChanged();
        }


        // If query search still up-to-date, set tmp search result to current and notify search result observer.
        if (!query_search_res_outdated_) {

            p_search_result_.reset(u_tmp_search_result_.release());

            if (result_observer_) {
                result_observer_->OnNewSearchResult(p_search_result_, is_new_query);
            }

        } else {
            u_tmp_search_result_.release();
        }
    }
#ifdef WIN32
    CoUninitialize();
#endif
}

void SearchEngineImpl::ProcessNewSearchQuery() {

	logger_->Debug(METHOD_METADATA + L"Called.");

    query_search_res_outdated_ = false;
    index_outdated_            = false;

    UNIQUE_LOCK
    if (last_query_) {
        file_infos_filter_->ResetQuery(move(last_query_));
    }
    UNLOCK

    if (file_infos_filter_->SearchInDirectorySpecified())
        SearchInSpecificDir();
    else
        SearchInAllIndicesFromRoot();

    logger_->Info(METHOD_METADATA + L"Search finished. Objects count (tmpSearchRes.size): " +
                  to_wstring(u_tmp_search_result_->Files->size()));
}

void SearchEngineImpl::SearchInAllIndicesFromRoot() {

    TIK auto mgrs = indices_container_->GetAllIndexManagers();
    for (const auto* mgr : mgrs)
        mgr->GetIndex()->LockData();

    auto deleter         = ClearIndexChangedArgs();
    u_tmp_search_result_ = make_unique<SearchResult>(move(deleter));

    for (const auto* mgr : mgrs) {
        if (!mgr->ReadingMFTFinished() || mgr->DisableIndexRequested()) continue;

        const auto* index = mgr->GetIndex();
        SearchInTree(*index->Root(), u_tmp_search_result_->Files.get());
    }

    Sort(u_tmp_search_result_->Files.get());

    for (const auto* mgr : mgrs)
        mgr->GetIndex()->UnlockData();

    TOK(METHOD_METADATA + L"Search and sort finished.");
}

void SearchEngineImpl::SearchInSpecificDir() {

    auto search_start_dir = file_infos_filter_->GetSearchDirectory();
    auto deleter          = ClearIndexChangedArgs();
    u_tmp_search_result_  = make_unique<SearchResult>(move(deleter));
    if (!search_start_dir) return;

    auto* index = indices_container_->GetIndexManager(search_start_dir->DriveLetter)->GetIndex();
    index->LockData();

	TIK
    SearchInTree(*search_start_dir, u_tmp_search_result_->Files.get());

    Sort(u_tmp_search_result_->Files.get());

    index->UnlockData();

    TOK(L"Search and sort with directory filter finished.");
}

// Do not need to lock here, this function must be called when indices are locked.
unique_ptr<OldFileInfosDeleter> SearchEngineImpl::ClearIndexChangedArgs() {

    auto fi_deleter = make_unique<OldFileInfosDeleter>();

    for (size_t i = 0; i < index_changed_args_.size(); ++i) {
        fi_deleter->AddItemsToDelete(move(index_changed_args_[i]->OldItems));
    }

    index_changed_args_.clear();

    return fi_deleter;
}

void SearchEngineImpl::ProcessIndexChanged() {

    logger_->Debug(METHOD_METADATA + L"Called.");

    UNIQUE_LOCK

    auto changed_args = move(index_changed_args_);
    index_outdated_   = false;

    UNLOCK

    // Old and changed items for quick lookup by address, to skip them in merging from the main list.
    unordered_set<const FileInfo*> do_not_include;

    // Old items for adding to OldFileInfosDeleter.
    vector<const FileInfo*> to_destroy;

    // New and changed items in one list.
    vector<const FileInfo*> items_to_include;

    for (const auto& args : changed_args) {

        copy(args->OldItems.begin(), args->OldItems.end(), inserter(do_not_include, do_not_include.end()));
        copy(args->ChangedItems.begin(), args->ChangedItems.end(), inserter(do_not_include, do_not_include.end()));

        auto filtered_new_items = move(file_infos_filter_->FilterFiles(args->NewItems));
        items_to_include.insert(items_to_include.end(), filtered_new_items.begin(), filtered_new_items.end());

        auto filtered_changed_items = move(file_infos_filter_->FilterFiles(args->ChangedItems));
        items_to_include.insert(items_to_include.end(), filtered_changed_items.begin(), filtered_changed_items.end());

        to_destroy.insert(to_destroy.end(), args->OldItems.begin(), args->OldItems.end());
    }

    // Remove from |items_to_include| duplicates and old items from |to_destroy| collection.
    sort(items_to_include.begin(), items_to_include.end());
    items_to_include.erase(unique(items_to_include.begin(), items_to_include.end()), items_to_include.end());

    sort(to_destroy.begin(), to_destroy.end());

    items_to_include.erase(remove_if(items_to_include.begin(), items_to_include.end(),
                                     [&to_destroy](const FileInfo* fi) {
                                         return binary_search(to_destroy.begin(), to_destroy.end(), fi);
                                     }),
                           items_to_include.end());

    // Items passed in Merge function must be sorted.
    auto cmp = sorter_.GetCurrentPropertyComparator();

    // Standard sort instead of our Sort method is used here to avoid indices locking on ProcessIndexChanged or the
    // situation, when new index changes come instantly and we cannot sort until process them and cannot process
    // without sorting.
    sort(items_to_include.begin(), items_to_include.end(), cmp);

    logger_->Debug(L"Processing " + to_wstring(do_not_include.size()) + L" old items, " +
                   to_wstring(items_to_include.size()) + L" new and changed items");

    auto merge_res =
        Merger::MergeWithMainCollection(*p_search_result_->Files, move(do_not_include), move(items_to_include), cmp);

    // Form new |u_tmp_search_result_|.
    auto deleter = make_unique<OldFileInfosDeleter>();
    deleter->AddItemsToDelete(move(to_destroy));

    u_tmp_search_result_        = make_unique<SearchResult>(move(deleter));
    u_tmp_search_result_->Files = move(merge_res);

    logger_->Debug(METHOD_METADATA + L"Finished");
}

// Called from other then SearchWorker thread (UI thread).
void SearchEngineImpl::Sort(const string& property_name, int direction) {

    logger_->Info(METHOD_METADATA + L"Called from UI.");

    auto sort_column = PropertyNameToSortingPropertyEnum(property_name);
    assert(sort_column != SortingProperty::SORT_NOTSUPPORTED_COLUMNS);

    UNIQUE_LOCK
    last_sort_prop_      = sort_column;
    last_sort_direction_ = direction;
    sort_outdated_       = true;
    UNLOCK_AND_NOTIFY_ONE
}

void SearchEngineImpl::Sort(vector<const FileInfo*>* file_infos) {

	logger_->Debug(L"Sorting search result.");

    sort_outdated_ = false;
    sorter_.Sort(file_infos);
}

// Traverses index tree with DFS algo.
void SearchEngineImpl::SearchInTree(const FileInfo& start_dir, vector<const FileInfo*>* result) const {

    static ushort counter = 0;

    // Check periodically whether the search result outdated and if so - stop further tree traversal.
    // Made for more responsive UI.
    if ((counter++ & 1023) == 0 && query_search_res_outdated_) {
        return;
    }

    const FileInfo* child = start_dir.FirstChild;

    while (child)  // Traverse children.
    {
        if (file_infos_filter_->PassesAllQueryFilters(*child)) {
            // Add to search result.
            result->push_back(child);
        }

        // Do not traverse hidden directories if exclude hidden filter set.
        if (file_infos_filter_->TraverseCurrentDir(*child)) {
            // Traverse directory children.
            SearchInTree(*child, result);
        }

        child = child->NextSibling;
    }
}

// Called from other then SearchWorker thread (UI thread).
void SearchEngineImpl::OnIndexChanged(pNotifyIndexChangedEventArgs p_args) {

    logger_->Debug(METHOD_METADATA + L"Called.");

    UNIQUE_LOCK

    index_changed_args_.push_back(move(p_args));
    index_outdated_ = true;

    UNLOCK_AND_NOTIFY_ONE
}
