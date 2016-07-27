// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include "FileInfoWrapper.h"
// clang-format off

namespace CLIInterop
{
	ref class FileInfoWrapperFactory
	{
		public:
            explicit FileInfoWrapperFactory(int max_elements, IIconProvider^ icons_provider, IThumbnailProvider^ thumbs_provider);

            FileInfoWrapper^ GetFileInfoWrapper(const FileInfo* fi, int index);

			void UpdateCachedItems();

            void Clear();

		private:

			void RemoveOldElements();

			int max_elements_;

			int next_time_;

			// From FileInfo object address to corresponding wrapper.
			System::Collections::Generic::Dictionary<System::Int64, FileInfoWrapper^>^ cache_;
	};
}