/**
* \require <ntddk.h> <wdm.h>
* \brief KernelSleep 内核中的睡眠函数
* \param milliSeconds 要睡眠的毫秒数
* \return 
*/
VOID KernelSleep(IN int milliSeconds)
{
	LARGE_INTEGER Interval;
	Interval.QuadPart = -10000 * (LONGLONG)milliSeconds;
	
	if (KeGetCurrentIrql() == PASSIVE_LEVEL)
	{
		KeDelayExecutionThread(KernelMode, FALSE, &Interval);
	}
	else
	{
		KEVENT waitEvent;
		KeInitializeEvent(&waitEvent, NotificationEvent, FALSE);
		KeWaitForSingleObject(&waitEvent, Executive, KernelMode, FALSE, &Interval);
	}
}
