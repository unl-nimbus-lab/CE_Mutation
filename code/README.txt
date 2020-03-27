Folder information (../mutationdocker/code):
-----------------------------------------------------------------

The mutation tool runs for all C programming language family. It was tested with three different MATLAB Simulink control system project, and one system with no corresponding model. One system with no corresponding model is ardupilot, the control files are present 'https://github.com/balajibalasubramaniam/ardupilot' folder.

Three different MATLAB Simulink control system project files are present in the folloing folders:
b747cl_grt_rtw
ccpi16a_grt_rtw       
rct_helico_grt_rtw


Sample mutants generated from these models are present in the following folders:
mutants_b747cl      
mutants_ccpi16a
mutants_rct_helico  


The actual code of the mutation tool is present '../mutationdocker/code/mutator' folder, and the output mutants after executing the tool will be present '../mutationdocker/code/mutator/mutants' folder. For every run, you have delete all the files present inside the 'mutator/mutants' folder, do not delete the folder, delete only the contents of the folder.

Post processing analysis are written using MATLAB scripts, the files for them are present in 'mutator/matlab_scripts'.

To run the tool:
1) cd into '../mutationdocker/code/mutator' folder
2) start the docker using the command 'docker start mutationcontainer' in the terminal.
3) generate mutants by './mutator -s 7 -n 0 -c PET b747cl.c', use help './mutator -help' to know about the options.
4) stop the docker using the command 'docker stop mutationcontainer' in the terminal.


Some sample commands:
--------------------
./mutator -s 7 -n 0 -c PET ../ccpi16a_ert_rtw/ccpi16a.c
./mutator -s 7 -n 0 -c PET ../rct_helico_ert_rtw/rct_helico.c
./mutator -s 7 -n 0 -c PET ../ardupilot/ardupilot/libraries/AP_Avoidance/AP_Avoidance.cpp
./mutator -s 7 -n 0 -c PET ../ardupilot/ardupilot/libraries/AC_AttitudeControl/AC_PosControl.cpp

