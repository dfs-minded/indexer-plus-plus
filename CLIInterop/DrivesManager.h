// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include "DriveInfo.h"
// clang-format off

namespace CLIInterop 
{
	public ref class DrivesManager
	{
	public:
		static DrivesManager();
		~DrivesManager();
		static System::Collections::Generic::List<DriveInfo^>^ GetDrives();

		// TODo: revive this functionality.
		//~DrivesManager() {
		//	if (isDisposed)
		//		return;

		//	// dispose managed data
		//	//delete m_managedData;
		//	this->!DataContainer(); // call finalizer
		//	isDisposed = true;
		//}

		//// Finalizer
		//!DrivesManager() {
		//	// free unmanaged data
		//	//DataProvider::DeleteUnmanagedData(m_unmanagedData);
		//}


		//static void AddSelectedDrive(System::Char drive);
		//static void RemoveSelectedDrive(System::Char drive);
		//static bool IsDriveSelected(System::Char drive);
		//static event System::Action<System::Char, bool>^ OnDriveSelectedChanged;

		//static event System::Action<DriveInfo^>^ OnDeviceInserted;
		//static event System::Action<char>^ OnDeviceRemoved;
	//private:
		//static System::Management::ManagementEventWatcher^ InsertEventWatcher;
		//static System::Management::ManagementEventWatcher^ RemoveEventWatcher;
		//static System::Collections::Generic::List<System::Char>^ selectedDrives;
		//static DriveInfo^ GetDriveInfo(char driveName);
		//static void WatchEvents();
		//static void WaitForInsertEvent();
	 //   static void WaitForRemoveEvent();
	 //   static void DeviceInsertedEvent(System::Object^ sender, System::Management::EventArrivedEventArgs^ e);
	 //   static void DeviceRemovedEvent(System::Object^ sender, System::Management::EventArrivedEventArgs^ e);

	//	bool isDisposed;
	};
}