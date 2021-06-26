#include <ntddk.h>

#define PTDBG_TRACE_ROUTINES            0x00000001
#define PTDBG_TRACE_OPERATION_STATUS    0x00000002

ULONG gTraceFlags = 0;

#ifndef FlagOn
#define FlagOn(_F,_SF)        ((_F) & (_SF))
#endif

/**
* \brief PT_DBG_PRINT 分级打印调试日志，通过FlagOn 按位与来判断级别
* \param _dbgLevel
* \param _string
*/
#define PT_DBG_PRINT( _dbgLevel, _string )          \
    (FlagOn(gTraceFlags,(_dbgLevel)) ?              \
        DbgPrint _string :                          \
        ((int)0))


NTSTATUS
DriverEntry (
    _In_ PDRIVER_OBJECT DriverObject,
    _In_ PUNICODE_STRING RegistryPath
    )
{
	NTSTATUS status = STATUS_SUCCESS;
	PT_DBG_PRINT( PTDBG_TRACE_ROUTINES,
				  ("FileBackup!DriverEntry: Entered\n") );

	return status;
}
