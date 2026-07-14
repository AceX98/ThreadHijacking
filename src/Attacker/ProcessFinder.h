#pragma once
#include<windows.h>
#include<tlhelp32.h>
#include<string>
#include<vector>

using namespace std;

class ProcessFinder{
    
    public:
       static DWORD FindProcessByName(const string& processName){

        HANDLE hSnap=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
        if(hSnap==INVALID_HANDLE_VALUE)
        {
            return 0;
        }

        PROCESSENTRY32 pe;
        pe.dwSize=sizeof(PROCESSENTRY32);
        
        if(!Process32First(hSnap,&pe))
        {
            CloseHandle(hSnap);
            return 0;
        }

        do{
            if(processName==pe.szExeFile)
            {
                CloseHandle(hSnap);
                return pe.th32ProcessID;
            }
        }while(Process32Next(hSnap,&pe));

        CloseHandle(hSnap);
        return 0;
    
    }
      
    static std::vector<std::pair<DWORD,string>> GetAllProcesses() {
     std::vector<std::pair<DWORD,string>>  processes;
     
    HANDLE hSnap=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
    if(hSnap==INVALID_HANDLE_VALUE)
        {
            return processes;
        }

    PROCESSENTRY32 pe;
    pe.dwSize=sizeof(PROCESSENTRY32);

    if(!Process32First(hSnap,&pe))
    {
        CloseHandle(hSnap);
        return processes;
    }

        do{
            processes.push_back({pe.th32ProcessID,pe.szExeFile});
        }while(Process32Next(hSnap,&pe));

    CloseHandle(hSnap);
    return processes;

    }
  
    };

