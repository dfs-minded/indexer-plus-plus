// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

namespace indexer {

	// Inherit from this class to listen volumes status change. Designed for ModelUpdater
	// to pass the newStatus string to UI.

    class StatusObserver {

       public:
        virtual void StatusChanged(const std::string& new_status) = 0;

        virtual ~StatusObserver() {}
    };

} // namespace indexer