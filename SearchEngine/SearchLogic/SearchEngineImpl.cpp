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
                                   bool search_mode_only)
    : interface_class_(interface_backlink),
      query_search_res_outdated_(false),
      index_outdated_(false),
      sort_outdated_(false),
      stop_processing_current_query_(false),
      p_search_result_(make_shared<SearchResult>()),
      u_tmp_search_result_(make_unique<SearchResult>()),
      last_query_(new SearchQuery()),
      file_infos_filter_(make_unique<FileInfosFilter>()),
      last_sort_prop_(SortingProperty::SORT_NAME),
      last_sort_direction_(1),
      sorter_(last_sort_prop_, last_sort_direction_),
      result_observer_(result_observer),
      search_mode_only_(search_mode_only) {

    GET_LOGGER
    logger_->Debug(METHOD_METADATA + L"SearchEngine_ctor");
    indices_container_ = &IndexManagersContainer::Instance();

    if (search_mode_only_) {
        indices_container_->RegisterIndexChangeObserver(interface_class_);
    }

    file_infos_filter_->ResetQuery(move(last_query_));

#ifndef SINGLE_THREAD

    if (!search_mode_only_) {
        logger_->Debug(METHOD_METADATA + L"SearchEngine. Constructor called, starting new worker thread.");

        locker_ = new mutex();
        conditional_var_ = new condition_variable();

        worker_thread_ = new thread(&SearchEngineImpl::SearchWorker, this);
        HelperCommon::SetThreadName(worker_thread_, "SearchWorker ");
        worker_thread_->detach();
    }
#endif
}

SearchEngineImpl::~SearchEngineImpl() {
    indices_container_->UnregisterIndexChangeObserver(interface_class_);

#ifndef SINGLE_THREAD
    if (!search_mode_only_) {
        delete locker_;
        delete conditional_var_;
        delete worker_thread_;
    }
#endif
}

pSearchResult SearchEngineImpl::Search(uSearchQuery query) {

    file_infos_filter_->ResetQuery(move(query));

    ProcessNewSearchQuery();
    Sort(u_tmp_search_result_->Files.get());

    p_search_result_.reset(u_tmp_search_result_.release());
    return p_search_result_;
}

// Called from other then SearchWorker thread (UI thread).
void SearchEngineImpl::SearchAsync(uSearchQuery query) {
    logger_->Debug(METHOD_METADATA + L"New search query arrived:" + move(SerializeQuery(*query)));

    UNIQUE_LOCK

    last_query_ = move(query);
    query_search_res_outdated_ = true;
    stop_processing_current_query_ = true;

    UNLOCK_AND_NOTIFY_ONE
}

bool SearchEngineImpl::ReadyToProcessSearch() const {

    // Do not need to lock here, this function called from already locked block.
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

        logger_->Debug(METHOD_METADATA + L"Worker thread awakened.");

        // Make a local snapshot of the changes flags that we are going to process.

        bool query_changed = query_search_res_outdated_;
        bool sort_changed = sort_outdated_;
        bool index_changed = index_outdated_;

        sorter_.ResetSortingProperties(last_sort_prop_, last_sort_direction_);

        if (last_query_) file_infos_filter_->ResetQuery(move(last_query_));

        // Reset all changes flags.

        query_search_res_outdated_ = false;
        sort_outdated_ = false;
        index_outdated_ = false;

        stop_processing_current_query_ = false;

        UNLOCK

        // Process changes in the priority:
        // new search query, new sorting property or direction (and send this result to the user), indices changes.
        if (query_changed) {

            ProcessNewSearchQuery();

            Sort(u_tmp_search_result_->Files.get());

            SendNewSearchResult(query_changed);

            index_changed = sort_changed = false;
        }

        if (sort_changed) {

            u_tmp_search_result_ = make_unique<SearchResult>();

            // Copy primary search result into tmp.
            u_tmp_search_result_->Files->insert(u_tmp_search_result_->Files->begin(), p_search_result_->Files->begin(),
                                                p_search_result_->Files->end());

            Sort(u_tmp_search_result_->Files.get());

            // We want to send processed sorting request immediately.
            SendNewSearchResult(query_changed);
        }

        if (index_changed) {

            ProcessIndexChanged();

            SendNewSearchResult(query_changed);
        }
    }

#ifdef WIN32
    CoUninitialize();
#endif
}

void SearchEngineImpl::SendNewSearchResult(bool query_changed) {
    UNIQUE_LOCK

    // If query search result still up-to-date, set tmp search result to current and notify search result observer.
    if (!query_search_res_outdated_) {

        logger_->Debug(METHOD_METADATA + L"Sending new search result back to User.");

        p_search_result_.reset(u_tmp_search_result_.release());

        if (result_observer_) result_observer_->OnNewSearchResult(p_search_result_, query_changed);
    } else
        u_tmp_search_result_.release();

    UNLOCK;
}

void SearchEngineImpl::ProcessNewSearchQuery() {

    logger_->Debug(METHOD_METADATA + L"Called.");

    if (file_infos_filter_->SearchInDirectorySpecified())
        SearchInSpecificDir();
    else
        SearchInAllIndicesFromRoot();

    logger_->Info(METHOD_METADATA + L"Indices traversal and filtering finished. Tmp Search result objects count: " +
                  to_wstring(u_tmp_search_result_->Files->size()));
}

void SearchEngineImpl::SearchInAllIndicesFromRoot() {
    TIK auto mgrs = indices_container_->GetAllIndexManagers();
    for (const auto* mgr : mgrs)
        mgr->GetIndex()->LockData();

    auto deleter = ClearIndexChangedArgs();
    u_tmp_search_result_ = make_unique<SearchResult>(move(deleter));

    for (const auto* mgr : mgrs) {
        if (!mgr->ReadingMFTFinished() || mgr->DisableIndexRequested()) continue;

        const auto* index = mgr->GetIndex();

        SearchInTree(*index->Root(), u_tmp_search_result_->Files.get());
    }

    TOK(METHOD_METADATA + L"Search in all Indices finished.");

    for (const auto* mgr : mgrs)
        mgr->GetIndex()->UnlockData();
}

void SearchEngineImpl::SearchInSpecificDir() {

    auto search_start_dir = file_infos_filter_->GetSearchDirectory();
    auto deleter = ClearIndexChangedArgs();
    u_tmp_search_result_ = make_unique<SearchResult>(move(deleter));
    if (!search_start_dir) return;

    auto* index = indices_container_->GetIndexManager(search_start_dir->DriveLetter)->GetIndex();
    index->LockData();

    TIK

        SearchInTree(*search_start_dir, u_tmp_search_result_->Files.get());

    TOK(METHOD_METADATA + L"Search finished.");

    index->UnlockData();
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

    // Lock Indexes.
    auto mgrs = indices_container_->GetAllIndexManagers();
    for (const auto* mgr : mgrs)
        mgr->GetIndex()->LockData();

    // Do not need to lock |index_changed_args_| because indices are locked now and neither filesystem changes
    // nor index changes can be done by IndexManager.
    auto changed_args = move(index_changed_args_);

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

    u_tmp_search_result_ = make_unique<SearchResult>(move(deleter));
    u_tmp_search_result_->Files = move(merge_res);

    // Unlock Indexes.
    for (const auto* mgr : mgrs)
        mgr->GetIndex()->UnlockData();

    logger_->Debug(METHOD_METADATA + L"Finished");
}

// Called from other then SearchWorker thread (UI thread).
void SearchEngineImpl::Sort(const string& property_name, int direction) {

    logger_->Info(METHOD_METADATA + L"Called from UI.");

    auto sort_column = PropertyNameToSortingPropertyEnum(property_name);
    assert(sort_column != SortingProperty::SORT_NOTSUPPORTED_COLUMNS);

    UNIQUE_LOCK

    last_sort_prop_ = sort_column;
    last_sort_direction_ = direction;
    sort_outdated_ = true;

    UNLOCK_AND_NOTIFY_ONE
}

void SearchEngineImpl::Sort(vector<const FileInfo*>* file_infos) {

    logger_->Debug(L"Sorting search result...");
    TIK

        sorter_.Sort(file_infos);

    TOK(L"Sorting finished.")
}

// Traverses index tree with DFS algo.
void SearchEngineImpl::SearchInTree(const FileInfo& start_dir, vector<const FileInfo*>* result) const {

    static ushort counter = 0;

    // Check periodically whether the search result outdated and if so - stop further tree traversal.
    // Made for more responsive UI.
    if ((counter++ & 1023) == 0 && stop_processing_current_query_) {
        counter--;
        logger_->Debug(METHOD_METADATA + L"Stopping current query processing.");
        return;
    }
    const FileInfo* child = start_dir.FirstChild;

    while (child)  // Traverse children.
    {
        if ((counter & 1023) == 0 && stop_processing_current_query_) return;

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
