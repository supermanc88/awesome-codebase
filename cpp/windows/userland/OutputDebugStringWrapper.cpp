#include <windows.h>
#include <stdlib.h>
#include <strsafe.h>
#include <yvals.h>

void TraceMsgW(const wchar_t *lpFormat, ...);

void TraceMsgA(const char *lpFormat, ...);

#if (defined UNICODE) || (defined _UNICODE)
#define TraceMsg TraceMsgW
#else
#define TraceMsg TraceMsgA
#endif

void TraceMsgW(const wchar_t *lpFormat, ...) 
{
    if(!lpFormat)
        return;

    wchar_t *pMsgBuffer = NULL;
    unsigned int iMsgBufCount = 0;

    va_list arglist;
    va_start(arglist, lpFormat);
    HRESULT hr = STRSAFE_E_INSUFFICIENT_BUFFER;
    while(hr == STRSAFE_E_INSUFFICIENT_BUFFER) {
        iMsgBufCount += 1024;
        if(pMsgBuffer) {
            free(pMsgBuffer);
            pMsgBuffer = NULL;
        }
        pMsgBuffer = (wchar_t*) malloc(iMsgBufCount * sizeof(wchar_t));
        if(!pMsgBuffer) {
            break;
        }

        hr = StringCchVPrintfW(pMsgBuffer, iMsgBufCount, lpFormat, arglist);
    }
    va_end(arglist);
    if(hr == S_OK) {
        OutputDebugStringW(pMsgBuffer);
    }

    if(pMsgBuffer) {
        free(pMsgBuffer);
        pMsgBuffer = NULL;
    }
}


void TraceMsgA(const char *lpFormat, ...) 
{
    if(!lpFormat)
        return;

    char *pMsgBuffer = NULL;
    unsigned int iMsgBufCount = 0;

    va_list arglist;
    va_start(arglist, lpFormat);
    HRESULT hr = STRSAFE_E_INSUFFICIENT_BUFFER;
    while(hr == STRSAFE_E_INSUFFICIENT_BUFFER) {
        iMsgBufCount += 1024;
        if(pMsgBuffer) {
            free(pMsgBuffer);
            pMsgBuffer = NULL;
        }
        pMsgBuffer = (char*) malloc(iMsgBufCount * sizeof(char));
        if(!pMsgBuffer) {
            break;
        }

        hr = StringCchVPrintfA(pMsgBuffer, iMsgBufCount, lpFormat, arglist);
    }
    va_end(arglist);
    if(hr == S_OK) {
        OutputDebugStringA(pMsgBuffer);
    }

    if(pMsgBuffer) {
        free(pMsgBuffer);
        pMsgBuffer = NULL;
    }
}