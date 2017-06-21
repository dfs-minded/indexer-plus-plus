// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "SearchEngineLogListener.h"

#include "AsyncLog.h"
#include "EmptyLog.h"
#include "OneThreadLog.h"
#include "Log.h"
#include "DebugLogModel.h"
// clang-format off

namespace CLIInterop
{
	SearchEngineLogListener::SearchEngineLogListener(DebugLogModel^ mdl) 
	{
		using indexer_common::AsyncLog;
		using indexer_common::EmptyLog;
		GET_LOGGER

		model = mdl;
		logger_->RegisterMessagesListener(this);
	}

	void SearchEngineLogListener::OnNewMessage(const std::wstring& msg) const 
	{
		System::String^ logMessage = gcnew System::String(msg.c_str());
		model->OnNewMessage(logMessage);
	}

	SearchEngineLogListener::~SearchEngineLogListener() 
	{
		logger_->UnregisterMessagesListener(this);
	}
}
