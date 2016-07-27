// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include <memory>

#include "Macros.h"

#include "NotifyIndexChangedEventArgs.h"

// Provides an interface for listening index changes.
// Each of its methods will be called when corresponding event occurs.

class IndexChangeObserver {

   public:
    IndexChangeObserver() = default;

    NO_COPY(IndexChangeObserver)

    virtual ~IndexChangeObserver() = default;


    // Fired when index files changed. |p_args| provide the information what exactly has changed.

    virtual void OnIndexChanged(pNotifyIndexChangedEventArgs p_args){};


    // Fired when the volume has been added or removed by user.

    virtual void OnVolumeStatusChanged(char drive_letter){};
};