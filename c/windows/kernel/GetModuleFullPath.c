/** ntdll!_LDR_DATA_TABLE_ENTRY */
/**    +0x000 InLoadOrderLinks : _LIST_ENTRY [ 0xfffffa80`01e68280 - 0xfffffa80`01e60d40 ] */
/**    +0x010 InMemoryOrderLinks : _LIST_ENTRY [ 0xfffff880`04fbf000 - 0x00000000`00000390 ] */
/**    +0x020 InInitializationOrderLinks : _LIST_ENTRY [ 0x00000000`00000000 - 0x00000000`00000000 ] */
/**    +0x030 DllBase          : 0xfffff880`04fb8000 Void */
/**    +0x038 EntryPoint       : 0xfffff880`04fc3ecc Void */
/**    +0x040 SizeOfImage      : 0xf000 */
/**    +0x048 FullDllName      : _UNICODE_STRING "\SystemRoot\system32\DRIVERS\kbdclass.sys" */
/**    +0x058 BaseDllName      : _UNICODE_STRING "kbdclass.sys" */
/**    +0x068 Flags            : 0x49104000 */
/**    +0x06c LoadCount        : 1 */
/**    +0x06e TlsIndex         : 0 */
/**    +0x070 HashLinks        : _LIST_ENTRY [ 0x00000000`00000000 - 0x00000000`0001b4c5 ] */
/**    +0x070 SectionPointer   : (null)  */
/**    +0x078 CheckSum         : 0x1b4c5 */
/**    +0x080 TimeDateStamp    : 0 */
/**    +0x080 LoadedImports    : (null)  */
/**    +0x088 EntryPointActivationContext : 0xfffff8a0`0023b1c0 _ACTIVATION_CONTEXT */
/**    +0x090 PatchInformation : (null)  */
/**    +0x098 ForwarderLinks   : _LIST_ENTRY [ 0x4a5bc116`0000f000 - 0x00000000`0000000f ] */
/**    +0x0a8 ServiceTagLinks  : _LIST_ENTRY [ 0xfffffa80`01e66800 - 0x00000000`000000a0 ] */
/**    +0x0b8 StaticLinks      : _LIST_ENTRY [ 0x00630064`0062006b - 0x00730073`0061006c ] */
/**    +0x0c8 ContextInformation : 0x00730079`0073002e Void */
/**    +0x0d0 OriginalBase     : 0 */
/**    +0x0d8 LoadTime         : _LARGE_INTEGER 0x0 */

/*
 * ldr 结构体 driverobject中的driverseciton指向这个结构，通常可用此结构遍历内核
 * 此结构未导出，需要自己根据系统版本调试找出
 */
#ifdef _X86_
	typedef struct _LDR_DATA_TABLE_ENTRY
	{
		LIST_ENTRY InLoadOrderLinks;
		LIST_ENTRY InMemoryOrderLinks;
		LIST_ENTRY InInitializationOrderLinks;
		PVOID      DllBase;
		PVOID      EntryPoint;
		ULONG32    SizeOfImage;
		UNICODE_STRING FullDllName;
		UNICODE_STRING BaseDllName;
		UINT32   Unknow[17];

	}LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;

#else

	typedef struct _LDR_DATA_TABLE_ENTRY
	{
		LIST_ENTRY InLoadOrderLinks;
		LIST_ENTRY InMemoryOrderLinks;
		LIST_ENTRY InInitializationOrderLinks;
		PVOID      DllBase;
		PVOID      EntryPoint;
		ULONG32    SizeOfImage;
		UINT8      Unknow0[0x4];
		UNICODE_STRING FullDllName;
		UNICODE_STRING BaseDllName;
	}LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;
#endif


/**
* \require <ntddk.h> <wdm.h> _LDR_DATA_TABLE_ENTRY 结构声明
* \brief GetModuleFullPath 获取模块的全路径
* \param DriverObject 随意一个驱动模块的DriverObject
* \param moduleName in 模块名
* \param fileName out 返回模块的全路径
* \return 
*/
VOID GetModuleFullPath(PDRIVER_OBJECT DriverObject, PWCHAR moduleName, PWCHAR fileName)
{
	PLDR_DATA_TABLE_ENTRY ldr = (PLDR_DATA_TABLE_ENTRY)DriverObject->DriverSection;
	PLIST_ENTRY listEntry = ldr->InLoadOrderLinks.Flink;
	WCHAR buffer[256] = { 0 };

	UNICODE_STRING uModuleName;
	RtlInitUnicodeString(&uModuleName, moduleName);

	while (listEntry != &ldr->InLoadOrderLinks)
	{
		listEntry = listEntry->Flink;
		KdPrint(("KbdProtector : DriverName = %wZ, DriverBase = %x\n",
			&((PLDR_DATA_TABLE_ENTRY)listEntry)->BaseDllName, ((PLDR_DATA_TABLE_ENTRY)listEntry)->DllBase));

		if (((PLDR_DATA_TABLE_ENTRY)listEntry)->BaseDllName.Buffer != NULL && ((PLDR_DATA_TABLE_ENTRY)listEntry)->BaseDllName.Length != 0)
		{
			if (RtlCompareUnicodeString(&((PLDR_DATA_TABLE_ENTRY)listEntry)->BaseDllName, &uModuleName, TRUE) == 0)
			{
				wcscpy(buffer, ((PLDR_DATA_TABLE_ENTRY)listEntry)->FullDllName.Buffer);
				DEBUGLOG("KbdProtector : GetNtoskrnlFileName kernel module use file = %ws\r\n", buffer);
				break;
			}
		}

	}
}
