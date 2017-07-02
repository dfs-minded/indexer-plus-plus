// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once

#include "FileInfoWrapper.h"
#include "CustomCacheDictionary.h"
// clang-format off

namespace CLIInterop
{

ref class FileInfoWrapperFactory
{
  public:
        explicit FileInfoWrapperFactory(int max_elements, IIconProvider^ icons_provider, IThumbnailProvider^ thumbs_provider);

        FileInfoWrapper^ GetFileInfoWrapper(const indexer_common::FileInfo* fi, int index);

		void UpdateCachedItems();

        void Clear();

  private:
	  // From FileInfoWrapper object UID to corresponding wrapper.
	  CustomCacheDictionary<FileInfoWrapper^>^ cache;
};

} // namespace CLIInterop