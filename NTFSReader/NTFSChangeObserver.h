// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include <memory>

#include "NotifyNTFSChangedEventArgs.h"

namespace ntfs_reader {

	// Implement this interface for listening file system changes. When an NTFS volume files changed
	// (created, modified or deleted) |OnNTFSChanged| method will be called.

    class NTFSChangeObserver {

       public:
        virtual ~NTFSChangeObserver() {}


        // Fired when file system changed. |args| provide the information what exactly has been changed.

        virtual void OnNTFSChanged(std::unique_ptr<NotifyNTFSChangedEventArgs> args){};
    };

} // namespace ntfs_reader