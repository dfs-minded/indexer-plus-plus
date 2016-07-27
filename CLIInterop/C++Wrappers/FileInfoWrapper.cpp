#include "FileInfoWrapper.h"

#include <string.h>
#include <string>

#include "FileInfoHelper.h"

#include "InteropHelper.h"

using namespace std;

using namespace System;
using namespace System::Threading;
using namespace System::Threading::Tasks;
using namespace System::ComponentModel;
// clang-format off

namespace CLIInterop
{
	FileInfoWrapper::FileInfoWrapper(const FileInfo* const fi, int index) :
		thumbnail(nullptr),
		mimeType(String::Empty)
	{
		this->fi = fi;
		ID		 = fi->ID;
		Name	 = InteropHelper::ToSystemString(fi->GetName());
	
		auto path = FileInfoHelper::GetPath(*fi, false);
		Path = InteropHelper::ToSystemString(path.get());

		auto fullName = FileInfoHelper::GetPath(*fi, true);
		FullName	  = InteropHelper::ToSystemString(fullName.get());
		
		Extension = InteropHelper::ToSystemString(FileInfoHelper::GetExtension(*fi));		

		// TODO starting too many threads via Task::Factory slows down performance.
		// Task^ typeLoadingTask = Task::Factory->StartNew(gcnew Action(this, &FileInfoWrapper::LoadType));

		LoadType();

		Thumbnail = FileInfoWrapper::IconProvider->GetIcon(FullName);

		// TODO: revive thumbnails.
		// Sync: Thumbnail = FileInfoWrapper::ThumbnailProvider->GetThumbnail(FullName);
		// auto uiContext = TaskScheduler::FromCurrentSynchronizationContext();
		//	Task^ thumbnailLoadingTask = Task::Factory->StartNew(gcnew Action(this, &FileInfoWrapper::LoadThumbnail),
		//	CancellationToken::None, TaskCreationOptions::None, uiContext);

		Size = fi->SizeReal;

		DateCreated	 = InteropHelper::UnixTimeSecondsToDateTime(fi->CreationTime);
		DateModified = InteropHelper::UnixTimeSecondsToDateTime(fi->LastWriteTime);
		DateAccessed = InteropHelper::UnixTimeSecondsToDateTime(fi->LastAccessTime);

		Index		 = index;
		LastTimeUsed = 0;
	}

	void FileInfoWrapper::Update()
	{
		Name = InteropHelper::ToSystemString(fi->GetName());

		auto path = FileInfoHelper::GetPath(*fi, false);
		Path = InteropHelper::ToSystemString(path.get());

		auto fullName = FileInfoHelper::GetPath(*fi, true);
		FullName = InteropHelper::ToSystemString(fullName.get());

		Extension = InteropHelper::ToSystemString(FileInfoHelper::GetExtension(*fi));

		Type = String::Empty;
		Task^ typeLoadingTask = Task::Factory->StartNew(gcnew Action(this, &FileInfoWrapper::LoadType));

		Size = fi->SizeReal;

		DateModified = InteropHelper::UnixTimeSecondsToDateTime(fi->LastWriteTime);
		DateAccessed = InteropHelper::UnixTimeSecondsToDateTime(fi->LastAccessTime);

		OnPropertyChanged("");
	}

	void FileInfoWrapper::OnPropertyChanged(String^ propertyName)
	{
		PropertyChanged(this, gcnew PropertyChangedEventArgs(propertyName));
	}

	void FileInfoWrapper::LoadType()
	{
		if (Type != String::Empty) return;

		Type = InteropHelper::ToSystemString(FileInfoHelper::GetType(*fi));

		//auto state = Thread::CurrentThread->ApartmentState;
		//WriteToOutput(METHOD_METADATA + wstring(L"LoadType ApartmentState = ") + (state == ApartmentState::STA ? L"STA" : L"MTA"));
	}

	void FileInfoWrapper::LoadThumbnail()
	{
		Thumbnail = FileInfoWrapper::ThumbnailProvider->GetThumbnail(FullName);
	}
}
