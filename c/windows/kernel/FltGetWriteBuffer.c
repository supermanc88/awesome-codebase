#include <fltKernel.h>

/**
* \require <fltKernel.h>
* \brief FltPreWrite 从Data中获取buffer地址
* \param Data
* \param FltObjects
* \param
* \return
*/
FLT_PREOP_CALLBACK_STATUS FltPreWrite (
    _Inout_ PFLT_CALLBACK_DATA Data,
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _Flt_CompletionContext_Outptr_ PVOID *CompletionContext
    )
{
	PFLT_IO_PARAMETER_BLOCK iopb = Data->Iopb;
	ULONG writeLen = iopb->Parameters.Write.Length;
	LARGE_INTEGER writeOffset = iopb->Parameters.Write.ByteOffset;

	PVOID writeBuffer = NULL;

	if (iopb->Parameters.Write.MdlAddress != NULL)
	{
		writeBuffer = MmGetSystemAddressForMdlSafe( iopb->Parameters.Write.MdlAddress, NormalPagePriority | MdlMappingNoExecute );
	}
	else
	{
		writeBuffer = iopb->Parameters.Write.WriteBuffer;
	}


	int i;
	for (i = 0; i < writeLen; i++)
	{
		KdPrint(("0x%02x ", *((UCHAR *)writeBuffer + 1) ));
		if ( i != 0 && i % 16 == 0 )
		{
			KdPrint(("\n"));
		}
	}
	return FLT_PREOP_SUCCESS_WITH_CALLBACK;
}
