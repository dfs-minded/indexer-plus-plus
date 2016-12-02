// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <unordered_set>
#include <vector>

#include "Macros.h"
#include "typedefs.h"

#include "FileInfosFilter.h"
#include "IndexManagersContainer.h"
#include "NotifyIndexChangedEventArgs.h"
#include "SearchQuery.h"
#include "SearchResult.h"
#include "SearchResultObserver.h"
#include "Sorter.h"

class SearchEngine;
class FileInfo;
class Log;


// Implements search logic, listens index changes, volume status changes and produces search results based on user queries.
// When new volume added - merges from index changed arguments all it's files (marked as new) with current search res.
// When a volume removed - merges from index changed arguments all it's files (marked as old) with current search res.
//
// All its work SE performs in one thread 
class SearchEngineImpl {

   public:
    SearchEngineImpl(SearchEngine* interface_backlink, SearchResultObserver* result_observer, bool listen_index_change);

    NO_COPY(SearchEngineImpl);

    ~SearchEngineImpl();


	// Called from other then SearchWorker thread (UI, CMD thread). Synchronous version of SearchAsync function.

    pSearchResult Search(uSearchQuery query);
	

	// Called from other then SearchWorker thread (UI thread).

    void SearchAsync(uSearchQuery query);


	// Called from other then SearchWorker thread (UI thread).

    void Sort(const std::string& prop_name, int direction);


	// Called from other then SearchWorker thread (UI thread).

    void OnIndexChanged(pNotifyIndexChangedEventArgs p_args);


   private:
    void SearchWorker();


	// Reads all data from Indices from scratch.

    void ProcessNewSearchQuery();


    void SearchInAllIndicesFromRoot();


	// Starts search (and an Index tree traversal) directly from the filter directory, specified in query.

    void SearchInSpecificDir();


    // Clears all index changed arguments. Returns items which needed to be destroyed.
    // Call when you will read everything from scratch directly from indices.

    std::unique_ptr<OldFileInfosDeleter> ClearIndexChangedArgs();


	// Returns "true" if there are changes, which SE must process, "false" otherwise.   

    bool ReadyToProcessSearch() const;


    // Merges changes with current search result into |u_tmp_search_result_|.

    void ProcessIndexChanged();


    // The function that does sorting.

    void Sort(std::vector<const FileInfo*>* file_infos);


    // Traverses files index tree and forms |tmp_search_result_| from filtered files.

    void SearchInTree(const FileInfo& start_dir, std::vector<const FileInfo*>* result) const;


	// Sends newly created search result back to a User.

	void SendNewSearchResult(bool query_changed);


    // Backlink to interface class in pImpl idiom.
    SearchEngine* interface_class_;

	// There are 3 types of events that the SE needs to process: new query from the user, 
	// index changed (due to filesystem had changed) and user asked to sort by another property. 3 corresponding 
	// variables defined below (after comments block end).
	// Values of these variables are shared between 2 different types of threads:
	//	 1. SE processing thread. Namely it always sets them to "false" after performing needed actions.
	//	 2. IndexManager thread, GUI (or other) user threads which always set them to "true".
	// 
	// Communication between these two thread types synchronized using condition variable |conditional_var_|.
	// When IndexManager or one of user threads set them to "true", they also notify the waiting SE thread via 
	// conditional variable. SE thread processes changes and goes back to waiting.
	// 
	// Even if the shared variable is atomic, it must be modified under the mutex in order to correctly publish 
	// the modification to the waiting thread.

    std::atomic<bool> query_search_res_outdated_;

    std::atomic<bool> index_outdated_;

    std::atomic<bool> sort_outdated_;


	// Used by SearchInTree(). If the new query arrived we want to stop processing the old one.
	std::atomic<bool> stop_processing_current_query_;


	// Search result which was last sent to GUI.
    pSearchResult p_search_result_;

	// Search result which we are forming now, than swap it with |p_search_result_| just before sending it to GUI.
    std::unique_ptr<SearchResult> u_tmp_search_result_;


    // Query received from Client (GUI, CMD, etc). Need to synchronize access via |mtx_|.
    std::unique_ptr<SearchQuery> last_query_;

	// Filter based on the value of |last_query_|.
    std::unique_ptr<FileInfosFilter> file_infos_filter_;

    // Set by GUI. Need to synchronize access via |mtx_|.
    SortingProperty last_sort_prop_;

    // Set by GUI. 1 == ascending, -1 == descending. Need to synchronize access via |mtx_|.
    int last_sort_direction_;

	// Object, which performs FileInfos sorting.
    Sorter sorter_;

    SearchResultObserver* result_observer_;

    IndexManagersContainer* indices_container_;


    std::vector<pNotifyIndexChangedEventArgs> index_changed_args_;


    Log* logger_;

    std::mutex* mtx_;

    std::condition_variable* conditional_var_;

    std::thread* worker_thread_;
};
