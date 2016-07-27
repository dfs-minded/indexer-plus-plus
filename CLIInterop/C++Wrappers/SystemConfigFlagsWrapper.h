// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once
#include "SystemConfigFlags.h"
// clang-format off

namespace CLIInterop 
{
	// C++/CLI Wrapper on SystemConfigFlags (from Common lib).
	public ref class SystemConfigFlagsWrapper
	{
	public:
		static SystemConfigFlagsWrapper^ Instance()
		{
			if (instance == nullptr)
			{
				instance = gcnew SystemConfigFlagsWrapper();
			}
			return instance;
		}

		bool TrayIcon;
		bool PipeManager;
		bool CallWatchChanges;
		bool ShelContextMenu;
		bool ShowDebugLogWindow;

	private:
		static SystemConfigFlagsWrapper^ instance;

		SystemConfigFlagsWrapper()
		{
			auto& flags = SystemConfigFlags::Instance();
			TrayIcon = flags.TrayIcon;
			PipeManager = flags.PipeManager;
			CallWatchChanges = flags.CallWatchChanges;
			ShelContextMenu = flags.ShelContextMenu;
			ShowDebugLogWindow = flags.ShowDebugLogWindow;
		}
	};
}
