// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include <functional>

#include "typedefs.h"
#include "NTFSChangeObserver.h"
#include "NotifyNTFSChangedEventArgs.h"

namespace indexer_test {

	using OnNtfsChangedCallback = std::function<void(ntfs_reader::uNotifyNTFSChangedEventArgs)>;

	class MockNTFSChangeObserver : public ntfs_reader::NTFSChangeObserver {
	  public:
		MockNTFSChangeObserver(OnNtfsChangedCallback on_ntfs_changed_callback) :
			on_ntfs_changed_callback_(on_ntfs_changed_callback) {
		}

		void OnNTFSChanged(ntfs_reader::uNotifyNTFSChangedEventArgs args) override {
			if (on_ntfs_changed_callback_) 
				on_ntfs_changed_callback_(std::move(args));
		}

	  private:
		OnNtfsChangedCallback on_ntfs_changed_callback_;
	};

} // namespace indexer_test