// Standard includes
#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h> 
#include <stdio.h>

using namespace std;

inline bool file_exists (const std::string& name) {
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
}

int * random_generator(int min, int max, int seed, int number){
	//printf("\n ---- Random generator  start    ---- \n");
	srand(seed);
	static int value[1000]; //change here if you want a template to have more than 1000 mutation. This is per template limitation, not a per category limitation. 
	for(int i=0; i <  number ; i++){
		bool check; //variable to check or number is already used
		int n; //variable to store the number in
		do
		{
		n = min + (rand() % static_cast<int>(max - min + 1));
		//check or number is already used:
		check=true;
		for (int j=0;j<i;j++)
		if (n == value[j]) //if number is already used
		{
			check=false; //set check to false
			break; //no need to check the other elements of value[]
		}
		} while (!check); //loop until new, unique number is found
		value[i]=n; //store the generated number in the array
		//cout << value[i] << endl;
	}
	//printf("\n ---- Random generator  end    ---- \n");
	return value;
}


std::string GetStdoutFromCommand(std::string cmd) {

    string data;
    FILE * stream;
    const int max_buffer = 256;
    char buffer[max_buffer];
    cmd.append(" 2>&1");

    stream = popen(cmd.c_str(), "r");
    if (stream) {
    while (!feof(stream))
    if (fgets(buffer, max_buffer, stream) != NULL) data.append(buffer);
    pclose(stream);
    }
    return data;
}

string readFile(int lineNumber, string read_filename){
	string line_content="";
	int n=0;
	
	if(file_exists(read_filename)){
		std::ifstream infile(read_filename);
		for( std::string line; getline( infile, line ); )
		{
			if(lineNumber==n)
				line_content = line;
			n++;
		}
	}else{
		cout<<"\nWARNING! An error occured while trying to read input file: "+read_filename+" .... Comments in the mutated code may not be complete.! \n" << endl;
	}
	return line_content;
}

void removeLine(string sFilename, int nInsertAt)
{	
    std::ifstream infile(sFilename);

    string line;
    vector<string> vLines;

    while (std::getline(infile, line))
    {
        vLines.push_back(line);
    }

    infile.close();
    std::ofstream outfile(sFilename);

    for (string::size_type i = 0; i < vLines.size(); i++)
    {
        //char buff[1024];
        if (i == (size_t)nInsertAt)
        {
            //sprintf(buff, "%s %s", vLines[i].c_str(), sDataToInsert.c_str());
            //outfile << buff << endl;
        }
        else
        {
            outfile << vLines[i] << endl;
        }
    }
    outfile.close();
}

void appendToFile(string sFilename, int nInsertAt, string sDataToInsert, bool deleteLineFlag)
{
	 
    std::ifstream infile(sFilename);
	 if ( !infile ){
		 cout<<"\nWARNING! An error occured while trying to write .... Comments in the mutated code may not be complete.! \n" << endl;
         exit(0);
	 }
	
	if(deleteLineFlag){
		removeLine(sFilename,nInsertAt);
		nInsertAt--;
	}
    string line;
    vector<string> vLines;

    while (std::getline(infile, line))
    {
        vLines.push_back(line);
    }

    infile.close();
    std::ofstream outfile(sFilename);

    for (string::size_type i = 0; i < vLines.size(); i++)
    {
        char buff[1024];
        if (i == (size_t)nInsertAt){
            sprintf(buff, "%s %s", vLines[i].c_str(), sDataToInsert.c_str());
            outfile << buff << endl;
        }
        else{
            outfile << vLines[i] << endl;
        }

    }
    outfile.close();
}


int get_unique_linelocations_count(string locationlist){
	std::vector<int> int_vect;
	std::istringstream ss(locationlist);
	std::string token;
	while(std::getline(ss, token, ',')) {
		int_vect.push_back( stoi(token.c_str()) );
	}
	vector<int>::iterator ip;
    // Sorting the array
    std::sort(int_vect.begin(), int_vect.end());
	int count = std::distance(int_vect.begin(),
                          std::unique(int_vect.begin(), int_vect.begin() + int_vect.size()));
    return count;
}
		
bool is_digits(const std::string &str)
{
    return str.find_first_not_of("0123456789.E+*-/") == std::string::npos;
}
		
bool replace(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

string basename(const string &filename)
{
    if (filename.empty()) {
        return {};
    }

    auto len = filename.length();
    auto index = filename.find_last_of("/\\");

    if (index == std::string::npos) {
        return filename;
    }

    if (index + 1 >= len) {

        len--;
        index = filename.substr(0, len).find_last_of("/\\");

        if (len == 0) {
            return filename;
        }

        if (index == 0) {
            return filename.substr(1, len - 1);
        }

        if (index == std::string::npos) {
            return filename.substr(0, len);
        }

        return filename.substr(index + 1, len - index - 1);
    }

    return filename.substr(index + 1, len - index);
}	

string getPlatform(string includePaths){
	string platformName="";
	const string sentence = includePaths;

	if (sentence.find("MATLAB") != std::string::npos && sentence.find("rt_defines.h") != std::string::npos && sentence.find("types.h") != std::string::npos)
		platformName="MATLAB";
	else
		platformName="C";

	return platformName;
}

bool check_valid_platform(string platformName){
	bool valid_platform=false;
	if( platformName.find("MATLAB")!=string::npos || platformName.find("C")!=string::npos || platformName.find("CPP")!=string::npos ){
		valid_platform=true;
	}
	return valid_platform;
}

string detect_target_platform(string filename, string target_platform_file){
	
	cout<<"---------------------------------------------------------------------------------------"<<endl;
	cout<< "Target platform detection:"<<endl;
	cout<< "--------------------------"<<endl;
	cout<< "Target file name: "<< filename<<endl;
	cout<< "Reading configuration from " <<target_platform_file<<"..."<< endl;
	
	string findplatform_command="", targetfile_platform="";
	if( readFile(0,target_platform_file) == "TRUE" ) {
		cout<< "Automatic detection selected..." <<endl;
		
		if(filename.substr(filename.find_last_of(".") + 1) == "cpp") {
			targetfile_platform="CPP";
		} else {
			// Find the platform
			//<<Insert search include paths for platform - do not delete this comment!!>>


			//findplatform_command=findplatform_command+basename(tmp_filepath);
			findplatform_command=findplatform_command+filename; 
			//remove unwanted includes - ensure the make of the target file is sent here
			replace(findplatform_command, "-I/usr/lib/clang/3.8.0/include ", "");
			replace(findplatform_command, "-I/home/balaji/clang-llvm/mutationdocker/code/mutator ", "");
			replace(findplatform_command, "-I/home/balaji/clang-llvm/mutationdocker/code ", "");

			targetfile_platform = getPlatform(GetStdoutFromCommand(findplatform_command.c_str()));
		}
	}else{
		cout<< "Manual configuration selected..." <<endl;
		targetfile_platform = readFile(1,target_platform_file);
	}
	
	cout<<"Target platform is determined as "<< targetfile_platform+"." << endl;
	
	cout<<"\nNOTE: The program attempts to automatically detect the target platform for abstract syntax tree parsing templates. If you want to manually configure the platform, then you can provide the information in the platform.txt file. First line is either true or false. True enables the program to automatically detect the platform. Second line is the platform name. Currently, the tool supports only MATLAB, C and CPP.\n"<<endl;
	
	if(!check_valid_platform(targetfile_platform)){
		cout<< "ERROR: The target platform is not supported! Please make sure the target platform for the target file is MATLAB or C or CPP !!\n\n" <<endl;
		exit(0);
	}
	
	cout<<"---------------------------------------------------------------------------------------"<<endl;
	return targetfile_platform;
}
