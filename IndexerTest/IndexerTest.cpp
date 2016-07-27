// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

#include "gtest/gtest.h"

#define SINGLE_THREAD

#include <locale.h>
#include <iostream>

GTEST_API_ int main(int argc, char **argv) {

    printf("Running main() from NTFSResderTest\n");

    setlocale(LC_ALL, ".OCP");

    testing::InitGoogleTest(&argc, argv);

    //testing::GTEST_FLAG(filter) = "*HandlesEqualCaseInsensitiveInput";

    auto res = RUN_ALL_TESTS();
    std::cin.get();
    return res;
}
