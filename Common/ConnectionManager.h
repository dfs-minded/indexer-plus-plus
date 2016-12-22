// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "WindowsWrapper.h"

#include "typedefs.h"

namespace indexer_common {

    class Log;

    class IQueryProcessor {
       public:
        virtual std::vector<std::wstring> Process(const std::wstring& query, const std::wstring& format, int max_files) = 0;

        virtual ~IQueryProcessor() {
        }
    };

    typedef std::shared_ptr<IQueryProcessor> pIQueryProcessor;

    class ConnectionManager {
       public:
        ConnectionManager();

        // Creates the server in the separate thread which listens
        // |query_processor| is a callback which is called when a new query arrives.

        void CreateServer(pIQueryProcessor query_processor);


        // The method for send query and receiving result on a client, this method is synchronous.
        // Return false in case of failure.

        bool SendQuery(const std::wstring& query, const std::wstring& format, int max_files,
                       std::vector<std::wstring>* result);

       private:
        void ServerWorker();

        void Reply(HANDLE pipe);

        static void ParseData(const std::wstring& data, std::wstring* query, std::wstring* format, int* max_files);


        HANDLE CreatePipe(const std::wstring& pipename);

        HANDLE OpenPipe(const std::wstring& pipename);

        bool WriteMessageToPipe(HANDLE pipe, const std::wstring& message);

        bool ReadMessageFromPipe(HANDLE pipe, TCHAR* buffer, std::wstring* result);

        Log* logger_;

        pIQueryProcessor query_processor_;

        static const int kBufSize = 1024;

        static const wchar_t kDefaultPipeName[];

        static const std::wstring kLastMessageIndicator;
    };

} // namespace indexer_common