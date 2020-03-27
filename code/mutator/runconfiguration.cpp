// Standard includes
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <stdlib.h> 
#include <stdio.h>
#include <array>
#include <algorithm> // for std::find
#include <stdlib.h>
#include <cstdlib>
//Read file
#include <fstream> 
#include <sstream>
#include <sys/stat.h>

//display
#include <iomanip>

using namespace std;

const char * spacetemplate_details = R"STD(
	timespaceTemplates.push_back("space variables mutation: $lhs_operand$ $math_operator$ $rhs_operand$");
	//<<Insert template details code here - do not delete this comment!!>>)STD";

const char * headerpath_template = R"HSP(
		  string clang_inc_dirNUMBER("$PATH$");
		  ArgumentsAdjuster ardjNUMBER = getInsertArgumentAdjuster(clang_inc_dirNUMBER.c_str());
		  Tool.appendArgumentsAdjuster(ardjNUMBER);
		  //<<Insert header search paths to compiler here - do not delete this comment!!>>)HSP";

const char * flag_template = R"FT(
	flagTemplateEnable[3][NUMBER]=true;
	//<<Insert flag array code here - do not delete this comment!!>>)FT";

const char * array_template = R"AT(
const int number_columns = NUMBER; //<<Insert array size code here - do not delete this comment!!>>)AT";

const char * print_template = R"PT(
	for (int j = 0; j < NUMBER; ++j) //<< Insert print code here - do not delete this comment!!>>)PT";

const char * findplatform_template = R"FP(
	findplatform_command="gcc $INCLUDE_PATH$ -M ";
	//<<Insert search include paths for platform - do not delete this comment!!>>)FP";

	
const char * matchhandler_template = R"MTH(
		template_index = NUMBER;
		if(flagTemplateEnable[category_index][template_index]){
			llvm::sys::fs::create_directory("./mutants/T"+to_string(template_index), true);
			const auto TSNUMBER = declRefExpr(
												isExpansionInMainFile(), 
												to(varDecl(hasName("$lhs_operand$"))),
												unless(hasAncestor(ifStmt())),
												unless(hasAncestor(arraySubscriptExpr())),
												unless(hasAncestor( varDecl(hasType(arrayType()))))
											).bind("tsNUMBER");
			MatchFinder MatchFinder3NUMBER;
			MatchFinder3NUMBER.addMatcher(TSNUMBER, &Handler);
			MatchFinder3NUMBER.matchAST(Context);
		}
		//<<Insert match handler code here - do not delete this comment!!>>)MTH";


const char * matchreader_template = R"MTR(
			else if(template_index == NUMBER){
				if(flagTemplateEnable[category_index][template_index]){
					const clang::DeclRefExpr* Variable3NUMBER = Result.Nodes.getNodeAs<clang::DeclRefExpr>("tsNUMBER");
					loc = Variable3NUMBER->getSourceRange();
					locStart = srcMgr.getPresumedLoc(loc.getBegin());
					locEnd = srcMgr.getPresumedLoc(loc.getEnd());

					counterMatch[category_index][template_index]=counterMatch[category_index][template_index]+1;
				}
			}
			//<<Insert match reader code here - do not delete this comment!!>>)MTR";

const char * matchwriter_template = R"MTW(
			else if(template_index == NUMBER){
					if(flagTemplateEnable[category_index][template_index]){
						if (found != end){
							const clang::DeclRefExpr* Variable3NUMBER = Result.Nodes.getNodeAs<clang::DeclRefExpr>("tsNUMBER");
							clang::SourceRange loc = Variable3NUMBER->getSourceRange();

							clang::LangOptions lopt;

							loc = Variable3NUMBER->getSourceRange();
							locStart = srcMgr.getPresumedLoc(loc.getBegin());
							locEnd = srcMgr.getPresumedLoc(loc.getEnd());

							SourceLocation endLoc = clang::Lexer::getLocForEndOfToken(loc.getEnd(), 0, srcMgr, lopt);

							/// The Rewriter used to insert the override keyword.
							clang::Rewriter rewriter;
							rewriter.setSourceMgr(Result.Context->getSourceManager(),Result.Context->getLangOpts());
							rewriter.InsertText(endLoc,"$math_operator$ $rhs_operand$",true,true);

							// Now emit the rewritten buffer.
							std::error_code error_code;
							write_filename="./mutants/T"+to_string(template_index)+input_filename+to_string(mutantNumber)+input_filename_extension;
							llvm::raw_fd_ostream outFile(write_filename, error_code, llvm::sys::fs::F_None);
							rewriter.getEditBuffer(rewriter.getSourceMgr().getMainFileID()).write(outFile); 
							outFile.close();

							mutation_comments = "\n/* Time and space category mutation - template TNUMBER: variable name $lhs_operand$ of space category is mutated.  \n* Before mutation => "+ 
															readFile(locStart.getLine()-1,tmp_filepath) +
															"\n* Mutation begins */\n"+
															readFile( locStart.getLine()-1, write_filename)+
															"\n/* Mutation ends */\n";
							appendToFile(write_filename, locStart.getLine()-1, mutation_comments,true);

							generated_mutants_count[category_index][template_index]++;
							generated_mutants_locations_count[category_index][template_index]=generated_mutants_locations_count[category_index][template_index]+(to_string)(locStart.getLine()-1)+",";
						}
						mutantNumber++;
					}
				}
				//<<Insert match writer code here - do not delete this comment!!>>)MTW";

void copyFile(string srcFile, string dstFile){
	std::ifstream  src(srcFile, std::ios::binary);
    std::ofstream  dst(dstFile,   std::ios::binary);

    dst << src.rdbuf();
	dst.flush();
	src.close();
	dst.close();
}

inline bool file_exists (const std::string& name) {
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
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
		cout<<"\nWARNING! An error occured while trying to read input file .... Comments in the mutated code may not be complete.! \n" << endl;
	}
	return line_content;
}

void removeLine(string sFilename, int nDeleteAt)
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

    for (int i = 0; i < vLines.size(); i++)
    {
        //char buff[1024];
        if (i == nDeleteAt)
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
	cout<< "sFilename: " << sFilename << "\n" <<endl;
    std::ifstream infile(sFilename);
	 if ( !infile ){
		 cout<<"\nWARNING! An error occured while trying to write .... Comments in the mutated code may not be complete.! \n" << endl;
         exit(0);
	 }
	
	if(deleteLineFlag){
		removeLine(sFilename,nInsertAt);
		nInsertAt--;
	}
	infile.close();
	
	exit(0);
	
}

int replaceStringFile(string sFilename, string wordToReplace, string wordToReplaceWith){
	string temp_file="temp.cpp";
	ifstream in(sFilename);
    ofstream out(temp_file);
	
    if (!in)
    {
        cerr << "Could not open " << sFilename << "\n";
        return 1;
    }

    if (!out)
    {
        cerr << "Could not open " << temp_file << "\n";
        return 1;
    }
	
    string line;int linno=0;
    size_t len = wordToReplace.length();
    while (getline(in, line))
    {
        //while (true)
        //{
            size_t pos = line.find(wordToReplace);
            if (pos != string::npos){
                line.replace(pos, len, wordToReplaceWith);
			}
            //else 
                //break;
        //}
        out << line << '\n';
		linno++;
    }
	//cout<< "lineno: "<<linno<<endl;
	out.flush();
	in.close();
	out.close();
	
	copyFile(temp_file, sFilename);
	return 0;
}

int findLine(string read_filename, string SearchWord){
    int counter = 0;
	size_t found;
	
	if(file_exists(read_filename)){
		std::ifstream infile(read_filename);
		for( std::string line; getline( infile, line ); )
		{
			found = line.find(SearchWord);
			if(found!=std::string::npos){
				return counter;
			}
			counter++;
		}
	}else{
		cout<<"\nWARNING! An error occured while trying to read " << read_filename << " file .... Comments in the mutated code may not be complete.! \n" << endl;
	}
	cout<< "SearchWord: "<<SearchWord<<endl;
	cout<<"\nERROR! NO match found\n"<<endl;
	return counter;
}

std::string ReplaceString(std::string subject, const std::string& search,
                          const std::string& replace) {
    size_t pos = 0;
    while((pos = subject.find(search, pos)) != std::string::npos) {
         subject.replace(pos, search.length(), replace);
         pos += replace.length();
    }
    return subject;
}

int main(int argc, char *argv[]){

	string config_file_name;
	if(argc==2)
	{
		config_file_name=argv[1];
	}
	if(argc>2){
		cout << "\nOnly one argument (configuration file name) is expected of the following format: ./runconfiguration config_projectname.txt\n"<<endl;
		return 0;
	}
	if(argc<2){
		cout << "\nPlease provide the configuration file name of the following format: ./runconfiguration config_projectname.txt\n"<<endl;
		return 0;
	}

	string mutator_file="mutator.cpp";
	string mutator_base="mutator_base.cpp";
	copyFile(mutator_base, mutator_file);
	
	int variable_offset=4; //four time related variables are already present
	string matchhandler="",matchreader="",matchwriter="";
	
	//int matchhandler_linenumber=0, matchreader_linenumber=0, matchwriter_linenumber=0;
	int arraysize_linenumber=0, printcode_linenumber=0;//, flagarray_linenumber=0, headerpath_linenumber=0, spacetemplate_linenumber=0;
	
	/*
	cout<< "matchhandler_linenumber: "<<matchhandler_linenumber<<endl;
	cout<< "matchreader_linenumber: "<<matchreader_linenumber<<endl;
	cout<< "matchwriter_linenumber: "<<matchwriter_linenumber<<endl;
	cout<< "flagarray_linenumber: "<<flagarray_linenumber<<endl;
	
	cout<< "arraysize_linenumber: "<<arraysize_linenumber<<endl;
	cout<< "printcode_linenumber: "<<printcode_linenumber<<endl<<endl;
		
	//cout<<ReplaceString((string)matchhandler_template,"NUMBER","13")<<endl;
	//cout<<ReplaceString((string)matchreader_template,"NUMBER","13")<<endl;
	//cout<<ReplaceString((string)matchwriter_template,"NUMBER","13")<<endl;
	*/
	
	string space_variable_file="spacevariable.txt", findtemplate_string="";
	string config_path_file=config_file_name; //config_rct_helico //config_ccpi16a //config_ardupilot //config_b747cl
	int no_of_variables_file=0;
	int var_idx=0;
	
	cout<<"\nConfiguring "<<space_variable_file<<"..."<<endl;
	if(file_exists(space_variable_file)){
		std::ifstream infile(space_variable_file);
		std::vector<string> vect;
		
		for( std::string line; getline( infile, line ); )
		{	
			std::stringstream ss(line);
			
			cout<<"Reading line["<< var_idx <<"]:"<< line << endl;
			//cout <<  "Reading line "<< var_idx <<".." << line << "\r";
			//fflush ( stdin );
	
			while( ss.good() )
			{
				string substr;
				getline( ss, substr, ',' );
				vect.push_back( substr );
			}
			/*
			for (string::size_type i=0; i< vect.size(); i++){
				//cout<<"D3"<<endl;
				//std::cout << vect.at(i)<<std::endl;
				if(i==0)
					lhs_operand[var_idx]=vect.at(i);
				if(i==1)
					math_operator[var_idx]=vect.at(i);
				if(i==2)
					rhs_operand[var_idx]=vect.at(i);
			}
			*/
			
			if(vect.size()==3){
				//matchhandler_linenumber = findLine(mutator_file,"//<<Insert match handler code here - do not delete this comment!!>>");
				matchhandler=ReplaceString( ReplaceString( (string)matchhandler_template,"$lhs_operand$",vect.at(0) ),"NUMBER",to_string(var_idx+variable_offset) );
				replaceStringFile(mutator_file,"//<<Insert match handler code here - do not delete this comment!!>>",matchhandler);
				//appendToFile(mutator_file,matchhandler_linenumber,matchhandler,true);

				//matchreader_linenumber = findLine(mutator_file,"//<<Insert match reader code here - do not delete this comment!!>>");
				matchreader=ReplaceString(  (string)matchreader_template, "NUMBER", to_string(var_idx+variable_offset) );
				//appendToFile(mutator_file,matchreader_linenumber,matchreader,true);
				replaceStringFile(mutator_file,"//<<Insert match reader code here - do not delete this comment!!>>",matchreader);

				//matchwriter_linenumber = findLine(mutator_file,"//<<Insert match writer code here - do not delete this comment!!>>");
				matchwriter=ReplaceString( ReplaceString( ReplaceString( ReplaceString( (string)matchwriter_template,"$lhs_operand$",vect.at(0) ),"$math_operator$",vect.at(1) ) ,"$rhs_operand$",vect.at(2) ),"NUMBER",to_string(var_idx+variable_offset) );
				//appendToFile(mutator_file,matchwriter_linenumber,matchwriter,true);
				replaceStringFile(mutator_file,"//<<Insert match writer code here - do not delete this comment!!>>",matchwriter);

				//flagarray_linenumber = findLine(mutator_file,"//<<Insert flag array code here - do not delete this comment!!>>"); 
				//appendToFile(mutator_file,flagarray_linenumber, ReplaceString(  (string)flag_template, "NUMBER", to_string(var_idx+variable_offset) ),true);
				replaceStringFile(mutator_file,"//<<Insert flag array code here - do not delete this comment!!>>",ReplaceString(  (string)flag_template, "NUMBER", to_string(var_idx+variable_offset) ));
				
				//spacetemplate_linenumber = findLine(mutator_file,"//<<Insert template details code here - do not delete this comment!!>>"); 
				//appendToFile(mutator_file,spacetemplate_linenumber, ReplaceString(  ReplaceString(  ReplaceString(  (string)spacetemplate_details, "$lhs_operand$",vect.at(0)  ), "$math_operator$",vect.at(1)  ), "$rhs_operand$",vect.at(2)  ),true);
				replaceStringFile(mutator_file,"//<<Insert template details code here - do not delete this comment!!>>",ReplaceString(  ReplaceString(  ReplaceString(  (string)spacetemplate_details, "$lhs_operand$",vect.at(0)  ), "$math_operator$",vect.at(1)  ), "$rhs_operand$",vect.at(2)  ));
				
				vect.clear();
				var_idx++;
			}else{
				cout<<"ERROR in line "<< var_idx+1 <<"! "<<space_variable_file<<" does not follow the following format.."<<endl;
				cout<<"<variable_name>,<operator>,<number>"<<endl;
				cout<<"Please ensure the above format with two comma separated values are provided in each line."<<endl;
			}
		}
		no_of_variables_file = var_idx;
		infile.close();
		
	}else{
		cout<<"ERROR! "<<space_variable_file<<" file does not exists."<<endl;
		return 0;
	}
	
	if(no_of_variables_file>6){
		arraysize_linenumber = findLine(mutator_file,"//<<Insert array size code here - do not delete this comment!!>>"); //const int number_columns = 15;  
		//appendToFile(mutator_file,arraysize_linenumber, ReplaceString(  (string)array_template, "NUMBER", to_string(10-6+no_of_variables_file) ),true); //10
		
		replaceStringFile(mutator_file,"//<<Insert array size code here - do not delete this comment!!>>",ReplaceString(  (string)array_template, "NUMBER", to_string(10-6+no_of_variables_file) ));
		removeLine(mutator_file,arraysize_linenumber);
	}
	
	printcode_linenumber = findLine(mutator_file,"//<<Insert print code here - do not delete this comment!!>>"); //for (int j = 0; j < 4; ++j) 
	//appendToFile(mutator_file,printcode_linenumber, ReplaceString(  (string)print_template, "NUMBER", to_string(4+no_of_variables_file) ),true); //4
	replaceStringFile(mutator_file,"//<<Insert print code here - do not delete this comment!!>>",ReplaceString(  (string)print_template, "NUMBER", to_string(4+no_of_variables_file) ));
	removeLine(mutator_file,printcode_linenumber);
	
	//including config.txt file header paths for compiler
	var_idx=0;
	cout<<"\n\nConfiguring "<<config_path_file<<"..."<<endl;
	if(file_exists(config_path_file)){
		std::ifstream infile(config_path_file);
		
		for( std::string line; getline( infile, line ); )
		{	
			std::stringstream ss(line);
			cout<<"Reading line["<< var_idx <<"]:"<< line << endl;
			//cout <<  "Reading line "<< var_idx <<".." << line << "\r";
			//fflush ( stdin );
	
			//headerpath_linenumber = findLine(mutator_file,"//<<Insert header search paths to compiler here - do not delete this comment!!>>"); 
			//appendToFile(mutator_file,headerpath_linenumber, ReplaceString(  ReplaceString(  (string)headerpath_template, "$PATH$", line), "NUMBER", to_string(var_idx+1) ),true);
			replaceStringFile(mutator_file,"//<<Insert header search paths to compiler here - do not delete this comment!!>>",ReplaceString(  ReplaceString(  (string)headerpath_template, "$PATH$", line), "NUMBER", to_string(var_idx+1) ));
			findtemplate_string=findtemplate_string+" "+line;
			var_idx++;
		}
		infile.close();
	}else{
		cout<<"ERROR! "<<config_path_file<<" File does not exists  "<<endl;
		return 0;
	}
		
	copyFile("lib_base.cpp", "lib.cpp");
	replaceStringFile("lib.cpp","//<<Insert search include paths for platform - do not delete this comment!!>>",ReplaceString(  (string)findplatform_template, "$INCLUDE_PATH$", findtemplate_string ));
	
	cout<<"\n"<<endl;
	return 0;
}
