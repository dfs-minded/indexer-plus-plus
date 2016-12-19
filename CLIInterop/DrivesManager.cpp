// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "DrivesManager.h"
#using <System.Core.dll>

using namespace System::Collections::Generic;
//using namespace System::Management;
//using namespace System::Threading::Tasks;
// clang-format off

namespace CLIInterop 
{

	static DrivesManager::DrivesManager()
	{
		//WatchEvents();
		//selectedDrives = gcnew System::Collections::Generic::List<System::Char>();
	}

	DrivesManager::~DrivesManager()
	{
		//InsertEventWatcher->Stop();
		//delete InsertEventWatcher;

		//RemoveEventWatcher->Stop();
		//delete RemoveEventWatcher;
	}

	List<DriveInfo^>^ DrivesManager::GetDrives()
	{
		auto res = gcnew List<DriveInfo^>();
		auto drivers = System::IO::DriveInfo::GetDrives();

		for each(System::IO::DriveInfo^ di in drivers)
		{
			if (di->IsReady && di->DriveFormat == "NTFS")
				res->Add(gcnew DriveInfo(di->VolumeLabel, static_cast<char>(di->Name[0])));
		}
		return res;
	}

	//
	//void DrivesManager::WatchEvents()
	//{
	//	Task::Factory->StartNew(gcnew System::Action(WaitForInsertEvent)); // uncomment these lines prevent application from Shutdown 
	//	Task::Factory->StartNew(gcnew System::Action(WaitForRemoveEvent)); // and throw some RCW caller first-chance exception
	//}
	//
	//void DrivesManager::WaitForInsertEvent() 
	//{
	//	WqlEventQuery^ query = gcnew WqlEventQuery("SELECT * FROM Win32_VolumeChangeEvent WHERE EventType = 2");
	//	InsertEventWatcher = gcnew ManagementEventWatcher(query);
	//	InsertEventWatcher->EventArrived += gcnew EventArrivedEventHandler(DeviceInsertedEvent);
	//	InsertEventWatcher->Start();
	//	InsertEventWatcher->WaitForNextEvent();
	//}
	//
	//void DrivesManager::WaitForRemoveEvent() 
	//{	
	//	WqlEventQuery^ query = gcnew WqlEventQuery("SELECT * FROM Win32_VolumeChangeEvent WHERE EventType = 3");
	//	RemoveEventWatcher = gcnew ManagementEventWatcher(query);
	//	RemoveEventWatcher->EventArrived += gcnew EventArrivedEventHandler(DeviceRemovedEvent);
	//	RemoveEventWatcher->Start();
	//	RemoveEventWatcher->WaitForNextEvent();
	//}
	//
	//DriveInfo^ DrivesManager::GetDriveInfo(char driveName)
	//{
	//	DriveInfo^ res;
	//	for each(System::IO::DriveInfo^ di in System::IO::DriveInfo::GetDrives())
	//	{
	//		if (di->IsReady && di->Name[0] == driveName)
	//			res = gcnew DriveInfo(di->VolumeLabel, driveName);
	//	}
	//	return res;
	//}
	//
	//void DrivesManager::DeviceInsertedEvent(System::Object^ sender, EventArrivedEventArgs^ e)
	//{
	//	auto mbo = e->NewEvent;
	//	char driveName = ((System::String^)mbo->Properties["DriveName"]->Value)[0];
	//	delete mbo;
	//	
	//	OnDeviceInserted(GetDriveInfo(driveName));
	//}
	//
	//void DrivesManager::DeviceRemovedEvent(System::Object^ sender, EventArrivedEventArgs^ e)
	//{
	//	auto mbo = e->NewEvent;
	//	char driveName = ((System::String^)mbo->Properties["DriveName"]->Value)[0];
	//	delete mbo;
	//
	//	OnDeviceRemoved(driveName);
	//}      
	//
	//void DrivesManager::AddSelectedDrive(System::Char drive)
	//{
	//	selectedDrives->Add(drive);
	//	OnDriveSelectedChanged(drive, true);
	//}
	//
	//void DrivesManager::RemoveSelectedDrive(System::Char drive)
	//{
	//	selectedDrives->Remove(drive);
	//	OnDriveSelectedChanged(drive, false);
	//}
	//
	//bool DrivesManager::IsDriveSelected(System::Char drive)
	//{
	//	return selectedDrives->Contains(drive);
	//}
}