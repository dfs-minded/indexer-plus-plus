// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include <memory>
#include <string>
#include <vector>
#include "WindowsWrapper.h"

#include "typedefs.h"

class IQueryProcessor {
   public:
    virtual std::vector<std::wstring> Process(const std::wstring& query, const std::wstring& format, int max_files) = 0;

    virtual ~IQueryProcessor() {
    }
};

typedef std::shared_ptr<IQueryProcessor> pIQueryProcessor;

class ConnectionManager {
   public:
    ConnectionManager() = default;

    // This method is synchronous, argument is callback which called when query is received.
    void CreateServer(pIQueryProcessor queryProcessor);

    // The method for send query and receiving result on client, this method is synchronous.
    // Return false in case of failure.
    bool SendQuery(const std::wstring& query, const std::wstring& format, int max_files,
                   std::vector<std::wstring>* result);

   private:
    void ServerMain();

    void Reply(const std::wstring& query, const std::wstring& format, const std::wstring& data_pipename, int max_files);

    static HANDLE CreatePipe(const std::wstring& pipename);

    static HANDLE OpenPipe(const std::wstring& pipename);

    static void ParseData(const std::wstring& data, std::wstring* pipename, std::wstring* query, std::wstring* format,
                          int* max_files);

    static bool WriteMessageToPipe(HANDLE pipe, const std::wstring& message);

    bool ReadMessageFromPipe(HANDLE pipe, std::wstring& result);

    HANDLE pipe_ = nullptr;

    pIQueryProcessor query_processor_;

    static const int kBufSize = 1024;

    TCHAR buffer_[kBufSize + 1];

    static const wchar_t kDefaultPipeName[];

    static const std::wstring kLastMessageIndicator;
};
