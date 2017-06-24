// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "FileInfoWrapper.h"

#include <string.h>
#include <string>

#include "FileInfoHelper.h"

#include "InteropHelper.h"

using namespace System::Threading;
using namespace System::Threading::Tasks;
using namespace System::ComponentModel;

using namespace indexer_common;
// clang-format off

namespace CLIInterop
{
	FileInfoWrapper::FileInfoWrapper(const FileInfo* const fi, int index) 
		: thumbnail(nullptr), mimeType(System::String::Empty)
	{
		this->fi = fi;
		ID		 = fi->ID;
		Name	 = Helper::ToSystemString(fi->GetName());
	
		auto path = FileInfoHelper::GetPath(*fi, false);
		Path = Helper::ToSystemString(path.get());

		auto fullName = FileInfoHelper::GetPath(*fi, true);
		FullName = Helper::ToSystemString(fullName.get());
		
		Extension = Helper::ToSystemString(FileInfoHelper::GetExtension(*fi));		

		// TODO starting too many threads via Task::Factory slows down performance.
		//Task^ typeLoadingTask = Task::Factory->StartNew(gcnew System::Action(this, &FileInfoWrapper::LoadType));

		LoadType();

		Thumbnail = FileInfoWrapper::IconProvider->GetIcon(FullName);
	
		FileInfoWrapper::ThumbnailProvider->GetThumbnail(FullName, 
			gcnew System::Action<System::Windows::Media::Imaging::BitmapSource^>(this, &FileInfoWrapper::SetTumbnail));

		Size = fi->SizeReal;

		DateCreated	 = Helper::UnixTimeSecondsToDateTime(fi->CreationTime);
		DateModified = Helper::UnixTimeSecondsToDateTime(fi->LastWriteTime);
		DateAccessed = Helper::UnixTimeSecondsToDateTime(fi->LastAccessTime);

		Index = index;
		LastTimeUsed = 0;
	}

	void FileInfoWrapper::Update()
	{
		Name = Helper::ToSystemString(fi->GetName());

		auto path = FileInfoHelper::GetPath(*fi, false);
		Path = Helper::ToSystemString(path.get());

		auto fullName = FileInfoHelper::GetPath(*fi, true);
		FullName = Helper::ToSystemString(fullName.get());

		Extension = Helper::ToSystemString(FileInfoHelper::GetExtension(*fi));

		Type = System::String::Empty;
		Task^ typeLoadingTask = Task::Factory->StartNew(gcnew System::Action(this, &FileInfoWrapper::LoadType));

		Size = fi->SizeReal;

		DateModified = Helper::UnixTimeSecondsToDateTime(fi->LastWriteTime);
		DateAccessed = Helper::UnixTimeSecondsToDateTime(fi->LastAccessTime);

		OnPropertyChanged("");
	}

	void FileInfoWrapper::OnPropertyChanged(System::String^ propertyName)
	{
		PropertyChanged(this, gcnew PropertyChangedEventArgs(propertyName));
	}

	void FileInfoWrapper::LoadType()
	{
		if (Type != System::String::Empty) return;

		Type = Helper::ToSystemString(FileInfoHelper::GetType(*fi));

		//auto state = Thread::CurrentThread->ApartmentState;
		//WriteToOutput(METHOD_METADATA + wstring(L"LoadType ApartmentState = ") + (state == ApartmentState::STA ? L"STA" : L"MTA"));
	}
}
