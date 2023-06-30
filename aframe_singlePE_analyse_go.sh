#!/bin/bash
#
start=$(date +%s)
# ====================== USER TO-DO: MANUALLY CHANGING  ====================== 
# A-frame 0
#Run_numbers=(5222 5348 5349 5350 5352) # 5208 -- 30 mins for 114 discarded. 
# A-frame 1
#Run_numbers=(5660 5661 5662 5680 5686) 
#Run_numbers=(5693 5702 5747 5709 5716 5746 5736) 
# 			 154  174  174  194  194  194  214  ^ this is only for FEB [73, 155] 
#Run_numbers=(5975 5977 5978 5979 5980) 

# A-frame 2 gain 50
#Run_numbers=(5752 5753 5754 5755 5756) 
#Run_numbers=(5902 5905 5910 5911 5912) 
# A-frame2 gain 52
Run_numbers=(6127 6137 6155 6156)

#  ====================== END OF USER TO-DO ====================== 

TARGET_bias_setting=180. # calculated based on Rohan's average triggered bias voltage. 
# Things below should not require any major changes....
n=4 # number of bias voltages.
Voltages=(140 170 200 230) 
#Voltages=(134 154 174 194 214) 
#Voltages=(154 174 194 214 234) # for A-frame new
Data_dir=/data/crt_Aframe_saveme

LAR_OUT_DIR_NAME=./LAr_output_ROOT_start_run${Run_numbers[0]}
LIST_FILE_NAME=./List_of_files_with_start_run${Run_numbers[0]}
FINAL_PLOT_DIR_NAME=./gain_vs_bias_setting_start_run${Run_numbers[0]}
RAW_FINAL_PLOT_DIR_NAME=./raw_gain_vs_bias_setting_start_run${Run_numbers[0]}
TARGET_VOLTAGE_TXT_DIR=./ADC_reponse_vs_bias_fit_results_TXT_start_run${Run_numbers[0]}
FULL_OUTPUT_DIR=./All_results_start_run${Run_numbers[0]}

# clean
rm -rf $LAR_OUT_DIR_NAME
rm -f  $LIST_FILE_NAME
rm -rf $FINAL_PLOT_DIR_NAME
rm -rf $RAW_FINAL_PLOT_DIR_NAME
rm -rf $TARGET_VOLTAGE_TXT_DIR
rm -rf $FULL_OUTPUT_DIR

# check the folder
if [ ! -d "$LAR_OUT_DIR_NAME" ] 
then
	echo "Error: Directory $LAR_OUT_DIR_NAME does not exists, creating now..." #>> $Run_log_dir/$Run_log_name
    mkdir ${LAR_OUT_DIR_NAME}
fi

for (( i = 0; i < ${n}; i++ )) ### loop in x coordinate ###
do 	
	FIT_ADC_HISTO_DIR=./fitted_adc_histos_voltage${Voltages[$i]}/
	rm -rf $FIT_ADC_HISTO_DIR
	cp analyze_event_singlePE.fcl.og analyze_event_singlePE.fcl

	echo "physics.analyzers.dumpall.bias_setting: ${Voltages[$i]}" 
	echo "physics.analyzers.dumpall.bias_setting: ${Voltages[$i]}" >> analyze_event_singlePE.fcl
	echo "physics.analyzers.dumpall.photo_dump_dir: \"$FIT_ADC_HISTO_DIR\" " 
	echo "physics.analyzers.dumpall.photo_dump_dir: \"$FIT_ADC_HISTO_DIR\" " >> analyze_event_singlePE.fcl
	
	echo "lar -c analyze_event_singlePE.fcl $Data_dir/*run${Run_numbers[$i]}*.root -T ${LAR_OUT_DIR_NAME}/run${Run_numbers[$i]}_${Voltages[$i]}V.root" # >> $Run_log_dir/$Run_log_name
	lar -c analyze_event_singlePE.fcl $Data_dir/*run${Run_numbers[$i]}*.root -T ${LAR_OUT_DIR_NAME}/run${Run_numbers[$i]}_${Voltages[$i]}V.root
	
	#if [ $i -eq 0 ];
	#then
	#	echo "lar -c analyze_event_singlePE.fcl $Data_dir/*run${Run_numbers[$i]}*.root -T ${LAR_OUT_DIR_NAME}/run${Run_numbers[$i]}_${Voltages[0]}V.root" # >> $Run_log_dir/$Run_log_name
	#	lar -c analyze_event_singlePE.fcl $Data_dir/*run${Run_numbers[$i]}*.root -T ${LAR_OUT_DIR_NAME}/run${Run_numbers[$i]}_${Voltages[0]}V.root
	#elif [ $i -eq 1 || $i -eq 2 ];
	#then
	#	echo "lar -c analyze_event_singlePE.fcl $Data_dir/*run${Run_numbers[$i]}*.root -T ${LAR_OUT_DIR_NAME}/run${Run_numbers[$i]}_${Voltages[1]}V.root" # >> $Run_log_dir/$Run_log_name
	#	lar -c analyze_event_singlePE.fcl $Data_dir/*run${Run_numbers[$i]}*.root -T ${LAR_OUT_DIR_NAME}/run${Run_numbers[$i]}_${Voltages[1]}V.root
	#elif [ $i -eq 3 || $i -eq 4 || $i -eq 5 ];
	#then
	#	echo "lar -c analyze_event_singlePE.fcl $Data_dir/*run${Run_numbers[$i]}*.root -T ${LAR_OUT_DIR_NAME}/run${Run_numbers[$i]}_${Voltages[2]}V.root" # >> $Run_log_dir/$Run_log_name
	#	lar -c analyze_event_singlePE.fcl $Data_dir/*run${Run_numbers[$i]}*.root -T ${LAR_OUT_DIR_NAME}/run${Run_numbers[$i]}_${Voltages[2]}V.root
	#else
	#	echo "lar -c analyze_event_singlePE.fcl $Data_dir/*run${Run_numbers[$i]}*.root -T ${LAR_OUT_DIR_NAME}/run${Run_numbers[$i]}_${Voltages[3]}V.root" # >> $Run_log_dir/$Run_log_name
	#	lar -c analyze_event_singlePE.fcl $Data_dir/*run${Run_numbers[$i]}*.root -T ${LAR_OUT_DIR_NAME}/run${Run_numbers[$i]}_${Voltages[3]}V.root
	#fi
done


# run the plotting script to link all bias voltage together. 
echo "Running the plotting script....."

rm $LIST_FILE_NAME
ls $LAR_OUT_DIR_NAME/* > $LIST_FILE_NAME
root -l -b <<EOF
.L plot_gain_vs_voltage.cc
plot_gain_vs_voltage("$LIST_FILE_NAME", "$FINAL_PLOT_DIR_NAME", "$RAW_FINAL_PLOT_DIR_NAME", $TARGET_bias_setting)
.q
EOF

mkdir -p $TARGET_VOLTAGE_TXT_DIR
mv target*.txt $TARGET_VOLTAGE_TXT_DIR

mkdir -p $FULL_OUTPUT_DIR
mkdir -p $FULL_OUTPUT_DIR/fitted_adc_histos

mv ./fitted_adc_histos_voltage* $FULL_OUTPUT_DIR/fitted_adc_histos
mv $LAR_OUT_DIR_NAME $FULL_OUTPUT_DIR
mv $RAW_FINAL_PLOT_DIR_NAME $FULL_OUTPUT_DIR
mv $FINAL_PLOT_DIR_NAME $FULL_OUTPUT_DIR
mv $TARGET_VOLTAGE_TXT_DIR $FULL_OUTPUT_DIR
mv $LIST_FILE_NAME $FULL_OUTPUT_DIR

end=$(date +%s)
runtime=$((end-start))

echo "Script executed in $runtime seconds."
