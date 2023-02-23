
void plateau()
{
 
  const int i=8;

  double volt[i] = {1450,1500,1550,1600,1650,1700,1750,1800};
  double effi[i] = {0,0,0,0,0,0,0,0};
  double p1_noise[i] = {0,0,0,0,0,0,0,0};
  double p2_noise[i] = {0,0,0,0,0,0,0,0};
  double p3_noise[i] = {0,0,0,0,0,0,0,0};
  double test_noise[i] = {0,0,0,0,0,0,0,0};
  double time_reso[i] = {0,0,0,0,0,0,0,0};
  double time_shift[i] = {0,0,0,0,0,0,0,0};
  double charge[i] = {0,0,0,0,0,0,0,0};
  
  /*
  double volt[i] = {1450,1500,1550,1600,1650,1700,1750,1800};
  {0,0,0,0,0,0,0,0};
  double effi[i] = {6.49,17.05,40.39,67.05,87.15,96.42,97.96,98.80};
  double p1_noise[i] = {11.24,10.90,11.17,11.31,11.28,11.08,10.82,11.11};
  double p2_noise[i] = {18.58,18.67,19.19,19.58,19.63,19.66,19.51,19.42};
  double p3_noise[i] = {13.81,13.59,13.87,13.72,13.42,13.92,13.83,13.63};
  double test_noise[i] = {0.69,1.67,3.36,5.60,9.11,15.71,24.33,37.51};
  double time_reso[i] = {2.229,1.479,1.775,1.819,1.774,1.669,1.129,1.039};
  double time_shift[i] = {49.89,48.83,48.16,47.28,46.12,44.64,43.25,42.58};
  double charge[i] = {15.2,16.74,18.67,21.1,24.24,28.19,33.59,39.37};
  */
  
{  
  auto  mycanvas1 = new TCanvas(); 
  TGraph* gr1 = new TGraph(i,volt,effi);
   
  gr1->SetMarkerSize(1.0);
  gr1->SetMarkerColor(4);
  gr1->SetMarkerStyle(21);
  gr1->SetTitle("Paddle plateau");
  gr1->GetXaxis()->SetTitle("HV");
  gr1->GetXaxis()->CenterTitle();
  gr1->GetYaxis()->SetTitle("Efficiency %");
  gr1->GetYaxis()->CenterTitle();
  gr1->Draw("ACP");
  mycanvas1->Print("paddle-plateau-20230213.pdf");
 }

{
  auto  mycanvas2 = new TCanvas(); 
  TGraph* gr2 = new TGraph(i,volt,p1_noise);
  
  gr2->SetMarkerSize(1.0);
  gr2->SetMarkerColor(4);
  gr2->SetMarkerStyle(21);
  gr2->SetTitle("Paddle 1 noise rate");
  gr2->GetXaxis()->SetTitle("HV");
  gr2->GetXaxis()->CenterTitle();
  gr2->GetYaxis()->SetTitle("Noise rate (Hz)");
    gr2->GetYaxis()->SetRangeUser(0,30);
  gr2->GetYaxis()->CenterTitle();
  gr2->Draw("ACP");
  mycanvas2->Print("paddle1-Noise-rate-20230213.pdf");
}

{
  auto  mycanvas3 = new TCanvas(); 
  TGraph* gr3 = new TGraph(i,volt,p2_noise);
  
  gr3->SetMarkerSize(1.0);
  gr3->SetMarkerColor(4);
  gr3->SetMarkerStyle(21);
  gr3->SetTitle("Paddle 2 noise rate");
  gr3->GetXaxis()->SetTitle("HV");
  gr3->GetXaxis()->CenterTitle();
  gr3->GetYaxis()->SetRangeUser(0,30);
  gr3->GetYaxis()->SetTitle("Noise rate (Hz)");
  gr3->GetYaxis()->CenterTitle();
  gr3->Draw("ACP");
  mycanvas3->Print("paddle2-noise-rate-20230213.pdf"); 
}

{
  auto  mycanvas4 = new TCanvas();
  TGraph* gr4 = new TGraph(i,volt,p3_noise);
 
  gr4->SetMarkerSize(1.0);
  gr4->SetMarkerColor(4);
  gr4->SetMarkerStyle(21);
  gr4->SetTitle("Paddle 3 noise rate");
  gr4->GetXaxis()->SetTitle("HV");
  gr4->GetXaxis()->CenterTitle();
    gr4->GetYaxis()->SetRangeUser(0,30);
  gr4->GetYaxis()->SetTitle("Noise rate (Hz)");
  gr4->GetYaxis()->CenterTitle();
  gr4->Draw("ACP");
  mycanvas4->Print("paddle3-noise-rate-20230213.pdf"); 
}

{
  auto  mycanvas5 = new TCanvas();  
  TGraph* gr5 = new TGraph(i,volt,test_noise);
  gr5->SetMarkerSize(1.0);
  gr5->SetMarkerColor(4);
  gr5->SetMarkerStyle(21);
  gr5->SetTitle("Test paddle noise rate");
  gr5->GetXaxis()->SetTitle("HV");
  gr5->GetXaxis()->CenterTitle();
  gr5->GetYaxis()->SetTitle("Noise rate (Hz)");
  gr5->GetYaxis()->CenterTitle();
  gr5->Draw("ACP");
  mycanvas5->Print("test-paddle-noise-rate-20230213.pdf");
}

{
  auto  mycanvas5 = new TCanvas();  
  TGraph* gr5 = new TGraph(i,volt,time_reso);
  gr5->SetMarkerSize(1.0);
  gr5->SetMarkerColor(4);
  gr5->SetMarkerStyle(21);
  gr5->SetTitle("Time resolution");
  gr5->GetXaxis()->SetTitle("HV");
  gr5->GetXaxis()->CenterTitle();
  gr5->GetYaxis()->SetTitle("Time(ns)");
  gr5->GetYaxis()->CenterTitle();
  gr5->Draw("ACP");
  mycanvas5->Print("Time-resolution-20230213.pdf");
}

{
  auto  mycanvas5 = new TCanvas();  
  TGraph* gr5 = new TGraph(i,volt,time_shift);
  gr5->SetMarkerSize(1.0);
  gr5->SetMarkerColor(4);
  gr5->SetMarkerStyle(21);
  gr5->SetTitle("Time shift");
  gr5->GetXaxis()->SetTitle("HV");
  gr5->GetXaxis()->CenterTitle();
  gr5->GetYaxis()->SetTitle("Time (ns)");
  gr5->GetYaxis()->CenterTitle();
  gr5->Draw("ACP");
  mycanvas5->Print("Time-shift-20230213.pdf");
}

{
  auto  mycanvas5 = new TCanvas();  
  TGraph* gr5 = new TGraph(i,volt,charge);
  gr5->SetMarkerSize(1.0);
  gr5->SetMarkerColor(4);
  gr5->SetMarkerStyle(21);
  gr5->SetTitle("Charge profile");
  gr5->GetXaxis()->SetTitle("HV");
  gr5->GetXaxis()->CenterTitle();
  gr5->GetYaxis()->SetTitle("Charge (pc)");
  gr5->GetYaxis()->CenterTitle();
  gr5->Draw("ACP");
  mycanvas5->Print("charge-20230213.pdf");
}

 }
