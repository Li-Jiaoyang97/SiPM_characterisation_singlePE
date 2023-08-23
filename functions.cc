
float calculate_gain_value(float slope, float offset, float bias){
    return slope*bias+offset;
}

float calculate_bias_setting(float slope, float offset, float gain){
    return (gain-offset)/slope;
}

float convertBiasSettingtoVoltage(int bias_setting){
  return 68.1 - ((255.-(float)bias_setting)/255.)*4;
}

float convertBiasVoltagetoSetting(float bias_voltage){
  return std::round(255.-(68.1-bias_voltage)*(255./4.));
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

std::vector<TH1F *> declare_histo_vec(TString h_histo_name, int vector_size, std::vector<int> bias_setting, int bins, double min, double max){
   std::vector<TH1F *> h_histo_vec;
   for (int ihisto=0; ihisto<vector_size; ihisto++){
    //std::cout<<convertBiasSettingtoVoltage(bias_setting[ihisto])<<std::endl;
      //TString h_histo_name_new = Form(h_histo_name+"_%.3f", bias_setting[ihisto]);
      TString h_histo_name_new = Form(h_histo_name+"_%i", bias_setting[ihisto]);
      //TString h_histo_name_new = Form(h_histo_name+"_%.3f", convertBiasSettingtoVoltage(bias_setting[ihisto]));
      TH1F *h_histo_temp = new TH1F(h_histo_name_new,"", bins, min, max); 
      TString h_histo_title= Form("Bias: %i V", bias_setting[ihisto]);
      //TString h_histo_title= Form("Bias: %.3f V", convertBiasSettingtoVoltage(bias_setting[ihisto])); // this is to use the bias voltages.
      h_histo_temp->SetTitle(h_histo_title);  
      h_histo_vec.push_back(h_histo_temp);
   }
   return h_histo_vec;
}

/*void drawTGraphErrors(std::vector<int> channel_ids, std::vector<float> slope_per_channel_vec, std::vector<int> FEB_vec, TString savedir, TString axes_label){
  auto c1 = new TCanvas("c1","",200,10,800,500);

    TGraphErrors* gr_mean = new TGraphErrors(slope_per_channel_vec.size(), &(channel_ids[0]), &(slope_per_channel_vec[0]), 0,  &(slope_err_per_channel_vec[0]));//&(gain_err_vec[0])
    TString histo_title= Form("FEB: %i", FEB_vec[iFEB]);
    gr_mean->GetXaxis()->SetTitle("Channel ID");
    gr_mean->GetYaxis()->SetTitle("Single PE ADC response vs. bias");
    gr_mean->SetTitle(histo_title);
    gr_mean->Draw("AP");
    gr_mean->GetYaxis()->SetRangeUser(0.2, 0.6);
    //c1->SaveAs(Form(savedir+"/FEB_%i_slope.pdf", FEB_vec[iFEB]));
    c1->SaveAs(Form(savedir+"/slope_vs_channel_FEB_%i.png", FEB_vec[iFEB]));
}*/

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

  gStyle->SetTitleYOffset(0.85);
  gStyle->SetTitleYSize(0.06);

  gStyle->SetMarkerStyle(33);
  gStyle->SetMarkerSize(1.5);
  gStyle->SetLineColor(46);
  gStyle->SetLineWidth(2);
  // Draw horizontal and vertical grids
  gStyle->SetPadGridX(kTRUE);                     
  gStyle->SetPadGridY(kTRUE);
}