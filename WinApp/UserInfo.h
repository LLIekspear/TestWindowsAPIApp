#pragma once
#include <string>
#include <vector>

class UserInfo {
private:
    std::wstring name = L"";
    std::wstring computer = L"";
    std::vector<std::wstring> groups;

public:
    UserInfo(std::wstring name, std::wstring computer, std::vector<std::wstring> groups);
    std::wstring getName();
    void setName(std::wstring name);
    std::wstring getComputer();
    void setComputer(std::wstring computer);
    std::vector<std::wstring> getGroups();
    void setGroups(std::vector<std::wstring> groups);
};
