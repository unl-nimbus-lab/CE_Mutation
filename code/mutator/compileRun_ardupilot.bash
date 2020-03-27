#!/bin/bash

home="/home/balaji/mutation/ardupilot/ardupilot/ArduCopter";
#home="/home/balaji/mutation/ardupilot/ardupilot/ArduCopter";
mutant_path="/home/balaji/clang-llvm/mutationdocker/code/mutator/mutants";
compile_command="sim_vehicle.py -w"

controls_file="libraries/AC_PID/AC_PID.cpp"
#controls_file="libraries/AC_AttitudeControl/AC_AttitudeControl_Multi.cpp"
#controls_file="libraries/AC_AttitudeControl/AC_AttitudeControl.cpp"
#controls_file="libraries/AC_WPNav/AC_Circle.cpp"
#controls_file="libraries/AP_Baro/AP_Baro.cpp"
#controls_file="libraries/AP_Avoidance/AP_Avoidance.cpp"
orig_file="/home/balaji/mutation/ardupilot/ardupilot/$controls_file"
dest_file="/home/balaji/mutation/AC_PID.cpp"

echo "Mutant location   : $mutant_path"
echo "Project directory : $home"
echo "Compile command   : sim_vehicle.py -w"
#make sure we are at the right place before the simulation starts
cd $home

export PATH=$PATH:/home/balaji/mutation/ardupilot/jsbsim/src
export PATH=$PATH:/home/balaji/mutation/ardupilot/ardupilot/Tools/autotest
export PATH=/usr/lib/ccache:$PATH
#Then reload your PATH by using the “dot” command in a terminal
. ~/.bashrc

#Save the original source code
cp $orig_file $dest_file

mutants=0;
total_mutants=0;

#Go to the mutant location
cd $mutant_path

function ctrl_c() {
	echo "** Trapped CTRL-C"
}

echo "---------------------------------------------"
echo "Templates : Compiled mutants/ Total mutants"
echo "---------------------------------------------"
for dir in $(find . -mindepth 1 -maxdepth 1 | sort --version-sort 2> /dev/null); #$mutant_path/*/ #
do
	full_path_dir=$mutant_path"/"$dir
    full_path_templ_dir=${full_path_dir%*/}
	templ_dir=${full_path_templ_dir##*/}
	
	cd $full_path_templ_dir
	mutant_succ_comp=0;
	temp_nooffiles=0;
	echo "pwd"
	for filename in $(find /$full_path_dir/*.cpp 2> /dev/null); do
			echo "pwd2"
			#./MyProgram.exe "$filename" "Logs/$(basename "$filename" .txt)_Log$i.txt"
			temp_nooffiles=`expr $temp_nooffiles + 1`;
			#Generate right file paths
			file_path="$mutant_path/"$templ_dir;
			name=${filename##*/}
			base=${name%.cpp}
			number=${base##*_}
			
			file=$file_path"/"$name
			#exe=$file_path"/"$base;
			log=$file_path"/log_"$number".txt";
			touch $log
			cp $file "/home/balaji/mutation/ardupilot/ardupilot/$controls_file"

			cd $home
			
			timeout 10s $compile_command </dev/null 
			if [ $? -eq 0 ]
			then 
				echo "YES"
				echo "YES" > $log
				mutants=`expr $mutants + 1`;
				mutant_succ_comp=`expr $mutant_succ_comp + 1`;
				#cd ..
				#cp b747cl $exe
			else
				echo "NO"
				echo "Compilation failed: "$name" in tempalate" $templ_dir", check log_"$number".txt in folder" $templ_dir" for more details."
				echo "NO" > $log
				
				exec 3>&1 4>&2 #set up extra file descriptors
				#stderr="$(timeout 10s $compile_command </dev/null 2>&1 )";
				exec 3>&- 4>&- # release the extra file descriptors
				
				echo $stdout >> $log
				#cd ..
			fi 
			# trap ctrl-c and call ctrl_c()
			trap ctrl_c INT
		done
		
	total_mutants=`expr $temp_nooffiles + $total_mutants`;
	total_files=`expr $total_files + $temp_nooffiles`;
	
	echo "  "$templ_dir"  : "$mutant_succ_comp"/ "$temp_nooffiles
   
done
#Restore the original source code
cp $dest_file $orig_file 

echo "----------------------------------------"
echo "Total mutants compiled: "$mutants"/ "$total_files
echo "----------------------------------------"
