#!/bin/bash
#

# ====================== USERS TO-DO: MANUALLY CHANGING  ====================== 
Run_numbers=(6127 6137 6155 6156) #example here
#  ====================== END OF USERS TO-DO ====================== 

# ====================== USERS TO-CHECK ====================== 
TARGET_bias_setting=180. # calculated based on Rohan's average triggered bias voltage. 
TARGET_eff_gain=60.
# Things below should not require any major changes....
n=5 # number of bias voltages.
Voltages=(140 160 180 200 220)  # example here
Data_dir=/data/crt_Aframe_saveme #temporary  # double-check here. 
#  ====================== END OF USERS TO-CHECK ====================== 

# From this stage on, USERS should not be asked to modify anything...
start=$(date +%s)
LAR_OUT_DIR_NAME=./LAr_output_ROOT_start_run${Run_numbers[0]}
LIST_FILE_NAME=./List_of_files_with_start_run${Run_numbers[0]}
FINAL_PLOT_DIR_NAME=./gain_vs_bias_setting_start_run${Run_numbers[0]}
RAW_FINAL_PLOT_DIR_NAME=./raw_gain_vs_bias_setting_start_run${Run_numbers[0]}
TARGETED_BIAS_TXT_FILE=./targetted_bias_setting_TXT_start_run${Run_numbers[0]}
EFF_GAIN_WITH_TARGET_BIAS_TXT_FILE=./eff_gain_with_targeted_bias_TXT_start_run${Run_numbers[0]}
FULL_OUTPUT_DIR=./All_results_start_run${Run_numbers[0]}

# clean
rm -rf $LAR_OUT_DIR_NAME
rm -f  $LIST_FILE_NAME
rm -rf $FINAL_PLOT_DIR_NAME
rm -rf $RAW_FINAL_PLOT_DIR_NAME
rm -rf $TARGETED_BIAS_TXT_FILE
rm -rf $EFF_GAIN_WITH_TARGET_BIAS_TXT_FILE
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
	
done


# run the plotting script to link all bias voltage together. 
echo "Running the plotting script....."

rm $LIST_FILE_NAME
ls $LAR_OUT_DIR_NAME/* > $LIST_FILE_NAME
root -l -b <<EOF
.L plot_gain_vs_voltage.cc
plot_gain_vs_voltage("$LIST_FILE_NAME", "$FINAL_PLOT_DIR_NAME", "$RAW_FINAL_PLOT_DIR_NAME", $TARGET_bias_setting, $TARGET_eff_gain)
.q
EOF

mkdir -p $TARGETED_BIAS_TXT_FILE
mv target_bias_setting_with_eff_gain_FEB*.txt $TARGETED_BIAS_TXT_FILE

mkdir -p $EFF_GAIN_WITH_TARGET_BIAS_TXT_FILE
mv effective_gain_with_target_bias_setting_FEB*.txt $EFF_GAIN_WITH_TARGET_BIAS_TXT_FILE

mkdir -p $FULL_OUTPUT_DIR
mkdir -p $FULL_OUTPUT_DIR/fitted_adc_histos

mv ./fitted_adc_histos_voltage* $FULL_OUTPUT_DIR/fitted_adc_histos
mv $LAR_OUT_DIR_NAME $FULL_OUTPUT_DIR
mv $RAW_FINAL_PLOT_DIR_NAME $FULL_OUTPUT_DIR
mv $FINAL_PLOT_DIR_NAME $FULL_OUTPUT_DIR
mv $TARGETED_BIAS_TXT_FILE $FULL_OUTPUT_DIR
mv $EFF_GAIN_WITH_TARGET_BIAS_TXT_FILE $FULL_OUTPUT_DIR
mv $LIST_FILE_NAME $FULL_OUTPUT_DIR

end=$(date +%s)
runtime=$((end-start))

echo "Script executed in $runtime seconds."
