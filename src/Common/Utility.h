#pragma once
#include<windows.h>
#include<string>
#include<iostream>

using namespace std;
namespace Utility
{
    inline string GetLastErrorString()
    {
        DWORD err=GetLastError();
        LPSTR msgBuffer=nullptr;

        FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,nullptr,err,MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),(LPSTR)&msgBuffer,0,nullptr);
        string msg=msgBuffer ? msgBuffer : "Unkown Error";
        LocalFree(msgBuffer);
        return msg;
    }
    inline void SetConsoleColor(WORD color)
    {
        HANDLE hConsole=GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole,color);
    }
    inline void ResetConsoleColor()
    {
        HANDLE hConsole=GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole,FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    }
    inline void PrintSuccess(const string& msg)
    {
        SetConsoleColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        cout<<" Success " << msg <<endl;
        ResetConsoleColor();
    }
    inline void PrintError(const string& msg)
    {
        SetConsoleColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
        cout<<" Error " << msg <<endl;
        ResetConsoleColor();
    }
    inline void PrintInfo(const string& msg)
    {
        SetConsoleColor(FOREGROUND_BLUE | FOREGROUND_INTENSITY);
        cout<<" Info " << msg <<endl;
        ResetConsoleColor();
    }
    inline void PrintWarning(const string& msg)
    {
        SetConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        cout<<" Warning " << msg <<endl;
        ResetConsoleColor();
    }
}