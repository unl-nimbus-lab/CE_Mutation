# Cyber-Physical Mutation tool

This tool is part of a [master thesis](https://digitalcommons.unl.edu/computerscidiss/165/) done at the University of Nebraska-Lincoln.

Author names: [Balaji Balasubramaniam](https://sites.google.com/site/balajibytes/download-pub), [Prof. Sebastian Elbaum](https://engineering.virginia.edu/faculty/sebastian-elbaum), and [Prof. Justin Bradley](https://justinbradley.unl.edu/).

# Definitions
Mutation - Change certain statement in the source code of a target file.<br />
Mutator - A software tool that automatically does the mutation on the programming files. <br />
Mutationdocker - Virtual machine environment configured to run the mutator.<br />
Mutant - Programming file that has changed the code that is different from the original program.<br />

# About
Cyber-Physical Mutation tool is part of a [master thesis](https://digitalcommons.unl.edu/computerscidiss/165/) done at the University of Nebraska-Lincoln. The tool has five different phases:
1) Installing the tool - download and build - read "Tool run - step by step tutorial" section of this document.
2) Mutant generation - using "./mutator" command, the order of execution is first ./runconfiguration (if `config.txt` file is changed), followed by make, and then the tool execution.
3) Mutant compilation - using "compilerun.bash" script - change the path in `compilerun.bash` depending on your project.
4) Mutant execution - using "runsave.bash" script - change the path in `runsave.bash` depending on your project.
5) Result analysis - using MATLAB&copy; scripts, see "folder structure" section of this document. 

In general, each mutation tool has different inputs, some mutation tools require a function/ sub-routine as input. The Cyber-Physical mutation tool requires a C family programming file as input. As a result, in order to understand about the input file, this tool has to recognize all the dependencies of the C project where the input C family programming file resides. This dependency linking is carried out using the process called "configuration". Under the "configure" section of this document, I have explained these steps as optional. The dependencies are provided in `config.txt` file (it will vary based on the project), after you change this file based on your project you have to execute `./runconfiguration <config file name>`. I have mentioned this instruction under "To build and run" section of this document.
 
Automation scripts are needed to automate the compilation and execution of the mutated target_file, here you have to specify the path based on your project. Please find instructions under the "configure" section of this document.

# Folder structure and sample files

Folder information (/mutationdocker/code):<br />
The mutation tool runs for all C programming language family. The tool was tested with three different MATLAB Simulink control system projects, and one system with no corresponding model. One system with no corresponding model is [Ardupilot](http://ardupilot.org/about), the control files are present 'https://github.com/balajibalasubramaniam/ardupilot' folder.

Three different MATLAB&copy; Simulink control system project files are present in the folloing folders (you have to make sure all the depedencies are properly configured in the `config.txt` file and available in your system):<br />
b747cl_grt_rtw<br />
ccpi16a_grt_rtw<br />
rct_helico_grt_rtw

Sample mutants generated from these models are present in the following folders:<br />
mutants_b747cl<br />
mutants_ccpi16a<br />
mutants_rct_helico

The actual code of the mutation tool is present `../mutationdocker/code/mutator` folder, and the output mutants after executing the tool will be present `../mutationdocker/code/mutator/mutants` folder. For every run, you have to delete all the files present inside the `mutator/mutants` folder, do not delete the folder, delete only the contents of the folder.

Post-processing analysis are written using MATLAB&copy; scripts, the files are present in the following folder:<br/>
`mutationdocker/code/mutator/matlab_scripts/`<br/>
Please visit the following link for master thesis and know more about the results: https://digitalcommons.unl.edu/computerscidiss/165/

To run the tool:
1) cd into `../mutationdocker/code/mutator` folder
2) start the docker using the command `docker start mutationcontainer` in the terminal.
3) generate mutants by `./mutator -s 7 -n 0 -c PET b747cl.c`, use help `./mutator -help` to know about the options.
4) stop the docker using the command 'docker stop mutationcontainer' in the terminal.


Some sample commands:<br />
./mutator -s 7 -n 0 -c PET ../ccpi16a_ert_rtw/ccpi16a.c<br />
./mutator -s 7 -n 0 -c PET ../rct_helico_ert_rtw/rct_helico.c<br />
./mutator -s 7 -n 0 -c PET ../ardupilot/ardupilot/libraries/AP_Avoidance/AP_Avoidance.cpp<br />
./mutator -s 7 -n 0 -c PET ../ardupilot/ardupilot/libraries/AC_AttitudeControl/AC_PosControl.cpp<br />

# Boeing 747 closed loop - b747cl
If you plan to run the Boeing 747 project then make sure all the dependency linking is specified in the config.txt file. For each project, I already created config files with suffix project names. For example, `config_b7474cl.txt` is for the b7474 project, you can find all these files under `mutationdocker/code/mutator/` folder. Make sure all these dependencies are available and discoverable. You can ensure that the b747 project can be run separately. The reason being it is generated from MATLAB&copy;, as a result, it may need some MATLAB&copy; dependencies. You can do this by getting into `mutationdocker/code/b747cl_grt_rtw` directory, and then running `make -f b747cl.mk` command for compilation and `./b747cl` for execution. 

Verify that the include directories and the dependencies are available. For example, make sure "I/usr/local/MATLAB/R2016a/extern/include" include directory is available. Again, sorry for the absolute paths. You need to get the MATLAB&copy; Airlib project for b747, here is the link: https://www.mathworks.com/matlabcentral/fileexchange/3019-airlib. Make sure you are able to run and generate C code from this. Similarly for cruise control: http://ctms.engin.umich.edu/CTMS/index.php?example=CruiseControl&section=SimulinkModeling, and helicopter: https://www.mathworks.com/help/control/examples/multi-loop-control-of-a-helicopter.html 

# Configure
1) Go to `Makefile` and change path, the below line, in headers to point to llvm installation in your computer.

HEADERS := -isystem /usr/lib/llvm-4.0/include/

2) Optional - Go to `compilemutator.bash` file and change the container name if you are using different docker container.`compilemutator.bash` file is used to automate two steps: make and then run. It is recommended to first follow the step by step tutorial before modifying or running the `compilemutator.bash` file.

3) Optional - change the configuration paths for the compiler of target_file in `config.txt` according to your software environment. If you make changes to `config.txt` file (inside `mutationdocker/code/mutator/` folder) then you have to execute `./runconfiguration` first. Similarly, if you make changes to `Makefile` then you have to rebuild it with `make` before running the tool. To summarize, the order of execution is first `./runconfiguration`, followed by `make`, and then the tool execution.

4) Optional - change the path in `compilemutator.bash`, `compilerun.bash`, and `runsave.bash` based on the location where you have unzipped and placed the mutationdocker for the project directory of the target_file. `compilerun.bash`, and `runsave.bash` file is used to automate the compilation and execution of target_file, it varies depending on the project. 

5) Optional - change `spacevariables.txt` file. Provide names of the variables that need to be mutated. For example: if the file has line "location,*,50" then the tool will find the occurrence of the variable name "location" in the target program and change it to "location * 50" in all the places.

6) Optional - The program attempts to automatically detect the target platform for abstract syntax tree parsing templates. If you want to manually configure the platform, then you can provide the information in the `platform.txt` file. The first line is either true or false. True enables the program to automatically detect the platform. The second line is the platform name. Currently, the tool supports only MATLAB&copy; and C. MATLAB&copy; represent Simulink generated C code with filename extension as `.c`.

# Dependecies
1) Ubuntu 16.04

2) docker version
--------------
Client:
 Version:      1.13.1
 API version:  1.26
 Go version:   go1.7.5
 Git commit:   092cba3
 Built:        Wed Feb  8 06:50:14 2017
 OS/Arch:      linux/amd64

Server:
 Version:      1.13.1
 API version:  1.26 (minimum version 1.12)
 Go version:   go1.7.5
 Git commit:   092cba3
 Built:        Wed Feb  8 06:50:14 2017
 OS/Arch:      linux/amd64
 Experimental: false

3) clang --version
---------------
clang version 3.8.0-2ubuntu4 (tags/RELEASE_380/final)
Target: x86_64-pc-linux-gnu
Thread model: posix
InstalledDir: /usr/bin

4) llvm-4.0

5) cloc -version
--------------
1.60

# To build and run
To run a code sample, you need to build the Docker container:

```sh
$ docker build -t clang .
```

If you then `cd` into mutator directory, like `code/mutator`, you will find a
`Makefile` specifically for this tool. To compile or use the tool, enter the
docker container and mount the folder under `/home`:

```sh
$ docker run -it -v $PWD:/home clang
```

Then just use the code executable already there, or re-build it with `make`. Please note that if you make changes to `config.txt` file or `spacevariables.txt` file then you have to execute `./runconfiguration` first. Similarly, if you make changes to `Makefile` then you have to rebuild it with `make` before running the tool. To summarize, the order of execution is first `./runconfiguration`, followed by `make`, and then the tool execution as below:

```sh
$ ./mutator -seed=7 -number=0 -category=PET b747cl.c
```
(or)

```sh
$ ./mutator -s 7 -n 0 -c PET b747cl.c
```

# Tool run - step by step tutorial

Normal Steps: 
1) Unzip mutationdocker.zip file 
2) Run in shell - cd to enter the directory, "cd mutationdocker" 
3) Run in shell - "docker build -t clang ." 
4) Run in shell - "cd code/mutator" 
5) Run in shell - "docker run -it -v $PWD:/home clang" 
6) Run in docker shell- "make" 
7) Run in docker shell - "exit" 
8) Run in shell - "./mutator -s 7 -n 0 -c PET b747cl.c" 
9) After step 6 you can run in docker shell - "./mutator -s 7 -n 0 -c PET b747cl.c", provided the docker has configured to access the system resources

Steps for naming the container (this should be carried out only after performing all the normal steps mentioned above): 
1) Run in shell - "cd code/mutator" 
2) Run in shell - "docker run -it --name "mutationcontainer" -v $PWD:/home clang" 
3) Run in docker shell - "exit" 
4) Run in shell - "docker start mutationcontainer" 
5) Run in shell - "./compilemutator.bash" (make and execute) 
6) Run in shell - "docker stop mutationcontainer" 

# Tool options
```sh
$ ./mutator <seed> <number> <categories> <target_file>
```

seed = random seed. 

number = number of mutants, please note that number 0 represents the maximum possible mutants to be generated from the target_file.

categories = PET (Precision&Accuracy, Exception Handling, Time and Space).

target_file = C/ C++ file name for which the mutants has to be generated.

# Output details
The generated mutants are present in a folder called 'mutants'. This folder contains subfolders for each template as mentioned in the MutationTemplates file.

# Troubleshoot
Thanks to William (Mike) Turner from NIMBUS lab. Here are some issues that was found and fixed.

1) It appears that Docker is now default configured to require sudo access. Mike suspects it is because they found a security flaw whereby a user without superuser access can actually gain it through a Docker container.
 
  Once you add sudo to my docker commands, it runs.

2) Troubles with running the mutator code:

  a) “./mutator” did not default to an executable script. Use chmod 775 to make it executable.
        
  b) First run of ./mutator as in README, “./mutator -s 7 -n 0 -c PET b747cl.c” resulted in a coredump error that “cloc” not found. Check your machine what version of cloc is used. In Ubuntu, if it returns a cloc –version of 1.60 then you have to run apt-get install cloc within Docker container. Even if you get version 1.74 or higher, you should be able to run the scripts.
  
  # FAQ
  **1) Can I run this mutation tool against my poject in C/ C++?<br />**
  Yes, you can. The tool is completely free to use. The `make` command you use for your project build will be setting up flags during the build process. First, you have to mention these flag settings, include directories and any dependencies in the mutation tool using the `config.txt` file. And then you have to execute `./runconfiguration`. Please note, if you make changes to `Makefile` of the mutation tool then you have to rebuild it with `make` before running the tool. To summarize, the order of execution is first `./runconfiguration`, followed by `make`, and then the tool execution.
  
 **2) Can I replicate your results?<br />**
  Yes, you can. But unfortunately, this docker contains only the tool and already mutated files for the three projects: b747cl, cruise control, and rct helicopter. Since MATLAB&copy; is licensed software and the three projects use MATLAB&copy; specific data types and libraries generated from MATLAB&copy; Simulink environment, we were unable to provide all the related files. However, if you have a MATLAB&copy; license then you can download these three different MATLAB&copy; Simulink control system project files for free in the following link:
  
Boeing 747 closed loop: https://www.mathworks.com/matlabcentral/fileexchange/3019-airlib. 

Cruise control: http://ctms.engin.umich.edu/CTMS/index.php?example=CruiseControl&section=SimulinkModeling

RCT helicopter: https://www.mathworks.com/help/control/examples/multi-loop-control-of-a-helicopter.html 
  
  Three different MATLAB&copy; Simulink control system project files are present in the following folders (you have to make sure all the dependencies are properly configured in the `config.txt` file and available in your system). **To run these projects separately, please read the `make` commands inside these folders. Also, read the `compilerun.bash`, and `runsave.bash` scripts.**<br/>
b747cl_grt_rtw<br/>
ccpi16a_grt_rtw<br/>
rct_helico_grt_rtw

Sample mutants generated from these models are present in the following folders:<br/>
mutants_b747cl<br/>
mutants_ccpi16a<br/>
mutants_rct_helico

Using this information and looking into the `config` files (inside `mutationdocker/code/mutator/` folder - config_b747cl.txt,  config_ccpi16a.txt, and config_rct_helico.txt) for each project will help you to replicate and also compare the results with mine. 

**3) How to configure MATLAB&copy; Simulink model for the mutation tool?**<br/>
For Boeing 747 project (you can download the Simulink model from https://www.mathworks.com/matlabcentral/fileexchange/3019-airlib), I will explain the steps. The first step is to add Simulink output blocks to the model and ensure you are able to store the output data (data such as altitude, airspeed, etc.) of the simulation in a MAT-file. After this generate C code from the Simulink model. Here is a useful link for generating C code from Simulink model: https://www.mathworks.com/help/rtw/gs/model-and-simulation.html?s_tid=mwa_osa_a. This should be fairly straight forward, just focus on generating C code section of the document. Next step is to build and execute the generated C code in a terminal shell (after closing the MATLAB&copy; environment), for this purpose, using the terminal get inside the directory where you have generated the C code. Please remember the directory in Github: mutationdocker/code/b747cl_grt_rtw/. You should have files something similar to these after generating C code. But more importantly, make sure you have the same suffix "grt_rtw" in the directory name. Because based on the platform and compiler these suffix names will be different. Once you have confirmed the suffix then you can do build using the "make -f b747cl.mk" command. To execute, use the "timeout 10s ./<exe file>" command.

If you are able to do this then open the b747cl.mk file. It should have "include paths" and "defines" section. These are the information that we want to configure in the mutation tool using the "mutationdocker/code/mutator/config_b747cl.txt" file. Once configured properly, you should not have any issues running the boeing747 project. Just note that the data stored in the MAT file will be used later for results analysis purposes. 

Congratulations! now you have successfully configured the mutation tool to run on MATLAB&copy; Simulink generated C file. You can now go to the beginning of the document and perform the following using the mutation tool:
1) Mutant generation - using "./mutator" command, the order of execution is first ./runconfiguration (if `config.txt` file is changed), followed by make, and then the tool execution.
2) Mutant compilation - using "compilerun.bash" script - change the path in `compilerun.bash` depending on your project.
3) Mutant execution - using "runsave.bash" script - change the path in `runsave.bash` depending on your project.
4) Result analysis - using MATLAB&copy; scripts, see "folder structure" section of this document. 

**4) Where to get more information about the tool (or) where is the master thesis?<br/>**
Please visit the following link for master thesis: https://digitalcommons.unl.edu/computerscidiss/165/<br/>
My personal website link: https://sites.google.com/site/balajibytes/download-pub
