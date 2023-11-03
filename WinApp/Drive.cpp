#include "Drive.h"

Drive::Drive(std::wstring name, std::wstring filesystem, unsigned __int64 size) {
    this->name = name;
    this->filesystem = filesystem;
    this->size = size;
}

std::wstring Drive::getName() {
    return name;
}

void Drive::setName(std::wstring name) {
    this->name = name;
}

std::wstring Drive::getFilesystem() {
    return filesystem;
}

void Drive::setFilesystem(std::wstring filesystem) {
    this->filesystem = filesystem;
}

unsigned __int64 Drive::getSize() {
    return size;
}

void Drive::setSize(unsigned __int64 size) {
    this->size = size;
}