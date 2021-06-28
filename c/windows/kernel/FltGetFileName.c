#include <fltKernel.h>

/**
* \require <fltKernel.h>
* \brief ExtractFileName miniFilter 获取文件路径
* \param Data
* \return
*/
UNICODE_STRING ExtractFileName(_In_ PFLT_CALLBACK_DATA Data)
{
	NTSTATUS status;
	UNICODE_STRING ret = {0};


	KIRQL irql = KeGetCurrentIrql();
	if (irql > APC_LEVEL)
	{
		return ret;
	}

	if (Data)
	{
		PFLT_FILE_NAME_INFORMATION pNameInfo = NULL;

		status = FltGetFileNameInformation( Data,
											FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_DEFAULT,
											&pNameInfo);

		if (NT_SUCCESS(status) && pNameInfo)
		{
			status = FltParseFileNameInformation(pNameInfo);

			if (NT_SUCCESS(status))
			{
				/** Init(&ret, pNameInfo->Name.MaximumLength); */
				// 由于这块的内存是动态申请的，所以要在用完之后手动释放
				ret.MaximumLength = pNameInfo->Name.MaximumLength;
				ret.Buffer = (PWCH)ExAllocatePool(NonPagedPool, ret.MaximumLength);
				RtlCopyUnicodeString(&ret, &pNameInfo->Name);
			}
			else
			{
				KdPrint(("FltParseFileNameInformation Error : [0x%p]\n", status));
			}

			FltReleaseFileNameInformation(pNameInfo);
		}
	}

	return ret;
}
