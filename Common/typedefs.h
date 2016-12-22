// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include <memory>
#include <string>
#include <vector>

namespace indexer_common {

    typedef long long int64;
    typedef unsigned long ulong;
    typedef unsigned long long uint64;
    typedef unsigned short ushort;
    typedef unsigned int uint;
    typedef unsigned char uchar;

    class FileInfo;

    typedef std::shared_ptr<std::vector<std::wstring>> pVectorWstring;

} // namespace indexer_common