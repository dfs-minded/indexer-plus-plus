#include "SearchResult.h"

using namespace std;

SearchResult::SearchResult(unique_ptr<OldFileInfosDeleter> u_old_file_infos /*= nullptr*/)
    : Files(make_unique<vector<const FileInfo*>>()), old_file_infos_(move(u_old_file_infos)) {
}