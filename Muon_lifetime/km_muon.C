void lifetime ()
{
 char file[50];
 int x,d1,d2,d3,d4,d5,d6;
 char r1,r2,r3,r4;
	TCanvas *c1 = new TCanvas("c1","Dynamic Filling Example",1920,1080);
	     c1->SetFillColor(42);
	TH1F *hpx = new TH1F("hpx","Muon Life time",86,0,260);
	    hpx->GetXaxis()->SetTitle("Time in units of 0.1 microsecond");
	    hpx->GetYaxis()->SetTitle("Counts");
	    hpx->GetXaxis()->CenterTitle();
	    hpx->GetYaxis()->CenterTitle();
	    hpx->Draw();
	TF1 *func = new TF1("func","[0]*exp(-0.1/[1]*x)+[2]",0,260);
		func->SetParName(0,"Coeff of exp");
		func->SetParName(1,"Lifetime of muon in microsecond");
		func->SetParName(2,"Constant");
		func->SetParameter(0,16);//16
		func->SetParameter(1,0.05);//0.05
		func->SetParameter(2,3);//3

	cout<<"Enter the file name=";
	cin>>file;
	ifstream in;
	in.open(file);

while (!in.eof())
 {
    in>>x>>d1>>r1>>d2>>r2>>d3>>d4>>r3>>d5>>r4>>d6;
    if(in.eof()) break;
      if(x!=0){
	       hpx->Fill(x);
	             }
 }

hpx->Fit("func");
}
