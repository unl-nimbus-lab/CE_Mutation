#!/bin/bash

home="/home/balaji/clang-llvm/mutationdocker/code";
mutant_path="/home/balaji/clang-llvm/mutationdocker/code/mutator/mutants";
compile_command="make -f rct_helico.mk"

echo "Mutant location   : $mutant_path"
echo "Project directory : $home"
echo "Compile command   : make -f <filename.mk>"
#make sure we are at the right place before the simulation starts
cd $home

#Save the original source code
cp "$home/rct_helico_grt_rtw/rct_helico.c"  "$home/rct_helico.c"

mutants=0;
total_mutants=0;

#Go to the mutant location
cd $mutant_path

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
	
	for filename in $(find /$full_path_dir/*.c 2> /dev/null); do
			#./MyProgram.exe "$filename" "Logs/$(basename "$filename" .txt)_Log$i.txt"
			temp_nooffiles=`expr $temp_nooffiles + 1`;
			#Generate right file paths
			file_path="$mutant_path/"$templ_dir;
			name=${filename##*/}
			base=${name%.c}
			number=${base##*_}
			
			file=$file_path"/"$name
			exe=$file_path"/"$base;
			log=$file_path"/log_"$number".txt";
			touch $log
			cp $file "$home/rct_helico_grt_rtw/rct_helico.c"

			cd $home
			cd rct_helico_grt_rtw

			$compile_command > /dev/null 2>/dev/null
			if [ $? -eq 0 ]
			then 
				#echo "YES"
				echo "YES" > $log
				mutants=`expr $mutants + 1`;
				mutant_succ_comp=`expr $mutant_succ_comp + 1`;
				cd ..
				cp rct_helico $exe
			else
				echo "Compilation failed: "$name" in tempalate" $templ_dir", check log_"$number".txt in folder" $templ_dir" for more details."
				echo "NO" > $log
				
				exec 3>&1 4>&2 #set up extra file descriptors
				stderr="$($compile_command </dev/null 2>&1 )";
				exec 3>&- 4>&- # release the extra file descriptors
				
				echo $stderr >> $log
				cd ..
			fi 
		done
		
	total_mutants=`expr $temp_nooffiles + $total_mutants`;
	total_files=`expr $total_files + $temp_nooffiles`;
	
	echo "  "$templ_dir"  : "$mutant_succ_comp"/ "$temp_nooffiles
   
done
#Restore the original source code
cp "$home/rct_helico.c" "$home/rct_helico_grt_rtw/rct_helico.c"
echo "----------------------------------------"
echo "Total mutants compiled: "$mutants"/ "$total_files
echo "----------------------------------------"
