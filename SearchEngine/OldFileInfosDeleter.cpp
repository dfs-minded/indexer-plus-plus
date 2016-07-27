#include "OldFileInfosDeleter.h"

#include "AsyncLog.h"
#include "FileInfo.h"

using namespace std;

OldFileInfosDeleter::~OldFileInfosDeleter() {

    if (file_infos_to_delete_.empty()) return;

    // Logger().Debug(L"OldFileInfosDeleter destructor::Destroying " + to_wstring(file_infos_to_delete_.size()) + L"
    // items.");

    for (const auto& fi_collection : file_infos_to_delete_) {

        for (auto fi : fi_collection) {
            delete fi;
            fi = nullptr;
        }
    }
}

void OldFileInfosDeleter::AddItemsToDelete(vector<const FileInfo*>&& u_file_infos) {
    file_infos_to_delete_.push_back(move(u_file_infos));
}

Log& OldFileInfosDeleter::Logger() {

    static Log* logger_{};

    if (!logger_) {
        GET_LOGGER
    }

    return *logger_;
}
