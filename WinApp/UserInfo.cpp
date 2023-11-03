#include "UserInfo.h"

UserInfo::UserInfo(std::wstring name, std::wstring computer, std::vector<std::wstring> groups) {
	this->name = name;
	this->computer = computer;
	this->groups = groups;
}

std::wstring UserInfo::getName() {
	return name;
}

void UserInfo::setName(std::wstring name) {
	this->name = name;
}

std::wstring UserInfo::getComputer() {
	return computer;
}

void UserInfo::setComputer(std::wstring computer) {
	this->computer = computer;
}

std::vector<std::wstring> UserInfo::getGroups() {
	return groups;
}

void UserInfo::setGroups(std::vector<std::wstring> groups) {
	this->groups = groups;
}
