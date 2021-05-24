/**
 * \brief ObReferenceObjectByName 此函数需要声名，虽然wdk没有把这个函数导出，
 * 但它确实存在，所以在用的时候声名一下。
 * 也把要使用的全局变量声明一下：
 * extern POBJECT_TYPE *IoDriverObjectType;
 * \param ObjectName 
 * \param Attributes 
 * \param PassedAccessState 
 * \param DesiredAccess 
 * \param ObjectType 
 * \param AccessMode 
 * \param ParseContext 
 * \param Object 
 * \return 
 */
NTSTATUS
NTAPI
ObReferenceObjectByName(
IN PUNICODE_STRING	ObjectName,
IN ULONG	 Attributes,
IN PACCESS_STATE	PassedAccessState OPTIONAL,
IN ACCESS_MASK	 DesiredAccess OPTIONAL,
IN POBJECT_TYPE	 ObjectType OPTIONAL,
IN KPROCESSOR_MODE	AccessMode,
IN OUT PVOID	 ParseContext OPTIONAL,
OUT	PVOID	 *Object
);

extern POBJECT_TYPE *IoDriverObjectType;


/**
 * \require <ntddk.h> <wdm.h> and declare ObReferenceObjectByName IoDriverObjectType
 * \brief 通过驱动名获取驱动对象
 * 注：此函数内部调用ObReferenceObjectByName，当不再使用DriverObject后，
 * 应调用ObDereferenceObject减少对DriverObject的引用计数。
 * \param DriverName 驱动名，如：L"\\Driver\\kbdclass"
 * \param DriverObject 返回 PDRIVER_OBJECT
 * \return ObReferenceObjectByName的结果
 */
NTSTATUS ChmGetDriverObjectByName(PWCHAR DriverName, PDRIVER_OBJECT *DriverObject)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	UNICODE_STRING uDriverName;
	RtlInitUnicodeString(&uDriverName, DriverName);

	status = ObReferenceObjectByName(&uDriverName,
		OBJ_CASE_INSENSITIVE,
		NULL,
		FILE_ALL_ACCESS,
		*IoDriverObjectType,
		KernelMode,
		NULL,
		(PVOID *)DriverObject);

	return status;
}
