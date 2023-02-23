
// Current Plots
// <Run#><HV><BT01Y><BT01X><BT02Y><BT02X><AB22Y><AB22X>

////g++ anal_inodaq_v6.cc  `root-config --cflags` `root-config --glibs` -lMinuit -o analyse


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

#include "langaufit.h"

using namespace std;

void analyse() {

  gStyle->SetPalette(1,0);
  
  gStyle->SetStatColor(10);
  //  gStyle->SetTitleFillColor(10);
  gStyle->SetLabelColor(1, "XYZ");
  //   gStyle->SetTitleBorderSize(1);
  
  gStyle->SetStatFont(22);        // Times New Roman
  gStyle->SetTextFont(22);        // Times New Roman
  gStyle->SetTitleFont(22,"XYZ"); // Times New Roman
  gStyle->SetLabelFont(22,"XYZ"); // Times New Roman
  gStyle->SetLabelSize(0.070, "X"); // Times New Roman  
  gStyle->SetLabelSize(0.055, "YZ"); // Times New Roman  
  gStyle->SetNdivisions(606, "XYZ");
  gStyle->SetLabelOffset(0.002, "XYZ"); 
  gStyle->SetTitleYOffset(0.1); //, "XYZ"); 
  gStyle->SetTitleFontSize(0.075);
  
  gStyle->SetOptStat(1111);
  gStyle->SetOptFit(111); //(101); //sar change
  gStyle->SetOptLogy(0);//(1); //sar change
  gStyle->SetStatW(.18);
  gStyle->SetStatH(.14);
  gStyle->SetStatX(.99);
  gStyle->SetStatY(.99);
  
  gStyle->SetMarkerColor(2);
  gStyle->SetMarkerStyle(24);
  gStyle->SetMarkerSize(0.60);
  
  
  gStyle->SetPadTopMargin(0.11);
  gStyle->SetPadBottomMargin(0.12);
  gStyle->SetPadLeftMargin(0.14);
  gStyle->SetPadRightMargin(0.14);
  
  gStyle->SetPadGridX(1);
  gStyle->SetPadGridY(1);
  gStyle->SetGridStyle(3);
  gStyle->SetGridWidth(1);
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);

  char outfile[100];
  char outfilx[100];
  char infilelist[200];  
  char infile[200];

  char variab[200];
  char variab2[200];
  char name[100];
  char title[100];
  //  const int nrow=1; //3;
  const int ncol=16; //64;
  const int ncntmx=12; // individual count
  const int sizeymx=10000;
  float varx,varx1;
  int vary[16];
  int irun;

  const int neffi=8; //channels for efficiency measurement
  int effitab[neffi]={0,2,4,6,8,10,12,14};
  const char* namex[neffi]={"xx1", "xx2", "xx3", "X44", "Xx5", "xx6", "xx7", "xx8"};

  const int nQDC=3;
  const float qdcfact=0.25; //250fC = 0.25pC
  const char* QDC_name[nQDC]={"QCD1", "QCDC2", "QDC3"};
  
  const int nTDC=5;
  //const float tdcscale=0.025; //100ps = 0.1 ns //sar change
  const float tdcscale=0.1; //100ps = 0.1 ns

  const char* TDC_name[nTDC]={"H1", "H2", "H3", "H4", "H5"};

  vector<double> all_effitab[neffi];
  vector<double> all_qdcmean[nQDC];
  vector<double> all_streamfrac[nQDC];
  vector<double> all_tdcmean[nTDC];
  vector<double> all_tdcwidth[nTDC];

  ifstream comfile_db;
  ifstream file_db1;
  ifstream file_db2;

  TGaxis *axisx[1000];
  int iaxis=0;
  
  cout <<"Give the run number "<<endl;
  cin>> irun;
  sprintf(outfilx, "test%i_dd", irun);
  int len = strlen(outfilx);
  outfilx[len]='\0';
  
  sprintf (outfile,"%s.root",outfilx);
  TFile* fileOut = new TFile(outfile, "recreate");  
  
  sprintf (outfile,"%s.ps",outfilx);
  TPostScript pss(outfile,111); //(112);  
  pss.Range(20,28);
  
  sprintf (outfile,"%s.txt",outfilx);
  ofstream file_out(outfile);
  
  
  TH1F* h_hist1d[ncol];
  for (int jk=0; jk<ncol; jk++) {
    sprintf(name, "hist_%i", jk);
    sprintf(title, "Col%i", jk);
    h_hist1d[jk] = new TH1F(name, title, 120, 0.0, 1200.0);
  }
  
  //    const int nchnmx=8;
  TH1F* h_histtdc[nTDC];
  TH1F* h_indtdc[nTDC];		  
  for (int ij=0; ij<nTDC; ij++) {
    sprintf(name, "histtdc_%i", ij);
    sprintf(title, "TDC (ns) %s", TDC_name[ij]);
    //h_histtdc[ij] = new TH1F(name, title, 206, -0.5, 102.5); //sar change
    h_histtdc[ij] = new TH1F(name, title, 400, -0.75, 99.25);
    //h_histtdc[ij] = new TH1F(name, title, 240, -29.5, 30.5);  
  }
  
  TH1F* h_histadc[nQDC];
  TH1F* h_indadc[nQDC];   
  for (int ij=0; ij<nQDC; ij++) {
    sprintf(name, "histadc_%i", ij);
    sprintf(title, "QDC (pC) %s", QDC_name[ij]);
    h_histadc[ij] = new TH1F(name, title, 800, -0.75, 399.25);
  }
  
  vector<double>tdcch[nTDC];
  vector<int>adcch[nQDC];
  
  sprintf(infile, "eve%i.txt", irun);
  cout <<"input file name " <<infile<<endl;
  file_db2.open(infile);
  file_db2.getline(variab,200);
  cout <<"input file " <<variab<<endl;
  //  file_db2.getline(variab,200);
  int ncntx=0;
  //while(!(file_db2.eof()) && ncntx<10) {//sar mod
  while(!(file_db2.eof()) && ncntx<1000000000) {
    ncntx++;
    file_db2.getline(variab2,200);
    if(file_db2.eof()) break;
    sscanf(variab2, "%d %d %d %d %d", &vary[0], &vary[1], &vary[2], &vary[3], &vary[4]);
    
    for (int ij=0; ij<nTDC; ij++) { //TDC
      //cout <<"tdcij "<< ij<<" "<<vary[ij]<<endl;
      tdcch[ij].push_back(tdcscale*vary[ij]);
      h_histtdc[ij]->Fill(tdcscale*vary[ij]);
    }
    
    file_db2.getline(variab2,200); 
    sscanf(variab2, "%d %d %d", &vary[0], &vary[1], &vary[2]); 
    for (int ij=0; ij<nQDC; ij++) {
      adcch[ij].push_back(qdcfact*vary[ij]);
      h_histadc[ij]->Fill(qdcfact*vary[ij]);
      //cout <<"qdcij "<< ij<<" "<<vary[ij]<<endl;
    }
    file_db2.getline(variab,200);//      file_db2>>varx;file_db2>>varx;
  }
  file_db2.close();
  
  int nsize = adcch[0].size();
  //  cin>>nsize;
  for (int ij=0; ij<nTDC; ij++) {
    sprintf(name, "histindtdc_%i", ij);
    sprintf(title, "TDC (ns) %s", TDC_name[ij]);
    h_indtdc[ij] = new TH1F(name, title, nsize, -0.5, nsize-0.5);
    
    for (int jk=0; jk<nsize; jk++) {
      h_indtdc[ij]->Fill(jk, tdcch[ij][jk]); 
    }
  }
  
  for (int ij=0; ij<nQDC; ij++) {
    sprintf(name, "histindadc_%i", ij);
    sprintf(title, "QDC ch%s", QDC_name[ij]);
    h_indadc[ij] = new TH1F(name, title, nsize, -0.5, nsize-0.5);
    
    for (int jk=0; jk<nsize; jk++) {
      h_indadc[ij]->Fill(jk, adcch[ij][jk]); 
    }
  }
  
  int icnt=0;
  //Book histogrammes for noise rate  
  TH1F* h_indhist1d[ncol];
  for (int jk=0; jk<ncol; jk++) {
    sprintf(name, "indhist_%i", jk);
    sprintf(title, "Ind Col%i", jk);
    h_indhist1d[jk] = new TH1F(name, title, nsize, -0.5, nsize-0.5);
  }
  
  //Book histogrammes for xxxx count rate
  TH1F* h_indcount[ncntmx];
  for (int ij=0; ij<ncntmx; ij++) {
    sprintf(name, "indcount_%i", ij);
    sprintf(title, "Ind Count %i", ij);
    h_indcount[ij] = new TH1F(name, title, nsize, -0.5, nsize-0.5);
  }
  
  sprintf(infile, "mon%i.txt", irun);
  file_db1.open(infile);
  cout <<"infile " <<endl;
  while(!(file_db1.eof()) && icnt<10) {
    file_db1.getline(variab,200);
    cout <<"variab "<<variab<<endl;
    if(file_db1.eof()) break;
    
    for (int jk=0; jk<ncol; jk++) {
      file_db1>>varx; // h_hist1d[jk];
      cout <<"jk " <<jk<<" "<<icnt<<" "<<varx<<endl;
      h_hist1d[jk]->Fill(varx);
      h_indhist1d[jk]->Fill(icnt, varx);
    }
  file_db1.getline(variab,200);
    icnt++;
  }
  file_db1.close();
  
  if (icnt>nsize) {
    cout<<"Noise count "<<icnt<<" is more than trigger count "<<nsize<<endl;
    cout<<"Increase the range of histogrammes h_indhist1d[ij][jk] and h_indcount[ij]"<<endl;
  }
  
  TCanvas* c1= new TCanvas("c1", "c1", 600, 800);
  //c1->Divide(2, 3, 1.e-6, 1.e-6);//sar change
  c1->Divide(1, 2, 1.e-6, 1.e-6);
  
  TCanvas* c2= new TCanvas("c2", "c2", 600, 800);
  //c2->Divide(2, 5, 1.e-6, 1.e-6);//sar change
  c2->Divide(1, 2, 1.e-6, 1.e-6);

  pss.NewPage();
  //for (int jk=0; jk<nQDC; jk++) {
  //  c1->cd(2*jk+1);
  //  h_histadc[jk]->Draw();
  //  c1->cd(2*jk+2);
  //  h_indadc[jk]->Draw();
  //}//sar change
  
  //TF1 *fitQDC = new TF1("fitQDC",XXXXX,h_hisatdc[0]->GetXaxis()->GetXmin(),h_histadc[0]->GetXaxis()->GetXmax());
  TF1 *fitADC = new TF1("fitADC",langaufun,2.0,100.0,4);
  fitADC->SetParNames("Width","MPV","Area","GSigma");
  for (int jk=0; jk<nQDC; jk++) {
    if(jk==1){//plotting only channel 0
      c1->cd(1);
      h_histadc[jk]->GetXaxis()->SetRangeUser(-0.75, 100.);
      h_histadc[jk]->Draw();
    
      fitADC->SetParameter(1,h_histadc[jk]->GetMean());
      fitADC->SetParameter(3,h_histadc[jk]->GetStdDev());
      fitADC->SetParameter(0,1.0);
      fitADC->SetParameter(1,22.0);
      fitADC->SetParameter(3,1.0);
      //h_histadc[jk]->Fit("fitADC","","",2., 100.);
      h_histadc[jk]->Fit("fitADC","","",2., 80.);
      c1->cd(2);
      h_indadc[jk]->Draw();
      break;
    }
  }
  c1->Update();

  pss.NewPage();
  //for (int jk=0; jk<nTDC; jk++) {
  //  c2->cd(2*jk+1);
  //  h_histtdc[jk]->Draw();
  //  c2->cd(2*jk+2);
  //  h_indtdc[jk]->Draw();   
  //} //sar change
  
  TF1 *fitTDC = new TF1("fitTDC","gaus",h_histtdc[0]->GetXaxis()->GetXmin(),h_histtdc[0]->GetXaxis()->GetXmax());
  
  for (int jk=0; jk<nTDC; jk++) {
    if(jk==1){//plotting only channel 0
      c2->cd(1);
      h_histtdc[jk]->Draw();
      //h_histtdc[jk]->Fit("gaus");
      fitTDC->SetParameter(1,h_histtdc[jk]->GetMean());
      fitTDC->SetParameter(2,h_histtdc[jk]->GetStdDev());
      h_histtdc[jk]->Fit("fitTDC");
      //h_histtdc[jk]->Fit("fitTDC","","",5., 80.);
      c2->cd(2);
      h_indtdc[jk]->Draw();  
      break;
    }
  }
  c2->Update();
  
  //pss.NewPage();
  //TCanvas* c3= new TCanvas("c3", "c3", 600, 800);
  //c3->Divide(4, 8, 1.e-6, 1.e-6);

  //for (int jk=0; jk<ncol; jk++) {
  //  c3->cd(2*jk+1);
  //  h_hist1d[jk]->Draw();
  //  c3->cd(2*jk+2);
  //  h_indhist1d[jk]->Draw();   
  //}
  //c3->Update(); 
  


  
  pss.Close();
  fileOut->Write();
  fileOut->Close();
  
}

