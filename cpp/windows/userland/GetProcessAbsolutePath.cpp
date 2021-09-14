#include <windows.h>
#include <psapi.h>


// get process absolute path by process id
BOOL GetProcessAbsolutePath(DWORD dwPID, char *szPath, DWORD dwSize)
{
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwPID);
    if (hProcess == NULL)
        return FALSE;

    if (!GetModuleFileNameExA(hProcess, NULL, szPath, dwSize))
    {
        CloseHandle(hProcess);
        return FALSE;
    }

    CloseHandle(hProcess);
    return TRUE;
}