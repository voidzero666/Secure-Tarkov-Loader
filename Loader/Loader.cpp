// Loader.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
//created by voidzero-development 15/06/2020

#include <iostream>
#include "anti_debug.hpp"
#include "windows.h"
#include "data.h"
#include <fstream>
#include <tlhelp32.h>
#include <string>
#include <direct.h>
#include "sha256.h"

DWORD disk_serialINT;
SYSTEM_INFO siSysInfo;
DWORD hwid;

bool  verified = false;

std::string users[] = {
    "73a12c4ce6c8f16a62ebad5d096f1cf2b55e81f32ab3128c0ef8ac772ea20363", //user1
    "73a12c4ce6c8f16a62ebad5d096f1cf2b55e81f32ab3128c0ef8ac772ea20363", //user2 etc.
};

DWORD WINAPI reCheck(LPVOID PARAMS) {
    while (true) {
        if (security::check_security() != security::internal::debug_results::none) {
            std::cout << std::hex << security::check_security() << std::endl;
            std::cout << "fuckyou" << std::endl;
            exit(0);
        }
        Sleep(100);
    }
    return true;
}

void writeDll(std::string path) {

    std::ofstream outfile(path + "Assembly-CSharp.dll", std::ofstream::binary);
    outfile.write(rawData, sizeof(rawData));
}

DWORD FindProcessId(const std::wstring& processName)
{
    PROCESSENTRY32 processInfo;
    processInfo.dwSize = sizeof(processInfo);

    HANDLE processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
    if (processesSnapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }

    Process32First(processesSnapshot, &processInfo);
    if (!processName.compare(processInfo.szExeFile))
    {
        CloseHandle(processesSnapshot);
        return processInfo.th32ProcessID;
    }

    while (Process32Next(processesSnapshot, &processInfo))
    {
        if (!processName.compare(processInfo.szExeFile))
        {
            CloseHandle(processesSnapshot);
            return processInfo.th32ProcessID;
        }
    }

    CloseHandle(processesSnapshot);
    return 0;
}

std::string get_str_between_two_str(const std::string& s, const std::string& start_delim, const std::string& stop_delim)
{
    unsigned first_delim_pos = s.find(start_delim);
    unsigned end_pos_of_first_delim = first_delim_pos + start_delim.length();
    unsigned last_delim_pos = s.find(stop_delim);

    return s.substr(end_pos_of_first_delim, last_delim_pos - end_pos_of_first_delim);
}

int main()
{
    if (security::check_security() != security::internal::debug_results::none) {
        exit(0);
    }
    else {
        std::cout << "Welcome to Escape from gopnik loader!\n";
    }

     //start antidebug thread
    CreateThread(0, 0, &reCheck, 0, 0, 0);

    // get volume info of c drive 
    GetVolumeInformationA("C:\\", NULL, NULL, &disk_serialINT, NULL, NULL, NULL, NULL);
    std::string hashed = sha256(std::to_string(disk_serialINT));

    std::cout << hashed << std::endl;

    //loop through array and check hwid
    for (std::string user : users) {
       // std::cout << user << std::endl;

        if (user == hashed) {
            verified = true;
        }
    }

    if (verified == false) {

        exit(0);
    }
    
    //make tarkov dev dir
    _mkdir("C:\\dev");

    char* pValue;
    size_t len;
    errno_t err = _dupenv_s(&pValue, &len, "APPDATA");

    std::string appdata_path = pValue;
    std::string cfg_path = appdata_path + "\\Battlestate Games\\BsgLauncher\\settings";

    //std::cout << cfg_path << std::endl;

    std::string line;
    std::ifstream myfile;
    myfile.open(cfg_path);
    getline(myfile, line);

    std::string start_delim = "gameRootDir\":\"";
    std::string stop_delim = "EFT (live)";
    
    std::string installroot = get_str_between_two_str(line, start_delim, stop_delim);
    std::string installfolder = installroot + "EFT (live)\\\\EscapeFromTarkov_Data\\\\Managed\\\\";
    std::string copyfolder = installroot + "EFT (live)\\\\EscapeFromTarkov_Data\\\\Managed\\\\Assembly-CSharp.dll";

    std::cout << "Tarkov Root Dir: " << installfolder << std::endl;

    CopyFileA(copyfolder.c_str(), "C:\\dev\\Assembly-CSharp.dll", TRUE);

    std::cout << "Please start escape from tarkov!" << std::endl;
 
    //look for tarkov or proc....
    while (FindProcessId(L"EscapeFromTarkov.exe") == 0) {
        Sleep(50);
    }
    std::cout << "found, injecting!" << std::endl;
   
    writeDll(installfolder);
    system("pause");
    exit(1);
}

