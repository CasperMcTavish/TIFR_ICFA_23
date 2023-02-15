#include <windows.h>
#include <stdio.h>
#include "ftd2xx.h"
#include <winbase.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>

int isRXQueueEmpty(FT_HANDLE ftHandle)
{
	DWORD RxBytes;
	FT_GetQueueStatus(ftHandle, &RxBytes);
	
	if(RxBytes == 0)
		return 1;
	else
		return 0;
}

int main(int argc, char* argv[])
{
	int i, j, k;
	FILE *FilePointer;
	char muon[200];      /* For filename getting */
	time_t starttime;         /* For Time and Data printing */
	struct tm *stime;
	FT_STATUS ftStatus;	
	FT_HANDLE ftHandle;
	DWORD numDevs;
	FT_DEVICE_LIST_INFO_NODE *devInfo;
	DWORD Flags,ID,Type,LocId;
	char SerialNumber[16],Description[64],TxBuffer[5]={'a'};
	DWORD RxBytes = 3,TxBytes,EventDWord,BytesWritten;
	unsigned char Rxbuffer[1024];// contains the data to read by the device
	DWORD BytesReceived;
	
	 printf("Enter Data File [c:\\Users\\Guest\\Desktop\\Sai\\muon.txt]: ");
	scanf("%s", muon);                             /* Getting Data File Name */          

	
	
	FilePointer = fopen(muon, "a+");               /* Openning Data File Name */
	
	// open device
	ftStatus = FT_Open(0,&ftHandle); 
	if (ftStatus == FT_OK)
	{
		printf("FT_Open OK, use ftHandle to access device\n");
		printf("ftHandle %u\n",&ftHandle);
	}
	else 
	{
		printf("FT_Open failed\n");
	}
	
	//FT_STATUS FT_CreateDeviceInfoList (LPDWORD lpdwNumDevs);
	ftStatus = FT_CreateDeviceInfoList(&numDevs);
	if (ftStatus == FT_OK) 
	{
		printf("Number of devices is %d\n",numDevs);
	}
	//numDevs=1;
	if (numDevs > 0) 
	{
	//allocate storage for list based on numDevs
	devInfo = (FT_DEVICE_LIST_INFO_NODE*)malloc(sizeof(FT_DEVICE_LIST_INFO_NODE)*numDevs);
	// get the device information list
		ftStatus = FT_GetDeviceInfoList(devInfo,&numDevs); 
		if (ftStatus == FT_OK) 
		{ 
			for (i = 0; i < numDevs; i++) 
			{ 
				printf("Dev %d:\n", i);
				printf(" Flags=0x%x\n", devInfo[i].Flags); 
				printf(" Type=0x%x\n", devInfo[i].Type);
				printf(" ID=0x%x\n", devInfo[i].ID); 
				printf(" LocId=0x%x\n", devInfo[i].LocId);
				printf(" SerialNumber=%s\n", devInfo[i].SerialNumber); 
				printf(" Description=%s\n", devInfo[i].Description); 
				printf(" ftHandle=0x%x\n", devInfo[i].ftHandle); 
			} 
		} 
	}
	
	FT_SetTimeouts(ftHandle, 500, 500);
	
	//Get the Status about the device
	//FT_STATUS FT_GetStatus (FT_HANDLE ftHandle, LPDWORD lpdwAmountInRxQueue, LPDWORD lpdwAmountInTxQueue, LPDWORD lpdwEventStatus)
	ftStatus=FT_GetStatus(ftHandle,&RxBytes,&TxBytes,&EventDWord);
	if(ftStatus == FT_OK)
	{
		printf("\nAmt in RXQ %d Amt in TXQ %d Status %d\n", RxBytes,TxBytes,EventDWord);
	}
	
	while(1)
	{
		while(isRXQueueEmpty(ftHandle));
		
		ftStatus = FT_Read(ftHandle, Rxbuffer, 1, &BytesReceived);
		printf("No. of bytes read from device %d\n", BytesReceived);
		printf("data read is %02X \n", Rxbuffer[0]);
		time (&starttime);                    /* To grab and store Current System Time */ 
			stime = localtime (&starttime);       /* To convert Time to struct tm */
			

			fprintf(FilePointer,"       %03d    %02d:%02d:%02d   %02d-%02d-%4d \n", Rxbuffer[0] , stime->tm_hour,stime->tm_min, stime->tm_sec, stime->tm_mday, stime->tm_mon+1, stime->tm_year+1900);
			fflush(FilePointer);
			
	}
	
	//FT_STATUS FT_Read(FT_HANDLE ftHandle, LPVOID lpBuffer, DWORD dwBytesToRead, LPDWORD lpdwBytesReturned);
	ftStatus = FT_Read(ftHandle, Rxbuffer, 1, &BytesReceived);
	
	if (ftStatus == FT_OK)
		printf("FT not OK\n");
	
	printf("No. of bytes read from device %d\n", BytesReceived);
	printf("data read is\n");
	for(j=0; j<1; j++)
	{
		printf("%02X\n",Rxbuffer[j]);
	}	
	
	
	/*else 
	{
		printf("FT_Read failed\n");
	}*/
}