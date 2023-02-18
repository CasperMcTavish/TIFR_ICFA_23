/*
   Name:           read_binary.cpp
   Created by:     Stefan Ritt <stefan.ritt@psi.ch>
   Date:           July 30th, 2014
   Purpose:        Example file to read binary data saved by DRSOsc.
 
   Compile and run it with:
 
      gcc -o read_binary read_binary.cpp
 
      ./read_binary <filename>
   This program assumes that a pulse from a signal generator is split
   and fed into channels #1 and #2. It then calculates the time difference
   between these two pulses to show the performance of the DRS board
   for time measurements.
   $Id: read_binary.cpp 21495 2014-09-26 14:20:49Z ritt $
*/
#include "TFile.h"
#include "TTree.h"
#include <cstdio>
#include <iostream>
#include <cmath>
#include <stdlib.h>
#include <fstream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <TMath.h>
#include "TF1.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TPostScript.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TMinuit.h"
#include "TLatex.h"
#include "TGraphErrors.h"
#include "TLegend.h"
#include <TStyle.h>
#include <TGraph2D.h>
#include <TRandom.h>
#include <TF2.h>
#include <TH1.h>
#include "TStyle.h"
#include <string.h>
#include <sstream>
#include <vector>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <fcntl.h>
//#include <unistd.h>



typedef struct {
   char           file_header[4];
   char           time_header[4];
} THEADER;

typedef struct {
   char           bn[2];
   unsigned short board_serial_number;
} BHEADER;

typedef struct {
   char           event_header[4];
   unsigned int   event_serial_number;
   unsigned short year;
   unsigned short month;
   unsigned short day;
   unsigned short hour;
   unsigned short minute;
   unsigned short second;
   unsigned short millisecond;
   unsigned short range;
} EHEADER;

typedef struct {
   char           bs[2];
   unsigned short board_serial_number;
   char           tc[2];
   unsigned short trigger_cell;
} DHEADER;

/*-----------------------------------------------------------------------------*/

//int main(int argc, const char * argv[])
using namespace std;
int main(){
	THEADER th;
	EHEADER eh;
	BHEADER bh;
	DHEADER dh;
	char hdr[4];
	const int n_chnls = 1;
	const int nbins_drs = 1024;
	unsigned short voltage[nbins_drs];
	double waveform[n_chnls][nbins_drs], time[n_chnls][nbins_drs];
	float bin_width[n_chnls][nbins_drs];
	int i, j, n, chn_index,jk;
	double t1, t2, dt;
	int group;
	char filename[256];
	int ndt;
	char name[300];
	double threshold, sumdt, sumdt2;
	const int N_events = 5000;
	const double V_to_mV = 1000.0;
	const int low_nextra = 4;
	const int hgh_nextra = 20;
	const int nextra = 24;
	const double resistance = 909.09;//ohms (1k||10k)
	const double scnd_gain = 1.37;//(1/2)*(1+RH4/RG4); RH4=270, RG4=470
	const float pedlow=150.0;
	const float pedhgh=250.0;
	const int nhv=5;
	const int HV1[nhv] = {52, 53, 53, 54, 54};
	const int HV2[nhv] = {6, 1, 6, 1, 6};
	
	TH1F* raw_events[450];
	int raw_count = 0;
	TH1F *hist_peak[nhv][n_chnls];
   double pedestal = 0.;
   
   for(int jk=0;jk<nhv;jk++){
	   for (int ch=0 ; ch<n_chnls ; ch++) { 	  
			  sprintf(name,"hist_vol_%ip%i_ch%i", HV1[jk], HV2[jk], ch);
	          hist_peak[jk][ch]=new TH1F(name,name,400,-0.5,3.5);
		  }
	  }
			  
  
    char folder_name[300];
	sprintf(folder_name, "C:\\icfa2023\\GROUP10");	
	
	char logfiles[300] = {};
	sprintf(logfiles,"C:\\icfa2023\\ch_led.log");
	ifstream file_db_1(logfiles);
	int nvolt=0;
	while(!file_db_1.eof()){
		char fileName_1[300] = {};
	    file_db_1 >> fileName_1;
		char datafile_1[300] = {};
		sprintf(datafile_1,"%s/%s", folder_name, fileName_1);
		cout << datafile_1 << endl;
		FILE *f = fopen(datafile_1, "rb");
		if (f == NULL) {
			printf("Cannot find file \'%s\'\n", filename);
			return 0;
		}
		// read time header
		fread(&th, sizeof(th), 1, f);
		//memset(bin_width, sizeof(bin_width), 0);
		fread(&bh, sizeof(bh), 1, f);
		printf("Found data for board #%d\n", bh.board_serial_number);
		for(int ch=0 ; ch<n_chnls ; ch++) {
			fread(hdr, sizeof(hdr), 1, f);
			printf("%i ch : %i %c %c %c %c\n",i, ch, hdr[0],hdr[1],hdr[2],hdr[3]);
			if (hdr[0] != 'C'){
				fseek(f, -4, SEEK_CUR);
			    break;
			}
			i = hdr[3] - '0' - 1;
			printf("Found timing calibration for channel #%d\n", i+1);
			fread(&bin_width[i][0], sizeof(float), 1024, f);
			for(int lm=0; lm<5; lm++){
				// printf("%i bw %f\n ", lm, bin_width[dr][i][lm]);
			}
		}
		
		for(int in=0 ; in<N_events ; in++){
			i = (int)fread(&eh, sizeof(eh), 1, f);
			if(in%1000==0){printf("Found event #%d\n", eh.event_serial_number);}
			if (i < 1)
			break;
			(int)fread(&dh, sizeof(dh), 1, f);
			//if (in % 1000 == 0) { printf("Found board number #%d\n", dh.board_serial_number); }
			for (int ch = 0; ch < n_chnls; ch++) {
				i = (int)fread(hdr, sizeof(hdr), 1, f);
				chn_index = hdr[3] - '0' - 1;
				fread(hdr, sizeof(hdr), 1, f);
				fread(voltage, sizeof(short), 1024, f);
				for (i = 0; i < nbins_drs; i++) {
					// convert data to volts
					waveform[chn_index][i] = (voltage[i] / 65536. + eh.range / 1000.0 - 0.5);
					for (j = 0, time[chn_index][i] = 0; j < i; j++) {
						time[chn_index][i] += bin_width[chn_index][(j + dh.trigger_cell) % 1024];
					}
				}
				TH1F* chnl_raw;
				chnl_raw = new TH1F("raw", "raw", nbins_drs - nextra, -0.5, (nbins_drs - 0.5 - nextra)/1.0);
				pedestal = 0.;
				for (i = 0; i < nbins_drs - nextra; i++) {
					//chnl_raw->Fill(i, waveform[chn_index][i + low_nextra] * V_to_mV);
					chnl_raw->SetBinContent(i+1, waveform[chn_index][i + low_nextra] * V_to_mV);
				}

				int peakbin = 0;
				bool isposSpike=true;
				while(isposSpike) { 
					//if(in%5000==0){cout<<in<<"  "<<isposSpike<<endl;}
					peakbin = chnl_raw->GetMaximumBin();
					double peakval = chnl_raw->GetBinContent(peakbin);
					double prvs_bincont = chnl_raw->GetBinContent(peakbin-1);
					double next_bincont = chnl_raw->GetBinContent(peakbin+1);
					if (peakval > 5.0 && prvs_bincont < 0.20 * peakval && next_bincont < 0.20 * peakval) {
						chnl_raw->SetBinContent(peakbin, 0.);
					}
					else {
						isposSpike = false;
					}
				}
				bool isnegSpike=true;
				while(isnegSpike) { 
					int min_bin = chnl_raw->GetMinimumBin();
					double neg_peakval = chnl_raw->GetBinContent(min_bin);
					double prvs_bincont = chnl_raw->GetBinContent(min_bin-1);
					double next_bincont = chnl_raw->GetBinContent(min_bin+1);
					if(neg_peakval<-5.0 && abs(prvs_bincont)<0.20*abs(neg_peakval) && next_bincont<0.20*abs(neg_peakval)){
						chnl_raw->SetBinContent(min_bin, 0.);
					} else {
						isnegSpike = false;
					}
				}		

				int ped_counter = 0;
				for (int ij = 1; ij < chnl_raw->GetNbinsX() + 1; ij++) {
					if ((chnl_raw->GetBinCenter(ij)) > pedlow && (chnl_raw->GetBinCenter(ij)) < pedhgh) {
						pedestal += chnl_raw->GetBinContent(ij); ped_counter++;
					}
				}
				pedestal = pedestal / ped_counter;
				TH1F* chnl_subraw;
				chnl_subraw = (TH1F*)chnl_raw->Clone("testxx");
				for (int ij = 1; ij < chnl_raw->GetNbinsX() + 1; ij++) {
					chnl_subraw->SetBinContent(ij, chnl_raw->GetBinContent(ij) - (pedestal));
				}

				peakbin = chnl_subraw->GetMaximumBin();
				double inv_widthx = 1.0 / (chnl_subraw->GetBinWidth(2));
				//cout<<"inv  "<<inv_widthx<<endl;
				double intg = chnl_subraw->Integral(peakbin - (20.0 * inv_widthx), peakbin + (80.0 * inv_widthx), "width");
				//double ped = chnl_subraw->Integral(peakbin-(150.0*inv_widthx), peakbin-(50.0*inv_widthx), "width");
				double peaktime = chnl_subraw->GetBinCenter(peakbin);
				if ((peaktime - 20.) > 10. && (peaktime + 85.) < 1000.) {
					hist_peak[nvolt][ch]->Fill(intg / (resistance * scnd_gain));
				}
				if (in < 10 && nvolt == 4) {
					raw_events[raw_count] = (TH1F*)chnl_raw->Clone("testxx");
					sprintf(name, "evtno_%i_ch%i_hv%i", in, ch, nvolt);
					raw_events[raw_count]->SetName(name);
					raw_events[raw_count]->SetTitle(name);
					raw_count++;
				}
				if (chnl_subraw) { delete chnl_subraw; chnl_subraw = 0; }
				if (chnl_raw) { delete chnl_raw; chnl_raw = 0; }
			}
			
		}
		nvolt++;
	}
	sprintf(name, "%s.root", folder_name);
	TFile *newfile = new TFile(name,"recreate");
    newfile->cd();
	for(int ij=0; ij<raw_count; ij++){
		//raw_events[ij]->GetXaxis()->SetRangeUser(300.0, 800.);
	    raw_events[ij]->Write();
	 }
	 for(int ij=0; ij<nhv; ij++){
		 for (int ch=0 ; ch<n_chnls ; ch++) {
			 hist_peak[ij][ch]->Write();
		 }
	 }
	 newfile->Close();
}

