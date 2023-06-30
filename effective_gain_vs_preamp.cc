#include "functions.cc"
void effective_gain_vs_preamp(TString FileList="./List_of_files_gain50", TString FileList2="./List_of_files_gain52", TString savedir="./gain_vs_bias_voltage"){
//void effective_gain_vs_preamp(TString FileList="./List_of_files_gain50", TString savedir="./gain_vs_bias_voltage"){
    define_plot_style();
	std::ifstream fileList(FileList), fileList2(FileList2);
    if (!fileList.is_open() || !fileList2.is_open()) {
    	std::cout << "Error opening " << FileList <<" and "<<FileList2<< std::endl; return;
    }

    std::vector<TString> preAmp50_files_vec;
    std::vector<TString> preAmp52_files_vec;
    std::string filename_temp;
    while (std::getline(fileList, filename_temp)) {
        preAmp50_files_vec.push_back(filename_temp);
    }
    while (std::getline(fileList2, filename_temp)) {
        preAmp52_files_vec.push_back(filename_temp);
    }
    if (preAmp50_files_vec.size()!=preAmp52_files_vec.size()){
    	std::cout<<"Numbers of files for two pre Amplifier settings do not match"<<std::endl;
    	return;
    }
    TH1F *histo_ratio_preAmp52_to_50 = new TH1F("histo_ratio_preAmp52_to_50","", 50, 1, 1.5);
    for(auto ifile=0; ifile<preAmp50_files_vec.size(); ifile++){
    	//std::cout<<preAmp50_files_vec[ifile]<<", "<<preAmp50_files_vec[ifile]<<std::endl;
    	std::vector<int> channel_vec; std::vector<float> gain_preAmp50_vec, gain_preAmp52_vec;
    	std::ifstream inputFile(preAmp50_files_vec[ifile]), inputFile2(preAmp52_files_vec[ifile]);
    	if (!inputFile.is_open()) { std::cout << "Error opening " << preAmp50_files_vec[ifile] << std::endl; return;}
    	int channel_temp; float gain_temp(-10);
    	while (inputFile >> channel_temp >> gain_temp) {
    		channel_vec.push_back(channel_temp);
			gain_preAmp50_vec.push_back(gain_temp);
	    }
	    inputFile.close();

	    while (inputFile2 >> channel_temp >> gain_temp) {
			gain_preAmp52_vec.push_back(gain_temp);
	    }
	    inputFile2.close();
	    for (auto iChannel=0; iChannel<channel_vec.size(); iChannel++){
    		//if (gain_preAmp52_vec[iChannel]/gain_preAmp50_vec[iChannel]<1)std::cout<<iChannel<<", "<<gain_preAmp50_vec[iChannel]/gain_preAmp52_vec[iChannel]<<std::endl;
    		histo_ratio_preAmp52_to_50->Fill(gain_preAmp52_vec[iChannel]/gain_preAmp50_vec[iChannel]);
    	}
    	//delete gain_preAmp50_vec, gain_preAmp52_vec;
    }
    fileList.close();
    fileList2.close();

    auto canvas = new TCanvas();
    double value=54.55/46.15;

	auto line = new TLine(value,0,value, 75);
    histo_ratio_preAmp52_to_50->SetLineWidth(3);
    histo_ratio_preAmp52_to_50->Draw();
	line->SetLineColor(kRed);
    line->Draw("same");
    histo_ratio_preAmp52_to_50->GetXaxis()->SetTitleOffset(1.25);
    histo_ratio_preAmp52_to_50->SetStats(0);
    histo_ratio_preAmp52_to_50->SetTitle("Aframe 1; #frac{effective_gain_preAmp52}{effective_gain_preAmp50} ; Channels");
	canvas->SetLeftMargin(0.13);
    canvas->SetBottomMargin(0.23);
    canvas->SaveAs("effective_gain_ratio.png");
    delete canvas;

}