#include <iostream>
#include <iomanip>
#include "src/attack/ProcessFinder.h"

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  ProcessFinder Test" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;

    // ============================================
    // TEST 1: Get all running processes
    // ============================================
    std::cout << "[*] Getting all running processes..." << std::endl;
    std::cout << std::endl;

    auto processes = ProcessFinder::GetAllProcesses();

    if (processes.empty()) {
        std::cout << "[-] No processes found!" << std::endl;
        return 1;
    }

    std::cout << "[+] Found " << processes.size() << " processes" << std::endl;
    std::cout << std::endl;

    // Display first 20 processes
    std::cout << "First 20 processes:" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "  PID     | Process Name" << std::endl;
    std::cout << "----------------------------------------" << std::endl;

    int count = 0;
    for (const auto& proc : processes) {
        if (count >= 20) {
            std::cout << "... and " << (processes.size() - 20) << " more" << std::endl;
            break;
        }
        std::cout << "  " << std::setw(6) << proc.first << "   | " << proc.second << std::endl;
        count++;
    }

    std::cout << std::endl;

    // ============================================
    // TEST 2: Find specific processes
    // ============================================
    std::cout << "[*] Looking for specific processes..." << std::endl;
    std::cout << std::endl;

    std::vector<std::string> testProcesses = {
        "notepad.exe",
        "calc.exe",
        "explorer.exe",
        "cmd.exe",
        "powershell.exe"
    };

    for (const auto& name : testProcesses) {
        DWORD pid = ProcessFinder::FindProcessByName(name);
        if (pid) {
            std::cout << "[+] Found " << name << " with PID: " << pid << std::endl;
        } else {
            std::cout << "[-] " << name << " not running" << std::endl;
        }
    }

    std::cout << std::endl;

    // ============================================
    // TEST 3: Search by user input
    // ============================================
    std::cout << "[*] Enter a process name to search: ";
    std::string searchName;
    std::getline(std::cin, searchName);

    if (!searchName.empty()) {
        DWORD pid = ProcessFinder::FindProcessByName(searchName);
        if (pid) {
            std::cout << "[+] Found " << searchName << " with PID: " << pid << std::endl;
        } else {
            std::cout << "[-] " << searchName << " not found" << std::endl;
        }
    }

    std::cout << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "[*] Test complete!" << std::endl;
    std::cout << "========================================" << std::endl;

    return 0;
}