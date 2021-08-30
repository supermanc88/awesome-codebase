#include <ntddk.h>

// get time in kernel mode
void get_system_time(LARGE_INTEGER *time)
{
	KeQuerySystemTime(time);
}