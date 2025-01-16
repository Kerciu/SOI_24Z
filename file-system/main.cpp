#include "fs.h"
#include <iostream>

int main() {
    FileSystem fs;

    std::cout << "Creating files:\n";
    if (fs.create("file1", 8) != FILE_CREATE_SUCCESS) {
        std::cout << "Error while creating 'file1'\n";
    } else {
        std::cout << "File 'file1' created successfully\n";
    }

    if (fs.create("file2", 64) != FILE_CREATE_SUCCESS) {
        std::cout << "Error while creating 'file2'\n";
    } else {
        std::cout << "File 'file2' created successfully\n";
    }

    if (fs.create("file3", 128) != FILE_CREATE_SUCCESS) {
        std::cout << "Error while creating 'file3'\n";
    } else {
        std::cout << "File 'file3' created successfully\n";
    }

    fs.displayState();

    std::cout << "\nOpening files:\n";
    if (fs.open("file1") == FILE_OPEN_SUCCESS) {
        std::cout << "File 'file1' opened successfully\n";
    } else {
        std::cout << "Error while opening 'file1'\n";
    }

    if (fs.open("file2") == FILE_OPEN_SUCCESS) {
        std::cout << "File 'file2' opened successfully\n";
    } else {
        std::cout << "Error while opening 'file2'\n";
    }

    fs.displayState();

    std::cout << "\nWriting to file 'file1':\n";
    char buffer1[] = "Hello, FileSystem!";
    if (fs.write("file1", buffer1, sizeof(buffer1)) == FILE_WRITE_SUCCESS) {
        std::cout << "Data written to 'file1'\n";
    } else {
        std::cout << "Error while writing to 'file1'\n";
    }

    fs.displayState();

    std::cout << "\nReading from file 'file1':\n";
    char read_buffer1[50] = {0};
    if (fs.read("file1", read_buffer1, sizeof(read_buffer1)) == FILE_READ_SUCCESS) {
        std::cout << "Data read from 'file1': " << read_buffer1 << "\n";
    } else {
        std::cout << "Error while reading from 'file1'\n";
    }

    fs.displayState();

    // Closing files
    std::cout << "\nClosing files:\n";
    if (fs.close("file1") == FILE_CLOSE_SUCCESS) {
        std::cout << "File 'file1' closed successfully\n";
    } else {
        std::cout << "Error while closing 'file1'\n";
    }

    fs.displayState();

    std::cout << "\nDeleting file:\n";
    if (fs.delete_("file3") == FILE_DELETE_SUCCESS) {
        std::cout << "File 'file3' deleted successfully\n";
    } else {
        std::cout << "Error while deleting 'file3'\n";
    }

    fs.displayState();

    return 0;
}
