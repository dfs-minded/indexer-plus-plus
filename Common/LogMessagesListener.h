// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include "macros.h"

// Used for debug log window.
namespace indexer_common {

    class LogMessagesListener {
       public:
        NO_COPY(LogMessagesListener)

        LogMessagesListener() = default;

        virtual ~LogMessagesListener() = default;

        virtual void OnNewMessage(const std::wstring& msg) const = 0;
    };

} // namespace indexer_common