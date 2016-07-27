// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include "typedefs.h"

struct MFTReadResult;

class ReaderDataComparator {
   public:
    void Compare(const std::vector<FileInfo*>& lhs, const FileInfo& lhs_root, const std::wstring& lhs_name,
                 const std::vector<FileInfo*>& rhs, const FileInfo& rhs_root, const std::wstring& rhs_name);

   private:
    void Compare(const std::vector<FileInfo*>& lhs, const std::vector<FileInfo*>& rhs);

    static bool Compare(const FileInfo& lhs, const FileInfo& rhs);

    static std::wstring SerializeOnlyWhatIsDiffer(const FileInfo& lhs, const FileInfo& rhs);

    std::wstring result_;
    std::wstring lhs_name_;
    std::wstring rhs_name_;
};
