void define_plot_style();

void plot_timing(TString filename="./run6127_140V.root", TString outdir="./timing_plots/"){
	define_plot_style();

	TFile *infile = new TFile(filename);

	TTree *crtbasicInfoTree = 0;
    infile->GetObject("dumpall/CRTHitBasicInfoTree", crtbasicInfoTree);

    int run;
    std::vector<int> * febid = new std::vector<int>(); std::vector<int> *channelA = new std::vector<int>(); std::vector<int> *channelB = new std::vector<int>();
    std::vector<int> * adcA  = new std::vector<int>(); std::vector<int> *adcB = new std::vector<int>(); 
    std::vector<uint> *ts0_hit = new std::vector<uint>(); std::vector<uint> *ts1_hit = new std::vector<uint>();

    if (crtbasicInfoTree) {
        crtbasicInfoTree->SetBranchAddress("febid", &febid);
        crtbasicInfoTree->SetBranchAddress("channelA", &channelA);
        crtbasicInfoTree->SetBranchAddress("run", &run);
        crtbasicInfoTree->SetBranchAddress("channelB", &channelB);
        crtbasicInfoTree->SetBranchAddress("adcA", &adcA);
        crtbasicInfoTree->SetBranchAddress("adcB", &adcB);
        crtbasicInfoTree->SetBranchAddress("ts0_hit", &ts0_hit);
        crtbasicInfoTree->SetBranchAddress("ts1_hit", &ts1_hit);
    }else{
        std::cout<<"corrupted file!"<<std::endl<<std::endl<<std::endl;
    }
    TH1F *h_t1_adc_pedstals = new TH1F("h_t1_adc_pedstals","", 100, 0, 1.2e9);
    TH1F *h_t1_adc = new TH1F("h_t1_adc","", 100, 0, 1.2e9);

    // loop through all hits. 
    Long64_t crt_info_tree_entries = crtbasicInfoTree->GetEntries();
    for (auto ii = 0; ii < 100; ii++){
    	crtbasicInfoTree->GetEntry(ii);
    	for (int iHit=0; iHit<febid->size(); iHit++){
	    	//if (febid->at(iHit)!=153) continue;
	    	h_t1_adc->Fill(ts1_hit->at(iHit));
	    	if ( adcA->at(iHit)<260){
	    		h_t1_adc_pedstals->Fill(ts1_hit->at(iHit));
	    	}else if (adcB->at(iHit)<260){
	    		h_t1_adc_pedstals->Fill(ts1_hit->at(iHit));
	    	} 
	    	
	    }
    }

    TCanvas *canvas = new TCanvas("timing","");
    h_t1_adc_pedstals->SetStats(0);
    h_t1_adc_pedstals->SetLineColor(kRed);
    h_t1_adc->SetLineColor(kBlack);
    h_t1_adc->Draw();
    h_t1_adc_pedstals->Draw("same");

	TLegend *leg = new TLegend(0.5, 0.8, 0.9, 0.9);//0.55, 0.2, 0.9, 0.3
	leg->SetTextFont(62); leg->SetTextSize(0.04);

	leg->SetBorderSize(0);
    leg->SetFillStyle(0); 
	//leg->SetNColumns(2);
	leg->AddEntry(h_t1_adc_pedstals, "ADC < 260", "l");
	leg->AddEntry(h_t1_adc, "ADC > 260", "l"); 
	leg->SetTextFont(62);
	leg->Draw("same");

	gSystem->Exec("mkdir -p "+outdir);
	gSystem->Exec("rm "+outdir+"*");

	canvas->SaveAs(outdir+"timing.pdf");
}


void define_plot_style(){
	// declare the global plotting style.
	gStyle->SetTitleFont(62, "TXYZ");
	gStyle->SetTitleSize(0.05,"TXYZ");

	// LABELS SIZE AND FONT
	gStyle->SetLabelFont(62, "TXYZ");
	gStyle->SetLabelSize(0.05,"TXYZ");

	// AXIS OFFSETS AND SIZES
	gStyle->SetTitleXOffset(0.80);
	gStyle->SetTitleXSize(0.06);

	gStyle->SetTitleYOffset(0.65);
	gStyle->SetTitleYSize(0.06);

	gStyle->SetMarkerStyle(33);
	gStyle->SetMarkerSize(1.5);
	gStyle->SetLineColor(46);
	gStyle->SetLineWidth(2);
	// Draw horizontal and vertical grids
	gStyle->SetPadGridX(kTRUE);                     
	gStyle->SetPadGridY(kTRUE);
}