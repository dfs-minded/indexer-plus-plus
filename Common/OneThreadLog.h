// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include <string>

#include "Log.h"
#include "Macros.h"

class OneThreadLog : public Log {
   public:
    static Log& Instance();

    NO_COPY(OneThreadLog)

    virtual void Debug(const std::wstring& message) override;

    virtual void Info(const std::wstring& message) override;

    virtual void Warning(const std::wstring& message) override;

    virtual void Error(const std::wstring& message) override;

   private:
    OneThreadLog();

    ~OneThreadLog();

    inline void WriteToFile(std::wstring& msg);

    FILE* log_file_;
};
