#include <ntddk.h>
#include <wdm.h>



NTSTATUS
DriverEntry (
    _In_ PDRIVER_OBJECT DriverObject,
    _In_ PUNICODE_STRING RegistryPath
    )
{
	NTSTATUS status = STATUS_SUCCESS;

	ANSI_STRING ansiStr;
	RtlInitAnsiString( &ansiStr, "ansiString" );

	UNICODE_STRING unicodeStr;
	RtlInitUnicodeString( &unicodeStr, L"unicodeString" );


	KdPrint( ("%Z\n", &ansiStr) );
	KdPrint( ("%wZ\n", &unicodeStr) );

	return status;
}
