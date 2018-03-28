// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "IndexManagersContainer.h"

#include <mutex>

#include "AsyncLog.h"
#include "OneThreadLog.h"
#include "EmptyLog.h"
#include "FileInfo.h"
#include "../Common/Helpers.h"

#include "Index.h"
#include "LetterCaseMatching.h"
#include "StatusObserver.h"
#include "TextComparison.h"

namespace indexer {

	using std::string;
	using std::wstring;
	using std::u16string;
	using std::vector;

	using namespace indexer_common;


    IndexManagersContainer::IndexManagersContainer() {

        NEW_MUTEX

        GET_LOGGER
    }

    IndexManagersContainer::~IndexManagersContainer() {
        DELETE_MUTEX
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

        auto new_index_mgr = std::make_unique<IndexManager>(drive_letter, this);

        logger_->Debug(METHOD_METADATA + L"Starting IndexManager for drive: " + drive_letter_w);
        new_index_mgr->RunAsync();

        PLOCK
        index_managers_.push_back(std::move(new_index_mgr));
        PUNLOCK

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

	// Called from SE thread.
    uint IndexManagersContainer::GetIndexRootID(char index_drive_letter) const {
        PLOCK_GUARD
        for (const auto& mgr : index_managers_) {

            if (mgr->DriveLetter() == index_drive_letter) return mgr->IndexRootID();
        }

        return -1;
    }

	// Called form IndexManager and from UI threads.
    void IndexManagersContainer::OnIndexChanged(pNotifyIndexChangedEventArgs p_args) {

        for (auto& observer : index_change_observers_) {
            observer->OnIndexChanged(p_args);
        }
    }

	void IndexManagersContainer::UpdateIndicesChangedEventPriority(indexer_common::FilesystemUpdatesPriority new_priotity) {
		PLOCK_GUARD

		for (const auto& mgr : index_managers_)
			mgr->UpdateIndexChangesPriority(new_priotity);
	}

#ifdef SINGLE_THREAD
    void IndexManagersContainer::CheckUpdates() {
        for (const auto& mgr : index_managers_) {
            mgr->CheckUpdates();
        }
    }
#endif

	// Called form IndexManager and from UI threads.
    void IndexManagersContainer::OnVolumeStatusChanged(char drive_letter) {

        logger_->Debug(METHOD_METADATA + L"Loading finished for drive " + wstring(1, drive_letter));

        auto status_text = GetStatus();

        for (auto& observer : status_observers_)
            observer->StatusChanged(status_text);

        for (auto& observer : index_change_observers_)
            observer->OnVolumeStatusChanged(drive_letter);
    }

    void IndexManagersContainer::RegisterIndexChangeObserver(IndexChangeObserver* observer) {
        index_change_observers_.push_back(observer);
    }

    void IndexManagersContainer::UnregisterIndexChangeObserver(IndexChangeObserver* observer) {
        index_change_observers_.remove(observer);
    }

	// Called form IndexManager and from UI threads.
    string IndexManagersContainer::GetStatus() const {

        string res("   ");

        PLOCK_GUARD

        for (const auto& mgr : index_managers_) {

            if (mgr->DisableIndexRequested()) continue;

            res += mgr->DriveLetter();
            res += mgr->ReadingMFTFinished() ? ": finished" : ": reading MFT";

            res += string(";  ");
        }

        logger_->Debug(METHOD_METADATA + helpers::StringToWstring(res));

        return res;
    }

	// Called from UI and SE thread.
    vector<const IndexManager*> IndexManagersContainer::GetAllIndexManagers() const {

        vector<const IndexManager*> res;

        PLOCK_GUARD
        for (const auto& mgr : index_managers_) {
            res.push_back(mgr.get());
        }

        return res;
    }

	// Called from SE thread.
    const IndexManager* IndexManagersContainer::GetIndexManager(char drive_letter) const {
        PLOCK_GUARD
        for (auto& mgr : index_managers_) {
            if (mgr->DriveLetter() == drive_letter) return mgr.get();
        }

        return nullptr;
    }

	// Called from UI and SE threads.
    const FileInfo* IndexManagersContainer::GetFileInfoByPath(const u16string& path) const {

        u16string delim(reinterpret_cast<const char16_t*>(L"/\\"));
        auto path_components = helpers::Split(path, delim);
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

                if (i == path_components.size()) return root;

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

} // namespace indexer