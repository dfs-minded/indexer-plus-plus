// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#pragma once
// A macros for defining constants of type char16_t*. Todo: remove when Visual Studio will support u"" macros.
#define __L__(str) reinterpret_cast<const char16_t*>(str)