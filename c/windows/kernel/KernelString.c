#include <wdm.h>
#include <ntddk.h>

/**
* \brief StringPrint 字符串打印
* \return
*/
VOID StringPrint()
{
	CHAR *str1 = "Hello";
	WCHAR *str2 = L"Hello";
	KdPrint(("%s %S\n", str1, str2));
}


/**
* \brief StringInit 字符串初始化
* \return
*/
VOID StringInit()
{
	CHAR *str1 = "Hello";
	WCHAR *str2 = L"Hello";

	ANSI_STRING aStr;
	UNICODE_STRING uStr;

	RtlInitAnsiString(&aStr, str1);
	RtlInitUnicodeString(&uStr, str2);

	KdPrint(("%Z\n", &aStr));
	KdPrint(("%wZ\n", &uStr));
}


/**
* \brief StringAlloc 自己动态申请空间
* \return
*/
VOID StringAlloc()
{
	#define BUFFER_SIZE 1024
	WCHAR *str = L"Hello";
	UNICODE_STRING uStr = {0};
	UNICODE_STRING uStr2;

	RtlInitUnicodeString(&uStr2, str);

	uStr.MaximumLength = BUFFER_SIZE;
	uStr.Buffer = (PWCH)ExAllocatePool(NonPagedPool, BUFFER_SIZE);
	uStr.Length = 2 * wcslen(str);
	RtlCopyMemory(uStr.Buffer, str, uStr.Length);

	KdPrint(("%wZ\n", &uStr));
	// 由于是动态申请的内存，当用完后需要手动释放内存
	RtlFreeUnicodeString(&uStr);


	// 从一个unicode string 拷贝内容
	uStr.MaximumLength = uStr2.MaximumLength;
	uStr.Buffer = (PWCH)ExAllocatePool(NonPagedPool, uStr2.MaximumLength);
	RtlCopyUnicodeString(&uStr, &uStr2);
	RtlFreeUnicodeString(&uStr);

}


/**
* \brief StringCompare 字符串比较
* \return
*/
VOID StringCompare()
{
	PCH astr1 = "string1";
	PCH astr2 = "string2";

	PWCH ustr1 = L"string1";
	PWCH ustr2 = L"string2";

	UNICODE_STRING uStr1;
	UNICODE_STRING uStr2;

	ANSI_STRING aStr1;
	ANSI_STRING aStr2;

	RtlInitAnsiString(&aStr1, astr1);
	RtlInitAnsiString(&aStr2, astr2);

	RtlInitUnicodeString(&uStr1, ustr1);
	RtlInitUnicodeString(&uStr2, ustr2);


	// 第三个参数为不区分大小写，true为不区分
	BOOLEAN b1 = RtlEqualString(&aStr1, aStr2, TRUE);
	BOOLEAN b2 = RtlEqualUnicodeString(&uStr1, &uStr2, TRUE);

}


/**
* \brief StringUpper 字符串转大写
* \return
*/
VOID StringUpper()
{

	PWCH ustr1 = L"string1";
	UNICODE_STRING uStr1;
	RtlInitUnicodeString(&uStr1, ustr1);

	UNICODE_STRING upperStr;

	// 第三个参数表示是否为目的字符串分配内存
	// 由于upperStr没有为Buffer分配空间，所以这里使用TRUE
	// 不再使用后需要释放内存
	RtlUpcaseUnicodeString(&upperStr, &uStr1, TRUE);

	KdPrint(("%wZ\n", &upperStr));

	RtlFreeUnicodeString(&upperStr);
}


/**
* \brief StringToInteger 字符串转数字
* \return
*/
VOID StringToInteger()
{
	PWCH ustr1 = L"12345";
	UNICODE_STRING uStr1;
	RtlInitUnicodeString(&uStr1, ustr1);

	ULONG value = 0;

	RtlUnicodeStringToInteger(&uStr1, 10, &value);
}


/**
* \brief StringFromInteger 数字转字符串
* \return
*/
VOID StringFromInteger()
{
	UNICODE_STRING uStr1;

	ULONG value = 12345;

	RtlUnicodeStringToInteger(value, 10, &uStr1);
}


/**
* \brief StringConvert ansi to unicode | unicode to ansi
* \return
*/
VOID StringConvert()
{
	PWCH ustr1 = L"111111";
	UNICODE_STRING uStr1;
	RtlInitUnicodeString(&uStr1, ustr1);

	ANSI_STRING aStr1;
	RtlUnicodeStringToAnsiString(&aStr1, &uStr1, TRUE);

	UNICODE_STRING uStr2;
	RtlAnsiStringToUnicodeString(&uStr2, &aStr1, TRUE);

	RtlFreeAnsiString(&aStr1);
	RtlFreeUnicodeString(&uStr2);

}

/**
* \brief StringAppend 字符串拼接
* \return
*/
VOID StringAppend()
{
	UNICODE_STRING ret;

	ret.MaximumLength = 256;
	ret.Buffer = (PWCH)ExAllocatePoolWithTag(NonPagedPool, 256, 'test');
	/** RtlCopyUnicodeString(&ret, &pNameInfo->Name); */

	PWCH str = L"str";

	UNICODE_STRING dosName;
	RtlInitUnicodeString(&dosName, str);

	RtlAppendUnicodeStringToString(&ret, &dosName);
	RtlAppendUnicodeToString(&ret, L"string2");


	KdPrint(("%wZ\n", &ret));

}


NTSTATUS Driver_Entry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
	NTSTATUS status = STATUS_SUCCESS;


	return status;
}
