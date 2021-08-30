// find keyboardservicecallback addresses

#include <ntddk.h>
#include <ntddkbd.h>

typedef VOID(__stdcall *KEYBOARDCLASSSERVICECALLBACK)(
	// KeyboardClassServiceCallback  定义回调函数类型

	/*
	https://docs.microsoft.com/en-us/previous-versions//ff542324(v=vs.85)
	此回调函数将输入缓冲区的数据传输到类驱动的缓冲队列
	*/
	_In_    PDEVICE_OBJECT       DeviceObject,
	_In_    PKEYBOARD_INPUT_DATA InputDataStart, // Pointer to the first keyboard input data packet in the input data buffer of the port device
	_In_    PKEYBOARD_INPUT_DATA InputDataEnd,
	// Pointer to the keyboard input data packet that immediately follows the last data packet in the input data buffer of the port device
	_Inout_ PULONG               InputDataConsumed  // Pointer to the number of keyboard input data packets that are transferred by the routine
	);

typedef struct _KBD_CALLBACK {
	PDEVICE_OBJECT classDeviceObject;  // 记录设备B
	KEYBOARDCLASSSERVICECALLBACK serviceCallback; // 记录回调函数的函数指针
	PVOID *AddrServiceCallback;  // 记录回调函数在驱动扩展中存储的位置
} KDB_CALLBACK, *PKDB_CALLBACK;

KDB_CALLBACK g_kdbCallback = { 0 }; // 全局变量



NTSTATUS SearchServiceCallback(PDRIVER_OBJECT kbdclass_driver_object, PDEVICE_OBJECT i8042_device_object, PVOID * callback_addr)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;

	PVOID device_extension = i8042_device_object->DeviceExtension;

	PDEVICE_OBJECT kbdclass_device_object = kbdclass_driver_object->DeviceObject;

	ULONG_PTR kbdDriverStart = (ULONG_PTR)kbdclass_driver_object->DriverStart;
	ULONG_PTR kbdDriverEnd = kbdDriverStart + (ULONG_PTR)kbdclass_driver_object->DriverSize;

	while (kbdclass_device_object)
	{
		// 判断当前的class设备是否attach在port上
		if (i8042_device_object->AttachedDevice != kbdclass_device_object)
		{
			kbdclass_device_object = kbdclass_device_object->NextDevice;
			continue;
		}

		// 开始遍历端口设备的设备扩展上的内容
		PCHAR deviceExt = (PCHAR)device_extension;
		BOOLEAN findDeviceObject = FALSE;
		BOOLEAN findServiceAddr = FALSE;
		for (int i = 0; i < 4096; deviceExt++)
		{
			if (findDeviceObject && findServiceAddr)
			{
				return STATUS_SUCCESS;
			}
			// 判断当前内存是否有效
			if (!MmIsAddressValid(deviceExt))
			{
				break;
			}


			// 查找kbdclass设备的地址
			PVOID temp = *(PVOID*)deviceExt;
			if (kbdclass_device_object == temp)
			{
				findDeviceObject = TRUE;
				g_kdbCallback.classDeviceObject = kbdclass_device_object;
				continue;
			}

			if ((temp > (PVOID)kbdDriverStart) &&
				(temp<(PVOID)kbdDriverEnd) &&
				MmIsAddressValid(temp))
			{
				findServiceAddr = TRUE;
				*callback_addr = *(PVOID *)deviceExt;
				g_kdbCallback.serviceCallback = (KEYBOARDCLASSSERVICECALLBACK)temp;
				g_kdbCallback.AddrServiceCallback = (PVOID *)deviceExt;
				continue;
			}
		}

		kbdclass_device_object = kbdclass_device_object->NextDevice;
	}

	return status;
}

PVOID GetKeyboardServiceCallbackAddr()
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	PDRIVER_OBJECT i8042_DriverObject = NULL;
	PDRIVER_OBJECT kbdclass_DriverObject = NULL;
	PVOID callbackAddr = NULL;

	status = ChmGetDriverObjectByName((PWCHAR)L"\\Driver\\i8042prt", &i8042_DriverObject);

	if (!NT_SUCCESS(status) || i8042_DriverObject == NULL)
	{
		// 有的系统不会启动这个驱动，地址直接返回为空
		return callbackAddr;
	}


	status = ChmGetDriverObjectByName((PWCHAR)L"\\Driver\\kbdclass", &kbdclass_DriverObject);
	if (!NT_SUCCESS(status) || kbdclass_DriverObject == NULL)
	{
		return callbackAddr;
	}

	// 遍历i8042驱动的设备对象
	PDEVICE_OBJECT i8042_DeviceObject = i8042_DriverObject->DeviceObject;

	while (i8042_DeviceObject)
	{
		status = SearchServiceCallback(kbdclass_DriverObject, i8042_DeviceObject, &callbackAddr);

		if (NT_SUCCESS(status))
		{
			break;
		}

		i8042_DeviceObject = i8042_DeviceObject->NextDevice;
	}

	if (NT_SUCCESS(status) && callbackAddr)
	{
		ObDereferenceObject(kbdclass_DriverObject);
		ObDereferenceObject(i8042_DriverObject);
		return callbackAddr;
	}
	return NULL;
}
