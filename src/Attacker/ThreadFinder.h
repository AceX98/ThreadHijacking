#pragma once
#include<windows.h>
#include<tlhelp32.h>
#include<vector>

using namespace std;

class ThreadFinder{

    public:
     static std::vector<DWORD> FindThreadsInProcess(DWORD targetPID)
     {
        std::vector<DWORD> threadIds;

        HANDLE hSnap=CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD,0);
        if(hSnap==INVALID_HANDLE_VALUE)
        {
            return threadIds;
        }
    THREADENTRY32 te;
    te.dwSize=sizeof(THREADENTRY32);
    
    if(!Thread32First(hSnap,&te))
    {
        CloseHandle(hSnap);
        return threadIds;
    }
     
    do {
        if(te.th32OwnerProcessID == targetPID)
        {
            threadIds.push_back(te.th32ThreadID);
        }
     
    }while(Thread32Next(hSnap,&te));
        CloseHandle(hSnap);
        return threadIds;
    
}
      static DWORD FindFirstThread(DWORD targetPID)
      {
        auto threads= FindThreadsInProcess(targetPID);
        if(threads.empty())
        {
            return 0;
        }
        return threads[0];
      }
    
      static int GetThreadCount(DWORD targetPID)
      {
         return static_cast<int>(FindThreadsInProcess(targetPID).size());
      }

};