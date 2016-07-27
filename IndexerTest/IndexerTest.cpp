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
