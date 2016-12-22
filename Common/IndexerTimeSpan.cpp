// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "IndexerTimeSpan.h"

#include "IndexerDateTime.h"

namespace indexer_common {

    using namespace std;

    IndexerTimeSpan::IndexerTimeSpan(uint64 tiks) {
        auto tmp = IndexerDateTime::TicksToFiletime(tiks);
        instance = IndexerDateTime::FiletimeToSystemtime(tmp);
    }

    uint64 IndexerTimeSpan::Tiks() const {
        return IndexerDateTime::SystemtimeToUInt64(instance);
    }

    unique_ptr<IndexerTimeSpan> IndexerTimeSpan::Diff(uint64 lhs, uint64 rhs) {
        if (rhs > lhs) swap(lhs, rhs);

        return make_unique<IndexerTimeSpan>(lhs - rhs);
    }

} // namespace indexer_common