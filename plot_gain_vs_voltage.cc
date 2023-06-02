TChain * CreateChainFromList_opt(TString ListFileName, std::string ChainName, bool DoCheck);
void define_plot_style();
float target_voltage_cal(float slope, float offset, float gain);

void plot_gain_vs_voltage(TString FileList="./List_of_files", TString savedir="./final_plots", TString savedir2="./raw_final_plots", float target_gain=68.64){	
	gSystem->Exec("rm "+savedir+"/*");
	gSystem->Exec("rm "+savedir2+"/*");
	define_plot_style();
	TChain * chain = CreateChainFromList_opt(FileList, "dumpall/CRTFitInfoTree", false);

	Int_t bias_voltage, febid, channel;
	Float_t gain, gain_err, chisqr, ndf;

	chain->SetBranchAddress("fbias_voltage",&bias_voltage);
	chain->SetBranchAddress("FEB_id",  &febid);
	chain->SetBranchAddress("channel", &channel);
	chain->SetBranchAddress("gain", &gain);
	chain->SetBranchAddress("gain_err", &gain_err);
	chain->SetBranchAddress("chisqr", &chisqr);
	chain->SetBranchAddress("ndf", &ndf);

	auto nEnt = chain->GetEntries();
	std::cout<<"Need to loop over "<<nEnt<<" entries."<<std::endl;
	std::vector<int> FEB_vec;
	for(auto i=0; i<nEnt; i++){
	  chain->GetEntry(i);
	  FEB_vec.push_back(febid);
	}

	std::sort(FEB_vec.begin(), FEB_vec.end());
  FEB_vec.erase(std::unique(FEB_vec.begin(), FEB_vec.end()), FEB_vec.end()); // Remove duplicate values from vector

	//int counter =0;
	for (int iFEB=0; iFEB<FEB_vec.size(); iFEB++){ // loop through the FEBs FEB_vec.size()
		ofstream outfile;
		outfile.open(Form("./target_voltage_FEB%i.txt", FEB_vec[iFEB]));
		outfile<<"Channel \t targeted_voltage"<<std::endl;

		//if(FEB_vec[iFEB]!=181) continue;
		for (int iChannel = 0; iChannel < 32; iChannel++){
			
			std::vector<float> bias_voltage_vec;
			std::vector<float> gain_vec;
			std::vector<float> gain_err_vec;

			std::vector<float> gain_vec_raw;
			std::vector<float> gain_err_vec_raw;
			std::vector<float> bias_voltage_vec_raw;

			bool doFitPlot=false;
			for(auto i=0; i<nEnt; i++){
				chain->GetEntry(i);
				if (febid== FEB_vec[iFEB] && channel==iChannel ){//
					bias_voltage_vec_raw.push_back(bias_voltage);
					gain_vec_raw.push_back(gain);
					gain_err_vec_raw.push_back(gain_err);
					
					// sanity check for the gain value makes sense or not.
					//if ( (ndf!=0 && chisqr/ndf < 100.0) || (ndf==0) ){
						float targeted_gain=0.4*bias_voltage;
						if (gain<targeted_gain+20 && gain>targeted_gain-20){
							bias_voltage_vec.push_back(bias_voltage);
							gain_vec.push_back(gain);
							gain_err_vec.push_back(gain_err);
							doFitPlot=true;
						}
					//}
				}
			}

			TString histo_title= Form("FEB: %i, channel: %i", FEB_vec[iFEB], iChannel);
			auto c2 = new TCanvas("c2","",200,10,800,500);
			TGraphErrors* gr_mean2 = new TGraphErrors(bias_voltage_vec_raw.size(), &(bias_voltage_vec_raw[0]), &(gain_vec_raw[0]), 0, &(gain_err_vec_raw[0]));

			gr_mean2->SetTitle(histo_title);  
			gr_mean2->GetXaxis()->SetTitle("Bias Voltage");
			gr_mean2->GetYaxis()->SetTitle("Fitted Gain Value");
			
			gr_mean2->Draw("AP"); 
			gr_mean2->GetYaxis()->SetRangeUser(40, 170);

			TPaveText* pt = new TPaveText(0.15, 0.8, 0.65, 0.9, "NDC");
			pt->SetFillColor(0);
			pt->SetTextAlign(22);
			pt->SetTextSize(0.06);
			pt->SetTextColor(kRed);
			pt->AddText("Raw gain: "+histo_title);
			pt->Draw();

			gSystem->Exec("mkdir -p "+savedir2);
			c2->SaveAs(Form(savedir2+"/FEB_%i_channel_%i.pdf", FEB_vec[iFEB], iChannel));
			c2->SaveAs(Form(savedir2+"/FEB_%i_channel_%i.png", FEB_vec[iFEB], iChannel));

			if (doFitPlot){
				
				/*bool isFirstErased=false;
				int nErased=0;
				for (int i=0; i<gain_vec.size()-1; i++){
					float thisGain=gain_vec[i];
					float nextGain=gain_vec[i+1];
					if(i==0 && thisGain>nextGain){
						bias_voltage_vec_new.erase(bias_voltage_vec_new.begin());
						gain_vec_new.erase(gain_vec_new.begin());
						gain_err_vec_new.erase(gain_err_vec_new.begin());
						nErased++;
					}else if(thisGain>nextGain){
						bias_voltage_vec_new.erase(bias_voltage_vec_new.begin()+i+1-nErased);
						gain_vec_new.erase(gain_vec_new.begin()+i+1-nErased);
						gain_err_vec_new.erase(gain_err_vec_new.begin()+i+1-nErased);
						nErased++;
						if (i<gain_vec.size()-2) {
							nextGain=gain_vec[i+2];
							if(thisGain>nextGain){
								//std::cout<<"3! "<<nextGain<<", "<<gain_vec_new.at(i+2-nErased);
								bias_voltage_vec_new.erase(bias_voltage_vec_new.begin()+i+2-nErased);
								gain_vec_new.erase(gain_vec_new.begin()+i+2-nErased);
								gain_err_vec_new.erase(gain_err_vec_new.begin()+i+2-nErased);
							}	
						}
					}
				}*/

				/*std::cout<<"Print new gain vector:"<<std::endl;
				for(int i=0; i<gain_vec_new.size();i++){
					std::cout<<gain_vec_new.at(i)<<", ";
				}
				std::cout<<std::endl;*/


				// Save the plots without abondoning points.
				auto c1 = new TCanvas("c1","",200,10,800,500);
				TGraphErrors* gr_mean = new TGraphErrors(bias_voltage_vec.size(), &(bias_voltage_vec[0]), &(gain_vec[0]), 0, &(gain_err_vec[0]));

				int bias_volatage_vector_size = bias_voltage_vec.size();
	  		TF1 *fit = new TF1("fit","[0] + [1]*x", bias_voltage_vec[0]-0.25, bias_voltage_vec[bias_volatage_vector_size-1]+0.25);

				gStyle->SetOptStat(0100);
				gStyle->SetOptFit(1111);
				gStyle->SetStatX(0.5);
				gStyle->SetStatY(0.9);
				gStyle->SetStatH(0.15);
				gStyle->SetStatW(0.2);

				gr_mean->SetTitle(histo_title);  
				gr_mean->GetXaxis()->SetTitle("Bias Voltage");
				gr_mean->GetYaxis()->SetTitle("Fitted Gain Value");
				
				gr_mean->Draw("AP");
	  		gr_mean->Fit(fit, "QR");  

	  		// store the fitting results. 
	  		fit->SetParName(0, "Slope");
	  		fit->SetParName(1, "Offset");
				float slope, offset;
			  slope = fit->GetParameter(1);  
			  offset = fit->GetParameter(0); 

			  float voltage = target_voltage_cal(slope, offset, target_gain);
			  outfile<<iChannel<<"\t"<<voltage<<std::endl;

				gSystem->Exec("mkdir -p "+savedir);
				c1->SaveAs(Form(savedir+"/FEB_%i_channel_%i.pdf", FEB_vec[iFEB], iChannel));
				c1->SaveAs(Form(savedir+"/FEB_%i_channel_%i.png", FEB_vec[iFEB], iChannel));

			}
			else{
				std::cerr<<Form("Go check: FEB %i Channel %i", FEB_vec[iFEB], iChannel)<<std::endl;
				//return;
			}
		}
		outfile.close(); 
	}
}



float target_voltage_cal(float slope, float offset, float gain){
		return (gain-offset)/slope;
}
TChain * CreateChainFromList_opt(TString ListFileName, std::string ChainName, bool DoCheck)
{
  ifstream InputFile(ListFileName);
  std::string FileName;

  TChain * TheChain = new TChain(ChainName.c_str());
  if(!DoCheck)
    {
      while(getline(InputFile, FileName))
        {
          //FileName=BaseDirectory+FileName;
          std::cout<<"Adding file "<< FileName.c_str()<<" to the TChain"<<std::endl;
          TheChain->Add(FileName.c_str());
        }
    }
  else
    {
      while(getline(InputFile, FileName))
        {
          //FileName=BaseDirectory+FileName;
          TFile*f=TFile::Open(FileName.c_str());
          if(f->Get(ChainName.c_str()))
            {
              TheChain->Add(FileName.c_str());
              std::cout<<"Adding file "<< FileName.c_str()<<" to the TChain"<<std::endl;
            }
          else std::cout<<"Chain " <<ChainName << " not found in file " << FileName<<std::endl;
        }


    }
  return TheChain;
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