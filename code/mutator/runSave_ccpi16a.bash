#!/bin/bash

mutant_path="/home/balaji/clang-llvm/mutationdocker/code/mutator/mutants";
execution_command="timeout 10s ./<exe file>"

echo "Mutant location   : $mutant_path"
echo "Execution command : $execution_command"

executions=0;
total_mutants=0;
total_compiled_files=0;

#Go to the mutant location
cd $mutant_path

echo "-----------------------------------------------"
echo "Templates : Executed mutants/ Compiled mutants"
echo "-----------------------------------------------"

for dir in $(find . -mindepth 1 -maxdepth 1 | sort --version-sort 2> /dev/null); #$mutant_path/*/ #
do
	full_path_dir=$mutant_path"/"$dir
    full_path_templ_dir=${full_path_dir%*/}
	templ_dir=${full_path_templ_dir##*/}
	cd $full_path_templ_dir
	
	mutant_succ_exec=0;
	compiled_nooffiles=0;

	for filename in $(find /$full_path_dir/*.c 2> /dev/null); do
		#Generate right file paths
		file_path="$mutant_path/"$templ_dir;
		name=${filename##*/}
		base=${name%.c}
		number=${base##*_}

		file=$file_path"/"$name
		
		exe=$file_path"/"$base;
		log=$file_path"/log_"$number".txt";
		log2=$file_path"/log2_"$number".txt";
		touch $log2
		#echo $base
		
		logV=`cat $log`;
		if [ "$logV" = "YES" ]; then
			total_compiled_files=`expr $total_compiled_files + 1`;
			compiled_nooffiles=`expr $compiled_nooffiles + 1`;
			#echo "compilation : YES"
			cd $file_path
			timeout 20s ./$base > /dev/null 2>/dev/null
			if [ $? -eq 0 ] 
			then
				#echo "executaion : YES"
				executions=`expr $executions + 1`;
				mutant_succ_exec=`expr $mutant_succ_exec + 1`;
				echo "YES" > $log2
				mv ccpi.mat $base.mat
			else
				echo "Executaion failed: "$name" in tempalate" $templ_dir", check log2_"$number".txt in folder" $templ_dir" for more details."
				
				echo "NO  
					
Execution failed becuase the code did not finish execution within the allocated time. It got timed out with no errors." > $log2
				
				exec 3>&1 4>&2 #set up extra file descriptors
				stderr="$(timeout 10s ./$base </dev/null 2>&1 )";
				exec 3>&- 4>&- # release the extra file descriptors
				
				echo $stderr >> $log2
			fi
		else
			#echo "compilation : NO"
			echo "NO" > $log2
			continue;
		fi
	done
	total_mutants=`expr $compiled_nooffiles + $total_mutants`;
	
	echo "  "$templ_dir"  : "$mutant_succ_exec"/ "$compiled_nooffiles
	
done
echo "----------------------------------------"
echo "Total mutants executed: "$executions"/ "$total_compiled_files
echo "----------------------------------------"

