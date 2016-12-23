// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "FileInfoWrapperFactory.h"

#include <algorithm>
#include <string>
#include <vector>
// clang-format off

namespace CLIInterop
{
	FileInfoWrapperFactory::FileInfoWrapperFactory(
		int max_elements, IIconProvider^ icons_provider, IThumbnailProvider^ thumbnails_provider) 
		: max_elements_(max_elements), next_time_(0)
	{
		cache_ = gcnew System::Collections::Generic::Dictionary<System::UInt32, FileInfoWrapper^>();
		FileInfoWrapper::IconProvider = icons_provider;
		FileInfoWrapper::ThumbnailProvider = thumbnails_provider;
	}

	FileInfoWrapper^ FileInfoWrapperFactory::GetFileInfoWrapper(const indexer_common::FileInfo* fi, int index)
	{
		FileInfoWrapper^ res;
		if (cache_->TryGetValue((System::Int64)fi, res))
		{
			res->LastTimeUsed = next_time_++;
			return res;
		}

		if (cache_->Count == max_elements_)
			RemoveOldElements();

		res = gcnew FileInfoWrapper(fi, index);
		res->LastTimeUsed = next_time_++;
		cache_->Add((System::Int64)fi, res);
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
		std::vector<std::pair<indexer_common::uint, indexer_common::uint>> tmp(cache_->Count);
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