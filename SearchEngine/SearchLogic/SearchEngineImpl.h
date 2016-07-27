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


// Implements search logic, listens index changes, volume status changes and produces corresponding search results.
// When new volume added - merges from index changed arguments all it's files (marked as new) with current search res.
// When a volume removed - merges from index changed arguments all it's files (marked as old) with current search res.

class SearchEngineImpl {

   public:
    SearchEngineImpl(SearchEngine* interface_backlink, SearchResultObserver* result_observer, bool listen_index_change);

    NO_COPY(SearchEngineImpl);

    ~SearchEngineImpl();


    pSearchResult Search(uSearchQuery query);


    void SearchAsync(uSearchQuery query);


    void Sort(const std::string& prop_name, int direction);


    void OnIndexChanged(pNotifyIndexChangedEventArgs p_args);


   private:
    void SearchWorker();


    void ProcessNewSearchQuery();


    void SearchInAllIndicesFromRoot();


    void SearchInSpecificDir();


    // Clears all index changed arguments. Returns items which need to be destroyed.
    // Called when we will read everything from scratch directly from indices.

    std::unique_ptr<OldFileInfosDeleter> ClearIndexChangedArgs();


    bool ReadyToProcessSearch() const;


    // Merges changes with current search result into |u_tmp_search_result_|.

    void ProcessIndexChanged();


    // The function that does sorting.

    void Sort(std::vector<const FileInfo*>* file_infos);


    // Traverses files index tree and forms |tmp_search_result_| from filtered files.

    void SearchInTree(const FileInfo& start_dir, std::vector<const FileInfo*>* result) const;


    // Backlink to interface class in pImpl idiom.
    SearchEngine* interface_class_;

    std::atomic<bool> query_search_res_outdated_;

    std::atomic<bool> index_outdated_;

    std::atomic<bool> sort_outdated_;


    pSearchResult p_search_result_;

    std::unique_ptr<SearchResult> u_tmp_search_result_;


    // Query received from Client (GUI, CMD, etc.).
    std::unique_ptr<SearchQuery> last_query_;

    std::unique_ptr<FileInfosFilter> file_infos_filter_;

    // Set by GUI.
    SortingProperty last_sort_prop_;

    // Set by GUI. 1 = ascending, -1 = descending.
    int last_sort_direction_;

    Sorter sorter_;

    SearchResultObserver* result_observer_;

    IndexManagersContainer* indices_container_;


    std::vector<pNotifyIndexChangedEventArgs> index_changed_args_;


    Log* logger_;

    std::mutex* mtx_;

    std::condition_variable* conditional_var_;

    std::thread* worker_thread_;
};
