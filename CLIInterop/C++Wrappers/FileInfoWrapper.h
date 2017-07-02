// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include "FileInfo.h"
#include "typedefs.h"
// clang-format off

namespace CLIInterop 
{
	public interface class IThumbnailProvider
	{
		// Asynchronyously loads thumbnail and via dispatcher using |setBitmapSource| callback assingns
		// loaded thumbnail to target FileInfoWrapper property.
		void GetThumbnail(System::UInt64 fileUid, System::String^ filename,
						  System::Action<System::Windows::Media::Imaging::BitmapSource^>^ setBitmapSource);

		void OnUserJumpedToOtherPlace();
	};

	public interface class IIconProvider
	{
		System::Windows::Media::Imaging::BitmapSource^ GetIcon(System::String^ fileName);
	};

	public ref class FileInfoWrapper : public System::ComponentModel::INotifyPropertyChanged
	{
	public:
		property System::UInt64 UID;
		property System::String^ Name;
		property System::String^ FullName;
		property System::String^ Path;
		property System::DateTime^ DateCreated;
		property System::DateTime^ DateModified;
		property System::DateTime^ DateAccessed;
		property System::Int32 Size;
		property System::String^ Extension;
		property System::String^ Type
		{
			virtual System::String^ get() { return mimeType; }
			virtual void set(System::String^ val) {	mimeType = val; OnPropertyChanged("Type"); }
		}


		static IThumbnailProvider^ ThumbnailProvider;
		static IIconProvider^ IconProvider;

		property System::Windows::Media::Imaging::BitmapSource^ Thumbnail
		{
			virtual System::Windows::Media::Imaging::BitmapSource^ get() { return thumbnail; }
			virtual void set(System::Windows::Media::Imaging::BitmapSource^  val)
			{
				thumbnail = val;
				OnPropertyChanged("Thumbnail");
			}
		}

		int Index;
		unsigned int LastTimeUsed;

		// Rereads properties which could be changed from the underlying FileInfo object |fi|.
		void Update();

		void SetTumbnail(System::Windows::Media::Imaging::BitmapSource^ newThumbnail)
		{
			Thumbnail = newThumbnail;
		}

		FileInfoWrapper() {}

		FileInfoWrapper(const indexer_common::FileInfo* const fi, int index);

		static System::UInt64 GetFileUID(System::UInt32 fileInfoID, char driveLetter);

		virtual event System::ComponentModel::PropertyChangedEventHandler^ PropertyChanged;

		virtual void OnPropertyChanged(System::String^ propertyName);

	private:

		const indexer_common::FileInfo* fi;

		System::String^ mimeType;

		System::Windows::Media::Imaging::BitmapSource^ thumbnail;

		void LoadType();
	};
}