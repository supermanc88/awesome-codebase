/**
 * \brief ZwQuerySystemInformation 这个也是未声明的函数，用来获取系统信息
 * \param SystemInformationClass
 * \param SystemInformation
 * \param SystemInformationLength
 * \param ReturnLength
 * \return 
 */
NTSYSAPI
NTSTATUS
NTAPI
ZwQuerySystemInformation
(
ULONG	SystemInformationClass,
PVOID	SystemInformation,
ULONG	SystemInformationLength,
PULONG	ReturnLength
);

// 这里是供ZwQuerySystemInformation第一个参数使用
typedef enum _SYSTEM_INFORMATION_CLASS {
	SystemBasicInformation,     // 0
	SystemProcessorInformation,    // 1
	SystemPerformanceInformation,    // 2
	SystemTimeOfDayInformation,    // 3
	SystemNotImplemented1,     // 4
	SystemProcessesAndThreadsInformation,   // 5
	SystemCallCounts,      // 6
	SystemConfigurationInformation,    // 7
	SystemProcessorTimes,     // 8
	SystemGlobalFlag,      // 9
	SystemNotImplemented2,     // 10
	SystemModuleInformation,     // 11
	SystemLockInformation,     // 12
	SystemNotImplemented3,     // 13
	SystemNotImplemented4,     // 14
	SystemNotImplemented5,     // 15
	SystemHandleInformation,     // 16
	SystemObjectInformation,     // 17
	SystemPagefileInformation,     // 18
	SystemInstructionEmulationCounts,    // 19
	SystemInvalidInfoClass1,     // 20
	SystemCacheInformation,     // 21
	SystemPoolTagInformation,     // 22
	SystemProcessorStatistics,     // 23
	SystemDpcInformation,     // 24
	SystemNotImplemented6,     // 25
	SystemLoadImage,      // 26
	SystemUnloadImage,     // 27
	SystemTimeAdjustment,     // 28
	SystemNotImplemented7,     // 29
	SystemNotImplemented8,     // 30
	SystemNotImplemented9,     // 31
	SystemCrashDumpInformation,    // 32
	SystemExceptionInformation,    // 33
	SystemCrashDumpStateInformation,    // 34
	SystemKernelDebuggerInformation,    // 35
	SystemContextSwitchInformation,    // 36
	SystemRegistryQuotaInformation,    // 37
	SystemLoadAndCallImage,     // 38
	SystemPrioritySeparation,     // 39
	SystemNotImplemented10,     // 40
	SystemNotImplemented11,     // 41
	SystemInvalidInfoClass2,     // 42
	SystemInvalidInfoClass3,     // 43
	SystemTimeZoneInformation,     // 44
	SystemLookasideInformation,    // 45
	SystemSetTimeSlipEvent,     // 46
	SystemCreateSession,     // 47
	SystemDeleteSession,     // 48
	SystemInvalidInfoClass4,     // 49
	SystemRangeStartInformation,    // 50
	SystemVerifierInformation,     // 51
	SystemAddVerifier,     // 52
	SystemSessionProcessesInformation    // 53
} SYSTEM_INFORMATION_CLASS;

typedef struct _SYSTEM_PROCESS_INFORMATION {
	ULONG NextEntryOffset;
	ULONG NumberOfThreads;
	LARGE_INTEGER SpareLi1;
	LARGE_INTEGER SpareLi2;
	LARGE_INTEGER SpareLi3;
	LARGE_INTEGER CreateTime;
	LARGE_INTEGER UserTime;
	LARGE_INTEGER KernelTime;
	UNICODE_STRING ImageName;
	KPRIORITY BasePriority;
	HANDLE UniqueProcessId;
	HANDLE InheritedFromUniqueProcessId;
	ULONG HandleCount;
	ULONG SessionId;
	ULONG_PTR PageDirectoryBase;
	SIZE_T PeakVirtualSize;
	SIZE_T VirtualSize;
	ULONG PageFaultCount;
	SIZE_T PeakWorkingSetSize;
	SIZE_T WorkingSetSize;
	SIZE_T QuotaPeakPagedPoolUsage;
	SIZE_T QuotaPagedPoolUsage;
	SIZE_T QuotaPeakNonPagedPoolUsage;
	SIZE_T QuotaNonPagedPoolUsage;
	SIZE_T PagefileUsage;
	SIZE_T PeakPagefileUsage;
	SIZE_T PrivatePageCount;
	LARGE_INTEGER ReadOperationCount;
	LARGE_INTEGER WriteOperationCount;
	LARGE_INTEGER OtherOperationCount;
	LARGE_INTEGER ReadTransferCount;
	LARGE_INTEGER WriteTransferCount;
	LARGE_INTEGER OtherTransferCount;
} SYSTEM_PROCESS_INFORMATION, *PSYSTEM_PROCESS_INFORMATION;


/**
 * \require <ntddk.h> <wdm.h> and declare ZwQuerySystemInformation
 * \brief ChmGetPidByProcessName 通过进程名获取pid
 * \param processName
 * \return 
 */
ULONG ChmGetPidByProcessName(PWCHAR processName)
{
	ULONG pid;
	UNICODE_STRING uProcessName;
	NTSTATUS status = STATUS_UNSUCCESSFUL;

	RtlInitUnicodeString(&uProcessName, processName);

	PVOID buffer = NULL;
	ULONG bufferSize = 0x1000;
	ULONG retSize;

	do
	{
		buffer = ExAllocatePool(NonPagedPool, bufferSize);

		// 由于bufferSize的长度可能不够，需要根据返回的retSize重新分配大小
		// 这里获取了所以进程的信息
		status = ZwQuerySystemInformation(SystemProcessesAndThreadsInformation,
				buffer,
				bufferSize,
				&retSize);

		 if ( !NT_SUCCESS(status) )
		 {
			ExFreePool(buffer);
			bufferSize = retSize;
		 }
	
	} while (status == STATUS_INFO_LENGTH_MISMATCH);

	PSYSTEM_PROCESS_INFORMATION processInfo = (PSYSTEM_PROCESS_INFORMATION)buffer;

	while (TRUE)
	{
		if (processInfo->ImageName.Buffer != NULL)
		{
			if (RtlCompareUnicodeString(&uProcessName, &processInfo->ImageName, TRUE) == 0 &&
					processInfo->SessionId != 0)
			{
				pid = (ULONG)processInfo->UniqueProcessId;
				goto out;
			}
		
		}
		if (processInfo->NextEntryOffset == 0)
		{
			pid = 0;
			goto out;
		}

		processInfo = (PSYSTEM_PROCESS_INFORMATION)((ULONG_PTR)processInfo + processInfo->NextEntryOffset);
	}

out:
	KdPrint((PROJECT_NAME "%s ProcessName = [%ws], pid = [%d]\n", __FUNCTION__, processName, pid));
	ExFreePool(buffer);
	return pid;
}
