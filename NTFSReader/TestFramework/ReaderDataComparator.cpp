// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "ReaderDataComparator.h"

#include <algorithm>
#include <string>
#include <unordered_set>
#include <vector>

#include "FileInfo.h"
#include "FileInfoHelper.h"
#include "HelperCommon.h"

namespace ntfs_reader {

    using namespace std;

    void ReaderDataComparator::Compare(const vector<FileInfo*>& lhs, const FileInfo& lhs_root, const wstring& lhs_name,
                                       const vector<FileInfo*>& rhs, const FileInfo& rhs_root, const wstring& rhs_name) {
        lhs_name_ = lhs_name;
        rhs_name_ = rhs_name;
        result_ = L"ReaderDataComparator::Compare\n";
        result_ += L"lhs count: " + to_wstring(lhs.size()) + L" rhs count: " + to_wstring(rhs.size());
        result_ += lhs_root.ID == rhs_root.ID ? L" Roots are the same\n" : L"Roots are the different\n";

        Compare(lhs, rhs);

        WriteToOutput(result_);
    }

    void ReaderDataComparator::Compare(const vector<FileInfo*>& lhs, const vector<FileInfo*>& rhs) {

        unordered_set<uint> seen_ids;
        vector<uint> only_in_lhs_ids, only_in_rhs_ids, different_fi_ids;

        for (const auto* fi_lhs : lhs) {
            if (!fi_lhs) continue;

            seen_ids.insert(fi_lhs->ID);

            auto fi_rhs = rhs[fi_lhs->ID];

            if (!fi_rhs)
                only_in_lhs_ids.push_back(fi_lhs->ID);

            else if (!Compare(*fi_lhs, *fi_rhs))
                different_fi_ids.push_back(fi_lhs->ID);
        }

        for (const auto* fi_rhs : rhs) {
            if (!fi_rhs) continue;

            if (seen_ids.find(fi_rhs->ID) != seen_ids.end()) continue;

            only_in_rhs_ids.push_back(fi_rhs->ID);
        }

        result_ += L" Result: only in lhs " + to_wstring(only_in_lhs_ids.size()) + L"  only in rhs " +
                   to_wstring(only_in_rhs_ids.size()) + L"   different in lhs and rhs " +
                   to_wstring(different_fi_ids.size()) + L"\n";

        result_ += L" Only in " + lhs_name_ + L" result\n;";
        for (size_t i = 0; i < min((size_t)100, only_in_lhs_ids.size()); ++i)
            result_ += SerializeFileInfoHumanReadable(*lhs.at(only_in_lhs_ids[i])) + L"\n";

        result_ += L" Only in " + rhs_name_ + L" result\n";
        for (size_t i = 0; i < min((size_t)100, only_in_rhs_ids.size()); ++i)
            result_ += SerializeFileInfoHumanReadable(*rhs.at(only_in_rhs_ids[i])) + L"\n";

        result_ += L"Different:\n";

        for (size_t i = 0; i < min((size_t)1000, different_fi_ids.size()); ++i) {

            FileInfo* lhs_fi = lhs.at(different_fi_ids[i]);
            FileInfo* rhs_fi = rhs.at(different_fi_ids[i]);

            result_ += L"ID = " + to_wstring(rhs_fi->ID);

            result_ += L"lhs: ";
            result_ += SerializeOnlyWhatIsDiffer(*lhs_fi, *rhs_fi) + L"\n";
            result_ += L"rhs: ";
            result_ += SerializeOnlyWhatIsDiffer(*rhs_fi, *lhs_fi) + L"\n";
            result_ += L"\n";
        }

        WriteToOutput(result_);
    }


// Timestamps, SizeAllocated and SizeReal that we receive from RawMFTReader are more accurate than in WinAPI.
    wstring ReaderDataComparator::SerializeOnlyWhatIsDiffer(const FileInfo& lhs, const FileInfo& rhs) {
        wstring res;

        if (lhs.ParentID != rhs.ParentID) res += L"; ParentID = " + to_wstring(lhs.ParentID);

        wstring ln = reinterpret_cast<const wchar_t*>(lhs.GetName());
        wstring rn = reinterpret_cast<const wchar_t*>(rhs.GetName());

        if (lhs.NameLength != rhs.NameLength) res += L"; NameLength = " + to_wstring(lhs.NameLength);

        if (ln.compare(rn) != 0) res += L"; lhsleInfo: Name = " + wstring(reinterpret_cast<const wchar_t*>(lhs.GetName()));

        if (lhs.FileAttributes != rhs.FileAttributes) res += L"; FileAttributes = " + to_wstring(lhs.FileAttributes);

        if (lhs.SizeReal != rhs.SizeReal) res += L"; SizeReal = " + to_wstring(lhs.SizeReal);

        /*if (lhs.SizeAllocated != rhs.SizeAllocated)
                res += L"; SizeAllocated = " + to_wstring(lhs.SizeAllocated);*/

        if (lhs.CreationTime != rhs.CreationTime) res += L"; CreationTime = " + to_wstring(lhs.CreationTime);

        if (lhs.LastAccessTime != rhs.LastAccessTime) res += L"; LastAccessTime = " + to_wstring(lhs.LastAccessTime);

        if (lhs.LastWriteTime != rhs.LastWriteTime) res += L"; LastWriteTime = " + to_wstring(lhs.LastWriteTime);

        return move(res);
    }

    bool ReaderDataComparator::Compare(const FileInfo& lhs, const FileInfo& rhs) {
        // bool equal_result = lhs == rhs;
        if (lhs.ParentID != rhs.ParentID) return false;

        if (wstring(reinterpret_cast<const wchar_t*>(lhs.GetName())) !=
            wstring(reinterpret_cast<const wchar_t*>(rhs.GetName())))
            return false;

        if (lhs.SizeReal != rhs.SizeReal) return false;

        // WinApi could not retrieve timestamps.
        if (!rhs.CreationTime || !rhs.LastAccessTime || !rhs.LastWriteTime) return true;

        if (lhs.CreationTime != rhs.CreationTime || lhs.LastAccessTime != rhs.LastAccessTime ||
            lhs.LastWriteTime != rhs.LastWriteTime)
            return false;

        return true;
    }

} // namespace ntfs_reader