#include <windows.h>
#include <sddl.h>
#include <stdlib.h>


// 创建一个 notepad.exe 的进程
int CreateNotepadProcess(void)
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    if (!CreateProcess("C:\\Windows\\notepad.exe",
        NULL,
        NULL,
        NULL,
        FALSE,
        0,
        NULL,
        NULL,
        &si,
        &pi))
    {
        return -1;
    }
    if (pi.hProcess)
    {
        CloseHandle(pi.hProcess);
    }
    if (pi.hThread)
    {
        CloseHandle(pi.hThread);
    }
    return 0;
}

#include <TlHelp32.h>
// 获取 explorer.exe 进程id
DWORD GetExplorerProcessId(void)
{
    PROCESSENTRY32 pe;
    ZeroMemory(&pe, sizeof(pe));
    pe.dwSize = sizeof(pe);
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE)
    {
        return 0;
    }
    if (Process32First(hSnapshot, &pe))
    {
        do
        {
            if (_stricmp(pe.szExeFile, "explorer.exe") == 0)
            {
                CloseHandle(hSnapshot);
                return pe.th32ProcessID;
            }
        } while (Process32Next(hSnapshot, &pe));
    }
    CloseHandle(hSnapshot);
    return 0;
}


// 获取 explorer.exe 进程的用户token
HANDLE GetExplorerProcessUserToken(void)
{
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetExplorerProcessId());
    if (hProcess == NULL)
    {
        return NULL;
    }
    HANDLE hToken;
    if (!OpenProcessToken(hProcess, TOKEN_ALL_ACCESS, &hToken))
    {
        CloseHandle(hProcess);
        return NULL;
    }
    CloseHandle(hProcess);
    return hToken;
}


// 模仿当前用户创建一个新的进程
int CreateNewProcess(void)
{
    HANDLE hToken = GetExplorerProcessUserToken();
    if (hToken == NULL)
    {
        return -1;
    }
    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));
    if (!CreateProcessAsUser(hToken,
        "C:\\Windows\\notepad.exe",
        NULL,
        NULL,
        NULL,
        FALSE,
        0,
        NULL,
        NULL,
        &si,
        &pi))
    {
        return -1;
    }

    if (pi.hProcess)
    {
        CloseHandle(pi.hProcess);
    }
    if (pi.hThread)
    {
        CloseHandle(pi.hThread);
    }

    return 0;
}

// set token integrity level
int SetTokenIntegrityLevel(HANDLE hToken, int level)
{
    TOKEN_MANDATORY_LABEL tml;
    ZeroMemory(&tml, sizeof(tml));
    tml.Label.Attributes = SE_GROUP_INTEGRITY;
    tml.Label.Sid = NULL;
    char szIntegritySid[20] = {0};
    PSID pIntegritySid = NULL;
    BOOL ret;
    switch (level)
    {
        case 0:
        // low
            strcpy(szIntegritySid, "S-1-16-4096");
            break;
        case 1:
        // medium
            strcpy(szIntegritySid, "S-1-16-8192");
            break;
        case 2:
        // high
            strcpy(szIntegritySid, "S-1-16-12288");
            break;
        case 3:
        // system
            strcpy(szIntegritySid, "S-1-16-16384");
            break;
    }
    ret = ConvertStringSidToSidA(szIntegritySid, &pIntegritySid);
    if (!ret)
    {
        return -1;
    }
    tml.Label.Sid = pIntegritySid;

    DWORD cbSize = sizeof(tml);
    if (!SetTokenInformation(hToken, TokenIntegrityLevel, &tml, cbSize + GetLengthSid(pIntegritySid)))
    {
        return -1;
    }
    return 0;
}


// create a low integrity process
int CreateLowIntegrityProcess(void)
{
    HANDLE hToken = GetExplorerProcessUserToken();
    if (hToken == NULL)
    {
        return -1;
    }
    HANDLE hNewToken;
    if (!DuplicateTokenEx(hToken,
        TOKEN_ALL_ACCESS,
        NULL,
        SecurityImpersonation,
        TokenPrimary,
        &hNewToken))
    {
        return -1;
    }
    CloseHandle(hToken);
    if (!SetTokenIntegrityLevel(hNewToken, SECURITY_MANDATORY_LOW_RID))
    {
        CloseHandle(hNewToken);
        return -1;
    }
    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));
    if (!CreateProcessAsUser(hNewToken,
        "C:\\Windows\\notepad.exe",
        NULL,
        NULL,
        NULL,
        FALSE,
        0,
        NULL,
        NULL,
        &si,
        &pi))
    {
        CloseHandle(hNewToken);
        return -1;
    }
    if (pi.hProcess)
    {
        CloseHandle(pi.hProcess);
    }
    if (pi.hThread)
    {
        CloseHandle(pi.hThread);
    }
    CloseHandle(hNewToken);
    return 0;
}