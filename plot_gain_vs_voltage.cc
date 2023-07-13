#include "functions.cc"

void plot_gain_vs_voltage(TString FileList="./List_of_files", TString savedir="./gain_vs_bias_voltage", TString savedir2="./raw_gain_vs_bias_voltage", float targeted_bias_setting=180., float targeted_effGain=60.){	
	gSystem->Exec("rm "+savedir+"*");
	gSystem->Exec("rm "+savedir2+"*");
	define_plot_style();
	TChain * chain = CreateChainFromList_opt(FileList, "dumpall/CRTFitInfoTree", false);

	Int_t bias_setting, febid, channel;
	Float_t eff_gain, eff_gain_err, chisqr, ndf;

	chain->SetBranchAddress("fbias_setting",&bias_setting);
	chain->SetBranchAddress("FEB_id",  &febid);
	chain->SetBranchAddress("channel", &channel);
	chain->SetBranchAddress("eff_gain", &eff_gain);
	chain->SetBranchAddress("eff_gain_err", &eff_gain_err);
	chain->SetBranchAddress("chisqr", &chisqr);
	chain->SetBranchAddress("ndf", &ndf);

	auto nEnt = chain->GetEntries();
	std::cout<<"Need to loop over "<<nEnt<<" entries."<<std::endl;
	std::vector<int> FEB_vec;
	std::vector<int> bias_setting_save_vec;
	for(auto i=0; i<nEnt; i++){
	  chain->GetEntry(i);
	  FEB_vec.push_back(febid);
	  bias_setting_save_vec.push_back(bias_setting);
	}

	std::sort(FEB_vec.begin(), FEB_vec.end());
	FEB_vec.erase(std::unique(FEB_vec.begin(), FEB_vec.end()), FEB_vec.end()); // Remove duplicate values from vector

	std::sort(bias_setting_save_vec.begin(), bias_setting_save_vec.end());
	bias_setting_save_vec.erase(std::unique(bias_setting_save_vec.begin(), bias_setting_save_vec.end()), bias_setting_save_vec.end()); // Remove duplicate values from vector

	TH1F * histo_slope_effective_vs_bias = new TH1F("histo_slope_effective_vs_bias", "", 20, 0, 50);
	std::vector<TH1F*> histo_eff_gain_vec = declare_histo_vec("h_effctive_gain", bias_setting_save_vec.size(), bias_setting_save_vec, 30, 30, 120);
	std::map<int,int> potential_dead_channel_map;

	// start to loop.
	for (int iFEB=0; iFEB<FEB_vec.size(); iFEB++){ // loop through the FEBs FEB_vec.size()
		//ofstream outfile;
		//outfile.open(Form("./fitted_effGain_offset_FEB%i.txt", FEB_vec[iFEB])); // this file saves the fitted parameters when fitting eff_gain vs. bias setting, --> slope and offset is going to be saved.
		//outfile<<"Channel \t fitted_effective_gain \t fitted_offset"<<std::endl;

		ofstream outfile1;
		outfile1.open(Form("./target_bias_setting_with_eff_gain_FEB%i.txt", FEB_vec[iFEB])); // this file saves the targeted bias setting when eff_gain provided.
		outfile1<<"Channel \t bias_setting"<<std::endl;

		ofstream outfile2;
		outfile2.open(Form("./effective_gain_with_target_bias_setting_FEB%i.txt", FEB_vec[iFEB]));
		outfile2<<"Channel \t effective_gain"<<std::endl;
		// vectors to save all slope 
		std::vector<double> slope_per_channel_vec(32);
		std::vector<double> slope_err_per_channel_vec(32);
		std::vector<double> channel_ids(32);

		//int channel_print_count(0);
		for (int iChannel = 0; iChannel < 32; iChannel++){
			
			std::vector<float> bias_voltage_vec;
			std::vector<float> eff_gain_vec;
			std::vector<float> eff_gain_err_vec;

			std::vector<float> eff_gain_vec_raw;
			std::vector<float> eff_gain_err_vec_raw;
			std::vector<float> bias_voltage_vec_raw;

			bool doFitPlot=false;
			for(auto i=0; i<nEnt; i++){
				chain->GetEntry(i);
				if (febid== FEB_vec[iFEB] && channel==iChannel ){
					bias_voltage_vec_raw.push_back(bias_setting);
					//bias_voltage_vec_raw.push_back(convertBiasSettingtoVoltage(bias_setting));
					eff_gain_vec_raw.push_back(eff_gain);
					eff_gain_err_vec_raw.push_back(eff_gain_err);
					
					// sanity check for the eff_gain value makes sense or not.
					//if ( (ndf!=0 && chisqr/ndf < 100.0) || (ndf==0) ){
						float targeted_eff_gain=0.4*bias_setting;
						if (eff_gain<targeted_eff_gain+25 && eff_gain>targeted_eff_gain-25){
							bias_voltage_vec.push_back(bias_setting);
							//bias_voltage_vec.push_back(convertBiasSettingtoVoltage(bias_setting));
							eff_gain_vec.push_back(eff_gain);
							eff_gain_err_vec.push_back(eff_gain_err);
							doFitPlot=true;

    					std::vector<int>::iterator itr = std::find(bias_setting_save_vec.begin(), bias_setting_save_vec.end(), bias_setting);
    					int ibias = std::distance(std::begin(bias_setting_save_vec), itr); // find the channel with the maximum hits. 
    					histo_eff_gain_vec.at(ibias)->Fill(eff_gain);
						}
					//}
				}
			}

			TString histo_title= Form("FEB: %i, channel: %i", FEB_vec[iFEB], iChannel);
			auto c2 = new TCanvas("c2","",200,10,800,500);
			TGraphErrors* gr_mean2 = new TGraphErrors(bias_voltage_vec_raw.size(), &(bias_voltage_vec_raw[0]), &(eff_gain_vec_raw[0]), 0, &(eff_gain_err_vec_raw[0]));

			gr_mean2->SetTitle();  
			gr_mean2->SetTitle(histo_title+"; Bias; Fitted effective gain");
			
			gr_mean2->Draw("AP"); 

			TPaveText* pt = new TPaveText(0.15, 0.8, 0.65, 0.9, "NDC");
			pt->SetFillColor(0);
			pt->SetTextAlign(22);
			pt->SetTextSize(0.06);
			pt->SetTextColor(kRed);
			pt->AddText("Raw effective gain: "+histo_title);
			pt->Draw();

			gSystem->Exec("mkdir -p "+savedir2);
			//c2->SaveAs(Form(savedir2+"FEB_%i_channel_%i.pdf", FEB_vec[iFEB], iChannel));
			c2->SaveAs(Form(savedir2+"/FEB_%i_channel_%i.png", FEB_vec[iFEB], iChannel));
			delete c2;

			if (doFitPlot){
				
				/*bool isFirstErased=false;
				int nErased=0;
				for (int i=0; i<eff_gain_vec.size()-1; i++){
					float thiseff_Gain=eff_gain_vec[i];
					float nexteff_Gain=eff_gain_vec[i+1];
					if(i==0 && thiseff_Gain>nexteff_Gain){
						bias_voltage_vec_new.erase(bias_voltage_vec_new.begin());
						eff_gain_vec_new.erase(eff_gain_vec_new.begin());
						eff_gain_err_vec_new.erase(eff_gain_err_vec_new.begin());
						nErased++;
					}else if(thiseff_Gain>nexteff_Gain){
						bias_voltage_vec_new.erase(bias_voltage_vec_new.begin()+i+1-nErased);
						eff_gain_vec_new.erase(eff_gain_vec_new.begin()+i+1-nErased);
						eff_gain_err_vec_new.erase(eff_gain_err_vec_new.begin()+i+1-nErased);
						nErased++;
						if (i<eff_gain_vec.size()-2) {
							nexteff_Gain=eff_gain_vec[i+2];
							if(thiseff_Gain>nexteff_Gain){
								//std::cout<<"3! "<<nexteff_Gain<<", "<<eff_gain_vec_new.at(i+2-nErased);
								bias_voltage_vec_new.erase(bias_voltage_vec_new.begin()+i+2-nErased);
								eff_gain_vec_new.erase(eff_gain_vec_new.begin()+i+2-nErased);
								eff_gain_err_vec_new.erase(eff_gain_err_vec_new.begin()+i+2-nErased);
							}	
						}
					}
				}*/

				/*std::cout<<"Print new eff_gain vector:"<<std::endl;
				for(int i=0; i<eff_gain_vec_new.size();i++){
					std::cout<<eff_gain_vec_new.at(i)<<", ";
				}
				std::cout<<std::endl;*/


				// Save the plots without abondoning points.
				auto c1 = new TCanvas("c1","",200,10,800,500);
				TGraphErrors* gr_mean = new TGraphErrors(bias_voltage_vec.size(), &(bias_voltage_vec[0]), &(eff_gain_vec[0]), 0, &(eff_gain_err_vec[0]));

				int bias_volatage_vector_size = bias_voltage_vec.size();
	  			TF1 *fit = new TF1("fit","[0] + [1]*x", bias_voltage_vec[0]-0.25, bias_voltage_vec[bias_volatage_vector_size-1]+0.3);

				gStyle->SetOptStat(0100);
				gStyle->SetOptFit(1111);
				gStyle->SetStatX(0.5);
				gStyle->SetStatY(0.9);
				gStyle->SetStatH(0.15);
				gStyle->SetStatW(0.2);

				gr_mean->SetTitle(histo_title);  
				gr_mean->SetTitle(histo_title+"; Bias voltage [V]; Fitted effective gain [ADC/PE]");
				
				gr_mean->Draw("AP");
		  		gr_mean->Fit(fit, "QR");  

		  		// store the fitting results. 
		  		fit->SetParName(0, "Slope");
		  		fit->SetParName(1, "Offset");
				float slope  			= fit->GetParameter(1);  
				float offset 			= fit->GetParameter(0); 
				float slope_error = fit->GetParError(1);

				histo_slope_effective_vs_bias->Fill(slope);
				slope_err_per_channel_vec.at(iChannel)=slope_error;
				slope_per_channel_vec.at(iChannel)=slope;
				channel_ids.at(iChannel)=iChannel;

				float calculated_eff_gain = calculate_gain_value(slope, offset, targeted_bias_setting);
				//outfile<<iChannel<<"\t"<<slope<<"\t"<<offset<<std::endl;
				// targeted gain
				outfile1<<iChannel<<"\t"<<calculate_bias_setting(slope, offset, targeted_effGain)<<std::endl;
				outfile2<<iChannel<<"\t"<<calculated_eff_gain<<std::endl;
				//outfile1<<iChannel<<"\t"<<convertBiasVoltagetoSetting(calculate_bias_setting(slope, offset, targeted_effGain))<<std::endl;

				gSystem->Exec("mkdir -p "+savedir);
				//c1->SaveAs(Form(savedir+"/FEB_%i_channel_%i.pdf", FEB_vec[iFEB], iChannel));
				c1->SaveAs(Form(savedir+"/FEB_%i_channel_%i.png", FEB_vec[iFEB], iChannel));
				delete c1;
			}
			else{
			  //std::cerr<<Form("Go check: FEB %i Channel %i", FEB_vec[iFEB], iChannel)<<std::endl;
			  //outfile<<iChannel<<"\t"<<"**DEAD**"<<std::endl;
			  outfile1<<iChannel<<"\t"<<"**DEAD**"<<std::endl;
			  outfile2<<iChannel<<"\t"<<"**DEAD**"<<std::endl;
			  potential_dead_channel_map.insert(std::make_pair(FEB_vec[iFEB], iChannel));
			}
		}//end of channel map
		//outfile.close(); 
		outfile1.close(); 
		outfile2.close(); 
		auto c1 = new TCanvas("c1","",200,10,800,500);

		TGraphErrors* gr_mean = new TGraphErrors(channel_ids.size(), &(channel_ids[0]), &(slope_per_channel_vec[0]), 0,  &(slope_err_per_channel_vec[0]));//&(gain_err_vec[0])
		TString histo_title= Form("FEB: %i", FEB_vec[iFEB]);
		gr_mean->SetTitle(histo_title+" ; Channel ID ; Effective gain vs. bias [ADC/PE/V]");
		gr_mean->Draw("AP");
		gr_mean->GetYaxis()->SetRangeUser(0,50);
		//c1->SaveAs(Form(savedir+"/FEB_%i_slope.pdf", FEB_vec[iFEB]));
		c1->SaveAs(Form(savedir+"/slope_vs_channel_FEB_%i.png", FEB_vec[iFEB]));
		delete c1;
	}// end of FEB loop

	auto canvas = new TCanvas("canvas","",200,10,800,500);
	//histo_slope_effective_vs_bias->SetStats(0);
	histo_slope_effective_vs_bias->Draw("E1");
	histo_slope_effective_vs_bias->Draw("hist same");
	histo_slope_effective_vs_bias->SetStats(0);
	histo_slope_effective_vs_bias->SetLineWidth(3);
	histo_slope_effective_vs_bias->SetTitle(" ; Effective gain vs. bias [ADC/PE/V]; CRT channels");
	canvas->SaveAs(savedir+"/histo_slope_effective_vs_bias.png");
	delete canvas;

	auto canvas2 = new TCanvas("canvas","",200,10,800,500);
	TLegend *leg = new TLegend(0.2, 0.7, 0.9, 0.9);//0.55, 0.2, 0.9, 0.3
	leg->SetTextFont(62);  leg->SetTextSize(0.04); leg->SetTextFont(62);
	leg->SetBorderSize(0); leg->SetFillStyle(0);   leg->SetNColumns(2);
	for(int ihisto=0; ihisto<histo_eff_gain_vec.size(); ihisto++){
		histo_eff_gain_vec.at(ihisto)->SetLineColor(30+ihisto*6);
		histo_eff_gain_vec.at(ihisto)->SetLineWidth(3);
		histo_eff_gain_vec.at(ihisto)->SetStats(0);
		leg->AddEntry(histo_eff_gain_vec.at(ihisto), histo_eff_gain_vec.at(ihisto)->GetTitle(), "lep");

		if (ihisto==0) {
			histo_eff_gain_vec.at(ihisto)->Draw("E1");	
			histo_eff_gain_vec.at(ihisto)->Draw("hist same");		
			histo_eff_gain_vec.at(ihisto)->GetYaxis()->SetRangeUser(0, histo_eff_gain_vec.at(ihisto)->GetMaximum()+80);
			histo_eff_gain_vec.at(ihisto)->SetTitle(" ; Effective gain [ADC/PE]; CRT channels");
		}else { 
			histo_eff_gain_vec.at(ihisto)->Draw("E1 same");
			histo_eff_gain_vec.at(ihisto)->Draw("hist same");
	}

	}
	leg->Draw("same");

	canvas2->SaveAs(savedir+"/histo_effective_gains.png");
	delete canvas2;

	// Print out the detected dead channels. 
	std::cout<<std::endl<<"Potentially dead channel: "<<std::endl;
	for (auto const& [ifeb, ichannel] : potential_dead_channel_map){
		std::cout<<Form("FEB %i Channel %i", ifeb, ichannel)<<std::endl;
	}

}
