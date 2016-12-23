// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include "IndexChangeObserver.h"

namespace indexer {

    class MockIndexChangeObserver : public IndexChangeObserver {
       public:
        MockIndexChangeObserver() = default;

        virtual void OnIndexChanged(pNotifyIndexChangedEventArgs p_args) override {
            IndexChangedArgs = p_args;
        }

        virtual void OnVolumeStatusChanged(char drive_letter) override {
        }

        pNotifyIndexChangedEventArgs IndexChangedArgs;
    };

} // namespace indexer