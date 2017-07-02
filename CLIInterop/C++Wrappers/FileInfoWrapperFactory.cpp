// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "FileInfoWrapperFactory.h"

#include <string>
// clang-format off

namespace CLIInterop
{
	FileInfoWrapperFactory::FileInfoWrapperFactory(int max_elements, IIconProvider^ icons_provider, 
												   IThumbnailProvider^ thumbnails_provider)
		: cache(gcnew CustomCacheDictionary<FileInfoWrapper^>(max_elements)) 
	{
		FileInfoWrapper::IconProvider = icons_provider;
		FileInfoWrapper::ThumbnailProvider = thumbnails_provider;
	}

	FileInfoWrapper^ FileInfoWrapperFactory::GetFileInfoWrapper(const indexer_common::FileInfo* fi, int index)
	{
		auto uid = FileInfoWrapper::GetFileUID(fi->ID, fi->DriveLetter);
		FileInfoWrapper^ res = cache->TryGetValue(uid);
		if (res == nullptr)
		{
			res = gcnew FileInfoWrapper(fi, index);
			cache->Add(uid, res);
		}
		
		return res;
	}

	void UpdateCachedItem(FileInfoWrapper^ val)
	{
		val->Update();
	}

	void FileInfoWrapperFactory::UpdateCachedItems()
	{
		CustomCacheDictionary<FileInfoWrapper^>::MyUnaryFunc^ updateFunc = 
			gcnew CustomCacheDictionary<FileInfoWrapper^>::MyUnaryFunc(UpdateCachedItem);

		cache->ForEach(updateFunc);
	}

	void FileInfoWrapperFactory::Clear()
	{
		cache->Clear();
	}
}