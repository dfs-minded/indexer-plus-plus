// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include <memory>

#include "WindowsWrapper.h"

#include "typedefs.h"

namespace indexer_common {

    class IndexerTimeSpan {
       public:
        explicit IndexerTimeSpan(uint64 tiks);

        ~IndexerTimeSpan() = default;

        uint64 Tiks() const;

        double Seconds() const {
            return instance.wSecond;
        }

        double Minutes() const {
            return instance.wMinute;
        }

        double Hours() const {
            return instance.wHour;
        }

        double Days() const {
            return instance.wDay;
        }

        // Finds difference between two nanosecond time representations and returns a IndexerTimeSpan object.
        static std::unique_ptr<IndexerTimeSpan> Diff(uint64 lhs, uint64 rhs);

       private:
        SYSTEMTIME instance;
    };

} // namespace indexer_common