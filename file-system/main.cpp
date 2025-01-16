#include "fs.h"
#include <iostream>

int main() 
{
    FileSystem fs;
    if (fs.create("file", 8) != FILE_CREATE_SUCCESS) {
        std::cout << "Error while creating file\n";
    }
    if (fs.create("next_file", 64) != FILE_CREATE_SUCCESS) {
        std::cout << "Error while creating file\n";
    }
    if (fs.create("file3", 128) != FILE_CREATE_SUCCESS) {
        std::cout << "Error while creating file\n";
    }

    fs.displayState();


    return 0;
}