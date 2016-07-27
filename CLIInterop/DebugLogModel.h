// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#using <WindowsBase.dll>
// clang-format off

namespace CLIInterop 
{
	class SearchEngineLogListener;

	public ref class DebugLogModel
	{
	public:
		DebugLogModel();

		~DebugLogModel();

		void OnNewMessage(System::String^ msg);

		void Clear();

		System::Collections::ObjectModel::ObservableCollection<System::String^>^ LogMessages;		

	private:
		SearchEngineLogListener* listener;

		System::Windows::Threading::Dispatcher^ dispatcher;
	};
}
