// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "Index.h"

#include <algorithm>
#include <cassert>
#include <mutex>

#include "AsyncLog.h"
#include "FileInfo.h"
#include "../Common/Helpers.h"
#include "Macros.h"
#include "OneThreadLog.h"

namespace indexer {

	using std::wstring;
	using std::to_wstring;
    using std::vector;
	using std::unique_ptr;

	using namespace indexer_common;

    Index::Index(char drive_letter)
        : drive_letter_(drive_letter), drive_letter_w_(1, drive_letter), root_id_(-1), start_time_(-1) {
        NEW_MUTEX

        GET_LOGGER

        logger_->Debug(METHOD_METADATA + L"Creating index for drive " + DriveLetterW());
    }

    Index::~Index() {

        logger_->Debug(METHOD_METADATA + L"Destroying index for drive " + DriveLetterW());

        DELETE_MUTEX

        if (!data_) return;

        for (auto* fi : *data_)
            delete fi;

        data_->clear();
        data_ = nullptr;
    }

    const vector<FileInfo*>* Index::LockData() {
        PLOCK
        return data_.get();
    }

    void Index::UnlockData() {
        PUNLOCK
    }

    void Index::BuildTree() {

        for (auto* fi : *data_) {
            if (!fi) continue;

            if (fi->ID == RootID()) continue;

            FileInfo* parent = GetNode(fi->ParentID);

            if (parent == nullptr) {
                logger_->Warning(METHOD_METADATA + L"Cannot find parent for file" +
                                 helpers::Char16ToWstring(fi->GetName()) + L" with ParentID = " +
                                 to_wstring(fi->ParentID) +
                                 L" Deleting this FileInfo and assigning nullptr to it in the Index.");

                (*data_)[fi->ID] = nullptr;
                delete fi;

                continue;
            }

            fi->Parent = parent;

            if (parent->FirstChild == nullptr) {
                parent->FirstChild = fi;
                continue;
            }

            parent->FirstChild->PrevSibling = fi;
            fi->NextSibling = parent->FirstChild;
            parent->FirstChild = fi;
        }
    }

    const FileInfo* Index::Root() const {
        if (root_id_ == -1) return nullptr;
        return (*data_)[RootID()];
    }

    vector<FileInfo*>* Index::ReleaseData() {
        logger_->Debug(METHOD_METADATA + L" called for drive " + DriveLetterW() + L" The index becomes empty.");

        return data_.release();
    }

    void Index::SetData(unique_ptr<vector<FileInfo*>> data) {

        logger_->Debug(METHOD_METADATA + L"Swapping unique ptrs for data on drive " + DriveLetterW() +
                       L" Elements count: " + to_wstring(data->size()));

        data_.swap(data);
    }

    bool Index::InsertNode(FileInfo* fi) {

        // Insert in the tree if the parent node found.

        FileInfo* parent = GetNode(fi->ParentID);

        if (!parent) {
            logger_->Warning(METHOD_METADATA + L"No parent dir found in the tree for item with ID: " + to_wstring(fi->ID) +
                             L" ParentID: " + to_wstring(fi->ParentID) + L", Name: " +
                             helpers::Char16ToWstring(fi->GetName()));
            return false;
        }

        fi->Parent = parent;

        if (parent->FirstChild == nullptr) {
            parent->FirstChild = fi;
        } else {
            parent->FirstChild->PrevSibling = fi;
            fi->NextSibling = parent->FirstChild;
            parent->FirstChild = fi;
        }

        // Insert into the map.
        (*data_)[fi->ID] = fi;

        logger_->Debug(METHOD_METADATA + L"Inserted node with ID: " + to_wstring(fi->ID) + L" Name: " +
                       helpers::Char16ToWstring(fi->GetName()) + L" ParentID: " + to_wstring(fi->ParentID));

        return true;
    }

    void Index::RemoveNode(FileInfo* node) {

        FileInfo* parent = node->Parent;

        if (node->PrevSibling == nullptr)  // Element is first.
        {
            if (node->NextSibling == nullptr) {  // If the only node.
                parent->FirstChild = nullptr;

            } else {  // If has siblings.

                parent->FirstChild = node->NextSibling;
                node->NextSibling->PrevSibling = nullptr;
            }

        } else if (node->NextSibling == nullptr) {  // Element is the last.

            node->PrevSibling->NextSibling = nullptr;

        } else {  // Element is in the middle of the linked list.

            node->PrevSibling->NextSibling = node->NextSibling;
            node->NextSibling->PrevSibling = node->PrevSibling;
        }

        // Cleanup all pointers on the tree.
        node->FirstChild = node->PrevSibling = node->NextSibling = nullptr;

        (*data_)[node->ID] = nullptr;
    }

    FileInfo* Index::GetNode(uint ID) const {

        // MFT can increase its size, so the allocated number of records must be increased accordingly.
        if (data_->size() <= ID) {
            logger_->Info(L"MFT size has increased. Current last record ID = " + to_wstring(data_->size() - 1) +
                          L" Requested record ID = " + to_wstring(ID));
            data_->resize(ID + 1);
        }

        return (*data_)[ID];
    }

    void Index::CalculateDirsSizes() {

        for (const auto* fi : *data_) {
            if (!fi) continue;
            UpdateParentDirsSize(fi, fi->SizeReal);
        }
    }

    void Index::UpdateParentDirsSize(const FileInfo* fi, int size_delta) {

        if (fi->IsDirectory() || size_delta == 0) return;

        FileInfo* tmp = GetNode(fi->ParentID);

        while (true) {
            if (tmp == nullptr) {
                logger_->Warning(METHOD_METADATA + L"Cannot find patent for file with ID = " + to_wstring(fi->ID) +
                                 L" with ParentID = " + to_wstring(fi->ParentID));
                return;
            }

            // Just in case for size rounding error accumulation, assign zero if less than zero.
            tmp->SizeReal = max(0, tmp->SizeReal + size_delta);

            if (tmp->ID == RootID()) break;
            tmp = GetNode(tmp->ParentID);
        }
    }

    static const wstring kDelim = L"|";

    void SerializeToOutput(const FileInfo* first_child, wstring indent /*= L"\t"*/) {

        const FileInfo* current = first_child;

        while (current != nullptr) {
            wstring message = indent + helpers::Char16ToWstring(current->GetName()) + kDelim +
                              to_wstring(current->ID) + kDelim + to_wstring(current->ParentID);

			helpers::WriteToOutput(message);

            if (current->FirstChild != nullptr) SerializeToOutput(current->FirstChild, indent + L'\t');

            current = current->NextSibling;
        }
    }
} // namespace indexer