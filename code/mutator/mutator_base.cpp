// Clang includes
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/Decl.h"
#include "clang/AST/Type.h"
#include "clang/AST/Expr.h"
  
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Frontend/CompilerInstance.h"	
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/ArgumentsAdjusters.h"
#include "clang/Rewrite/Frontend/Rewriters.h"
#include "clang/Rewrite/Core/RewriteBuffer.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Lex/Lexer.h"

// LLVM includes
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"

// Standard includes
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <stdlib.h> 
#include <stdio.h>
#include <array>
#include <iomanip>
#include <algorithm> // for std::find

//Read file
#include <iostream>
#include <fstream> 
#include <sstream>
#include <sys/stat.h>

//custom libraries
#include "lib.cpp"

using namespace std;
using namespace llvm;
using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::tooling;

#define ROUNDF(f, c) (((float)((int)((f) * (c))) / (c)))

const int number_rows = 5;
const int number_columns = 10; //<<Insert array size code here - do not delete this comment!!>>
int counterMatch[number_rows][number_columns];// = { [0 ... ROW-1] = { [0 ... COLUMN-1] = 1 } }; // all elements initialized to 0.
bool flagTemplateEnable[number_rows][number_columns];
int mutants_number[number_rows][number_columns]; // mutant count matrix - generated based on user inputs of random seed and number of mutants
int generated_mutants_count[number_rows][number_columns]; // finally generated mutants count
string generated_mutants_locations_count[number_rows][number_columns]; // finally generated mutants unique locations count
int category_index=0;
int template_index=0;
bool countMatchCompleted=false;
int mutantNumber=1;
int * random(int,int,int);
int * randomarray;
string user_categories = ""; //user specified categories to generate mutant
int sum_possible_mutants=0;//total possible mutatnts - dont change this - this is automatically detected and changed by the tool
int no_of_mutants=0; //total mutants that the user wants - this is set from the command line argument
int user_seed=0; // random seed number- this is set from the command line argument
string tmp_filepath="";
string input_filename=""; // input file name to mutate 
string input_filename_extension=""; // input file name to mutate 
vector<string> precisionaccuracyTemplates;
vector<string> exceptionhandlingTemplates;
vector<string> timespaceTemplates;
int total_unique_locations=0;
string targetfile_platform="";

//Read variables from file
string space_variable_file="spacevariable.txt";
string target_platform_file="platform.txt";
int no_of_variables_file=0;

namespace { //helper man page
  llvm::cl::OptionCategory ToolCategory("mutator options");

  llvm::cl::opt<string> Filename(llvm::cl::Positional, llvm::cl::desc("<c or cpp input file for mutation>"), llvm::cl::init("-"));

  llvm::cl::opt<bool> verbose_compiler(
  "vc",
  llvm::cl::desc("pass -v to compiler instance (default false)"),
  llvm::cl::cat(ToolCategory),
  llvm::cl::init(false)
);

  cl::opt<int> Seed(
  "seed", 
  cl::desc("Specify seed to generate random mutation."),
  cl::value_desc("integer"),
  cl::init(1),
  cl::cat(ToolCategory)
); 

  llvm::cl::alias SeedShortOption("s",
                       llvm::cl::desc("Alias for the -seed option"),
                       llvm::cl::aliasopt(Seed));

  cl::opt<int> Number(
  "number", 
  cl::desc("Specify number to limit the number of mutants generated."),
  cl::value_desc("integer"),
  cl::init(1),
  cl::cat(ToolCategory)
); 

  llvm::cl::alias NumberShortOption("n",
                       llvm::cl::desc("Alias for the -number option"),
                       llvm::cl::aliasopt(Number));

  cl::opt<string> Category(
  "category", 
  cl::desc("P for Precision, E for Exception handling, and T for Time and space model."), // R for Resource limitation,
  cl::value_desc("CPS categories"),
  cl::init("P"),
  cl::cat(ToolCategory)
);

  llvm::cl::alias CategoryShortOption("c",
                       llvm::cl::desc("Alias for the -category option"),
                       llvm::cl::aliasopt(Category));

llvm::cl::extrahelp MoreHelp(R"(
	Tool name: Cyber-Physical Mutation tool
	Author names: Balaji Balasubramaniam, Prof. Justin Bradley, Prof. Sebastian Elbaum
	University of Nebraska-Lincoln
)");
  
//llvm::cl::extrahelp
    //CommonHelp(clang::tooling::CommonOptionsParser::HelpMessage);
}  // namespace helper

int * mutant_random_generator(){
		//cout<<"*********      mutant_random_generator   START  **********"<<endl;
		//get percentage representation
		float percentage_counterMatch[number_rows][number_columns];
		for (int i = 0; i < number_rows; ++i)
		{
			for (int j = 0; j < number_columns; ++j)
			{
				percentage_counterMatch[i][j] = (float) counterMatch[i][j] / sum_possible_mutants;
			}
		}
		
		//ensure minimum one mutatnt of each type is generated
		for (int i = 0; i < number_rows; ++i)
		{
			for (int j = 0; j < number_columns; ++j)
			{
				if(counterMatch[i][j]>0){
					mutants_number[i][j]=1;
				}else{
					mutants_number[i][j]=0;
				}
			}
		}
		
		//percentage split of user input mutants
		for (int i = 0; i < number_rows; ++i)
		{
			for (int j = 0; j < number_columns; ++j)
			{
				if(counterMatch[i][j]>0){
					if(percentage_counterMatch[i][j]*no_of_mutants>1)
						mutants_number[i][j]=round(percentage_counterMatch[i][j]*no_of_mutants);
						//printf("\ni:%d, j:%d, el:%d\n",i,j,mutants_number[i][j]);
				}
			}
		}
		
		// make sure number of mutants is equal to user input number
		int sum_representative_mutants=0;
		for (int i = 0; i < number_rows; ++i)
		{
			for (int j = 0; j < number_columns; ++j)
			{
				sum_representative_mutants += mutants_number[i][j] ;
			}
		}
		
		while(sum_representative_mutants > no_of_mutants){
			int max_idx_row=0, max_idx_column=0;
			int highest=0; //mutants_number[max_idx_row][max_idx_column];
			
			for (int i = 0; i < number_rows; ++i)
			{
				for (int j = 0; j < number_columns; ++j)
				{
					if(mutants_number[i][j]>highest){
						max_idx_row=i;
						max_idx_column=j;
						highest=mutants_number[i][j];
					}
				}
			}
			mutants_number[max_idx_row][max_idx_column] -=1;
			sum_representative_mutants-=1;
		}
		//printf ("\n After normalizing Total Mutants: %d \n\n",sum_representative_mutants);
		
		int total_enabled_templates_categories[number_rows]={0};

		int total_enabled_categories=0;

		for(int idx_r=0;idx_r<number_rows;idx_r++){
			for(int idx_c=0;idx_c<number_columns;idx_c++){
				if(flagTemplateEnable[idx_r][idx_c]){
					total_enabled_templates_categories[idx_r]++;
				}
			}
		}

		for(int idx_r=0;idx_r<number_rows;idx_r++){
			//cout<<"total enabled templates categories["<<idx_r<<"]:  "<<total_enabled_templates_categories[idx_r]<<endl;
			if(total_enabled_templates_categories[idx_r]>0){
				total_enabled_categories++;
			}
		}
		//cout<<"total_enabled_categories: "<<total_enabled_categories<<endl;
		
		// a pointer to an int.
		int *p;
		int min_sum=1;  
		int final_mutant_numbers[no_of_mutants];  
		int final_mutant_index=0;
		  
		for(int idx_r=0;idx_r<number_rows;idx_r++){
			if(total_enabled_templates_categories[idx_r]>0){
				for(int idx_c=0;idx_c<number_columns;idx_c++){
					
					p = random_generator(min_sum,min_sum - 1 + counterMatch[idx_r][idx_c],user_seed,mutants_number[idx_r][idx_c]);

					for ( int i = 0; i < mutants_number[idx_r][idx_c]; i++ ) {
						final_mutant_numbers[final_mutant_index]=*(p + i);
						final_mutant_index++;
					}
					
					min_sum = min_sum+counterMatch[idx_r][idx_c] ;
				}
			}
		}
	//cout<<"*********      mutant_random_generator   END   **********"<<endl;
	return final_mutant_numbers;
}

namespace Mutator {

/// Callback class for mutator matches.
class MatchHandler : public clang::ast_matchers::MatchFinder::MatchCallback {
 public:
  using MatchResult = clang::ast_matchers::MatchFinder::MatchResult;

  /// Handles the matched variable.
  ///
  /// Checks if the name of the matched variable is either empty or prefixed
  /// with `clang_` else emits a diagnostic and FixItHint.
  void run(const MatchResult& Result) {
	clang::SourceManager &srcMgr = Result.Context->getSourceManager();
	
	clang::SourceRange loc;
	clang::PresumedLoc locStart;
	clang::PresumedLoc locEnd;
	  
	int * mut_arr;//store random numbers based on user seed for representative sampling from mutation population
	int final_mutant_numbers[no_of_mutants]; 
	size_t numElements;
	int* end = nullptr;
	int* found = nullptr;
	vector<int> v;
	string write_filename="", mutation_comments = "", platform_comments="";
			
    if (!countMatchCompleted){
		if(category_index == 0){ //Precision
			if(template_index == 0){
				if(flagTemplateEnable[category_index][template_index]){
					const clang::VarDecl* Variable0 = Result.Nodes.getNodeAs<clang::VarDecl>("pa0");
					loc = Variable0->getSourceRange();
					locStart = srcMgr.getPresumedLoc(loc.getBegin());
					locEnd = srcMgr.getPresumedLoc(loc.getEnd());

					counterMatch[category_index][template_index]=counterMatch[category_index][template_index]+1;
				}
			}else if(template_index == 1){
				if(flagTemplateEnable[category_index][template_index]){
				const clang::VarDecl* Variable1 = Result.Nodes.getNodeAs<clang::VarDecl>("pa1");
				loc = Variable1->getSourceRange();
				locStart = srcMgr.getPresumedLoc(loc.getBegin());
				locEnd = srcMgr.getPresumedLoc(loc.getEnd());
				
				counterMatch[category_index][template_index]=counterMatch[category_index][template_index]+1;
				}
			}else if(template_index == 2){
				if(flagTemplateEnable[category_index][template_index]){
				
				const clang::VarDecl* Variable2 = Result.Nodes.getNodeAs<clang::VarDecl>("pa2");
				loc = Variable2->getSourceRange();
				locStart = srcMgr.getPresumedLoc(loc.getBegin());
				locEnd = srcMgr.getPresumedLoc(loc.getEnd());
				
				counterMatch[category_index][template_index]=counterMatch[category_index][template_index]+1;
				}
			}else if(template_index == 3){
				if(flagTemplateEnable[category_index][template_index]){
				const clang::VarDecl* Variable3 = Result.Nodes.getNodeAs<clang::VarDecl>("pa3");
				loc = Variable3->getSourceRange();
				locStart = srcMgr.getPresumedLoc(loc.getBegin());
				locEnd = srcMgr.getPresumedLoc(loc.getEnd());
				
				counterMatch[category_index][template_index]=counterMatch[category_index][template_index]+1;
				}	
			}else if(template_index == 4){
				if(flagTemplateEnable[category_index][template_index]){
				const clang::IntegerLiteral* Variable4 = Result.Nodes.getNodeAs<clang::IntegerLiteral>("pa4");
				loc = Variable4->getSourceRange();
				locStart = srcMgr.getPresumedLoc(loc.getBegin());
				locEnd = srcMgr.getPresumedLoc(loc.getEnd());
				
				counterMatch[category_index][template_index]=counterMatch[category_index][template_index]+1;
				}	
			}else if(template_index == 5){
				if(flagTemplateEnable[category_index][template_index]){
				const clang::IntegerLiteral* Variable5 = Result.Nodes.getNodeAs<clang::IntegerLiteral>("pa5");
				loc = Variable5->getSourceRange();
				locStart = srcMgr.getPresumedLoc(loc.getBegin());
				locEnd = srcMgr.getPresumedLoc(loc.getEnd());
				
				counterMatch[category_index][template_index]=counterMatch[category_index][template_index]+1;
				}	
			}else if(template_index == 6){
				if(flagTemplateEnable[category_index][template_index]){
				const clang::FloatingLiteral* Variable6 = Result.Nodes.getNodeAs<clang::FloatingLiteral>("pa6");
				loc = Variable6->getSourceRange();
				locStart = srcMgr.getPresumedLoc(loc.getBegin());
				locEnd = srcMgr.getPresumedLoc(loc.getEnd());
				
				counterMatch[category_index][template_index]=counterMatch[category_index][template_index]+1;
				}	
			}else if(template_index == 7){
				if(flagTemplateEnable[category_index][template_index]){
					const clang::CallExpr* Variable7 = Result.Nodes.getNodeAs<clang::CallExpr>("pa7");
					loc = Variable7->getSourceRange();
					locStart = srcMgr.getPresumedLoc(loc.getBegin());
					locEnd = srcMgr.getPresumedLoc(loc.getEnd());

					counterMatch[category_index][template_index]=counterMatch[category_index][template_index]+1;
				}
			}else if(template_index == 8){
				if(flagTemplateEnable[category_index][template_index]){
				
				const clang::CallExpr* Variable8 = Result.Nodes.getNodeAs<clang::CallExpr>("pa8");
				loc = Variable8->getSourceRange();
				locStart = srcMgr.getPresumedLoc(loc.getBegin());
				locEnd = srcMgr.getPresumedLoc(loc.getEnd());
				
				counterMatch[category_index][template_index]=counterMatch[category_index][template_index]+1;
				}
			}else if(template_index == 9){
				if(flagTemplateEnable[category_index][template_index]){
					const clang::IntegerLiteral* Variable9 = Result.Nodes.getNodeAs<clang::IntegerLiteral>("pa9");
					loc = Variable9->getSourceRange();
					locStart = srcMgr.getPresumedLoc(loc.getBegin());
					locEnd = srcMgr.getPresumedLoc(loc.getEnd());

					counterMatch[category_index][template_index]=counterMatch[category_index][template_index]+1;
				}	
			}
		}else if(category_index == 1){ //Exception
			if(template_index == 0){
				if(flagTemplateEnable[category_index][template_index]){
				const clang::DeclRefExpr* Variable10 = Result.Nodes.getNodeAs<clang::DeclRefExpr>("eh0");
			    loc = Variable10->getSourceRange();
				locStart = srcMgr.getPresumedLoc(loc.getBegin());
				locEnd = srcMgr.getPresumedLoc(loc.getEnd());
				
				counterMatch[category_index][template_index]=counterMatch[category_index][template_index]+1;
				}
			}else if(template_index == 1){
				if(flagTemplateEnable[category_index][template_index]){
				const clang::DeclRefExpr* Variable11 = Result.Nodes.getNodeAs<clang::DeclRefExpr>("eh1");
			    loc = Variable11->getSourceRange();
				locStart = srcMgr.getPresumedLoc(loc.getBegin());
				locEnd = srcMgr.getPresumedLoc(loc.getEnd());
				
				counterMatch[category_index][template_index]=counterMatch[category_index][template_index]+1;
				}
			}else if(template_index == 2){
				if(flagTemplateEnable[category_index][template_index]){
				const clang::DeclRefExpr* Variable12 = Result.Nodes.getNodeAs<clang::DeclRefExpr>("eh2");
			    loc = Variable12->getSourceRange();
				locStart = srcMgr.getPresumedLoc(loc.getBegin());
				locEnd = srcMgr.getPresumedLoc(loc.getEnd());
				
				counterMatch[category_index][template_index]=counterMatch[category_index][template_index]+1;
				}
			}else if(template_index == 3){
				if(flagTemplateEnable[category_index][template_index]){
				const clang::DeclRefExpr* Variable13 = Result.Nodes.getNodeAs<clang::DeclRefExpr>("eh3");
			    loc = Variable13->getSourceRange();
				locStart = srcMgr.getPresumedLoc(loc.getBegin());
				locEnd = srcMgr.getPresumedLoc(loc.getEnd());
				
				counterMatch[category_index][template_index]=counterMatch[category_index][template_index]+1;
				}
			}else if(template_index == 4){
				if(flagTemplateEnable[category_index][template_index]){
					const clang::BinaryOperator* Variable14 = Result.Nodes.getNodeAs<clang::BinaryOperator>("eh4");
					clang::LangOptions lopt;
					
					Expr *RHSexpr = Variable14->getRHS();

					SourceLocation startLoc = RHSexpr->getLocStart();
					SourceLocation _endLoc = RHSexpr->getLocEnd();				
					SourceLocation endLoc = clang::Lexer::getLocForEndOfToken(_endLoc, 0, srcMgr, lopt);

					std::string RHS_string = std::string(srcMgr.getCharacterData(startLoc), srcMgr.getCharacterData(endLoc) - srcMgr.getCharacterData(startLoc));

					bool checkZero=false;
					if(is_digits(RHS_string)){
						if (std::stoi (RHS_string)!=0){
							checkZero=false;
						}
						else{
							checkZero = true;
						}
					}else{
						checkZero = true;
					}

					if(checkZero){
						counterMatch[category_index][template_index]=counterMatch[category_index][template_index]+1; 
					}
				}
			}else if(template_index == 5){
				if(flagTemplateEnable[category_index][template_index]){
				const clang::IfStmt* Variable15 = Result.Nodes.getNodeAs<clang::IfStmt>("eh5");
				
			    loc = Variable15->getSourceRange();
				locStart = srcMgr.getPresumedLoc(loc.getBegin());
				locEnd = srcMgr.getPresumedLoc(loc.getEnd());
				
				counterMatch[category_index][template_index]=counterMatch[category_index][template_index]+1;
				}
			}else if(template_index == 6){
				if(flagTemplateEnable[category_index][template_index]){
				const clang::BinaryOperator* Variable16 = Result.Nodes.getNodeAs<clang::BinaryOperator>("eh6");
				
			    loc = Variable16->getSourceRange();
				locStart = srcMgr.getPresumedLoc(loc.getBegin());
				locEnd = srcMgr.getPresumedLoc(loc.getEnd());
				
				counterMatch[category_index][template_index]=counterMatch[category_index][template_index]+1;
				}
			}
		}else if(category_index == 2){ //Resource
			
		}else if(category_index == 3){ //Time
			if(template_index == 0){
				if(flagTemplateEnable[category_index][template_index]){
					const clang::MemberExpr* Variable30 = Result.Nodes.getNodeAs<clang::MemberExpr>("ts0");
					loc = Variable30->getSourceRange();
					locStart = srcMgr.getPresumedLoc(loc.getBegin());
					locEnd = srcMgr.getPresumedLoc(loc.getEnd());

					counterMatch[category_index][template_index]=counterMatch[category_index][template_index]+1;
				}
			}else if(template_index == 1){
				if(flagTemplateEnable[category_index][template_index]){
					const clang::IfStmt* Variable31 = Result.Nodes.getNodeAs<clang::IfStmt>("ts1");
					loc = Variable31->getSourceRange();
					locStart = srcMgr.getPresumedLoc(loc.getBegin());
					locEnd = srcMgr.getPresumedLoc(loc.getEnd());

					counterMatch[category_index][template_index]=counterMatch[category_index][template_index]+1;
				}
			}else if(template_index == 2){
				if(flagTemplateEnable[category_index][template_index]){
					const clang::MemberExpr* Variable32 = Result.Nodes.getNodeAs<clang::MemberExpr>("ts2");
					loc = Variable32->getSourceRange();
					locStart = srcMgr.getPresumedLoc(loc.getBegin());
					locEnd = srcMgr.getPresumedLoc(loc.getEnd());

					counterMatch[category_index][template_index]=counterMatch[category_index][template_index]+1;
				}
			}else if(template_index == 3){
				if(flagTemplateEnable[category_index][template_index]){
					const clang::IfStmt* Variable33 = Result.Nodes.getNodeAs<clang::IfStmt>("ts3");
					loc = Variable33->getSourceRange();
					locStart = srcMgr.getPresumedLoc(loc.getBegin());
					locEnd = srcMgr.getPresumedLoc(loc.getEnd());

					counterMatch[category_index][template_index]=counterMatch[category_index][template_index]+1;
				}
			}
				
			//<<Insert match reader code here - do not delete this comment!!>>
			
		}
		
    }else{
		
		mut_arr = mutant_random_generator(); //generate random numbers based on user seed for representative sampling from mutation population

		for ( int i = 0; i < no_of_mutants; i++ ) {
			final_mutant_numbers[i]=*(mut_arr + i);
		}

		numElements = sizeof (final_mutant_numbers) / sizeof (final_mutant_numbers[0]);
		end = final_mutant_numbers + numElements;
		found = std::find (final_mutant_numbers, end, mutantNumber);
		
		if(category_index == 0){ //Precision
			if(template_index == 0){
				if(flagTemplateEnable[category_index][template_index] ){
					if (found != end){
						const clang::VarDecl* Variable0 = Result.Nodes.getNodeAs<clang::VarDecl>("pa0");
						clang::SourceRange loc = Variable0->getSourceRange();
						locStart = srcMgr.getPresumedLoc(loc.getBegin());
						/// The `Rewriter` used to insert the `override` keyword.
						clang::Rewriter rewriter;
						rewriter.setSourceMgr(Result.Context->getSourceManager(),Result.Context->getLangOpts());
						if(targetfile_platform=="C" || targetfile_platform=="CPP"){
							rewriter.ReplaceText(loc.getBegin(),3,"float"); 
							platform_comments = "int to float";
						}
						if(targetfile_platform=="MATLAB"){
							rewriter.ReplaceText(loc.getBegin(),5,"uint32_T"); 
							platform_comments = "int_T to uint32_T";
						}

						// Now emit the rewritten buffer.
						std::error_code error_code;
						write_filename="./mutants/P"+to_string(template_index)+input_filename+to_string(mutantNumber)+input_filename_extension;
						llvm::raw_fd_ostream outFile(write_filename, error_code, llvm::sys::fs::F_None);
						rewriter.getEditBuffer(rewriter.getSourceMgr().getMainFileID()).write(outFile); 
						outFile.close();
						
						mutation_comments = "\n/* Precision and accuracy category mutation - template P0: changed "+platform_comments+". \n* Before mutation => "+ 
														readFile(locStart.getLine()-1,tmp_filepath) +
														"\n* Mutation begins */\n"+
														readFile( locStart.getLine()-1, write_filename)+
														"\n/* Mutation ends*/\n";
						//writeToFile( write_filename.c_str(), locStart.getLine()-1,  " ");
						appendToFile(write_filename, locStart.getLine()-1, mutation_comments,true);
						
						generated_mutants_count[category_index][template_index]++;
						generated_mutants_locations_count[category_index][template_index]=generated_mutants_locations_count[category_index][template_index]+(to_string)(locStart.getLine()-1)+",";
					}
					mutantNumber++;
				}
			}else if(template_index == 1){
				if(flagTemplateEnable[category_index][template_index]){
					if (found != end){
						const clang::VarDecl* Variable1 = Result.Nodes.getNodeAs<clang::VarDecl>("pa1");
						clang::SourceRange loc = Variable1->getSourceRange();
						locStart = srcMgr.getPresumedLoc(loc.getBegin());
						
						/// The `Rewriter` used to insert the `override` keyword.
						clang::Rewriter rewriter;
						rewriter.setSourceMgr(Result.Context->getSourceManager(),Result.Context->getLangOpts());
						if(targetfile_platform=="C" || targetfile_platform=="CPP"){
							rewriter.ReplaceText(loc.getBegin(),3,"double"); 
							platform_comments = "int to double";
						}
						if(targetfile_platform=="MATLAB"){
							rewriter.ReplaceText(loc.getBegin(),5,"real_T"); 
							platform_comments = "int to real_T";
						}

						// Now emit the rewritten buffer.
						std::error_code error_code;
						write_filename="./mutants/P"+to_string(template_index)+input_filename+to_string(mutantNumber)+input_filename_extension;
						llvm::raw_fd_ostream outFile(write_filename, error_code, llvm::sys::fs::F_None);
						rewriter.getEditBuffer(rewriter.getSourceMgr().getMainFileID()).write(outFile); 
						outFile.close();
						
						mutation_comments = "\n/* Precision and accuracy category mutation - template P1: changed "+platform_comments+". \n* Before mutation => "+ 
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
			}else if(template_index == 2){
				if(flagTemplateEnable[category_index][template_index]){
					if (found != end){
						const clang::VarDecl* Variable2 = Result.Nodes.getNodeAs<clang::VarDecl>("pa2");
						clang::SourceRange loc = Variable2->getSourceRange();
						locStart = srcMgr.getPresumedLoc(loc.getBegin());
						
						/// The `Rewriter` used to insert the `override` keyword.
						clang::Rewriter rewriter;
						rewriter.setSourceMgr(Result.Context->getSourceManager(),Result.Context->getLangOpts());
						if(targetfile_platform=="C" || targetfile_platform=="CPP"){
							rewriter.ReplaceText(loc.getBegin(),5,"int"); 
							platform_comments = "float to int";
						}
						if(targetfile_platform=="MATLAB"){
							rewriter.ReplaceText(loc.getBegin(),8,"int_T");
							platform_comments = "uint32_T to  int_T";
						}

						// Now emit the rewritten buffer.
						std::error_code error_code;
						write_filename="./mutants/P"+to_string(template_index)+input_filename+to_string(mutantNumber)+input_filename_extension;
						llvm::raw_fd_ostream outFile(write_filename, error_code, llvm::sys::fs::F_None);
						rewriter.getEditBuffer(rewriter.getSourceMgr().getMainFileID()).write(outFile); 
						outFile.close();
						
						mutation_comments = "\n/* Precision and accuracy category mutation - template P2: changed "+platform_comments+". \n* Before mutation => "+ 
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
			}else if(template_index == 3){
				if(flagTemplateEnable[category_index][template_index]){
					if (found != end){
						const clang::VarDecl* Variable3 = Result.Nodes.getNodeAs<clang::VarDecl>("pa3");
						clang::SourceRange loc = Variable3->getSourceRange();
						locStart = srcMgr.getPresumedLoc(loc.getBegin());

						/// The `Rewriter` used to insert the `override` keyword.
						clang::Rewriter rewriter;
						rewriter.setSourceMgr(Result.Context->getSourceManager(),Result.Context->getLangOpts());
						if(targetfile_platform=="C" || targetfile_platform=="CPP"){
							rewriter.ReplaceText(loc.getBegin(),6,"int"); 
							platform_comments = "double to int";
						}
						if(targetfile_platform=="MATLAB"){
							rewriter.ReplaceText(loc.getBegin(),6,"int_T");
							platform_comments = "real_T to  int_T";
						}

						// Now emit the rewritten buffer.
						std::error_code error_code;
						write_filename="./mutants/P"+to_string(template_index)+input_filename+to_string(mutantNumber)+input_filename_extension;
						llvm::raw_fd_ostream outFile(write_filename, error_code, llvm::sys::fs::F_None);
						rewriter.getEditBuffer(rewriter.getSourceMgr().getMainFileID()).write(outFile); 
						outFile.close();
						
						mutation_comments = "\n/* Precision and accuracy category mutation - template P3: changed "+platform_comments+". \n* Before mutation => "+ 
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
			}else if(template_index == 4){
				if(flagTemplateEnable[category_index][template_index]){
					if (found != end){
						const clang::IntegerLiteral* Variable4 = Result.Nodes.getNodeAs<clang::IntegerLiteral>("pa4");
						clang::SourceRange loc = Variable4->getSourceRange();
						locStart = srcMgr.getPresumedLoc(loc.getBegin());

						/// The `Rewriter` used to insert the `override` keyword.
						clang::Rewriter rewriter;
						rewriter.setSourceMgr(Result.Context->getSourceManager(),Result.Context->getLangOpts());
						rewriter.InsertText(loc.getBegin(),"(float)",true,true); 

						// Now emit the rewritten buffer.
						std::error_code error_code;
						write_filename="./mutants/P"+to_string(template_index)+input_filename+to_string(mutantNumber)+input_filename_extension;
						llvm::raw_fd_ostream outFile(write_filename, error_code, llvm::sys::fs::F_None);
						rewriter.getEditBuffer(rewriter.getSourceMgr().getMainFileID()).write(outFile); 
						outFile.close();
						
						mutation_comments = "\n/* Precision and accuracy category mutation - template P4: changed d* -> d*.0f \n* Before mutation => "+ 
														readFile(locStart.getLine()-1,tmp_filepath) +
														"\n/* Mutation begins */\n"+
														readFile( locStart.getLine()-1, write_filename)+
														"\n/* Mutation ends */\n";
						appendToFile(write_filename, locStart.getLine()-1, mutation_comments,true);
						
						generated_mutants_count[category_index][template_index]++;
						generated_mutants_locations_count[category_index][template_index]=generated_mutants_locations_count[category_index][template_index]+(to_string)(locStart.getLine()-1)+",";
					}
					mutantNumber++;
				}
			}else if(template_index == 5){
				if(flagTemplateEnable[category_index][template_index]){
					if (found != end){
						const clang::IntegerLiteral* Variable5 = Result.Nodes.getNodeAs<clang::IntegerLiteral>("pa5");
						clang::SourceRange loc = Variable5->getSourceRange();
						locStart = srcMgr.getPresumedLoc(loc.getBegin());

						/// The `Rewriter` used to insert the `override` keyword.
						clang::Rewriter rewriter;
						rewriter.setSourceMgr(Result.Context->getSourceManager(),Result.Context->getLangOpts());
						rewriter.InsertText(loc.getBegin(),"(double)",true,true); 

						// Now emit the rewritten buffer.
						std::error_code error_code;
						write_filename="./mutants/P"+to_string(template_index)+input_filename+to_string(mutantNumber)+input_filename_extension;
						llvm::raw_fd_ostream outFile(write_filename, error_code, llvm::sys::fs::F_None);
						rewriter.getEditBuffer(rewriter.getSourceMgr().getMainFileID()).write(outFile); 
						outFile.close();
						
						mutation_comments = "\n/* Precision and accuracy category mutation - template P5: changed d* -> d*.0 \n* Before mutation => "+ 
														readFile(locStart.getLine()-1,tmp_filepath) +
														"\n/* Mutation begins */\n"+
														readFile( locStart.getLine()-1, write_filename)+
														"\n/* Mutation ends */\n";
						appendToFile(write_filename, locStart.getLine()-1, mutation_comments,true);
						
						generated_mutants_count[category_index][template_index]++;
						generated_mutants_locations_count[category_index][template_index]=generated_mutants_locations_count[category_index][template_index]+(to_string)(locStart.getLine()-1)+",";
					}
					mutantNumber++;	
				}
			}else if(template_index == 6){
				if(flagTemplateEnable[category_index][template_index]){
					if (found != end){
						const clang::FloatingLiteral* Variable6 = Result.Nodes.getNodeAs<clang::FloatingLiteral>("pa6");
						clang::SourceRange loc = Variable6->getSourceRange();
						locStart = srcMgr.getPresumedLoc(loc.getBegin());

						locStart = srcMgr.getPresumedLoc(loc.getBegin());
						locEnd = srcMgr.getPresumedLoc(loc.getEnd());
						double x = Variable6->getValueAsApproximateDouble();
						int y = (int) x;
						
						/// The `Rewriter` used to insert the `override` keyword.
						clang::Rewriter rewriter;
						rewriter.setSourceMgr(Result.Context->getSourceManager(),Result.Context->getLangOpts());
						rewriter.ReplaceText(loc,std::to_string(y)); 

						// Now emit the rewritten buffer.
						std::error_code error_code;
						write_filename="./mutants/P"+to_string(template_index)+input_filename+to_string(mutantNumber)+input_filename_extension;
						llvm::raw_fd_ostream outFile(write_filename, error_code, llvm::sys::fs::F_None);
						rewriter.getEditBuffer(rewriter.getSourceMgr().getMainFileID()).write(outFile); 
						outFile.close();
						
						mutation_comments = "\n/* Precision and accuracy category mutation - template P6: changed d*.d/ d*.df -> d* \n* Before mutation => "+ 
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
			}else if(template_index == 7){
				if(flagTemplateEnable[category_index][template_index]){
					if (found != end){
						const clang::CallExpr* Variable7 = Result.Nodes.getNodeAs<clang::CallExpr>("pa7");
						clang::SourceRange loc = Variable7->getSourceRange();
						locStart = srcMgr.getPresumedLoc(loc.getBegin());

						/// The `Rewriter` used to insert the `override` keyword.
						clang::Rewriter rewriter;
						rewriter.setSourceMgr(Result.Context->getSourceManager(),Result.Context->getLangOpts());
						rewriter.InsertText(loc.getBegin(),"(double)",true,true); 

						// Now emit the rewritten buffer.
						std::error_code error_code;
						write_filename="./mutants/P"+to_string(template_index)+input_filename+to_string(mutantNumber)+input_filename_extension;
						llvm::raw_fd_ostream outFile(write_filename, error_code, llvm::sys::fs::F_None);
						rewriter.getEditBuffer(rewriter.getSourceMgr().getMainFileID()).write(outFile); 
						outFile.close();
						
						mutation_comments = "\n/* Precision and accuracy category mutation - template P7: changed float F() -> (double) F() \n* Before mutation => "+ 
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
			}else if(template_index == 8){
				if(flagTemplateEnable[category_index][template_index]){
					if (found != end){
						const clang::CallExpr* Variable8 = Result.Nodes.getNodeAs<clang::CallExpr>("pa8");
						clang::SourceRange loc = Variable8->getSourceRange();
						locStart = srcMgr.getPresumedLoc(loc.getBegin());

						/// The `Rewriter` used to insert the `override` keyword.
						clang::Rewriter rewriter;
						rewriter.setSourceMgr(Result.Context->getSourceManager(),Result.Context->getLangOpts());
						rewriter.InsertText(loc.getBegin(),"(float)",true,true); 

						// Now emit the rewritten buffer.
						std::error_code error_code;
						write_filename="./mutants/P"+to_string(template_index)+input_filename+to_string(mutantNumber)+input_filename_extension;
						llvm::raw_fd_ostream outFile(write_filename, error_code, llvm::sys::fs::F_None);
						rewriter.getEditBuffer(rewriter.getSourceMgr().getMainFileID()).write(outFile); 
						outFile.close();
						
						mutation_comments = "\n/* Precision and accuracy category mutation - template P8: changed double F() -> (float) F() \n* Before mutation => "+ 
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
			}else if(template_index == 9){
				if(flagTemplateEnable[category_index][template_index]){
					if (found != end){
						const clang::IntegerLiteral* Variable9 = Result.Nodes.getNodeAs<clang::IntegerLiteral>("pa9");
						clang::SourceRange loc = Variable9->getSourceRange();
						locStart = srcMgr.getPresumedLoc(loc.getBegin());

						/// The `Rewriter` used to insert the `override` keyword.
						clang::Rewriter rewriter;
						rewriter.setSourceMgr(Result.Context->getSourceManager(),Result.Context->getLangOpts());
						rewriter.InsertText(loc.getBegin(),"(float)",true,true); 

						// Now emit the rewritten buffer.
						std::error_code error_code;
						write_filename="./mutants/P"+to_string(template_index)+input_filename+to_string(mutantNumber)+input_filename_extension;
						llvm::raw_fd_ostream outFile(write_filename, error_code, llvm::sys::fs::F_None);
						rewriter.getEditBuffer(rewriter.getSourceMgr().getMainFileID()).write(outFile); 
						outFile.close();
						
						mutation_comments = "\n/* Precision and accuracy category mutation - template P9: changed float F() -> (double) F() \n* Before mutation => "+ 
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
		}else if(category_index == 1){ //Exception handling
			if(template_index == 0){
				if(flagTemplateEnable[category_index][template_index]){
					if (found != end){
						const clang::DeclRefExpr* Variable10 = Result.Nodes.getNodeAs<clang::DeclRefExpr>("eh0");
						clang::SourceRange loc = Variable10->getSourceRange();
						locStart = srcMgr.getPresumedLoc(loc.getBegin());

						/// The `Rewriter` used to insert the `override` keyword.
						clang::Rewriter rewriter;
						rewriter.setSourceMgr(Result.Context->getSourceManager(),Result.Context->getLangOpts());
						rewriter.InsertText(loc.getBegin(),"!",true,true); 

						// Now emit the rewritten buffer.
						std::error_code error_code;
						write_filename="./mutants/E"+to_string(template_index)+input_filename+to_string(mutantNumber)+input_filename_extension;
						llvm::raw_fd_ostream outFile(write_filename, error_code, llvm::sys::fs::F_None);
						rewriter.getEditBuffer(rewriter.getSourceMgr().getMainFileID()).write(outFile); 
						outFile.close();
						
						if(targetfile_platform=="C" || targetfile_platform=="CPP") 
							platform_comments = "if(isnan(X)) -> if(!isnan(X))";
						if(targetfile_platform=="MATLAB")
							platform_comments = "if(rtIsNaN(X)) -> if(!rtIsNaN(X))";
						
						mutation_comments = "\n/* Precision and accuracy category mutation - template E0: changed "+platform_comments+".\n* Before mutation => "+ 
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
			}else if(template_index == 1){
				if(flagTemplateEnable[category_index][template_index]){
					if (found != end){
						const clang::DeclRefExpr* Variable11 = Result.Nodes.getNodeAs<clang::DeclRefExpr>("eh1");
						clang::SourceRange loc = Variable11->getSourceRange();
						locStart = srcMgr.getPresumedLoc(loc.getBegin());

						/// The `Rewriter` used to insert the `override` keyword.
						clang::Rewriter rewriter;
						rewriter.setSourceMgr(Result.Context->getSourceManager(),Result.Context->getLangOpts());
						rewriter.InsertText(loc.getBegin(),"!",true,true); 

						// Now emit the rewritten buffer.
						std::error_code error_code;
						write_filename="./mutants/E"+to_string(template_index)+input_filename+to_string(mutantNumber)+input_filename_extension;
						llvm::raw_fd_ostream outFile(write_filename, error_code, llvm::sys::fs::F_None);
						rewriter.getEditBuffer(rewriter.getSourceMgr().getMainFileID()).write(outFile); 
						outFile.close();
						
						if(targetfile_platform=="C" || targetfile_platform=="CPP") 
							platform_comments = "if(isinf(X)) -> if(!isinf(X))";
						if(targetfile_platform=="MATLAB")
							platform_comments = "if(rtIsInf(X)) -> if(!rtIsInf(X))";
						
						mutation_comments = "\n/* Exception handling category mutation - template E1: changed "+platform_comments+".\n* Before mutation => "+ 
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
			}else if(template_index == 2){
				if(flagTemplateEnable[category_index][template_index]){
					if (found != end){
						const clang::DeclRefExpr* Variable12 = Result.Nodes.getNodeAs<clang::DeclRefExpr>("eh2");
						clang::SourceRange loc = Variable12->getSourceRange();
						locStart = srcMgr.getPresumedLoc(loc.getBegin());

						/// The `Rewriter` used to insert the `override` keyword.
						clang::Rewriter rewriter;
						rewriter.setSourceMgr(Result.Context->getSourceManager(),Result.Context->getLangOpts());
						rewriter.ReplaceText(loc.getBegin(),1,""); 

						// Now emit the rewritten buffer.
						std::error_code error_code;
						write_filename="./mutants/E"+to_string(template_index)+input_filename+to_string(mutantNumber)+input_filename_extension;
						llvm::raw_fd_ostream outFile(write_filename, error_code, llvm::sys::fs::F_None);
						rewriter.getEditBuffer(rewriter.getSourceMgr().getMainFileID()).write(outFile); 
						outFile.close();
						
						if(targetfile_platform=="C" || targetfile_platform=="CPP") 
							platform_comments = "if(!isnan(X)) -> if(isnan(X))";
						if(targetfile_platform=="MATLAB")
							platform_comments = "if(!rtIsNaN((X)) -> if(rtIsNaN((X))";
						
						mutation_comments = "\n/* Exception handling category mutation - template E2: changed "+platform_comments+".\n* Before mutation => "+ 
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
			}else if(template_index == 3){
				if(flagTemplateEnable[category_index][template_index]){
					if (found != end){
						const clang::DeclRefExpr* Variable13 = Result.Nodes.getNodeAs<clang::DeclRefExpr>("eh3");
						clang::SourceRange loc = Variable13->getSourceRange();
						locStart = srcMgr.getPresumedLoc(loc.getBegin());

						/// The `Rewriter` used to insert the `override` keyword.
						clang::Rewriter rewriter;
						rewriter.setSourceMgr(Result.Context->getSourceManager(),Result.Context->getLangOpts());
						rewriter.ReplaceText(loc.getBegin(),1,"");  

						// Now emit the rewritten buffer.
						std::error_code error_code;
						write_filename="./mutants/E"+to_string(template_index)+input_filename+to_string(mutantNumber)+input_filename_extension;
						llvm::raw_fd_ostream outFile(write_filename, error_code, llvm::sys::fs::F_None);
						rewriter.getEditBuffer(rewriter.getSourceMgr().getMainFileID()).write(outFile); 
						outFile.close();
						
						if(targetfile_platform=="C" || targetfile_platform=="CPP") 
							platform_comments = "if(!isinf(X)) -> if(isinf(X))";
						if(targetfile_platform=="MATLAB")
							platform_comments = "if(!rtIsInf((X)) -> if(rtIsInf((X))";
						
						mutation_comments = "\n/* Exception handling category mutation - template E3: changed "+platform_comments+".\n* Before mutation => "+ 
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
			}else if(template_index == 4){
				if(flagTemplateEnable[category_index][template_index]){
					if (found != end){
						const clang::BinaryOperator* Variable14 = Result.Nodes.getNodeAs<clang::BinaryOperator>("eh4");
						clang::LangOptions lopt;
						clang::SourceRange loc = Variable14->getSourceRange();
						locStart = srcMgr.getPresumedLoc(loc.getBegin());

						Expr *RHSexpr = Variable14->getRHS();
						Expr *LHSexpr = Variable14->getLHS();

						SourceLocation startLoc = RHSexpr->getLocStart();
						SourceLocation _endLoc = RHSexpr->getLocEnd();				
						SourceLocation endLoc = clang::Lexer::getLocForEndOfToken(_endLoc, 0, srcMgr, lopt);

						std::string RHS_string = std::string(srcMgr.getCharacterData(startLoc), srcMgr.getCharacterData(endLoc) - srcMgr.getCharacterData(startLoc));

						startLoc = LHSexpr->getLocStart();
						_endLoc = LHSexpr->getLocEnd();				
						endLoc = clang::Lexer::getLocForEndOfToken(_endLoc, 0, srcMgr, lopt);

						std::string LHS_string = std::string(srcMgr.getCharacterData(startLoc), srcMgr.getCharacterData(endLoc) - srcMgr.getCharacterData(startLoc));

						bool checkZero=false;
						if(is_digits(RHS_string)){
							if (std::stoi (RHS_string)!=0){
								checkZero=false;
							}
							else{
								checkZero = true;
							}
						}else{
							checkZero = true;
						}

						if(checkZero){
							string bounding_if_statement = "\nif( "+ RHS_string +"!=0 ) ";

							// The `Rewriter` used to insert the `override` keyword.
							clang::Rewriter rewriter;
							rewriter.setSourceMgr(Result.Context->getSourceManager(),Result.Context->getLangOpts());
							loc.setBegin( loc.getBegin().getLocWithOffset(- srcMgr.getPresumedLoc(loc.getBegin()).getColumn() ) );
							rewriter.InsertText(loc.getBegin(),bounding_if_statement,true,true);

							// Now emit the rewritten buffer.
							std::error_code error_code;
							write_filename="./mutants/E"+to_string(template_index)+input_filename+to_string(mutantNumber)+input_filename_extension;
							llvm::raw_fd_ostream outFile(write_filename, error_code, llvm::sys::fs::F_None);
							rewriter.getEditBuffer(rewriter.getSourceMgr().getMainFileID()).write(outFile); 
							outFile.close();
						
							mutation_comments = "\n/* Exception handling category mutation - template E4: insert if statment - check divide by 0. \n* Before mutation => there was no if statement \n* Mutation begins */\n"+
															readFile( locStart.getLine()-1, write_filename)+
															"\n/* Mutation ends */\n";
							appendToFile(write_filename, locStart.getLine()-1, mutation_comments,true);

							generated_mutants_count[category_index][template_index]++;
							generated_mutants_locations_count[category_index][template_index]=generated_mutants_locations_count[category_index][template_index]+(to_string)(locStart.getLine()-1)+",";
							mutantNumber++; //this case is an exceptional case
						}
					}
				}
			}else if(template_index == 5){
				if(flagTemplateEnable[category_index][template_index]){
					if (found != end){
						const clang::IfStmt* Variable15 = Result.Nodes.getNodeAs<clang::IfStmt>("eh5");
						clang::SourceRange loc = Variable15->getSourceRange();
						locStart = srcMgr.getPresumedLoc(loc.getBegin());

						const Expr *ifCond = Variable15->getCond();
						clang::SourceRange locCond = ifCond->getSourceRange();

						/// The `Rewriter` used to insert the `override` keyword.
						clang::Rewriter rewriter;
						rewriter.setSourceMgr(Result.Context->getSourceManager(),Result.Context->getLangOpts());
						rewriter.ReplaceText(locCond,"true");  

						// Now emit the rewritten buffer.
						std::error_code error_code;
						write_filename="./mutants/E"+to_string(template_index)+input_filename+to_string(mutantNumber)+input_filename_extension;
						llvm::raw_fd_ostream outFile(write_filename, error_code, llvm::sys::fs::F_None);
						rewriter.getEditBuffer(rewriter.getSourceMgr().getMainFileID()).write(outFile); 
						outFile.close();
						
						mutation_comments = "\n/* Exception handling category mutation - template E5: remove if statment - check divide by 0. \n* Before mutation => "+ 
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
			}else if(template_index == 6){
				if(flagTemplateEnable[category_index][template_index]){
					if (found != end){
						const clang::BinaryOperator* Variable16 = Result.Nodes.getNodeAs<clang::BinaryOperator>("eh6");
						clang::LangOptions lopt;
						clang::SourceRange loc = Variable16->getSourceRange();
						locStart = srcMgr.getPresumedLoc(loc.getBegin());

						Expr *RHSexpr = Variable16->getRHS();

						SourceLocation startLoc = RHSexpr->getLocStart();
						SourceLocation _endLoc = RHSexpr->getLocEnd();				
						SourceLocation endLoc = clang::Lexer::getLocForEndOfToken(_endLoc, 0, srcMgr, lopt);

						std::string RHS_string = std::string(srcMgr.getCharacterData(startLoc), srcMgr.getCharacterData(endLoc) - srcMgr.getCharacterData(startLoc));

						//std::cout << "RHS_string :" << RHS_string << std::endl;

						// The `Rewriter` used to insert the `override` keyword.
						clang::Rewriter rewriter;
						rewriter.setSourceMgr(Result.Context->getSourceManager(),Result.Context->getLangOpts());
						rewriter.InsertText(endLoc,"* 0",true,true);

						// Now emit the rewritten buffer.
						std::error_code error_code;
						write_filename="./mutants/E"+to_string(template_index)+input_filename+to_string(mutantNumber)+input_filename_extension;
						llvm::raw_fd_ostream outFile(write_filename, error_code, llvm::sys::fs::F_None);
						rewriter.getEditBuffer(rewriter.getSourceMgr().getMainFileID()).write(outFile); 
						outFile.close();
						
						mutation_comments = "\n/* Exception handling category mutation - template E6: miultiply denominator by zero. \n* Before mutation => "+ 
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
		}else if(category_index == 2){ //Resource limitation	
		}else if(category_index == 3){ //Time and Space
			if(template_index == 0){
				if(flagTemplateEnable[category_index][template_index]){
					if (found != end){
						const clang::MemberExpr* Variable30 = Result.Nodes.getNodeAs<clang::MemberExpr>("ts0");
						clang::SourceRange loc = Variable30->getSourceRange();
						locStart = srcMgr.getPresumedLoc(loc.getBegin());

						clang::LangOptions lopt;

						locStart = srcMgr.getPresumedLoc(loc.getBegin());
						locEnd = srcMgr.getPresumedLoc(loc.getEnd());

						SourceLocation endLoc = clang::Lexer::getLocForEndOfToken(loc.getEnd(), 0, srcMgr, lopt);

						/// The `Rewriter` used to insert the `override` keyword.
						clang::Rewriter rewriter;
						rewriter.setSourceMgr(Result.Context->getSourceManager(),Result.Context->getLangOpts());
						rewriter.InsertText(endLoc,"* 1000",true,true);

						// Now emit the rewritten buffer.
						std::error_code error_code;
						write_filename="./mutants/T"+to_string(template_index)+input_filename+to_string(mutantNumber)+input_filename_extension;
						llvm::raw_fd_ostream outFile(write_filename, error_code, llvm::sys::fs::F_None);
						rewriter.getEditBuffer(rewriter.getSourceMgr().getMainFileID()).write(outFile); 
						outFile.close();
						
						mutation_comments = "\n/* Time and Space category mutation - template T0: dataype of time is multiplied by 1000.  \n* Before mutation => "+ 
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
			} else if(template_index == 1){
				if(flagTemplateEnable[category_index][template_index]){
					if (found != end){
						const clang::IfStmt* Variable31 = Result.Nodes.getNodeAs<clang::IfStmt>("ts1");
						clang::SourceRange loc = Variable31->getSourceRange();
						locStart = srcMgr.getPresumedLoc(loc.getBegin());

						clang::LangOptions lopt;

						loc = Variable31->getSourceRange();
						locStart = srcMgr.getPresumedLoc(loc.getBegin());
						locEnd = srcMgr.getPresumedLoc(loc.getEnd());

						const Expr *ifCond = Variable31->getCond();
						clang::SourceRange locCond = ifCond->getSourceRange();

						/// The `Rewriter` used to insert the `override` keyword.
						clang::Rewriter rewriter;
						rewriter.setSourceMgr(Result.Context->getSourceManager(),Result.Context->getLangOpts());
						rewriter.InsertText(locCond.getBegin(),"!",true,true); 

						// Now emit the rewritten buffer.
						std::error_code error_code;
						write_filename="./mutants/T"+to_string(template_index)+input_filename+to_string(mutantNumber)+input_filename_extension;
						llvm::raw_fd_ostream outFile(write_filename, error_code, llvm::sys::fs::F_None);
						rewriter.getEditBuffer(rewriter.getSourceMgr().getMainFileID()).write(outFile); 
						outFile.close();
						
						mutation_comments = "\n/* Time and Space category mutation - template T1: dataype of time in ifstmt() is negated.  \n* Before mutation => "+ 
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
			}  else if(template_index == 2){
				if(flagTemplateEnable[category_index][template_index]){
					if (found != end){
						const clang::MemberExpr* Variable32 = Result.Nodes.getNodeAs<clang::MemberExpr>("ts2");
						clang::SourceRange loc = Variable32->getSourceRange();
						locStart = srcMgr.getPresumedLoc(loc.getBegin());

						clang::LangOptions lopt;

						loc = Variable32->getSourceRange();
						locStart = srcMgr.getPresumedLoc(loc.getBegin());
						locEnd = srcMgr.getPresumedLoc(loc.getEnd());

						SourceLocation endLoc = clang::Lexer::getLocForEndOfToken(loc.getEnd(), 0, srcMgr, lopt);
						
						/// The `Rewriter` used to insert the `override` keyword.
						clang::Rewriter rewriter;
						rewriter.setSourceMgr(Result.Context->getSourceManager(),Result.Context->getLangOpts());
						rewriter.InsertText(endLoc,"* 1000",true,true);

						// Now emit the rewritten buffer.
						std::error_code error_code;
						write_filename="./mutants/T"+to_string(template_index)+input_filename+to_string(mutantNumber)+input_filename_extension;
						llvm::raw_fd_ostream outFile(write_filename, error_code, llvm::sys::fs::F_None);
						rewriter.getEditBuffer(rewriter.getSourceMgr().getMainFileID()).write(outFile); 
						outFile.close();
						
						mutation_comments = "\n/* Time and Space category mutation - template T2: variable of time is multiplied by 1000.  \n* Before mutation => "+ 
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
			}  else if(template_index == 3){
				if(flagTemplateEnable[category_index][template_index]){
					if (found != end){
						const clang::IfStmt* Variable33 = Result.Nodes.getNodeAs<clang::IfStmt>("ts3");
						clang::SourceRange loc = Variable33->getSourceRange();
						locStart = srcMgr.getPresumedLoc(loc.getBegin());

						clang::LangOptions lopt;

						loc = Variable33->getSourceRange();
						locStart = srcMgr.getPresumedLoc(loc.getBegin());
						locEnd = srcMgr.getPresumedLoc(loc.getEnd());

						const Expr *ifCond = Variable33->getCond();
						clang::SourceRange locCond = ifCond->getSourceRange();

						/// The `Rewriter` used to insert the `override` keyword.
						clang::Rewriter rewriter;
						rewriter.setSourceMgr(Result.Context->getSourceManager(),Result.Context->getLangOpts());
						rewriter.InsertText(locCond.getBegin(),"!",true,true); 

						// Now emit the rewritten buffer.
						std::error_code error_code;
						write_filename="./mutants/T"+to_string(template_index)+input_filename+to_string(mutantNumber)+input_filename_extension;
						llvm::raw_fd_ostream outFile(write_filename, error_code, llvm::sys::fs::F_None);
						rewriter.getEditBuffer(rewriter.getSourceMgr().getMainFileID()).write(outFile); 
						outFile.close();
						
						mutation_comments = "\n/* Time and Space category mutation - template T3: variable of time in ifstmt() is negated.  \n* Before mutation => "+ 
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
			
			//<<Insert match writer code here - do not delete this comment!!>>
			
		}
    }
  }
};  // namespace Mutator

/// Dispatches the ASTMatcher.
class Consumer : public clang::ASTConsumer {
 public:
  /// Creates the matcher for MUTATION and dispatches it on the TU.
  void HandleTranslationUnit(clang::ASTContext& Context) override {
    using namespace clang::ast_matchers;  // NOLINT(build/namespaces)

	MatchHandler Handler;
    
	//llvm::sys::fs::remove("./mutants",true); //not working
	llvm::sys::fs::create_directory("./mutants",true);
	string templateTypeName="";
	if (user_categories.find("P") != std::string::npos) {
		category_index = 0;
		template_index = 0;
		if(flagTemplateEnable[category_index][template_index]){
			llvm::sys::fs::create_directory("./mutants/P"+to_string(template_index), true);
			
			if(targetfile_platform=="C" || targetfile_platform=="CPP")
				templateTypeName="int";
			if(targetfile_platform=="MATLAB")
				templateTypeName="int_T";
			
			const auto PA0 = declaratorDecl(
									isExpansionInMainFile(), 
									hasType(asString(templateTypeName)),
									unless(parmVarDecl())
								).bind("pa0");
			
			MatchFinder MatchFinder0;
			MatchFinder0.addMatcher(PA0, &Handler);
			MatchFinder0.matchAST(Context);
		}
		
		template_index = 1;
		if(flagTemplateEnable[category_index][template_index]){
			llvm::sys::fs::create_directory("./mutants/P"+to_string(template_index), true);
			
			if(targetfile_platform=="C" || targetfile_platform=="CPP")
				templateTypeName="int";
			if(targetfile_platform=="MATLAB")
				templateTypeName="int_T";
			
			const auto PA1 = declaratorDecl(
								isExpansionInMainFile(), 
								hasType(asString(templateTypeName)),
								unless(parmVarDecl())
							).bind("pa1");

			MatchFinder MatchFinder1;
			MatchFinder1.addMatcher(PA1, &Handler);
			MatchFinder1.matchAST(Context);
		}
		
		template_index = 2;
		if(flagTemplateEnable[category_index][template_index]){
			llvm::sys::fs::create_directory("./mutants/P"+to_string(template_index), true);
			
			if(targetfile_platform=="C" || targetfile_platform=="CPP")
				templateTypeName="int";
			if(targetfile_platform=="MATLAB")
				templateTypeName="uint32_T";
			
			const auto PA2 = declaratorDecl(
								isExpansionInMainFile(), 
								hasType(asString(templateTypeName)),
								unless(parmVarDecl())
							).bind("pa2");
			
			MatchFinder MatchFinder2;
			MatchFinder2.addMatcher(PA2, &Handler);
			MatchFinder2.matchAST(Context);
		}
		
		template_index = 3;
		if(flagTemplateEnable[category_index][template_index]){
			llvm::sys::fs::create_directory("./mutants/P"+to_string(template_index), true);
			
			if(targetfile_platform=="C" || targetfile_platform=="CPP")
				templateTypeName="int";
			if(targetfile_platform=="MATLAB")
				templateTypeName="real_T";
			
			const auto PA3 = declaratorDecl(
								isExpansionInMainFile(), 
								hasType(asString(templateTypeName)),
								unless(parmVarDecl())
							).bind("pa3");
			
			MatchFinder MatchFinder3;
			MatchFinder3.addMatcher(PA3, &Handler);
			MatchFinder3.matchAST(Context);
		}
		
		template_index = 4;
		if(flagTemplateEnable[category_index][template_index]){
			llvm::sys::fs::create_directory("./mutants/P"+to_string(template_index), true);
			
			const auto PA4 = integerLiteral(
									isExpansionInMainFile(), 
									unless(hasAncestor(arraySubscriptExpr())),
									unless(hasAncestor( varDecl(hasType(arrayType())) ))
								).bind("pa4");
			
			MatchFinder MatchFinder4;
			MatchFinder4.addMatcher(PA4, &Handler);
			MatchFinder4.matchAST(Context);
		}
		
		template_index = 5;
		if(flagTemplateEnable[category_index][template_index]){
			llvm::sys::fs::create_directory("./mutants/P"+to_string(template_index), true);
			const auto PA5 = integerLiteral(
									isExpansionInMainFile(), 
									unless(hasAncestor(arraySubscriptExpr())),
									unless(hasAncestor( varDecl(hasType(arrayType())) ))
								).bind("pa5");
			
			MatchFinder MatchFinder5;
			MatchFinder5.addMatcher(PA5, &Handler);
			MatchFinder5.matchAST(Context);
		}
		
		template_index = 6;
		if(flagTemplateEnable[category_index][template_index]){
			llvm::sys::fs::create_directory("./mutants/P"+to_string(template_index), true);
			const auto PA6 = floatLiteral(
									isExpansionInMainFile(), 
									unless(hasAncestor(arraySubscriptExpr()))
								).bind("pa6");
			
			MatchFinder MatchFinder6;
			MatchFinder6.addMatcher(PA6, &Handler);
			MatchFinder6.matchAST(Context);
		}
		
		template_index = 7;
		if(flagTemplateEnable[category_index][template_index]){
			llvm::sys::fs::create_directory("./mutants/P"+to_string(template_index), true);
			const auto PA7 = callExpr(isExpansionInMainFile(),
				hasType(asString("float"))
				).bind("pa7");
			
			MatchFinder MatchFinder7;
			MatchFinder7.addMatcher(PA7, &Handler);
			MatchFinder7.matchAST(Context);
		}
		
		template_index = 8;
		if(flagTemplateEnable[category_index][template_index]){
			llvm::sys::fs::create_directory("./mutants/P"+to_string(template_index), true);
			const auto PA8 = callExpr(isExpansionInMainFile(),
				hasType(asString("double"))
				).bind("pa8");
			MatchFinder MatchFinder8;
			MatchFinder8.addMatcher(PA8, &Handler);
			MatchFinder8.matchAST(Context);
		}
		
		template_index = 9; //Not used anymore
		if(flagTemplateEnable[category_index][template_index]){
			llvm::sys::fs::create_directory("./mutants/P"+to_string(template_index), true);
			const auto PA9 = integerLiteral(
									isExpansionInMainFile(), 
									unless(hasAncestor(arraySubscriptExpr()))
								).bind("pa9");
			
			MatchFinder MatchFinder9;
			MatchFinder9.addMatcher(PA9, &Handler);
			MatchFinder9.matchAST(Context);
		}
	}
	if (user_categories.find("E") != std::string::npos) {
		category_index = 1;
		template_index = 0;
		if(flagTemplateEnable[category_index][template_index]){
			llvm::sys::fs::create_directory("./mutants/E"+to_string(template_index), true);
			
			if(targetfile_platform=="C" || targetfile_platform=="CPP")
				templateTypeName="isnan";
			if(targetfile_platform=="MATLAB")
				templateTypeName="rtIsNaN";
			
			const auto EH0 = declRefExpr(to( 
									functionDecl( 
									hasName(templateTypeName))), //rtIsNaN(y1) || rtIsInf
									isExpansionInMainFile(), 
									hasAncestor(ifStmt()),
									unless(hasAncestor(unaryOperator(hasOperatorName("!"))))
								).bind("eh0");

			MatchFinder MatchFinder10;
			MatchFinder10.addMatcher(EH0, &Handler);
			MatchFinder10.matchAST(Context);
		}
		
		template_index = 1;
		if(flagTemplateEnable[category_index][template_index]){
			llvm::sys::fs::create_directory("./mutants/E"+to_string(template_index), true);
			
			if(targetfile_platform=="C" || targetfile_platform=="CPP")
				templateTypeName="isinf";
			if(targetfile_platform=="MATLAB")
				templateTypeName="rtIsInf";
			
			const auto EH1 = declRefExpr(to( 
									functionDecl( 
									hasName(templateTypeName))), //rtIsNaN(y1) || rtIsInf
									isExpansionInMainFile(), 
									hasAncestor(ifStmt()),
									unless(hasAncestor(unaryOperator(hasOperatorName("!"))))
								).bind("eh1");
			MatchFinder MatchFinder11;
			MatchFinder11.addMatcher(EH1, &Handler);
			MatchFinder11.matchAST(Context);
		}
		
		template_index = 2;
		if(flagTemplateEnable[category_index][template_index]){
			llvm::sys::fs::create_directory("./mutants/E"+to_string(template_index), true);
			
			if(targetfile_platform=="C" || targetfile_platform=="CPP")
				templateTypeName="isnan";
			if(targetfile_platform=="MATLAB")
				templateTypeName="rtIsNaN";
			
			const auto EH2 = declRefExpr(to( 
									functionDecl( 
									hasName(templateTypeName))), //rtIsNaN(y1) || rtIsInf
									isExpansionInMainFile(), 
									allOf(hasAncestor(ifStmt()),
									hasAncestor(unaryOperator(hasOperatorName("!"))))
								).bind("eh2");
			MatchFinder MatchFinder12;
			MatchFinder12.addMatcher(EH2, &Handler);
			MatchFinder12.matchAST(Context);
		}
		
		template_index = 3;
		if(flagTemplateEnable[category_index][template_index]){
			llvm::sys::fs::create_directory("./mutants/E"+to_string(template_index), true);
			
			if(targetfile_platform=="C" || targetfile_platform=="CPP")
				templateTypeName="isinf";
			if(targetfile_platform=="MATLAB")
				templateTypeName="rtIsInf";
			
			const auto EH3 = declRefExpr(to( 
									functionDecl( 
									hasName(templateTypeName))), //rtIsNaN(y1) || rtIsInf
									isExpansionInMainFile(), 
									allOf(hasAncestor(ifStmt()),
									hasAncestor(unaryOperator(hasOperatorName("!"))))
								).bind("eh3");
			MatchFinder MatchFinder13;
			MatchFinder13.addMatcher(EH3, &Handler);
			MatchFinder13.matchAST(Context);
		}
		
		template_index = 4;
		if(flagTemplateEnable[category_index][template_index]){
			llvm::sys::fs::create_directory("./mutants/E"+to_string(template_index), true);
			const auto EH4 = binaryOperator(
									isExpansionInMainFile(), 
									(hasOperatorName("/"))
								).bind("eh4");
			
			MatchFinder MatchFinder14;
			MatchFinder14.addMatcher(EH4, &Handler);
			MatchFinder14.matchAST(Context);
		}
		
		template_index = 5;
		if(flagTemplateEnable[category_index][template_index]){
			llvm::sys::fs::create_directory("./mutants/E"+to_string(template_index), true);
			const auto EH5 = ifStmt(
									isExpansionInMainFile(),
									hasCondition(binaryOperator
									(hasOperatorName("!="))), 
									hasDescendant(binaryOperator
										(hasOperatorName("/")))
							).bind("eh5");
			MatchFinder MatchFinder15;
			MatchFinder15.addMatcher(EH5, &Handler);
			MatchFinder15.matchAST(Context);
		}
		
		template_index = 6;
		if(flagTemplateEnable[category_index][template_index]){
			llvm::sys::fs::create_directory("./mutants/E"+to_string(template_index), true);
			const auto EH6 = binaryOperator(
									isExpansionInMainFile(), 
									hasOperatorName("/")
								).bind("eh6");
			MatchFinder MatchFinder16;
			MatchFinder16.addMatcher(EH6, &Handler);
			MatchFinder16.matchAST(Context);
		}
	}
	if (user_categories.find("R") != std::string::npos) {
	}
	if (user_categories.find("T") != std::string::npos) {
		category_index = 3;
		template_index = 0;
		if(flagTemplateEnable[category_index][template_index]){
			llvm::sys::fs::create_directory("./mutants/T"+to_string(template_index), true);
			
			if(targetfile_platform=="C" || targetfile_platform=="CPP")
				templateTypeName="time_t";
			if(targetfile_platform=="MATLAB")
				templateTypeName="time_T";
			
			const auto TS0 = memberExpr( 
										isExpansionInMainFile(), 
										hasType(asString(templateTypeName)),
										unless(hasAncestor(ifStmt()))
									).bind("ts0");
			MatchFinder MatchFinder30;
			MatchFinder30.addMatcher(TS0, &Handler);
			MatchFinder30.matchAST(Context);
		}
		
		template_index = 1;
		if(flagTemplateEnable[category_index][template_index]){
			llvm::sys::fs::create_directory("./mutants/T"+to_string(template_index), true);
			
			if(targetfile_platform=="C" || targetfile_platform=="CPP")
				templateTypeName="time_t";
			if(targetfile_platform=="MATLAB")
				templateTypeName="time_T";
			
			const auto TS1 = ifStmt(
									isExpansionInMainFile(), 
									hasDescendant(
										memberExpr(
											member(hasType(asString(templateTypeName)))
											)
										)
									).bind("ts1");
			
			MatchFinder MatchFinder31;
			MatchFinder31.addMatcher(TS1, &Handler);
			MatchFinder31.matchAST(Context);
		}
		
		template_index = 2;
		if(flagTemplateEnable[category_index][template_index]){
			llvm::sys::fs::create_directory("./mutants/T"+to_string(template_index), true);
			const auto TS2 = memberExpr(
										isExpansionInMainFile(), 
										member(anyOf(hasName("clockTick0"),hasName("simTimeStep"),hasName("tArray"),hasName("rtDW"),hasName("rtM"),hasName("rtM_"),hasName("rtU"),hasName("rtY"))),
										unless(hasAncestor(ifStmt()))
									).bind("ts2");
			MatchFinder MatchFinder32;
			MatchFinder32.addMatcher(TS2, &Handler);
			MatchFinder32.matchAST(Context);
		}
		
		template_index = 3;
		if(flagTemplateEnable[category_index][template_index]){
			llvm::sys::fs::create_directory("./mutants/T"+to_string(template_index), true);
			const auto TS3 = ifStmt(
										isExpansionInMainFile(), 
										hasDescendant(
											memberExpr(
												member(anyOf(hasName("clockTick0"),hasName("simTimeStep"),hasName("tArray"),hasName("rtDW"),hasName("rtM"),hasName("rtM_"),hasName("rtU"),hasName("rtY")))
												)
											)
									).bind("ts3");
			MatchFinder MatchFinder33;
			MatchFinder33.addMatcher(TS3, &Handler);
			MatchFinder33.matchAST(Context);
		}
			
		//<<Insert match handler code here - do not delete this comment!!>>
		
	}
	
  }
};

/// Creates an `ASTConsumer` and logs begin and end of file processing.
class Action : public clang::ASTFrontendAction {
 public:
  using ASTConsumerPointer = std::unique_ptr<clang::ASTConsumer>;

  ASTConsumerPointer CreateASTConsumer(clang::CompilerInstance& Compiler,
                                       llvm::StringRef) override {
    return std::make_unique<Consumer>();
  }

  bool BeginSourceFileAction(clang::CompilerInstance& Compiler,
                             llvm::StringRef Filename) override {
	tmp_filepath = Filename;
    llvm::errs() << "Processing " << Filename << "\n\n";
    return true;
  }

  void EndSourceFileAction() override {
    llvm::errs() << "\nFinished processing file ...\n";
  }
};
}  // namespace Mutator

void print_mutation_results(){
	
	char buf[256];
    char pattern[]  = "| %-55s :   %7.2i  :   %7.2i    |" ;
	int category=0, sum_mutants=0;
	string tmp_temp="";
	string mutation_precisionaccuracy_locations="", mutation_exceptionhandling_locations="", mutation_timespace_locations="";
	cout<<"\n\n  ****     Mutation  count    **** "<<endl;
	cout<<"---------------------------------------------------------------------------------------"<<endl;
	cout<<"| Precision and accuracy category                         | # Mutants   | # Locations |"<<endl;
	cout<<"---------------------------------------------------------------------------------------"<<endl;
	for (int j = 0; j < 9; ++j)  //for (int j = 0; j < number_columns; ++j)
	{
		tmp_temp="Template["+precisionaccuracyTemplates[j]+"]";
		sprintf(buf, pattern, tmp_temp.c_str(), generated_mutants_count[category][j], get_unique_linelocations_count(generated_mutants_locations_count[category][j]) );
   	 	cout << buf << endl;
		mutation_precisionaccuracy_locations=mutation_precisionaccuracy_locations+generated_mutants_locations_count[category][j];
		sum_mutants=sum_mutants+generated_mutants_count[category][j];
	}
	cout<<"---------------------------------------------------------------------------------------"<<endl;
	cout<<"        Total unique mutation locations    :   "<< get_unique_linelocations_count(mutation_precisionaccuracy_locations) <<endl;
	
	category=1;
	cout<<"---------------------------------------------------------------------------------------"<<endl;
	cout<<"| Exception handling category                             | # Mutants   | # Locations |"<<endl;
	cout<<"---------------------------------------------------------------------------------------"<<endl;
	for (int j = 0; j < 7; ++j)
	{
		tmp_temp="Template["+exceptionhandlingTemplates[j]+"]";
		sprintf(buf, pattern, tmp_temp.c_str(), generated_mutants_count[category][j], get_unique_linelocations_count(generated_mutants_locations_count[category][j]) );
   	 	cout << buf << endl;
		mutation_exceptionhandling_locations=mutation_exceptionhandling_locations+generated_mutants_locations_count[category][j];
		sum_mutants=sum_mutants+generated_mutants_count[category][j];
	}
	cout<<"---------------------------------------------------------------------------------------"<<endl;
	cout<<"        Total unique mutation locations    :   "<< get_unique_linelocations_count(mutation_exceptionhandling_locations) <<endl;
	
	category=3;
	cout<<"---------------------------------------------------------------------------------------"<<endl;
	cout<<"| Time and space category                                 | # Mutants   | # Locations |"<<endl;
	cout<<"---------------------------------------------------------------------------------------"<<endl;
	for (int j = 0; j < 4; ++j)  //<<Insert print code here - do not delete this comment!!>>
	{
		tmp_temp="Template["+timespaceTemplates[j]+"]";
		sprintf(buf, pattern, tmp_temp.c_str(), generated_mutants_count[category][j], get_unique_linelocations_count(generated_mutants_locations_count[category][j]) );
   	 	cout << buf << endl;
		mutation_timespace_locations=mutation_timespace_locations+generated_mutants_locations_count[category][j];
		sum_mutants=sum_mutants+generated_mutants_count[category][j];
	}
	cout<<"---------------------------------------------------------------------------------------"<<endl;
	cout<<"        Total unique mutation locations    :   "<< get_unique_linelocations_count(mutation_timespace_locations) <<endl;
	
	total_unique_locations = get_unique_linelocations_count(mutation_precisionaccuracy_locations+mutation_exceptionhandling_locations+mutation_timespace_locations);
	cout<<"---------------------------------------------------------------------------------------"<<endl;
   	cout <<"|                                         Total mutants   :   "<< sum_mutants << "      :     "<< total_unique_locations << "      |" <<endl;
	cout<<"---------------------------------------------------------------------------------------"<<endl;
}

auto main(int argc, const char* argv[]) -> int {
  using namespace clang::tooling;

  CommonOptionsParser OptionsParser(argc, argv, ToolCategory);
  ClangTool Tool(OptionsParser.getCompilations(),
	         OptionsParser.getSourcePathList());

	user_seed = Seed.getValue();
	no_of_mutants = Number.getValue();
	string cps_categories= Category.getValue();
	user_categories= Category.getValue();
	
	targetfile_platform = detect_target_platform(argv[4], target_platform_file);
	
	if(targetfile_platform=="C" || targetfile_platform=="CPP"){
		precisionaccuracyTemplates.push_back("int -> float");
		precisionaccuracyTemplates.push_back("int -> double");
		precisionaccuracyTemplates.push_back("float -> int"); 
		precisionaccuracyTemplates.push_back("double -> int");
	}
	if(targetfile_platform=="MATLAB"){
		precisionaccuracyTemplates.push_back("int_T -> uint32_T");
		precisionaccuracyTemplates.push_back("int_T -> real_T");
		precisionaccuracyTemplates.push_back("uint32_T -> int_T"); 
		precisionaccuracyTemplates.push_back("real_T -> int_T");
	}
	precisionaccuracyTemplates.push_back("d*-> d*.0f");
	precisionaccuracyTemplates.push_back("d* -> d*.0");
	precisionaccuracyTemplates.push_back("d*.0f or d*.0 -> d* ");
	precisionaccuracyTemplates.push_back("float F() -> (double) F()");
	precisionaccuracyTemplates.push_back("double F() -> (float) F()");
	precisionaccuracyTemplates.push_back("double F() -> (float) F()");
	
	if(targetfile_platform=="C" || targetfile_platform=="CPP"){
		exceptionhandlingTemplates.push_back("if(isnan(X)) -> if(!isnan(X))");
		exceptionhandlingTemplates.push_back("if(isinf(X)) -> if(!isinf(X))");
		exceptionhandlingTemplates.push_back("if(!isnan(X)) -> if(isnan(X))");
		exceptionhandlingTemplates.push_back("if(!isinf(X)) -> if(isinf(X)");
	}
	if(targetfile_platform=="MATLAB"){
		exceptionhandlingTemplates.push_back("if(rtIsNaN(X)) -> if(!rtIsNaN(X))");
		exceptionhandlingTemplates.push_back("if(rtIsInf(X)) -> if(!rtIsInf(X))");
		exceptionhandlingTemplates.push_back("if(!rtIsNaN(X)) -> if(rtIsNaN(X))");
		exceptionhandlingTemplates.push_back("if(!rtIsInf(X)) -> if(rtIsInf(X)");
	}
	exceptionhandlingTemplates.push_back("insert if statment - check divide by 0");
	exceptionhandlingTemplates.push_back("remove if statment - check divide by 0");
	exceptionhandlingTemplates.push_back("miultiply denominator by zero");
	 
	timespaceTemplates.push_back("dataype of time is multiplied by 1000");
	timespaceTemplates.push_back("dataype of time in ifstmt() is negated");
	timespaceTemplates.push_back("variable of time is multiplied by 1000");
	timespaceTemplates.push_back("variable of time in ifstmt() is negated");
	//<<Insert template details code here - do not delete this comment!!>>
	
	flagTemplateEnable[0][0]=true;
	flagTemplateEnable[0][1]=true;
	flagTemplateEnable[0][2]=true;
	flagTemplateEnable[0][3]=true;
	flagTemplateEnable[0][4]=true;
	flagTemplateEnable[0][5]=true;
	flagTemplateEnable[0][6]=true;
	flagTemplateEnable[0][7]=true;
	flagTemplateEnable[0][8]=true;
	flagTemplateEnable[0][9]=false;

	flagTemplateEnable[1][0]=true;
	flagTemplateEnable[1][1]=true;
	flagTemplateEnable[1][2]=true;
	flagTemplateEnable[1][3]=true;
	flagTemplateEnable[1][4]=true;
	flagTemplateEnable[1][5]=true;
	flagTemplateEnable[1][6]=true;
	flagTemplateEnable[1][7]=true;
	flagTemplateEnable[1][8]=true;
	flagTemplateEnable[1][9]=true;

	flagTemplateEnable[2][0]=false;
	flagTemplateEnable[2][1]=false;
	flagTemplateEnable[2][2]=false;
	flagTemplateEnable[2][3]=false;
	flagTemplateEnable[2][4]=false;
	flagTemplateEnable[2][5]=false;
	flagTemplateEnable[2][6]=false;
	flagTemplateEnable[2][7]=false;
	flagTemplateEnable[2][8]=false;
	flagTemplateEnable[2][9]=false;

	flagTemplateEnable[3][0]=true;
	flagTemplateEnable[3][1]=true;
	flagTemplateEnable[3][2]=true;
	flagTemplateEnable[3][3]=true;
	//<<Insert flag array code here - do not delete this comment!!>>

	flagTemplateEnable[4][0]=false;
	flagTemplateEnable[4][1]=false;
	flagTemplateEnable[4][2]=false;
	flagTemplateEnable[4][3]=false;
	flagTemplateEnable[4][4]=false;
	flagTemplateEnable[4][5]=false;
	flagTemplateEnable[4][6]=false;
	flagTemplateEnable[4][7]=false;
	flagTemplateEnable[4][8]=false;
	flagTemplateEnable[4][9]=false;

	flagTemplateEnable[5][0]=false;
	flagTemplateEnable[5][1]=false;
	flagTemplateEnable[5][2]=false;
	flagTemplateEnable[5][3]=false;
	flagTemplateEnable[5][4]=false;
	flagTemplateEnable[5][5]=false;
	flagTemplateEnable[5][6]=false;
	flagTemplateEnable[5][7]=false;
	flagTemplateEnable[5][8]=false;
	flagTemplateEnable[5][9]=false;

	//fs::remove_all("./mutants");
	//fs::create_directory("./mutants");
	if(user_seed>0){

		  // add header search paths to compiler
		  //<<Insert header search paths to compiler here - do not delete this comment!!>>
		  
		if(verbose_compiler){
		  ArgumentsAdjuster ardj32 = getInsertArgumentAdjuster("-v");
		  Tool.appendArgumentsAdjuster(ardj32);
		}

			const auto Action = newFrontendActionFactory<Mutator::Action>();
			Tool.run(Action.get());

			// Remove directory if present.
			// Do this before extension removal incase directory has a period character.
			const size_t last_slash_idx = tmp_filepath.find_last_of("\\/");
			if (std::string::npos != last_slash_idx)
			{
				tmp_filepath.erase(0, last_slash_idx + 1);
			}

			input_filename_extension="."+tmp_filepath.substr(tmp_filepath.find_last_of(".") + 1);

			//Check for input file type - only c or cpp is allowed
			if( input_filename_extension.compare(".c") && input_filename_extension.compare(".cpp") ) {
				cout<<"\n Error in input file! \n"<<endl;
				cout<<"Please input only .c or .cpp extension file. \n Try ./mutator -help for more options.\n"<<endl;
				exit(0);
			}
			//printf("\n input_filename_extension: %s\n",input_filename_extension.c_str() );

			// Remove extension if present.
			const size_t period_idx = tmp_filepath.rfind('.');
			if (std::string::npos != period_idx)
			{
				tmp_filepath.erase(period_idx);
			}
			//printf("\n input_fullpath_filename: %s\n",tmp_filepath.c_str() );

			input_filename = "/"+tmp_filepath+"_";

		  //total possible mutatnts
		  for (int i = 0; i < number_rows; ++i)
			{
				for (int j = 0; j < number_columns; ++j)
			  {
				  sum_possible_mutants+=counterMatch[i][j];//cout<<"counterMatch["<<i<<"]["<<j<<"]: "<<counterMatch[i][j]<<endl;

			  }
			}

		  if (no_of_mutants==0 || no_of_mutants>sum_possible_mutants ){
			  cout<<"\n NOTE: User have provided number of mutants as zero or a number that is greater than the maximum possible mutants, therefore maximum possible mutants will be generated.\n"<<endl;
			  no_of_mutants=sum_possible_mutants;
		  }

		  if(no_of_mutants>0){
			 countMatchCompleted = true;
			 Tool.run(Action.get());

		  }else{
			 cout << "\nError in input argument: Make sure the number of mutants generated is greater than zero or equal to zero for maximum possible mutants!" << endl;
			}

		cout<<"\nUser input summary:"<<endl;
		cout<<"-------------------"<<endl;
		printf("Input file: %s",tmp_filepath.c_str());
		printf("\nRandom seed: %d",user_seed);
		printf("\n# mutants: %d",no_of_mutants);
		printf("\nMutant categories: %s\n",cps_categories.c_str());

		print_mutation_results();
		
		int nooflines = stoi(GetStdoutFromCommand("cloc "+tmp_filepath+"| sed -n 9p |awk '{print $5}'"));
		double mutationcoverage=(double)total_unique_locations/nooflines;

		cout<<"\n**************************************************************"<<endl;
		cout<<"Code mutation coverage for "+tmp_filepath+":"<<endl;
		cout<<"**************************************************************"<<endl;
    	cout << "Number of lines in file: " << nooflines << endl;
		cout << "Number of unique mutated locations: " << total_unique_locations << endl;
		cout<< "Total mutation coverage: "<< ROUNDF(mutationcoverage*100,100)<< "%" << endl;
		cout<<"--------------------------------------------------------------\n\n"<<endl;
			
	}else{
	 cout << "\nError in input argument: Make sure the random seed is greater than zero!" << endl;
	}
  	return 0;
}
