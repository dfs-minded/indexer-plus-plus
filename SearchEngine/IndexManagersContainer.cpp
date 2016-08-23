// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "IndexManagersContainer.h"

#include "AsyncLog.h"
#include "FileInfo.h"
#include "HelperCommon.h"
#include "Log.h"
#include "OneThreadLog.h"

#include "Index.h"
#include "LetterCaseMatching.h"
#include "StatusObserver.h"
#include "TextComparison.h"

using namespace std;

IndexManagersContainer::IndexManagersContainer() {
    GET_LOGGER
}

IndexManagersContainer& IndexManagersContainer::Instance() {
    static IndexManagersContainer instance;
    return instance;
}

void IndexManagersContainer::AddDrive(char drive_letter) {

    auto drive_letter_w = wstring(1, drive_letter);

    for (const auto& mgr : index_managers_) {

        if (mgr->DriveLetter() == drive_letter) {

            logger_->Debug(METHOD_METADATA + L"Enabling index manager for drive: " + drive_letter_w);
            mgr->EnableIndex();

            OnVolumeStatusChanged(drive_letter);
            return;
        }
    }

    logger_->Debug(METHOD_METADATA + L"Adding new drive: " + drive_letter_w);

    auto newIndexMgr = make_unique<IndexManager>(drive_letter, this);

    logger_->Debug(METHOD_METADATA + L"Starting IndexManager fro drive: " + drive_letter_w);
    newIndexMgr->RunAsync();

    index_managers_.push_back(move(newIndexMgr));

    OnVolumeStatusChanged(drive_letter);
}

void IndexManagersContainer::RemoveDrive(char drive_letter) {

    for (const auto& mgr : index_managers_) {

        if (mgr->DriveLetter() == drive_letter) {
            mgr->DisableIndex();
            break;
        }
    }
}

uint IndexManagersContainer::GetIndexRootID(char index_drive_letter) const {

    for (const auto& mgr : index_managers_) {

        if (mgr->DriveLetter() == index_drive_letter) return mgr->IndexRootID();
    }

    return -1;
}

void IndexManagersContainer::OnIndexChanged(pNotifyIndexChangedEventArgs p_args) {

    for (auto& observer : index_change_observers_) {
        observer->OnIndexChanged(p_args);
    }
}

#ifdef SINGLE_THREAD
void IndexManagersContainer::CheckUpdates() {
    for (const auto& mgr : index_managers_) {
        mgr->CheckUpdates();
    }
}
#endif

void IndexManagersContainer::OnVolumeStatusChanged(char drive_letter) {

    logger_->Debug(METHOD_METADATA + L"Loading finished for drive " + wstring(1, drive_letter));

    for (auto& observer : status_observers_) {
        observer->StatusChanged(GetStatus());
    }

    for (auto& observer : index_change_observers_) {
        observer->OnVolumeStatusChanged(drive_letter);
    }
}

void IndexManagersContainer::RegisterIndexChangeObserver(IndexChangeObserver* observer) {
    index_change_observers_.push_back(observer);
}

void IndexManagersContainer::UnregisterIndexChangeObserver(IndexChangeObserver* observer) {
    index_change_observers_.remove(observer);
}

string IndexManagersContainer::GetStatus() const {

    string res("   ");

    for (const auto& mgr : index_managers_) {

        if (mgr->DisableIndexRequested()) continue;

        res += mgr->DriveLetter();
        res += mgr->ReadingMFTFinished() ? ": finished" : ": reading MFT";

        res += string(";  ");
    }

    logger_->Debug(METHOD_METADATA + HelperCommon::StringToWstring(res));

    return res;
}

vector<const IndexManager*> IndexManagersContainer::GetAllIndexManagers() const {

    vector<const IndexManager*> res;

    for (const auto& mgr : index_managers_) {
        res.push_back(mgr.get());
    }

    return res;
}

const IndexManager* IndexManagersContainer::GetIndexManager(char drive_letter) const {

    for (auto& mgr : index_managers_) {
        if (mgr->DriveLetter() == drive_letter) return mgr.get();
    }

    return nullptr;
}

const FileInfo* IndexManagersContainer::GetFileInfoByPath(const u16string& path) const {

    u16string delim(reinterpret_cast<const char16_t*>(L"/\\"));
    auto path_components = HelperCommon::Split(path, delim);
    const FileInfo* root = nullptr;

    auto table = GetLowerMatchTable();

    auto managers = GetAllIndexManagers();

    for (const auto* mgr : managers) {
        if (!mgr->ReadingMFTFinished() || mgr->DisableIndexRequested()) continue;

        if (table[mgr->DriveLetterW()[0]] == table[path_components[0][0]]) {
            root = mgr->GetIndex()->Root();
            break;
        }
    }

    if (root == nullptr)  // Probably disk is not loaded.
        return nullptr;


    for (int i = 0;;) {

        if (compare(root->GetName(), path_components[i].c_str(), table) == 0) {
            ++i;

            if (i == path_components.size()) 
				return root;

            root = root->FirstChild;
            continue;
        }

        root = root->NextSibling;

        if (!root)  // Reached the end of the search.
            return nullptr;
    }
}

void IndexManagersContainer::RegisterStatusChangeObserver(StatusObserver* observer) {
    status_observers_.push_back(observer);
}

void IndexManagersContainer::UnregisterStatusChangeObserver(StatusObserver* observer) {
    status_observers_.remove(observer);
}
