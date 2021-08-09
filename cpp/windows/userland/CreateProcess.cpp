#include <windows.h>
#include <sddl.h>
#include <stdlib.h>
#include <userenv.h>


/******************************************************************************
* CreateProcess 和 ShellExecuteEx 函数一般用在普通应用程序中
*/
// 创建一个 notepad.exe 的进程
int CreateNotepadProcess(void)
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    if (!CreateProcess(L"C:\\Windows\\notepad.exe",
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


// 使用 shellexecute函数执行一个程序
int RunProgram(char *program)
{
    HINSTANCE hShellExecute = LoadLibraryA("Shell32.dll");
    if (hShellExecute)
    {
        SHELLEXECUTEINFOA sei;
        ZeroMemory(&sei, sizeof(sei));
        sei.cbSize = sizeof(sei);
        sei.lpVerb = "runas";
        sei.lpFile = program;
        sei.nShow = SW_NORMAL;
        if (ShellExecuteExA(&sei))
        {
            if (sei.hProcess)
            {
                CloseHandle(sei.hProcess);
            }
            return 0;
        }
    }
    return -1;
}




/******************************************************************************
* CreateProcessAsUser 函数一般用在服务中
*/
#include <TlHelp32.h>
// 获取 explorer.exe 进程id
DWORD GetExplorerProcessId(void)
{
    PROCESSENTRY32 pe;
    ZeroMemory(&pe, sizeof(PROCESSENTRY32));
    pe.dwSize = sizeof(PROCESSENTRY32);

    // 获取当前用户的sessionId
    DWORD dwSessionId = WTSGetActiveConsoleSessionId();
    if (dwSessionId == 0xFFFFFFFF)
    {
        return 0;
    }

    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if ( hSnapshot == INVALID_HANDLE_VALUE )
    {
        return 0;
    }

    if (Process32First(hSnapshot, &pe))
    {
        do
        {
            if ( _wcsicmp(pe.szExeFile, L"explorer.exe") == 0 )
            {
                DWORD dwExpSessId;
                // 获取 explorer的sessionId
                if ( ProcessIdToSessionId(pe.th32ProcessID, &dwExpSessId) )
                {
                    if ( dwExpSessId == dwSessionId )
                    {
                        CloseHandle(hSnapshot);
                        return pe.th32ProcessID;
                    }
                    else
                    {
                        continue;
                    }
                }
                else
                {
                    CloseHandle(hSnapshot);
                    return 0;
                }

            }

        } while ( Process32Next(hSnapshot, &pe) );
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

/**
 * @brief 创建一个和 explorer 同token的进程
 * 
 * @param lpApplicationName 
 * @param lpCommandLine 
 * @param dwCreationFlags 
 * @return BOOL 
 */
BOOL CreateANewProcessWithExplorerToken(WCHAR *lpApplicationName, WCHAR *lpCommandLine, DWORD dwCreationFlags, DWORD dwWaitTime)
{
    BOOL bRet = TRUE;

    HANDLE hToken = GetExplorerProcessUserToken();

    if ( hToken == NULL )
    {
        return FALSE;
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

    STARTUPINFO si;
    ZeroMemory(&si, sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);
    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

    // int dwCreateFlags = CREATE_UNICODE_ENVIRONMENT | CREATE_NEW_CONSOLE;

    if ( !CreateProcessAsUser(hNewToken, 
        lpApplicationName,
        lpCommandLine,
        NULL,
        NULL,
        FALSE,
        dwCreationFlags,
        NULL,
        NULL,
        &si,
        &pi) )
    {
        bRet = FALSE;
    }

    WaitForSingleObject(pi.hProcess, dwWaitTime);

    if (pi.hProcess)
    {
        CloseHandle(pi.hProcess);
    }
    if (pi.hThread)
    {
        CloseHandle(pi.hThread);
    }
    
    return bRet;
}

BOOL CreateANewProcessWithUAC(WCHAR *lpApplicationName, WCHAR *lpCommandLine, DWORD dwCreationFlags, DWORD dwWaitTime)
{
    BOOL bRet = TRUE;

    HANDLE hToken = GetExplorerProcessUserToken();

    if ( hToken == NULL )
    {
        return FALSE;
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

    STARTUPINFO si;
    ZeroMemory(&si, sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);
    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

    // int dwCreateFlags = CREATE_UNICODE_ENVIRONMENT | CREATE_NEW_CONSOLE;
    WCHAR uacApplicationName[MAX_PATH] = {0};
    WCHAR uacCommandLine[1024] = {0};

    // cmd /c <command> 执行完命令后就关闭
    wcscpy_s(uacApplicationName, MAX_PATH, L"C:\\Windows\\system32\\cmd.exe");
    swprintf(uacCommandLine, L"C:\\Windows\\system32\\cmd.exe /c \"%s\" %s", lpApplicationName, lpCommandLine);

    if ( !CreateProcessAsUser(hNewToken, 
        lpApplicationName,
        lpCommandLine,
        NULL,
        NULL,
        FALSE,
        dwCreationFlags,
        NULL,
        NULL,
        &si,
        &pi) )
    {
        bRet = FALSE;
    }

    WaitForSingleObject(pi.hProcess, dwWaitTime);

    if (pi.hProcess)
    {
        CloseHandle(pi.hProcess);
    }
    if (pi.hThread)
    {
        CloseHandle(pi.hThread);
    }
    
    return bRet;
}


bool CreateProcessWithAdmin(WCHAR *lpApplicationName, WCHAR *lpCommandLine)
{
   HANDLE hToken = NULL;
   HANDLE hTokenDup = NULL;


    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken))
    {
        return false;
    }


    if (!DuplicateTokenEx(hToken, TOKEN_ALL_ACCESS, NULL, SecurityAnonymous, TokenPrimary, &hTokenDup))
    {
        CloseHandle(hToken);
        return false;
    }

    STARTUPINFO si;
    LPVOID pEnv = NULL;
    DWORD dwSessionId = WTSGetActiveConsoleSessionId();
    PROCESS_INFORMATION pi = {0};

    ZeroMemory(&si, sizeof(STARTUPINFO));

    if (!SetTokenInformation(hTokenDup, TokenSessionId, &dwSessionId, sizeof(DWORD)))
    {
        CloseHandle(hToken);
        CloseHandle(hTokenDup);
        return false;
    }

    si.cb = sizeof(STARTUPINFO);
    si.lpDesktop = L"WinSta0\\Default";
    si.wShowWindow = SW_SHOW;
    si.dwFlags = STARTF_USESHOWWINDOW;

    if (!CreateEnvironmentBlock(&pEnv, hTokenDup, FALSE))
    {
        CloseHandle(hToken);
        CloseHandle(hTokenDup);
        return false;
    }

    if (!CreateProcessAsUser(hTokenDup, lpApplicationName, lpCommandLine, NULL, NULL, FALSE,
        NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE | CREATE_UNICODE_ENVIRONMENT,
        pEnv, NULL, &si, &pi))
    {
        CloseHandle(hToken);
        CloseHandle(hTokenDup);
        return false;
    }

    if (pEnv)
    {
        DestroyEnvironmentBlock(pEnv);
    }

    CloseHandle(hToken);
    CloseHandle(hTokenDup);
    return true;
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
    if (SetTokenIntegrityLevel(hNewToken, 0))
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
        L"C:\\Windows\\notepad.exe",
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



/*************************************************************************
* 一些可能用到的函数
*/
#include <wtsapi32.h>
// 获取当前登录用户的token
BOOL GetCurrentLogonUserToken(PHANDLE phToken)
{
    BOOL bRet = FALSE;
    DWORD dwCurSessionId = WTSGetActiveConsoleSessionId();
    if (WTSQueryUserToken(dwCurSessionId, phToken) == TRUE)
        return TRUE;

    DWORD err = GetLastError();
    HANDLE hProcessSnap = NULL;
    PROCESSENTRY32W pe32 = {0};
    DWORD dwSessionId = 0;

    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }
    pe32.dwSize = sizeof(PROCESSENTRY32W);
    if (Process32FirstW(hProcessSnap, &pe32))
    {
        do
        {
            if (_wcsicmp(pe32.szExeFile, L"explorer.exe") == 0)
            {
                ProcessIdToSessionId(pe32.th32ProcessID, &dwSessionId);
                if (dwSessionId == dwCurSessionId)
                {
                    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pe32.th32ProcessID);
                    err = GetLastError();
                    bRet = OpenProcessToken(hProcess, TOKEN_ALL_ACCESS, phToken);
                    CloseHandle(hProcessSnap);
                    return TRUE;
                }

            }
        } while (Process32NextW(hProcessSnap, &pe32));
    }
    else
    {
        bRet = FALSE;
    }
    CloseHandle(hProcessSnap);
    return bRet;
}


// 以当前用户token创建进程
BOOL CreateProcessAsLogonUser(HANDLE hToken, char * app, char * cmdline)
{
    HANDLE hTokenDup = NULL;
    if (!DuplicateTokenEx(hToken, MAXIMUM_ALLOWED, NULL, SecurityIdentification, TokenPrimary, &hTokenDup))
    {
        return FALSE;
    }

    DWORD dwSessionId = WTSGetActiveConsoleSessionId();

    if (!SetTokenInformation(hTokenDup, TokenSessionId, &dwSessionId, sizeof(DWORD)))
    {
        CloseHandle(hTokenDup);
        return FALSE;
    }

    STARTUPINFO si = {0};
    si.cb = sizeof(si);
    si.lpDesktop = "winsta0\\default";
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_SHOW;
    PROCESS_INFORMATION pi = {0};

    LPVOID lpEnvironment = NULL;
    DWORD dwCreateFlags = CREATE_UNICODE_ENVIRONMENT | CREATE_NEW_CONSOLE;

    if (!CreateEnvironmentBlock(&lpEnvironment, hTokenDup, FALSE))
    {
        CloseHandle(hTokenDup);
        return FALSE;
    }

    BOOL bRet = CreateProcessAsUser(hTokenDup, app, cmdline, NULL, NULL, FALSE, dwCreateFlags, lpEnvironment, NULL, &si, &pi);
    if (!bRet)
    {
        CloseHandle(hTokenDup);
        if (lpEnvironment != NULL)
        {
            DestroyEnvironmentBlock(lpEnvironment);
        }
        return FALSE;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    CloseHandle(hTokenDup);
    if (lpEnvironment != NULL)
    {
        DestroyEnvironmentBlock(lpEnvironment);
    }

    return TRUE;
}


// improve current process privilged
BOOL EnablePrivilege(LPCTSTR lpszPrivilege, BOOL bEnablePrivilege)
{
    BOOL bSuccess = FALSE;
    HANDLE hToken;
    if (GetCurrentLogonUserToken(&hToken) &&
        OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
    {
        TOKEN_PRIVILEGES tp = {0};
        if (LookupPrivilegeValue(NULL, lpszPrivilege, &tp.Privileges[0].Luid))
        {
            tp.PrivilegeCount = 1;
            tp.Privileges[0].Attributes = bEnablePrivilege ? SE_PRIVILEGE_ENABLED : 0;
            bSuccess = AdjustTokenPrivileges(hToken, FALSE, &tp, 0, NULL, NULL);
        }
        CloseHandle(hToken);
    }
    return bSuccess;
}