#pragma once
#include <string>

class Drive {
private:
    std::wstring name = L"";
    std::wstring filesystem = L"";
    unsigned __int64 size = 0;

public:
    Drive(std::wstring name, std::wstring filesystem, unsigned __int64 size);
    std::wstring getName();
    void setName(std::wstring name);
    std::wstring getFilesystem();
    void setFilesystem(std::wstring filesystem);
    unsigned __int64 getSize();
    void setSize(unsigned __int64 size);
};