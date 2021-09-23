#include <windows.h>
#include <stdio.h>

typedef struct _TestStruct_ {
    int a;
    char b;
    unsigned char *c;
} TestStruct, *PTestStruct;



// create share memory class
class CShareMemory
{
public:
    CShareMemory(const char *name, int size);
    ~CShareMemory();
    void *GetBuffer();

private:
    HANDLE m_hFileMap;
    void * m_pAddress;
    int m_size;
};

CShareMemory::CShareMemory(const char *name, int size)
{
    m_hFileMap = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, size, name);
    if (m_hFileMap == NULL)
    {
        printf("CreateFileMapping failed with error %d\n", GetLastError());
        return;
    }

    m_pAddress = MapViewOfFile(m_hFileMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    if (m_pAddress == NULL)
    {
        printf("MapViewOfFile failed with error %d\n", GetLastError());
        return;
    }

    m_size = size;
}

CShareMemory::~CShareMemory()
{
    if (m_pAddress)
    {
        UnmapViewOfFile(m_pAddress);
        m_pAddress = NULL;
    }

    if (m_hFileMap)
    {
        CloseHandle(m_hFileMap);
        m_hFileMap = NULL;
    }
}


void *CShareMemory::GetBuffer()
{
    return m_pAddress;
}


int main()
{
    CShareMemory shareMemory("test", sizeof(TestStruct));

    TestStruct *pTestStruct = (TestStruct *)shareMemory.GetBuffer();
}