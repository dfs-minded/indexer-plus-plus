// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include <vcclr.h>
#include <string>

#include "DebugLogModel.h"
#include "LogMessagesListener.h"
#include "macros.h"
// clang-format off

namespace indexer_common { class Log; }

namespace CLIInterop {
	class SearchEngineLogListener : public indexer_common::LogMessagesListener {
	  public:
		explicit SearchEngineLogListener(DebugLogModel^ mdl);

		NO_COPY(SearchEngineLogListener)

		virtual ~SearchEngineLogListener();

		virtual void OnNewMessage(const std::wstring& msg) const override;

	  private:
		gcroot<DebugLogModel^> model;

		indexer_common::Log* logger_;
	};
}
