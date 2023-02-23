//gcc -lm -DLINUX -o DAQ_CAMAC_v17 DAQ_CAMAC_v17.c console.c  crate_lib.c -lpthread -lncurses
//to read 64 channels
 
//DAQ started

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "crate_lib.h"
#include <unistd.h>
#include <string.h>
#include <curses.h>
//#include <ncurses.h>
#include "console.h"
// station numbers of various module
# define st_scaler_1 17;	//22-17
# define st_adc 1;
# define st_trg 3;
# define TDC 15;
//# define ADC 8;
# define ADC 12;
# define QDC 9;


unsigned int i,min,event=1,itt=0,ii=1;
time_t starttime,start,end,m1,m2;
double t_dif,t_dif1;
unsigned int  cmp, r_time,m_time;
short int CrId;
//char IPNum[16]=158.144.55.107;
CRATE_OP   CrOp;  //CRATE_OP in crate_lib.h
unsigned long int channel[32],X_Strip[32],tdcdata[16],ch[12],X_Strip_1[32],par_ch[12],sam_par_ch[12],qch[12];
unsigned long LAM_MASK_0,LAM_MASK_1;
char* aqtime;
FILE *fp,*ft;
char runno[50];
char runno_1[50];
char runname[5]="mon";

char hotkey;
int sclr_pos;
int sclr_stn_1 = 17;	//22-17
char name_1[5]="eve";
int fflush_ret = 1;




char*  printTimerScreen (void)
{
	
	struct tm  *stime;
	time (&starttime); //grab and store current system time  
	stime = localtime (&starttime); //convert it to struct tm
    	return(asctime(stime));
}










int select_bank_0_s1()	//This will select bank 0
{
	CrOp.F = 12;CrOp.A = 1; CrOp.DATA = 0;CrOp.N = st_scaler_1;
	CFSA(CrId, &CrOp);	//writing to bank select register (Bank 0 , Sequential address from 00000 )
}

int select_bank_1_s1()//This will select bank 1
{
	CrOp.F = 12;CrOp.A = 1; CrOp.DATA = 0x1;CrOp.N = st_scaler_1;
	CFSA(CrId, &CrOp);	//writing to bank select register (Bank 1 , Sequential address from 11111 )
}



void clear_scaler_1()
{
	CrOp.F=11;CrOp.A=4;CrOp.DATA=0;CrOp.N=st_scaler_1;
	CSSA(CrId, &CrOp);	
}


void read_scaler_1()
{
	select_bank_0_s1(); // salim
	CrOp.F=0;CrOp.A=0;CrOp.DATA=0;CrOp.N=st_scaler_1;
	for(i=0;i<8;i++)	//this will read channel data 1 - 16
	{
		CFSA(CrId, &CrOp);
		channel[i] = CrOp.DATA;
		CrOp.A++;
	}
	
	//MNS : Read only first 8 channels - 5/11/2018
	
	/*select_bank_1_s1(); //salim
	CrOp.F=0;CrOp.A=0;CrOp.DATA=0;CrOp.N=st_scaler_1;
	for(i=0;i<16;i++)	//this will read channel data 17 - 32
	{
		CFSA(CrId, &CrOp);
		channel[i+16] = CrOp.DATA;
		CrOp.A++;
	}*/

}



int main()
{

	con_init();
	gotoxy(1,1);
	con_printf("Enter Run No :");
	con_scanf("%s",runno);		//ask run no to start
	//runno_1=runno;
	strcpy(runno_1,runno);
	
    strcat(runname,runno);
	strcat(runname,".txt");
//	con_printf("%s  %s",name_1,runno_1);
    con_printf("Enter the monitering time in Second : ");  //ask for MON time
    con_scanf("%d",&m_time);
	con_printf("Enter update time in Second : ");	//ask for update time
	con_scanf("%d",&r_time);
	
    fp=fopen(runname,"w");//open data file
	
    if(!fp)
    {
		con_end(); 
        printf("\ncannot open file sorry");
		exit(1);// exit if data file not open
    }
	//fflush(fp);
	
	strcat(name_1,runno_1);
	strcat(name_1,".txt");
	
	ft=fopen(name_1,"w");//open data file
   	if(!ft)
  	{	
		con_end(); 
    	printf("\ncannot open file sorry");
		exit(1);// exit if data file not open
    }


	con_printf("Opening the crate please wait ..........");
	aqtime=printTimerScreen();
	con_printf("START TIME  : %s",aqtime);
	CrId=CROPEN("192.168.55.124");
  	if( CrId < 0)
 	{
		con_end();
		printf("\nCrId::%d\n",CrId);
  		printf("\nCould not open \"C111C \n");
  		exit(1);
 	}
 	//con_printf("\nCAMAC INITALISE");
 	CCCZ(CrId);	// generate crate clear	
	//CrOp.F = 0;CrOp.A = 0; CrOp.DATA = 0;CrOp.N = st_adc;
	//CFSA(CrId, &CrOp);	//enable ADC module
	
	clear_scaler_1();
	
	for(i=0;i<12;i++)	// clearing 
	{
		sam_par_ch[i]=0;
	}


	CrOp.F = 0;CrOp.A = 0; CrOp.DATA = 0;CrOp.N = QDC;
	CFSA(CrId, &CrOp);	//enable module

	// tdc made in to adc for testing 7186 tdc

	CrOp.F = 10;CrOp.A = 0; CrOp.DATA = 0;CrOp.N = QDC;	//CLEAR LAM
	CSSA(CrId,&CrOp);
	CrOp.F = 9;CrOp.A = 0; CrOp.DATA = 0;CrOp.N = QDC;	//CLEAR MODULE
	CSSA(CrId,&CrOp);
	CrOp.F = 26;CrOp.A = 0; CrOp.DATA = 0;CrOp.N = QDC;	//ENABLE LAM
	CSSA(CrId,&CrOp);

	time(&start);


	LACK(CrId);	//clear pending LAM
	//reading scaler data
	do
	{
		gotoxy(1,13);
		
		//read scaler
		
		aqtime=printTimerScreen();
		con_printf("CURRENT TIME  : %s",aqtime);
		usleep(150*1000);
		time(&m1);	
		time(&m2);
	//	con_printf("\ninter time %d",m1);
		t_dif = difftime(m1, start);
		t_dif1 = difftime(m2, start);
	//	con_printf("\ndiff time at 1 %.21f",t_dif);
		cmp =	(int)t_dif % r_time;
		con_printf("Refresh Time Remaining  : %d ",cmp);
		if( (cmp == 0) && (t_dif >= r_time ) )
		{	
						
			gotoxy(1,17);
	//		con_printf("\n refresh loop ");
	//		con_printf("\nTIME 1 : %2.4f and  %3d",t_dif,r_time);	
			read_scaler_1();
			
			//MNS : Read and print only first 8 channels - 5/11/2018
			for(i=0;i<8;i++)	//printing frequency
			{
				if(i%4==0)
				{
					con_printf("\n");
				}
				con_printf("p-[%2d]\t%8d\t%4.2f\t",i,channel[i],(float)channel[i]/(float)(t_dif) );
			}

			
			con_printf("\n");
			
			//MNS : Read and print only first 8 channels - 5/11/2018
			
			//read_scaler_2();
			//read_scaler_3();
			
			/*for(i=0;i<32;i++)	//printing frequency
			{
				if(i%4==0)
				{
					con_printf("\n");
				}
				con_printf("X-Strip[%02d]: %8ld %6.2f\t",i,X_Strip[i],(float)X_Strip[i]/(float)(t_dif));
				//con_printf("X-Strip[%02d]: %8ld %5d Hz\t",i,X_Strip[i],(int)(X_Strip[i]/t_dif));
			}
			con_printf("\n");
			
			for(i=0;i<32;i++)	//printing frequency
			{
				if(i%4==0)
				{
					con_printf("\n");
				}
				  con_printf("X-Strip[%02d]: %8ld %6.2f\t",i+32,X_Strip_1[i],(float)X_Strip_1[i]/(float)(t_dif));
				//con_printf("X-Strip[%02d]: %8ld %5d Hz\t",i+32,X_Strip_1[i],(int)(X_Strip_1[i]/t_dif));
			}*/

			//ii++;	
		}
		
		//end = time(NULL);	
		//t_dif = difftime(m1, start);
	//	con_printf("\ndiff time at 2 %3d",t_dif);
	//		gotoxy(2,22);
	//	con_printf("\ninter time %d",m1);
		gotoxy(1,15);
		cmp = (int)t_dif % m_time;
		con_printf("EVENT NO : %d\n",channel[9]);
		con_printf("Elapse Time  : %d ",cmp);
		
		if((cmp == 0 )&& (t_dif >= r_time ))
		{
			gotoxy(1,50);
			i=1;
		//	con_printf("\nTIME 2 : %2d and %2d",t_dif,m_time);
			read_scaler_1();
			//read_scaler_2();
			//read_scaler_3();
			fprintf(fp,"%d %s",time(&starttime), aqtime);
			//fprintf(fp,"%d \n ",time(&starttime)); //salim
		//	con_printf("safe 1");

			/*for(i=0;i<32;i++)
			{
				fprintf(fp,"%5ld  %5.2f  ",X_Strip[i],(float)X_Strip[i]/(float)t_dif );  //saving; float changed on 12 nov2012 by rrs		
			}
		//	con_printf("safe 2");	
			fprintf(fp,"\n");
			for(i=0;i<32;i++)
			{
				fprintf(fp,"%5ld  %5.2f  ",X_Strip_1[i],(float)X_Strip_1[i]/(float)t_dif );  //saving;float changed on 12 nov2012 by rrs	
			}
			fprintf(fp,"\n");
			for(i=0;i<12;i++)	// writing the avg value of ADC in file
			{
				fprintf(fp,"%f  ",(float)sam_par_ch[i]/itt);
			}
	//		con_printf("safe 3");
			fprintf(fp,"\n");*/
			
			for(i=0;i<8;i++)
			{
				fprintf(fp,"%4d  %4.2f  ",channel[i],(float)channel[i]/(float)t_dif );  //saving		
			}
			fprintf(fp,"\n");
			fflush_ret = fflush(fp);
			if(fflush_ret != 0)
			{
			  // error in flushing exit
			  con_end();
			  printf("Error in flusing fp\n");
			  printf("errno = %d\n", errno);
			  //printf("Error: %s\n", sys_errlist[errno]);
			  exit(1);
			}
			  
			
			clear_scaler_1();
			//clear_scaler_2();
			//clear_scaler_3();
			start = time(NULL);
			
		}
		
		CrOp.F=8;CrOp.A=0;CrOp.DATA=0;CrOp.N=QDC;	//2228 generated LAM on Trigger pulse
		CSSA(CrId, &CrOp);	//Test LOOK-AT-ME (Q=1 if LAM is generated, Q=0 if LAM is not generated)
		if(CrOp.Q != 0)  
		{
		//	TDC Generated LAM	
		//MNS : Read and print only first 5 channels - 5/11/2018
			CrOp.F=0;CrOp.A=0;CrOp.DATA=0;CrOp.N=TDC;
			for(i=0;i<5;i++)	//this will read TDC data, 0 - 4 channels
			{
				CSSA(CrId, &CrOp);
				//tdcdata[i] = CrOp.DATA; //for Lecroy TDC_22 Nov 2018
				tdcdata[i] = CrOp.DATA & 0xFFF; // for phi sci 7186, strip off the chnl id -RRS-24jan2016
				CrOp.A++;
			}
			
		//MNS : Read and print only 2nd QDC - 5/11/2018
			/*CrOp.F=0;CrOp.A=0;CrOp.DATA=0;CrOp.N=ADC;
			for(i=0;i<8;i++)	//this will  read ADC data, 0 - 7, 8 channels (Changed to 8 channels(RRS-25jan2017)
			{
				CSSA(CrId, &CrOp);
				ch[i] = CrOp.DATA;
				CrOp.A++;
			}*/	
				
			CrOp.F=0;CrOp.A=0;CrOp.DATA=0;CrOp.N=QDC;
			for(i=0;i<3;i++)	//this will  read qDC data, 0, 1, & 2 channels
			{
				CSSA(CrId, &CrOp);
				qch[i] = CrOp.DATA;
				CrOp.A++;
			}	
					
			gotoxy(1,5);
			fprintf(ft,"%d %s",time(&starttime), aqtime);
			for(i=0; i<5; i++)	//print  and save TDC data  (Changed to 16channles by RRS 24 jan 2016)
			{
				/*if(i%8==0)
				{
					con_printf("\n");
				}*/
				con_printf("TDC:%2d = %6d    ",i,tdcdata[i]); //printing 
				fprintf(ft,"%4d  ",tdcdata[i]);  //saving
			}
			con_printf("\n");  
			fprintf(ft,"\n");
			
		//MNS : Read and print only 2nd QDC - 5/11/2018
			/*for(i=0;i<8;i++)	//print  and save ADC  data (station 8)
			{
				if(i%4==0)
				{
					con_printf("\n");
				}
				con_printf("tDC:%2d = %6d    ",i,ch[i]); //printing 
				fprintf(ft,"%4d  ",ch[i]);  //saving
                
			}*/
			
			
			for(i=0;i<3;i++)	//print  and save ADC  data (station 8)
			{
				/*if(i%6==0)
				{
					con_printf("\n");
				}*/
				con_printf("QDC:%2d = %6d    ",i,qch[i]); //printing 
				fprintf(ft,"%4d  ",qch[i]);  //saving

			}
			fprintf(ft,"\n");
			
			       
			CrOp.F = 9;CrOp.A = 0; CrOp.DATA = 0;CrOp.N = ADC;	//CLEAR LAM
			CSSA(CrId,&CrOp);

			CrOp.F = 9;CrOp.A = 0; CrOp.DATA = 0;CrOp.N = TDC;	//CLEAR LAM
			CSSA(CrId,&CrOp);
			
			CrOp.F = 9;CrOp.A = 0; CrOp.DATA = 0;CrOp.N = QDC;	//CLEAR LAM
			CSSA(CrId,&CrOp);
			
			fflush_ret = 0;
			fflush_ret = fflush(ft);
			if(fflush_ret != 0)
			{
			  // error in flushing ft exit
			  con_end();
			  printf("Error in flusing ft\n");
			  printf("errno = %d\n", errno);
			  //printf("Error: %s\n", sys_errlist[errno]);
			  exit(1);
			}
		}
		
		//sleep(min);
		hotkey = con_kbhit ();
      	if (hotkey)
	  toupper(hotkey);
	} while (!(hotkey=='q' || hotkey == 'Q'));
	con_end();
	fclose(fp);
	fclose(ft);

}

