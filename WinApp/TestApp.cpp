#include <windows.h>
#include <iostream>
#include <lm.h>
#include <fstream>
#include <Shlwapi.h>
#include "Drive.h"
#include "UserInfo.h"
#pragma comment(lib, "Netapi32.lib")
#pragma comment(lib, "Shlwapi.lib")

std::vector<Drive> getLogicalDrives(std::string filename, int flag) {
    std::vector<Drive> drivesVector;
    DWORD drives = GetLogicalDrives();
    char letter = 'A';
    while (drives != 0) {
        if (drives & 1) {
            unsigned __int64 size; //размер тома в байтах
            WCHAR filesystem[MAX_PATH+1];
            std::wstring filesystemStr; //тип файловой системы
            std::wstring name=std::wstring(1, letter)+L":\\"; //имя тома 
            
            if (GetDiskFreeSpaceEx(name.c_str(), NULL, (PULARGE_INTEGER)&size, NULL)==0) {
                std::wcerr << L"Сбой в GetDiskFreeSpaceEx. У Вас достаточно прав? Ошибка: "<<GetLastError()<<"\n";
                //exit(1);
            }

            if (GetVolumeInformation(name.c_str(), NULL, 0, NULL, NULL, NULL, filesystem, sizeof(filesystem)) == 0) {
                std::wcerr << L"Не все запрошенные сведения были извлечены (GetVolumeInformation). Ошибка: "<<GetLastError()<<"\n";
            }
            filesystemStr = std::wstring(filesystem);
            Drive drive(name, filesystemStr, size);
            drivesVector.push_back(drive);
        }
        drives >>= 1;
        ++letter;
    }

    if (flag == 0) {
        std::wofstream out(filename, std::ios::app);
        out.imbue(std::locale("ru_RU.UTF-8"));
        for (int i = 0; i < drivesVector.size(); ++i) {
            if (out.is_open()) {
                out << L"Локальный диск: " << drivesVector.at(i).getName() << "\n";
                out << L"Размер тома: " << drivesVector.at(i).getSize() << L" байт" << "\n";
                out << L"Имя файловой системы тома: " << drivesVector.at(i).getFilesystem() << "\n\n";
            }
            else {
                //выбросить сообщение об ошибке? Или о том, что не получается открыть файл?
            }
        }
        out.close();
    }
    return drivesVector;
}

void getFilesAndInfo(std::vector<Drive> drives, std::string filename) {
    std::wofstream out(filename, std::ios::app);
    out.imbue(std::locale("ru_RU.UTF-8"));
    for (int i = 0; i < drives.size(); ++i) {
        WIN32_FIND_DATA findData;
        HANDLE hFind = INVALID_HANDLE_VALUE;
        hFind = FindFirstFile((drives.at(i).getName() + L"*").c_str(), &findData);

        int fileCount = 0;
        int dirCount = 0;
        while (FindNextFile(hFind, &findData) != 0) {
            if (findData.cFileName[0] != '.') {
                std::wstring filePath = drives.at(i).getName() + std::wstring(findData.cFileName);
                std::wstring creationTime;
                SYSTEMTIME sysTime;
                if (FileTimeToSystemTime(&findData.ftCreationTime, &sysTime) == 0) {
                    std::wcerr << L"Сбой в FileTimeToSystemTime. Ошибка: " << GetLastError() << "\n";
                    //exit(1);
                }
                creationTime = std::to_wstring(sysTime.wYear) + L"-" + std::to_wstring(sysTime.wMonth) + L"-" + std::to_wstring(sysTime.wDay) + L" " + std::to_wstring(sysTime.wHour) + L"-" + std::to_wstring(sysTime.wMinute);
                if (out.is_open()) {
                    out << L"Имя файла/папки: " << filePath << "\n";
                    out << L"Тип объекта: " << ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? L"Папка" : L"Файл") << "\n";
                }
                else {
                    //выбросить сообщение об ошибке? Или о том, что не получается открыть файл?
                }

                if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                    ++dirCount;
                }
                else {
                    ++fileCount;
                }
            }
        }
        if (out.is_open()) {
            out << L"Общее число папок: " << dirCount << "\n";
            out << L"Общее число файлов: " << fileCount << "\n\n";
        }
        else {
            //выбросить сообщение об ошибке? Или о том, что не получается открыть файл?
        }
        FindClose(hFind);
    }
    out.close();
}

void getInformation(std::string filename) {
    std::wofstream out(filename, std::ios::app);
    out.imbue(std::locale("ru_RU.UTF-8"));
    WCHAR username[MAX_PATH + 1];
    std::wstring usernameStr; //имя
    DWORD username_length = MAX_PATH + 1;
    WCHAR compName[MAX_PATH + 1];
    std::wstring compNameStr; //имя рабочей станции
    DWORD compName_length = MAX_PATH + 1;
    std::vector<std::wstring> groupsStr; //группы пользователя
    LOCALGROUP_USERS_INFO_0* groups = { 0 };
    DWORD entriesRead = 0;
    DWORD totalEntries = 0;

    if(GetUserName(username, &username_length)==0)
        std::wcerr << L"Сбой в GetUserName. Ошибка: " << GetLastError() << "\n";
    if(GetComputerName(compName, &compName_length)==0)
        std::wcerr << L"Сбой в GetComputerName. Ошибка: " << GetLastError() << "\n";

    usernameStr = std::wstring(username);
    compNameStr = std::wstring(compName);

    DWORD status = NetUserGetLocalGroups(NULL, username, 0, LG_INCLUDE_INDIRECT, reinterpret_cast<PBYTE*>(&groups), MAX_PREFERRED_LENGTH, &entriesRead, &totalEntries);
    if(status==ERROR_ACCESS_DENIED)
        std::wcerr << L"Сбой в NetUserGetLocalGroups. Ошибка: ERROR_ACCESS_DENIED - недостаточно прав." << "\n";
    else if (status == ERROR_INVALID_LEVEL)
        std::wcerr << L"Сбой в NetUserGetLocalGroups. Ошибка: ERROR_INVALID_LEVEL - неправильный уровень системного вызова." << "\n";
    else if (status == ERROR_INVALID_PARAMETER)
        std::wcerr << L"Сбой в NetUserGetLocalGroups. Ошибка: ERROR_INVALID_PARAMETER - неправильный параметр." << "\n";
    else if (status == ERROR_MORE_DATA)
        std::wcerr << L"Сбой в NetUserGetLocalGroups. Ошибка: ERROR_MORE_DATA - доступны дополнительные записи, необходим больший буфер." << "\n";
    else if (status == ERROR_NOT_ENOUGH_MEMORY)
        std::wcerr << L"Сбой в NetUserGetLocalGroups. Ошибка: ERROR_NOT_ENOUGH_MEMORY - недостаточно памяти для завершения операции." << "\n";
    else if (status == NERR_DCNotFound)
        std::wcerr << L"Сбой в NetUserGetLocalGroups. Ошибка: NERR_DCNotFound - не удалось найти контроллер домена." << "\n";
    else if (status == NERR_UserNotFound)
        std::wcerr << L"Сбой в NetUserGetLocalGroups. Ошибка: NERR_UserNotFound - не удалось найти пользователя." << "\n";

    for (int i = 0; i < entriesRead; ++i) {
        groupsStr.push_back(groups[i].lgrui0_name);
    }

    UserInfo userInfo(usernameStr, compNameStr, groupsStr);

    if (out.is_open()) {
        out << L"Имя пользователя, запустившего приложение: " << userInfo.getName() << "\n";
        out << L"Имя рабочей станции: " << userInfo.getComputer() << "\n";
        out << L"Список групп, в которые входит пользователь: " << "\n";
        for (int i = 0; i < userInfo.getGroups().size(); ++i) {
            if (i + 1 != userInfo.getGroups().size())
                out << userInfo.getGroups().at(i) << ", ";
            else
                out << userInfo.getGroups().at(i) << "\n\n";
        }
    }
    else {
        //выбросить сообщение об ошибке? Или о том, что неп получается открыть файл?
    }
    out.close(); 
}

int main() {
    try {
        std::wofstream out;
        out.imbue(std::locale("ru_RU.UTF-8"));
        std::vector<Drive> drives;
        std::string filename;
        int mode = 0;

        std::wcout.imbue(std::locale("rus_rus.866"));
        std::wcerr.imbue(std::locale("rus_rus.866"));

        std::wcout << L"Выберите функцию:\n" << L"1 - Перечисление локальных дисков рабочей станции, на которой приложение выполняется\n" << L"2 - Перечисление всех файлов и папок, располагаемых на каждом из логических томов\n" << L"3 - Сбор информации\n" << L"4 - Весь функционал сразу\n";
        std::cin >> mode;

        if (std::cin.fail()) {
            std::wcerr << L"Выбранная функция должна представлять число: 1,2,3,4.\n";
            system("pause");
            return 1;
        }
        if (mode > 4 || mode < 1) {
            std::wcerr << L"Выбранная функция должна представлять число: 1,2,3,4.\n";
            system("pause");
            return 1;
        }

        std::wcout << L"Введите название файла, в котором нужно сохранить полученную информацию. Формат: D:\\\\Programs\\\\data.txt \n";
        std::cin >> filename;
        std::wstring filenameW(filename.begin(), filename.end());
        if (PathIsRelative(filenameW.c_str())!=FALSE) {
            std::wcerr << L"Необходимо ввести полный путь до файла (абсолютный).\n";
            system("pause");
            return 1;
        }
        
        out.open(filename);
        if (out.is_open()) {
            //
        }
        else if(out.fail()) {
            std::wcerr << L"Ошибка при создании или при открытии файла. Перепроверьте корректность указанного названия файла или свои права в системе.\n";
            out.close();
            system("pause");
            return 1;
        }
        out.close();

        if(mode==1||mode==4)
            drives = getLogicalDrives(filename, 0);
        if(mode==3||mode==4) 
            getInformation(filename);
        if(mode==2||mode==4)
            drives = getLogicalDrives(filename, 1);
            getFilesAndInfo(drives, filename);
    }
    catch (const std::exception &error) {
        std::cerr << error.what();
    }
    system("pause");
    return 0;
}