#include <gtest/gtest.h>
#include <iostream>

int main(int argc, char **argv) {
    std::cout << "==========================================" << std::endl;
    std::cout << "  Family Budget System - Unit Tests" << std::endl;
    std::cout << "  Build Date: " << __DATE__ << " " << __TIME__ << std::endl;
    std::cout << "==========================================" << std::endl;
    
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}