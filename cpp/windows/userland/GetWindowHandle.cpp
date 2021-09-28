#include <windows.h>
// find windows handle for the specified process id
HWND GetWindowHwndByProcessId(DWORD dwProcessId)
{
    DWORD dwPid = 0;
    HWND hwndRet = NULL;

    HWND hwndWindow = GetTopWindow(0);
    while(hwndWindow)
    {
        dwPid = 0;

        // 通过窗口句柄获取进程id
        DWORD dwThreadId = GetWindowThreadProcessId(hwndWindow, &dwPid);

		char window_text[256] = {0};
		GetWindowTextA(hwndWindow, window_text, 256);

		if (strlen(window_text) == 0 || !IsWindowVisible(hwndWindow))
		{
			hwndWindow = GetNextWindow(hwndWindow, GW_HWNDNEXT);
			continue;
		}

        if (dwThreadId != 0)
        {
            if (dwPid == dwProcessId)
            {
                hwndRet = hwndWindow;
                break;
            }
        }

        // 获取下一个窗口句柄
        hwndWindow = GetNextWindow(hwndWindow, GW_HWNDNEXT);
    }


    // 上面获取的不一定是最上层窗口，需要通过GetParent获取
    HWND hwndParentWindow = NULL;

    while(hwndRet)
    {
        hwndParentWindow = GetParent(hwndRet);
        if (hwndParentWindow == NULL)
        {
            break;
        }

        hwndRet = hwndParentWindow;
    }

    return hwndRet;
}