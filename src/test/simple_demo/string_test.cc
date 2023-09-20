#include "core/tt_test.h"
#include <iostream>

// JUST_RUN_TEST(string, test)
TEST(string, test)
{
    std::string str1 = "hellllllllle";
    std::string str2 = "helllllllllo";

    for(int i = 0; i < 100000000; i++)
    {
        // if (strcmp("hellllllllle", "helllllllllo") == 0)
        // {
            
        // }
        if(strcmp(str1.c_str(), str2.c_str()) == 0)
        {
            
        }
        // if(str1 == str2)
        // {
            
        // }
    }
}
