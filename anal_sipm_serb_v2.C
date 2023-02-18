/*
name fitv_hv2_dr3_ch0 0.0982143

g++ -g  -std=gnu++11 -I`root-config --incdir`  anal_sipm_serb19.C -o anal_sipm_serb19 `root-config --glibs`

*/
// Current Plots
// <Run#><HV><BT01Y><BT01X><BT02Y><BT02X><AB22Y><AB22X>

#include <iostream>
#include <iomanip>
#include <cmath>
#include <stdlib.h>
#include <fstream>
#include <new>
#include<climits>
#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TPad.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TH2.h"
#include "TProfile.h"

#include "TStyle.h"
#include "TAttFill.h"
#include "TPaveStats.h"
#include "TMinuit.h"
#include "TPostScript.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"
#include "TRandom.h"
#include "TPaletteAxis.h"
#include "TLegend.h"
#include "TLatex.h"
#include "TMath.h"

using namespace std;

const int npeakMx=8; //Total number of peaks

Double_t polfunc(Double_t* x, Double_t* par){
  return par[0]+par[1]*x[0];
}

//First (0:2) three are for Landau function
// 3 : First peak position
// 4 : Gap between two peaks 
// 5 : Gaussian width
// 6-12 : Peak height
Double_t totalfunc(Double_t* x, Double_t* par){
  double aval = par[0]*TMath::Landau(x[0], par[1], par[2]);
  for (int ij=0; ij<npeakMx; ij++) {
    aval += par[7+ij]*TMath::Gaus(x[0], par[3]+ij*par[4], par[5]*sqrt(1.0+par[6]*ij));
  }
  return aval;
}

int main() {
  //void sipmpeaks() {
  const char* namex="C:\\icfa2023\\GROUP10";
  gStyle->SetPalette(1,0);
  gStyle->SetStatColor(10);
  gStyle->SetLabelColor(1, "XYZ");
  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0); //1110);
  gStyle->SetOptFit(0);
  gStyle->SetOptLogy(0);
  gStyle->SetStatW(.20);
  gStyle->SetStatH(.19);
  gStyle->SetStatX(.98);
  gStyle->SetStatY(.98);
  gStyle->SetMarkerColor(2);
  gStyle->SetMarkerStyle(24);
  gStyle->SetMarkerSize(0.60);
  gStyle->SetPadTopMargin(0.02);
  gStyle->SetPadBottomMargin(0.2);
  gStyle->SetPadLeftMargin(0.115);
  gStyle->SetPadRightMargin(0.035);
  gStyle->SetPadGridX(1);
  gStyle->SetPadGridY(1);
  gStyle->SetGridStyle(3);
  gStyle->SetGridWidth(1);
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);
  gStyle->SetFuncWidth(1);
  gStyle->SetStatFont(42);        // Times New Roman
  gStyle->SetTextFont(42);        // Times New Roman
  gStyle->SetTitleFont(42); // Times New Roman
  gStyle->SetLabelFont(42,"XYZ"); // Times New Roman
  gStyle->SetTitleSize(0.070, "X");
  gStyle->SetTitleOffset(1.0, "X");
  gStyle->SetLabelSize(0.090, "X");
  gStyle->SetLabelSize(0.055, "YZ"); // Times New Roman  
  gStyle->SetNdivisions(202, "XYZ");
  gStyle->SetLabelOffset(0.002, "XYZ"); 
  gStyle->SetTitleYOffset(0.1); //, "XYZ"); 
  gStyle->SetTitleFontSize(0.075);
  char outfile[100];
  char outfilx[100];
  char infilename[200];  
  char infile[200];
  char variab[200];
  char variab2[200];
  char name[100];
  char title[100];
  char rootfilename[100];
  //  ofstream file_out("test.txt");
	
  TLatex latex;
  latex.SetNDC();
  latex.SetTextSize(0.07);
  latex.SetTextFont(42);
  latex.SetTextAlign(11); // 11 left; // 21 centre, // (31); // align right, 22, 23, shift bottom
  const int n_drs = 1;
  const int n_chnls = 1;
  //const int nhv=4;//2075,2139
  //const double HV[nhv] = {53, 53.5, 54.0, 54.5};//2075
  //const int HV1[nhv] = {53, 53, 54, 54};//2075
  //const int HV2[nhv] = {0, 5, 0, 5};//2075
  //double OV[nhv]={1.0, 2.0, 2.5, 3.0};//2075
  //const double errx[nhv] = {0.0, 0.0, 0.0, 0.0};
  
  const int nhv=5;
  //const double HV[nhv] = {52.5, 53.5, 53.0, 54.0, 54.5};//2048
  const double HV[nhv] = {52.6, 53.1, 53.6, 54.1, 54.6};
  const double errx[nhv] = {0.0, 0.0, 0.0, 0.0, 0.0};
  const int HV1[nhv] = {52, 53, 53, 54, 54};
  const int HV2[nhv] = {6, 1, 6, 1, 6};
  double OV[nhv]={1.0, 1.5, 2.0, 2.5, 3.0};
  double tenpseven = pow(10,7);
  
  
  TH1F *hist_peak[nhv][n_drs][n_chnls]={0};
  TH1F *hist_peak2[nhv][n_drs][n_chnls]={0};
  double avg_gp[n_drs][n_chnls][nhv]={0};
  double avg_gperr[n_drs][n_chnls][nhv]={0};
  double gain[n_drs][n_chnls][nhv]={0};
  double gainerr[n_drs][n_chnls][nhv]={0};
 
  TF1* fitv[nhv][n_drs][n_chnls]={0};
  const int irebin = 2;

  
  
  //sprintf(rootfilename, "%s/%s.root", namex, namex);
  sprintf(rootfilename, "%s.root", namex);
  TFile* fileIn = new TFile(rootfilename, "open");  
  for (int ij=0; ij<nhv; ij++) {
	  for (int dr=0; dr<n_drs; dr++){
		  for (int ch=0 ; ch<n_chnls ; ch++) { 
			  sprintf(name,"hist_vol_%ip%i_ch%i", HV1[ij], HV2[ij], ch);
        hist_peak[ij][dr][ch]=(TH1F*)fileIn->Get(name);
		  }
	  }
  }

  for (int ij=0; ij<nhv; ij++) {
	  for (int dr=0; dr<n_drs; dr++){
		  for (int ch=0 ; ch<n_chnls ; ch++) { 
			  hist_peak2[ij][dr][ch] = (TH1F*)hist_peak[ij][dr][ch]->Clone();
			  hist_peak2[ij][dr][ch]->Rebin(irebin);
		  }
	  }
  }
  sprintf(rootfilename, "%s.txt", namex);
  ofstream MyFile(rootfilename); 
  sprintf(rootfilename, "%s.ps", namex);
  TPostScript ps(rootfilename,111);
  ps.Range(20,30);
  ps.NewPage();
  TCanvas *c2 = new TCanvas("c2", "SiPM", 700, 900);
  c2->Divide(1,5);//,1.e-5, 1.e-5); 
  
	for (int ij=0; ij<nhv; ij++) {
   
		for(int dr=0; dr<n_drs; dr++){
        for (int ch=0 ; ch<n_chnls ; ch++) {
		c2->cd((4*dr)+ij+ch+1);
        int iiter =0;
        int npeak = 0;
        double peakht[npeakMx]={0};
        double parv[7+npeakMx]={0.};
        double peakpos[npeakMx]={0};
        while(npeak<npeakMx/2. && iiter<6) { 
          iiter++;
          //c2->cd((4*dr)+ch+1);
          double prepos=-1;
          double preht=-1;
          double prepreht=-1;
          //          int npeak = 0;
          double peakpos[npeakMx]={0};
          for (int ix=0; ix<7; ix++) {parv[ix]=0;}
          for (int ix=0; ix<npeakMx; ix++) { peakht[ix] = parv[7+ix]=peakpos[ix]=0;}
          cout<<"nbinx "<< hist_peak[ij][dr][ch]->GetName()<<" "<<hist_peak[ij][dr][ch]->GetNbinsX()<<" "<<hist_peak2[ij][dr][ch]->GetNbinsX()<<endl;
          for (int jk=1; npeak<npeakMx && jk<hist_peak2[ij][dr][ch]->GetNbinsX(); jk++) {
            double xval = hist_peak2[ij][dr][ch]->GetBinCenter(jk);
            double yval = hist_peak2[ij][dr][ch]->GetBinContent(jk);
						//if (npeak>0) {cout <<"jk "<<jk<<" "<<xval<<" "<<preht<<" "<<yval<<" "<<prepreht<<" "<<prepos<<" rat "<<int((preht/irebin)>2.0)<<" "<<int(preht>prepreht)<<" "<<npeak<<" "<<(preht/irebin)<<" "<<0.05*peakht[npeak-1]<<endl;}
            if (jk>1 && preht> yval && prepos>-0.1 && (preht/irebin)>1.0 && preht> prepreht && ((npeak==0 &&  (preht/irebin)>2.0) || (npeak>0 && (preht/irebin)>(0.1-iiter*0.01)*peakht[npeak-1]))) {
              peakpos[npeak] = prepos;
              peakht[npeak] = 2.0*preht/irebin;
              cout <<"npeak "<<npeak<<" "<<peakpos[npeak]<<" "<<peakht[npeak]<<endl;
              npeak++;
            }
            prepreht = preht;
            preht = yval;
            prepos = xval;
         }
          
          double parv1[7+npeakMx]={1, 0.1, 0.05, peakpos[0], (peakpos[npeak-1]-peakpos[0])/(npeak-1),
                                   0.006+0.001*ij, 1.0, 1.2*peakht[0], peakht[1], peakht[2], peakht[3], peakht[4], peakht[5], peakht[6],
                                   peakht[7]};//, peakht[8], peakht[9]};//, peakht[10], peakht[11]};
          
          for(int lm=0; lm<7+npeak; lm++){
            parv[lm]=parv1[lm];
          }	 
        }
        sprintf(name, "fitv_hv%i_dr%i_ch%i", ij, dr, ch);
        fitv[ij][dr][ch] = new TF1(name, totalfunc, -0.1, 0.65 + 0.65*ij, 7+npeakMx);
        cout<<"name "<<name<<" "<<peakpos[npeak-1]+0.5*parv[4]<<endl;
        fitv[ij][dr][ch]->SetParameters(parv);
        fitv[ij][dr][ch]->SetParLimits(0, 0.001*parv[0], 1000.0*parv[0]);
        fitv[ij][dr][ch]->SetParLimits(1, 0.005, 2.5);
        fitv[ij][dr][ch]->SetParLimits(2, 0.007, 1.5);
        fitv[ij][dr][ch]->SetParLimits(3, -0.1, 0.1);
        fitv[ij][dr][ch]->SetParLimits(4, 0.05 + (ij*0.02), 0.35 + (ij*0.1));
        fitv[ij][dr][ch]->SetParLimits(5, 0.005, 0.025);
        fitv[ij][dr][ch]->SetParLimits(6, 0.05, 3.0);
        for (int jk=7; jk<7+npeak; jk++) {
          fitv[ij][dr][ch]->SetParLimits(jk, 0.05*parv[jk], 3.5*parv[jk]);
        }
        //        file_out<<"ij00 "<<ij<<" "<<<<dr<<" "<<ch<<" "<<npeak<<" "<<parv1[5]<<" "<<parv1[6]<<endl;
				
        for (int jk=0; jk<7+npeak; jk++) {
          cout<<jk<<"="<<parv[jk]<<" ";
        }
			 
        cout<<endl;
        
        hist_peak[ij][dr][ch]->GetYaxis()->SetLabelSize(.1);
        hist_peak[ij][dr][ch]->GetYaxis()->SetNdivisions(606);
        //hist_peak[ij][dr][ch]->SetLineColor(kBlack);
        hist_peak[ij][dr][ch]->GetXaxis()->SetLabelSize(.1);
        hist_peak[ij][dr][ch]->GetXaxis()->SetLabelOffset(.0001);
        hist_peak[ij][dr][ch]->GetXaxis()->SetLabelColor(kBlack);
        hist_peak[ij][dr][ch]->GetXaxis()->SetTitle("Charge (pC)");
        hist_peak[ij][dr][ch]->GetXaxis()->SetTitleSize(.1);
        hist_peak[ij][dr][ch]->GetXaxis()->SetTitleOffset(0.78);
        hist_peak[ij][dr][ch]->GetXaxis()->SetTitleFont(42);
        hist_peak[ij][dr][ch]->GetXaxis()->SetLabelFont(42);
        hist_peak[ij][dr][ch]->GetYaxis()->SetLabelFont(42);
        hist_peak[ij][dr][ch]->GetXaxis()->CenterTitle();
        hist_peak[ij][dr][ch]->GetXaxis()->SetTitleColor(kBlack);
        hist_peak[ij][dr][ch]->GetXaxis()->SetNdivisions(606);
        hist_peak[ij][dr][ch]->GetYaxis()->SetNdivisions(504);
        hist_peak[ij][dr][ch]->GetXaxis()->SetRangeUser(-0.1, 2.5);
        //hist_peak[ij][dr][ch]->GetXaxis()->SetRangeUser(-0.1, 0.65 + 0.65*ij);
        hist_peak[ij][dr][ch]->Draw();
        if (npeak>=npeakMx/2.) { 
          hist_peak[ij][dr][ch]->Fit(name, "LBMR");
          avg_gp[dr][ch][ij] = fitv[ij][dr][ch]->GetParameter(4);
          avg_gperr[dr][ch][ij] = fitv[ij][dr][ch]->GetParError(4);
          
          //				file_out<<"ij00 "<<ij<<" "<<dr<<" "<<ch<<" "<<fitv[ij][dr][ch]->GetParameter(5)<<" "<<fitv[ij][dr][ch]->GetParameter(6)<<endl;
          
          cout<<"hhhhhhhhhhhhhhhhhhhhhhh    "<<ij<<"   "<<HV[ij]<<"    "<<avg_gp[dr][ch][ij]<<"   "<<avg_gperr[dr][ch][ij]<<endl;
          latex.DrawLatex(0.6, 0.72, Form("V_{OV} : %gV", int(100*OV[ij])/100.));
          latex.DrawLatex(0.6, 0.62, Form("DRS : %i", dr));
          latex.DrawLatex(0.6, 0.52, Form("CH : %i", ch));
          latex.DrawLatex(0.4, 0.82, Form("(%g #pm %g) pC", int(10000*avg_gp[dr][ch][ij])/10000., int(10000*avg_gperr[dr][ch][ij])/10000. ));
        }
      }
    }
  }
  c2->Update();
  c2->SaveAs("charge_spec.png");
 // ps.NewPage(); 
 // if(c2) delete c2; 
  for (int ij=0; ij<nhv; ij++) {
    for(int dr=0; dr<n_drs; dr++){
      for (int ch=0 ; ch<n_chnls ; ch++) { 
        gain[dr][ch][ij] = (avg_gp[dr][ch][ij]*tenpseven)/1.6;
        gainerr[dr][ch][ij] = (avg_gperr[dr][ch][ij]*tenpseven)/1.6;
        cout<<ij<<"   "<<HV[ij]<<"    "<<gain[dr][ch][ij]<<"   "<<(gainerr[dr][ch][ij]*tenpseven)/1.6<<endl;
      }
    }
  }
  gStyle->SetPadRightMargin(0.005);
  gStyle->SetPadLeftMargin(0.3);
  gStyle->SetPadTopMargin(0.09);
  gStyle->SetPadBottomMargin(0.15);
  TF1* polf[n_drs][n_chnls];
  TGraphErrors* gr[n_drs][n_chnls];
  TCanvas *c1 = new TCanvas("c2", "SiPM", 500, 400);
  c1->Divide(1,1,1.e-5, 1.e-5); 
  ps.NewPage(); 
  for(int dr=0; dr<n_drs; dr++){
    for (int ch=0 ; ch<n_chnls ; ch++) { 
      c1->cd((4*dr)+ch+1);
      gr[dr][ch] = new TGraphErrors(nhv, HV, avg_gp[dr][ch], errx, avg_gperr[dr][ch]);
      gr[dr][ch]->GetXaxis()->CenterTitle();
      gr[dr][ch]->GetYaxis()->CenterTitle();
      sprintf(title,"DRS_%i_CH_%i", dr, ch);
      gr[dr][ch]->SetTitle(title);
      gr[dr][ch]->GetXaxis()->SetTitleSize(0.08);	  
      gr[dr][ch]->GetYaxis()->SetTitleSize(0.08);	
      gr[dr][ch]->GetXaxis()->SetTitleOffset(0.88);  
      gr[dr][ch]->GetYaxis()->SetTitleOffset(1.5);  
      gr[dr][ch]->GetYaxis()->SetLabelOffset(0.011);
      gr[dr][ch]->GetXaxis()->SetLabelOffset(0.0001);
      gr[dr][ch]->GetXaxis()->SetLabelSize(0.08);
      gr[dr][ch]->GetYaxis()->SetLabelSize(0.08);
      gr[dr][ch]->GetXaxis()->SetNdivisions(103);
      gr[dr][ch]->GetYaxis()->SetNdivisions(606);
      gr[dr][ch]->SetMarkerSize(1.2);
      gr[dr][ch]->SetMarkerStyle(kCircle);
      gr[dr][ch]->SetMarkerColor(kBlue);
      gr[dr][ch]->GetXaxis()->SetTitle("Bias Voltage (V)");
      gr[dr][ch]->GetYaxis()->SetTitle("Avg gap b/w peaks (pC)");
      gr[dr][ch]->Draw("AP");
      sprintf(name,"polf_%i_%i", dr, ch);
      polf[dr][ch] = new TF1(name, polfunc, HV[0]-0.5, HV[nhv-1]+0.5, 2);
      gr[dr][ch]->Fit(name);
      double p0 = polf[dr][ch]->GetParameter(0);
      double p1 = polf[dr][ch]->GetParameter(1);
      double p0err = polf[dr][ch]->GetParError(0);
      double p1err = polf[dr][ch]->GetParError(1);
      double BD_V = -(p0/p1);
      double BD_V_err = ((p0err/p0)+(p1err/p1))*BD_V;
      latex.DrawLatex(0.35, 0.80, Form("(%g #pm %g)V",int(10000*BD_V)/10000., int(10000*BD_V_err)/10000.)); 
      //latex.DrawLatex(0.35, 0.70, Form("(%g #pm %g)V",int(10000*p1)/10000., int(10000*p1err)/10000.)); 
      
    }
  }
  c1->Update();
  c1->SaveAs("Calibration_p1.png");
  ps.NewPage(); 
  
  TF1* polf1[n_drs][n_chnls];
  TGraphErrors* gr1[n_drs][n_chnls];
  TCanvas *c3 = new TCanvas("c3", "SiPM", 500, 400);
  c3->Divide(1,1,1.e-5, 1.e-5); 
   ps.NewPage(); 
  for(int dr=0; dr<n_drs; dr++){
    for (int ch=0 ; ch<n_chnls ; ch++) { 
      c3->cd((4*dr)+ch+1);
      gr1[dr][ch] = new TGraphErrors(nhv, HV, gain[dr][ch], errx, gainerr[dr][ch]);
      gr1[dr][ch]->GetXaxis()->CenterTitle();
      gr1[dr][ch]->GetYaxis()->CenterTitle();
      sprintf(title,"DRS%i_CH_%i", dr, ch);
      gr1[dr][ch]->SetTitle(title);
      gr1[dr][ch]->GetXaxis()->SetTitleSize(0.08);	  
      gr1[dr][ch]->GetYaxis()->SetTitleSize(0.08);	
      gr1[dr][ch]->GetXaxis()->SetTitleOffset(0.88);  
      gr1[dr][ch]->GetYaxis()->SetTitleOffset(1.6);  
      gr1[dr][ch]->GetYaxis()->SetLabelOffset(0.011);
      gr1[dr][ch]->GetXaxis()->SetLabelOffset(0.0001);
      gr1[dr][ch]->GetXaxis()->SetLabelSize(0.08);
      gr1[dr][ch]->GetYaxis()->SetLabelSize(0.08);
      gr1[dr][ch]->GetXaxis()->SetNdivisions(103);
      gr1[dr][ch]->GetYaxis()->SetNdivisions(606);
      gr1[dr][ch]->SetMarkerSize(1.2);
      gr1[dr][ch]->SetMarkerStyle(kCircle);
      gr1[dr][ch]->SetMarkerColor(kBlue);
      gr1[dr][ch]->GetXaxis()->SetTitle("Bias Voltage (V)");
      gr1[dr][ch]->GetYaxis()->SetTitle("Gain");
      gr1[dr][ch]->Draw("AP");
      sprintf(name,"polf1_%i_%i", dr, ch);
      polf1[dr][ch] = new TF1(name, polfunc, HV[0]-0.5, HV[nhv-1]+0.5, 2);
      gr1[dr][ch]->Fit(name);
      double p0 = polf1[dr][ch]->GetParameter(0);
      double p1 = polf1[dr][ch]->GetParameter(1);
      double p0err = polf1[dr][ch]->GetParError(0);
      double p1err = polf1[dr][ch]->GetParError(1);
      double BD_V = -(p0/p1);
      double BD_V_err = -((p0err/p0)+(p1err/p1))*BD_V;
      //MyFile<<int(1000*BD_V)/1000.<<","<<int(1000*BD_V_err)/1000.<<","<<setprecision(3)<<int(1*p1)/1000000.<<","<<int(1*p1err)/1000.<<endl;
      MyFile<<setprecision(5)<<int(1000*BD_V)/1000.<<","<<setprecision(3)<<int(1000*BD_V_err)/1000.<<","<<setprecision(3)<<int(1*p1)/1000000.<<","<<setprecision(6)<<int(1*p1err)/1000000.<<endl;
      latex.DrawLatex(0.35, 0.80, Form("(%g #pm %g)V",int(10000*BD_V)/100000., int(10000*BD_V_err)/10000.)); 
      latex.DrawLatex(0.35, 0.70, Form("(%g #pm %g)V",int(1*p1)/1., int(1*p1err)/1.)); 
      
    }
  }
  //c3->Update();
  c3->SaveAs("Calibration_p2.png");
  
  
  ps.Close();
  MyFile.close();
  //  file_out.close();
}

