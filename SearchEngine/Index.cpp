// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "Index.h"

#include <algorithm>
#include <cassert>
#include <mutex>

#include "AsyncLog.h"
#include "FileInfo.h"
#include "HelperCommon.h"
#include "Macros.h"
#include "OneThreadLog.h"

using namespace std;

Index::Index(char drive_letter) : drive_letter_(drive_letter), root_id_(-1), start_time_(-1) {
    NEW_RECURSIVE_LOCKER

    GET_LOGGER

    logger_->Debug(METHOD_METADATA + L"Creating index for drive " + wstring(1, drive_letter_));
}

Index::~Index() {

    logger_->Debug(METHOD_METADATA + L"Destroying index for drive " + wstring(1, drive_letter_));

    DELETE_LOCKER

    if (!data_) return;

    for (auto* fi : *data_)
        delete fi;

    data_->clear();
    data_ = nullptr;
}

const vector<FileInfo*>* Index::LockData() const {
    PLOCK
    return data_.get();
}

void Index::UnlockData() const {
    PUNLOCK
}

void Index::BuildTree() const {

    P_RECURSIVE_LOCK_GUARD

    for (auto* fi : *data_) {

        if (!fi) continue;

        if (fi->ID == RootID()) continue;

        FileInfo* parent = GetNode(fi->ParentID);

		if (parent == nullptr) {
			logger_->Warning(METHOD_METADATA + L"Cannot find patent for file" +
				HelperCommon::Char16ToWstring(fi->GetName()) + L" with ParentID = " + to_wstring(fi->ParentID));
			continue;
		}

        fi->Parent = parent;

        if (parent->FirstChild == nullptr) {
            parent->FirstChild = fi;
            continue;
        }

        parent->FirstChild->PrevSibling = fi;
        fi->NextSibling                 = parent->FirstChild;
        parent->FirstChild              = fi;
    }
}

const FileInfo* Index::Root() const {
    if (root_id_ == -1) return nullptr;
    return (*data_)[RootID()];
}

vector<FileInfo*>* Index::ReleaseData() {
    P_RECURSIVE_LOCK_GUARD

    return data_.release();
}

void Index::SetData(unique_ptr<vector<FileInfo*>> data) {

    logger_->Debug(METHOD_METADATA + L"Swapping unique ptrs for data on drive " + to_wstring(drive_letter_) +
                   L" Elements count: " + to_wstring(data->size()));

    P_RECURSIVE_LOCK_GUARD

    data_.swap(data);
}

void Index::InsertNode(FileInfo* node) const {
    P_RECURSIVE_LOCK_GUARD

        // Add to the map.
        (*data_)[node->ID] = node;

    // Insert in the tree.
    FileInfo* parent = GetNode(node->ParentID);

    if (!parent) {
        WriteToOutput(METHOD_METADATA + L"No parent dir found in the tree for item: ID" + to_wstring(node->ID) +
                      L", Name:" + HelperCommon::Char16ToWstring(node->GetName()));

        parent = (*data_)[RootID()];
    }

    node->Parent = parent;

    if (parent->FirstChild == nullptr) {
        parent->FirstChild = node;
        return;
    }

    parent->FirstChild->PrevSibling = node;
    node->NextSibling               = parent->FirstChild;
    parent->FirstChild              = node;
}

void Index::RemoveNode(const FileInfo* node) const {

    P_RECURSIVE_LOCK_GUARD

    FileInfo* parent = node->Parent;

    if (node->PrevSibling == nullptr)  // Element is first.
    {
        if (node->NextSibling == nullptr) {  // If the only node.
            parent->FirstChild = nullptr;

        } else {  // If has siblings.

            parent->FirstChild             = node->NextSibling;
            node->NextSibling->PrevSibling = nullptr;
        }

    } else if (node->NextSibling == nullptr) {  // Element is the last.

        node->PrevSibling->NextSibling = nullptr;

    } else if (node->PrevSibling != nullptr && node->NextSibling != nullptr) {  // Element in the middle of linked list.

        node->PrevSibling->NextSibling = node->NextSibling;
        node->NextSibling->PrevSibling = node->PrevSibling;
    }

    (*data_)[node->ID] = nullptr;
}

FileInfo* Index::GetNode(uint ID) const {

    P_RECURSIVE_LOCK_GUARD
    return (*data_)[ID];
}

void Index::CalculateDirsSizes() const {

    P_RECURSIVE_LOCK_GUARD

    for (const auto* fi : *data_) {
        if (!fi) continue;
        UpdateParentDirsSize(fi, fi->SizeReal);
    }
}

void Index::UpdateParentDirsSize(const FileInfo* fi, int size_delta) const {

    P_RECURSIVE_LOCK_GUARD

    if (fi->IsDirectory() || size_delta == 0) return;

    FileInfo* tmp = fi->Parent;

    while (true) {

		if (tmp == nullptr) {
			logger_->Warning(METHOD_METADATA + L"Cannot find patent for file with ParentID = " + to_wstring(fi->ParentID));
			return;
		}

        // Just in case for size rounding error accumulation, assign zero if less than zero.
        tmp->SizeReal = max(0, tmp->SizeReal + size_delta);

        if (tmp->ID == RootID()) break;
        tmp = tmp->Parent;
    }
}

static const wstring kDelim = L"|";

void SerializeToOutput(const FileInfo* first_child, wstring indent /*= L"\t"*/) {

    const FileInfo* current = first_child;

    while (current != nullptr) {
        wstring message = indent + HelperCommon::Char16ToWstring(current->GetName()) + kDelim +
                          to_wstring(current->ID) + kDelim + to_wstring(current->ParentID);

        WriteToOutput(message);

        if (current->FirstChild != nullptr) {
            SerializeToOutput(current->FirstChild, indent + L'\t');
        }
        current = current->NextSibling;
    }
}