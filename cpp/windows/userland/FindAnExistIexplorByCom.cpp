#include <windows.h>

#import   <mshtml.tlb>   //   Internet   Explorer   5  
#import   <shdocvw.dll>  

/**
 * @brief 通过Com接口获取一个打开的IE的控制指针
 * @return SHDocVw::IWebBrowser2* 
 */
SHDocVw::IWebBrowser2 * FindAnExistIeplore()
{
    SHDocVw::IWebBrowser2 * spBrowser = NULL;
    CoInitialize(NULL);
    SHDocVw::IShellWindowsPtr m_spSHWinds;
    CoCreateInstance(CLSID_ShellWindows, 
        NULL,  
        CLSCTX_LOCAL_SERVER, 
        IID_PPV_ARGS(&m_spSHWinds));

    long nCount = m_spSHWinds->GetCount();

    IDispatchPtr spDisp;

    for (long i = 0; i < nCount; i++)
    {
        _variant_t va(i, VT_I4);
        spDisp = m_spSHWinds->Item(va);

        SHDocVw::IWebBrowser2Ptr spBrowser2(spDisp);

        if (spBrowser2 != NULL)
        {
            MSHTML::IHTMLDocument2Ptr spDoc(spBrowser2->GetDocument());

            if (spDoc != NULL)
            {
                spBrowser = spBrowser2;
                break;
            }
        }
    }

    return spBrowser;
}