// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "FileInfoWrapperFactory.h" 

#include <algorithm>
#include <vector>
#include <string>
// clang-format off

using namespace System;
using namespace System::Collections::Generic;

namespace CLIInterop
{
	FileInfoWrapperFactory::FileInfoWrapperFactory(int max_elements, IIconProvider^ icons_provider, IThumbnailProvider^ thumbnails_provider) :
		max_elements_(max_elements),
		next_time_(0)
	{
		cache_ = gcnew Dictionary<Int64, FileInfoWrapper^>();
		FileInfoWrapper::IconProvider = icons_provider;
		FileInfoWrapper::ThumbnailProvider = thumbnails_provider;
	}

	FileInfoWrapper^ FileInfoWrapperFactory::GetFileInfoWrapper(const FileInfo* fi, int index)
	{
		FileInfoWrapper^ res;
		if (cache_->TryGetValue((Int64)fi, res))
		{
			res->LastTimeUsed = next_time_++;
			return res;
		}

		if (cache_->Count == max_elements_)
			RemoveOldElements();

		res = gcnew FileInfoWrapper(fi, index);
		res->LastTimeUsed = next_time_++;
		cache_->Add((Int64)fi, res);
		return res;
	}

	void FileInfoWrapperFactory::UpdateCachedItems()
	{
		for each (auto& id_to_wrapper in cache_)
			id_to_wrapper.Value->Update();
	}

	void FileInfoWrapperFactory::Clear()
	{
		cache_->Clear();
		next_time_ = 0;
	}

	void FileInfoWrapperFactory::RemoveOldElements()
	{
		// Copy |LastTimeUsed| and IDs to |tmp| array.
		std::vector<std::pair<uint, uint>> tmp(cache_->Count);
		int i = 0;
		for each (auto& kv in cache_)
		{
			tmp[i].first = kv.Value->LastTimeUsed;
			tmp[i].second = kv.Key;
			++i;
		}

		// Partially sort |tmp| array by finding median.
		std::nth_element(tmp.begin(), tmp.begin() + max_elements_/2, tmp.end());

		// Remove from |cache_| all elements that are earlier to median in |tmp|.
		auto it_median = tmp.begin() + max_elements_ / 2;

		for (auto it = tmp.begin(); it != it_median; ++it)
			cache_->Remove(it->second);
	}
}