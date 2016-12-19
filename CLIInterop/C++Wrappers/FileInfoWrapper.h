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
		System::Windows::Media::Imaging::BitmapSource^ GetThumbnail(System::String^ fileName);
	};

	public interface class IIconProvider
	{
		System::Windows::Media::Imaging::BitmapSource^ GetIcon(System::String^ fileName);
	};

	public ref class FileInfoWrapper : public System::ComponentModel::INotifyPropertyChanged
	{
	public:
		property System::UInt32 ID;
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

		FileInfoWrapper() {}

		FileInfoWrapper(const FileInfo* const fi, int index);


		virtual event System::ComponentModel::PropertyChangedEventHandler^ PropertyChanged;

		virtual void OnPropertyChanged(System::String^ propertyName);

	private:

		const FileInfo* fi;

		System::String^ mimeType;

		System::Windows::Media::Imaging::BitmapSource^ thumbnail;

		void LoadType();

		void LoadThumbnail();
	};
}