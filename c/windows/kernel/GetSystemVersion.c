#define WIN7_7600			7600
#define WIN7SP1_7601		7601

#define WIN8_9200			9200
#define WIN8_1_9600			9600

#define WIN10_1507_10240	10240
#define WIN10_1511_10586	10586
#define WIN10_1607_14393	14393
#define WIN10_1703_15063	15063
#define WIN10_1709_16299	16299
#define WIN10_1803_17134	17134
#define WIN10_1809_17763	17763
#define WIN10_1903_18362	18362
#define WIN10_1909_18363	18363
#define WIN10_2004_19041	19041
#define WIN10_20H2_19042	19042

/**
* \require <ntddk.h> <wdm.h>
* \brief ChmGetSystemVersion 获取系统的build number
* \return 
*/
ULONG ChmGetSystemVersion()
{
	ULONG buildNumber = 0;
	RTL_OSVERSIONINFOW versionInfo = {0};
	ULONG ulMajorVersion = 0;
	ULONG ulMinorVersion = 0;
	ULONG ulBuildNumber = 0;

	versionInfo.dwOSVersionInfoSize = sizeof(RTL_OSVERSIONINFOW);

	RtlGetVersion(&versionInfo);
	ulMajorVersion = versionInfo.dwMajorVersion;
	ulMinorVersion = versionInfo.dwMinorVersion;
	ulBuildNumber = versionInfo.dwBuildNumber;

	KdPrint((PROJECT_NAME "%s ulMajorVersion = [%d], ulMinorVersion = [%d], ulBuildNumber = [%d]\n", __FUNCTION__, ulMajorVersion, ulMinorVersion, ulBuildNumber));

	switch (ulMajorVersion)
	{
		case 6:
		{
			if (ulMinorVersion == 1 && ulBuildNumber == 7600)
			{
				buildNumber = WIN7_7600;
			}
			else if (ulMinorVersion == 1 && ulBuildNumber == 7601)
			{
				buildNumber = WIN7SP1_7601;		
			}
			else if (ulMinorVersion == 2 && ulBuildNumber == 9200)
			{
				buildNumber = WIN8_9200;		
			}
			else if (ulMinorVersion == 3 && ulBuildNumber == 9600)
			{
				buildNumber = WIN8_1_9600;		
			}
			break;
		}

		case 10:
		{
			if (ulMinorVersion == 0 && ulBuildNumber == 19042)
			{
				buildNumber = WIN10_20H2_19042;
			}
			else if (ulMinorVersion == 0 && ulBuildNumber == 19041)
			{
				buildNumber = WIN10_2004_19041;
			}
			else if (ulMinorVersion == 0 && ulBuildNumber == 18363)
			{
				buildNumber = WIN10_1909_18363;
			}
			else if (ulMinorVersion == 0 && ulBuildNumber == 18362)
			{
				buildNumber = WIN10_1903_18362;
			}
			else if (ulMinorVersion == 0 && ulBuildNumber == 17763)
			{
				buildNumber = WIN10_1809_17763;
			}
			else if (ulMinorVersion == 0 && ulBuildNumber == 17134)
			{
				buildNumber = WIN10_1803_17134;
			}
			else if (ulMinorVersion == 0 && ulBuildNumber == 16299)
			{
				buildNumber = WIN10_1709_16299;
			}
			else if (ulMinorVersion == 0 && ulBuildNumber == 15063)
			{
				buildNumber = WIN10_1703_15063;
			}
			else if (ulMinorVersion == 0 && ulBuildNumber == 14393)
			{
				buildNumber = WIN10_1607_14393;
			}
			else if (ulMinorVersion == 0 && ulBuildNumber == 10586)
			{
				buildNumber = WIN10_1511_10586;
			}
			else if (ulMinorVersion == 0 && ulBuildNumber == 10240)
			{
				buildNumber = WIN10_1507_10240;
			}
			break;
		}

		default:
		{
			buildNumber = 0;
			break;
		}
		
	}

	return buildNumber;
}
