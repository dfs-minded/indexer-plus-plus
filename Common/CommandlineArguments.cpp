// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "CommandlineArguments.h"

using namespace std;

namespace {
const wchar_t* kSaveParsedInFileInfoRecordArgName = L"saveFileInfos";

const wchar_t* kReplayFileInfosPathArgName = L"replayFileInfosPath";

const wchar_t* kSaveUSNJournalRecordsArgName = L"saveUSNJournalRecords";

const wchar_t* kReplayUSNRecPathArgName = L"replayUSNRecPath";

const wchar_t* kSaveRawMFTArgName = L"saveRawMFT";

const wchar_t* kRawMFTPathArgName = L"rawMFTPath";
}

CommandlineArguments::CommandlineArguments()
    : SaveUSNJournalRecords(false),
      SaveFileInfos(false),
      SaveRawMFT(false),
      ReplayUSNRecPath(L""),
      ReplayFileInfosPath(L"") {
}

void CommandlineArguments::SetPath(const wstring& arg, const wchar_t* path_arg_name, std::wstring* path) {
    if (arg.find(path_arg_name) != -1) {
        int splitter_index = arg.find(L'=');
        if (splitter_index != -1) *path = arg.substr(splitter_index + 1);
    }
}

void CommandlineArguments::SetBoolFlag(const wstring& arg, const wchar_t* flag_name, bool* flag) {
    if (arg == flag_name) *flag = true;
}

void CommandlineArguments::Parse(const vector<wstring>& command_line_args) {
    auto& instance = Instance();
    for (const auto& arg : command_line_args) {
        SetPath(arg, kReplayFileInfosPathArgName, &instance.ReplayFileInfosPath);
        SetPath(arg, kReplayUSNRecPathArgName, &instance.ReplayUSNRecPath);
        SetPath(arg, kRawMFTPathArgName, &instance.RawMFTPath);
        SetBoolFlag(arg, kSaveParsedInFileInfoRecordArgName, &instance.SaveFileInfos);
        SetBoolFlag(arg, kSaveUSNJournalRecordsArgName, &instance.SaveUSNJournalRecords);
        SetBoolFlag(arg, kSaveRawMFTArgName, &instance.SaveRawMFT);
    }
}
