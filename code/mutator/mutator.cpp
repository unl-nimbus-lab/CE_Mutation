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
const int number_columns = 28; //<<Insert array size code here - do not delete this comment!!>>
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
				
			
			else if(template_index == 4){
				if(flagTemplateEnable[category_index][template_index]){
					const clang::DeclRefExpr* Variable34 = Result.Nodes.getNodeAs<clang::DeclRefExpr>("ts4");
					loc = Variable34->getSourceRange();
					locStart = srcMgr.getPresumedLoc(loc.getBegin());
					locEnd = srcMgr.getPresumedLoc(loc.getEnd());

					counterMatch[category_index][template_index]=counterMatch[category_index][template_index]+1;
				}
			}
			
			else if(template_index == 5){
				if(flagTemplateEnable[category_index][template_index]){
					const clang::DeclRefExpr* Variable35 = Result.Nodes.getNodeAs<clang::DeclRefExpr>("ts5");
					loc = Variable35->getSourceRange();
					locStart = srcMgr.getPresumedLoc(loc.getBegin());
					locEnd = srcMgr.getPresumedLoc(loc.getEnd());

					counterMatch[category_index][template_index]=counterMatch[category_index][template_index]+1;
				}
			}
			
			else if(template_index == 6){
				if(flagTemplateEnable[category_index][template_index]){
					const clang::DeclRefExpr* Variable36 = Result.Nodes.getNodeAs<clang::DeclRefExpr>("ts6");
					loc = Variable36->getSourceRange();
					locStart = srcMgr.getPresumedLoc(loc.getBegin());
					locEnd = srcMgr.getPresumedLoc(loc.getEnd());

					counterMatch[category_index][template_index]=counterMatch[category_index][template_index]+1;
				}
			}
			
			else if(template_index == 7){
				if(flagTemplateEnable[category_index][template_index]){
					const clang::DeclRefExpr* Variable37 = Result.Nodes.getNodeAs<clang::DeclRefExpr>("ts7");
					loc = Variable37->getSourceRange();
					locStart = srcMgr.getPresumedLoc(loc.getBegin());
					locEnd = srcMgr.getPresumedLoc(loc.getEnd());

					counterMatch[category_index][template_index]=counterMatch[category_index][template_index]+1;
				}
			}
			
			else if(template_index == 8){
				if(flagTemplateEnable[category_index][template_index]){
					const clang::DeclRefExpr* Variable38 = Result.Nodes.getNodeAs<clang::DeclRefExpr>("ts8");
					loc = Variable38->getSourceRange();
					locStart = srcMgr.getPresumedLoc(loc.getBegin());
					locEnd = srcMgr.getPresumedLoc(loc.getEnd());

					counterMatch[category_index][template_index]=counterMatch[category_index][template_index]+1;
				}
			}
			
			else if(template_index == 9){
				if(flagTemplateEnable[category_index][template_index]){
					const clang::DeclRefExpr* Variable39 = Result.Nodes.getNodeAs<clang::DeclRefExpr>("ts9");
					loc = Variable39->getSourceRange();
					locStart = srcMgr.getPresumedLoc(loc.getBegin());
					locEnd = srcMgr.getPresumedLoc(loc.getEnd());

					counterMatch[category_index][template_index]=counterMatch[category_index][template_index]+1;
				}
			}
			
			else if(template_index == 10){
				if(flagTemplateEnable[category_index][template_index]){
					const clang::DeclRefExpr* Variable310 = Result.Nodes.getNodeAs<clang::DeclRefExpr>("ts10");
					loc = Variable310->getSourceRange();
					locStart = srcMgr.getPresumedLoc(loc.getBegin());
					locEnd = srcMgr.getPresumedLoc(loc.getEnd());

					counterMatch[category_index][template_index]=counterMatch[category_index][template_index]+1;
				}
			}
			
			else if(template_index == 11){
				if(flagTemplateEnable[category_index][template_index]){
					const clang::DeclRefExpr* Variable311 = Result.Nodes.getNodeAs<clang::DeclRefExpr>("ts11");
					loc = Variable311->getSourceRange();
					locStart = srcMgr.getPresumedLoc(loc.getBegin());
					locEnd = srcMgr.getPresumedLoc(loc.getEnd());

					counterMatch[category_index][template_index]=counterMatch[category_index][template_index]+1;
				}
			}
			
			else if(template_index == 12){
				if(flagTemplateEnable[category_index][template_index]){
					const clang::DeclRefExpr* Variable312 = Result.Nodes.getNodeAs<clang::DeclRefExpr>("ts12");
					loc = Variable312->getSourceRange();
					locStart = srcMgr.getPresumedLoc(loc.getBegin());
					locEnd = srcMgr.getPresumedLoc(loc.getEnd());

					counterMatch[category_index][template_index]=counterMatch[category_index][template_index]+1;
				}
			}
			
			else if(template_index == 13){
				if(flagTemplateEnable[category_index][template_index]){
					const clang::DeclRefExpr* Variable313 = Result.Nodes.getNodeAs<clang::DeclRefExpr>("ts13");
					loc = Variable313->getSourceRange();
					locStart = srcMgr.getPresumedLoc(loc.getBegin());
					locEnd = srcMgr.getPresumedLoc(loc.getEnd());

					counterMatch[category_index][template_index]=counterMatch[category_index][template_index]+1;
				}
			}
			
			else if(template_index == 14){
				if(flagTemplateEnable[category_index][template_index]){
					const clang::DeclRefExpr* Variable314 = Result.Nodes.getNodeAs<clang::DeclRefExpr>("ts14");
					loc = Variable314->getSourceRange();
					locStart = srcMgr.getPresumedLoc(loc.getBegin());
					locEnd = srcMgr.getPresumedLoc(loc.getEnd());

					counterMatch[category_index][template_index]=counterMatch[category_index][template_index]+1;
				}
			}
			
			else if(template_index == 15){
				if(flagTemplateEnable[category_index][template_index]){
					const clang::DeclRefExpr* Variable315 = Result.Nodes.getNodeAs<clang::DeclRefExpr>("ts15");
					loc = Variable315->getSourceRange();
					locStart = srcMgr.getPresumedLoc(loc.getBegin());
					locEnd = srcMgr.getPresumedLoc(loc.getEnd());

					counterMatch[category_index][template_index]=counterMatch[category_index][template_index]+1;
				}
			}
			
			else if(template_index == 16){
				if(flagTemplateEnable[category_index][template_index]){
					const clang::DeclRefExpr* Variable316 = Result.Nodes.getNodeAs<clang::DeclRefExpr>("ts16");
					loc = Variable316->getSourceRange();
					locStart = srcMgr.getPresumedLoc(loc.getBegin());
					locEnd = srcMgr.getPresumedLoc(loc.getEnd());

					counterMatch[category_index][template_index]=counterMatch[category_index][template_index]+1;
				}
			}
			
			else if(template_index == 17){
				if(flagTemplateEnable[category_index][template_index]){
					const clang::DeclRefExpr* Variable317 = Result.Nodes.getNodeAs<clang::DeclRefExpr>("ts17");
					loc = Variable317->getSourceRange();
					locStart = srcMgr.getPresumedLoc(loc.getBegin());
					locEnd = srcMgr.getPresumedLoc(loc.getEnd());

					counterMatch[category_index][template_index]=counterMatch[category_index][template_index]+1;
				}
			}
			
			else if(template_index == 18){
				if(flagTemplateEnable[category_index][template_index]){
					const clang::DeclRefExpr* Variable318 = Result.Nodes.getNodeAs<clang::DeclRefExpr>("ts18");
					loc = Variable318->getSourceRange();
					locStart = srcMgr.getPresumedLoc(loc.getBegin());
					locEnd = srcMgr.getPresumedLoc(loc.getEnd());

					counterMatch[category_index][template_index]=counterMatch[category_index][template_index]+1;
				}
			}
			
			else if(template_index == 19){
				if(flagTemplateEnable[category_index][template_index]){
					const clang::DeclRefExpr* Variable319 = Result.Nodes.getNodeAs<clang::DeclRefExpr>("ts19");
					loc = Variable319->getSourceRange();
					locStart = srcMgr.getPresumedLoc(loc.getBegin());
					locEnd = srcMgr.getPresumedLoc(loc.getEnd());

					counterMatch[category_index][template_index]=counterMatch[category_index][template_index]+1;
				}
			}
			
			else if(template_index == 20){
				if(flagTemplateEnable[category_index][template_index]){
					const clang::DeclRefExpr* Variable320 = Result.Nodes.getNodeAs<clang::DeclRefExpr>("ts20");
					loc = Variable320->getSourceRange();
					locStart = srcMgr.getPresumedLoc(loc.getBegin());
					locEnd = srcMgr.getPresumedLoc(loc.getEnd());

					counterMatch[category_index][template_index]=counterMatch[category_index][template_index]+1;
				}
			}
			
			else if(template_index == 21){
				if(flagTemplateEnable[category_index][template_index]){
					const clang::DeclRefExpr* Variable321 = Result.Nodes.getNodeAs<clang::DeclRefExpr>("ts21");
					loc = Variable321->getSourceRange();
					locStart = srcMgr.getPresumedLoc(loc.getBegin());
					locEnd = srcMgr.getPresumedLoc(loc.getEnd());

					counterMatch[category_index][template_index]=counterMatch[category_index][template_index]+1;
				}
			}
			
			else if(template_index == 22){
				if(flagTemplateEnable[category_index][template_index]){
					const clang::DeclRefExpr* Variable322 = Result.Nodes.getNodeAs<clang::DeclRefExpr>("ts22");
					loc = Variable322->getSourceRange();
					locStart = srcMgr.getPresumedLoc(loc.getBegin());
					locEnd = srcMgr.getPresumedLoc(loc.getEnd());

					counterMatch[category_index][template_index]=counterMatch[category_index][template_index]+1;
				}
			}
			
			else if(template_index == 23){
				if(flagTemplateEnable[category_index][template_index]){
					const clang::DeclRefExpr* Variable323 = Result.Nodes.getNodeAs<clang::DeclRefExpr>("ts23");
					loc = Variable323->getSourceRange();
					locStart = srcMgr.getPresumedLoc(loc.getBegin());
					locEnd = srcMgr.getPresumedLoc(loc.getEnd());

					counterMatch[category_index][template_index]=counterMatch[category_index][template_index]+1;
				}
			}
			
			else if(template_index == 24){
				if(flagTemplateEnable[category_index][template_index]){
					const clang::DeclRefExpr* Variable324 = Result.Nodes.getNodeAs<clang::DeclRefExpr>("ts24");
					loc = Variable324->getSourceRange();
					locStart = srcMgr.getPresumedLoc(loc.getBegin());
					locEnd = srcMgr.getPresumedLoc(loc.getEnd());

					counterMatch[category_index][template_index]=counterMatch[category_index][template_index]+1;
				}
			}
			
			else if(template_index == 25){
				if(flagTemplateEnable[category_index][template_index]){
					const clang::DeclRefExpr* Variable325 = Result.Nodes.getNodeAs<clang::DeclRefExpr>("ts25");
					loc = Variable325->getSourceRange();
					locStart = srcMgr.getPresumedLoc(loc.getBegin());
					locEnd = srcMgr.getPresumedLoc(loc.getEnd());

					counterMatch[category_index][template_index]=counterMatch[category_index][template_index]+1;
				}
			}
			
			else if(template_index == 26){
				if(flagTemplateEnable[category_index][template_index]){
					const clang::DeclRefExpr* Variable326 = Result.Nodes.getNodeAs<clang::DeclRefExpr>("ts26");
					loc = Variable326->getSourceRange();
					locStart = srcMgr.getPresumedLoc(loc.getBegin());
					locEnd = srcMgr.getPresumedLoc(loc.getEnd());

					counterMatch[category_index][template_index]=counterMatch[category_index][template_index]+1;
				}
			}
			
			else if(template_index == 27){
				if(flagTemplateEnable[category_index][template_index]){
					const clang::DeclRefExpr* Variable327 = Result.Nodes.getNodeAs<clang::DeclRefExpr>("ts27");
					loc = Variable327->getSourceRange();
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
			
			
			else if(template_index == 4){
					if(flagTemplateEnable[category_index][template_index]){
						if (found != end){
							const clang::DeclRefExpr* Variable34 = Result.Nodes.getNodeAs<clang::DeclRefExpr>("ts4");
							clang::SourceRange loc = Variable34->getSourceRange();

							clang::LangOptions lopt;

							loc = Variable34->getSourceRange();
							locStart = srcMgr.getPresumedLoc(loc.getBegin());
							locEnd = srcMgr.getPresumedLoc(loc.getEnd());

							SourceLocation endLoc = clang::Lexer::getLocForEndOfToken(loc.getEnd(), 0, srcMgr, lopt);

							/// The Rewriter used to insert the override keyword.
							clang::Rewriter rewriter;
							rewriter.setSourceMgr(Result.Context->getSourceManager(),Result.Context->getLangOpts());
							rewriter.InsertText(endLoc,"* 1000",true,true);

							// Now emit the rewritten buffer.
							std::error_code error_code;
							write_filename="./mutants/T"+to_string(template_index)+input_filename+to_string(mutantNumber)+input_filename_extension;
							llvm::raw_fd_ostream outFile(write_filename, error_code, llvm::sys::fs::F_None);
							rewriter.getEditBuffer(rewriter.getSourceMgr().getMainFileID()).write(outFile); 
							outFile.close();

							mutation_comments = "\n/* Time and space category mutation - template T4: variable name distance of space category is mutated.  \n* Before mutation => "+ 
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
				
			else if(template_index == 5){
					if(flagTemplateEnable[category_index][template_index]){
						if (found != end){
							const clang::DeclRefExpr* Variable35 = Result.Nodes.getNodeAs<clang::DeclRefExpr>("ts5");
							clang::SourceRange loc = Variable35->getSourceRange();

							clang::LangOptions lopt;

							loc = Variable35->getSourceRange();
							locStart = srcMgr.getPresumedLoc(loc.getBegin());
							locEnd = srcMgr.getPresumedLoc(loc.getEnd());

							SourceLocation endLoc = clang::Lexer::getLocForEndOfToken(loc.getEnd(), 0, srcMgr, lopt);

							/// The Rewriter used to insert the override keyword.
							clang::Rewriter rewriter;
							rewriter.setSourceMgr(Result.Context->getSourceManager(),Result.Context->getLangOpts());
							rewriter.InsertText(endLoc,"* 1000",true,true);

							// Now emit the rewritten buffer.
							std::error_code error_code;
							write_filename="./mutants/T"+to_string(template_index)+input_filename+to_string(mutantNumber)+input_filename_extension;
							llvm::raw_fd_ostream outFile(write_filename, error_code, llvm::sys::fs::F_None);
							rewriter.getEditBuffer(rewriter.getSourceMgr().getMainFileID()).write(outFile); 
							outFile.close();

							mutation_comments = "\n/* Time and space category mutation - template T5: variable name dst of space category is mutated.  \n* Before mutation => "+ 
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
				
			else if(template_index == 6){
					if(flagTemplateEnable[category_index][template_index]){
						if (found != end){
							const clang::DeclRefExpr* Variable36 = Result.Nodes.getNodeAs<clang::DeclRefExpr>("ts6");
							clang::SourceRange loc = Variable36->getSourceRange();

							clang::LangOptions lopt;

							loc = Variable36->getSourceRange();
							locStart = srcMgr.getPresumedLoc(loc.getBegin());
							locEnd = srcMgr.getPresumedLoc(loc.getEnd());

							SourceLocation endLoc = clang::Lexer::getLocForEndOfToken(loc.getEnd(), 0, srcMgr, lopt);

							/// The Rewriter used to insert the override keyword.
							clang::Rewriter rewriter;
							rewriter.setSourceMgr(Result.Context->getSourceManager(),Result.Context->getLangOpts());
							rewriter.InsertText(endLoc,"* 1000",true,true);

							// Now emit the rewritten buffer.
							std::error_code error_code;
							write_filename="./mutants/T"+to_string(template_index)+input_filename+to_string(mutantNumber)+input_filename_extension;
							llvm::raw_fd_ostream outFile(write_filename, error_code, llvm::sys::fs::F_None);
							rewriter.getEditBuffer(rewriter.getSourceMgr().getMainFileID()).write(outFile); 
							outFile.close();

							mutation_comments = "\n/* Time and space category mutation - template T6: variable name dst_x of space category is mutated.  \n* Before mutation => "+ 
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
				
			else if(template_index == 7){
					if(flagTemplateEnable[category_index][template_index]){
						if (found != end){
							const clang::DeclRefExpr* Variable37 = Result.Nodes.getNodeAs<clang::DeclRefExpr>("ts7");
							clang::SourceRange loc = Variable37->getSourceRange();

							clang::LangOptions lopt;

							loc = Variable37->getSourceRange();
							locStart = srcMgr.getPresumedLoc(loc.getBegin());
							locEnd = srcMgr.getPresumedLoc(loc.getEnd());

							SourceLocation endLoc = clang::Lexer::getLocForEndOfToken(loc.getEnd(), 0, srcMgr, lopt);

							/// The Rewriter used to insert the override keyword.
							clang::Rewriter rewriter;
							rewriter.setSourceMgr(Result.Context->getSourceManager(),Result.Context->getLangOpts());
							rewriter.InsertText(endLoc,"* 1000",true,true);

							// Now emit the rewritten buffer.
							std::error_code error_code;
							write_filename="./mutants/T"+to_string(template_index)+input_filename+to_string(mutantNumber)+input_filename_extension;
							llvm::raw_fd_ostream outFile(write_filename, error_code, llvm::sys::fs::F_None);
							rewriter.getEditBuffer(rewriter.getSourceMgr().getMainFileID()).write(outFile); 
							outFile.close();

							mutation_comments = "\n/* Time and space category mutation - template T7: variable name dst_y of space category is mutated.  \n* Before mutation => "+ 
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
				
			else if(template_index == 8){
					if(flagTemplateEnable[category_index][template_index]){
						if (found != end){
							const clang::DeclRefExpr* Variable38 = Result.Nodes.getNodeAs<clang::DeclRefExpr>("ts8");
							clang::SourceRange loc = Variable38->getSourceRange();

							clang::LangOptions lopt;

							loc = Variable38->getSourceRange();
							locStart = srcMgr.getPresumedLoc(loc.getBegin());
							locEnd = srcMgr.getPresumedLoc(loc.getEnd());

							SourceLocation endLoc = clang::Lexer::getLocForEndOfToken(loc.getEnd(), 0, srcMgr, lopt);

							/// The Rewriter used to insert the override keyword.
							clang::Rewriter rewriter;
							rewriter.setSourceMgr(Result.Context->getSourceManager(),Result.Context->getLangOpts());
							rewriter.InsertText(endLoc,"/ 1000",true,true);

							// Now emit the rewritten buffer.
							std::error_code error_code;
							write_filename="./mutants/T"+to_string(template_index)+input_filename+to_string(mutantNumber)+input_filename_extension;
							llvm::raw_fd_ostream outFile(write_filename, error_code, llvm::sys::fs::F_None);
							rewriter.getEditBuffer(rewriter.getSourceMgr().getMainFileID()).write(outFile); 
							outFile.close();

							mutation_comments = "\n/* Time and space category mutation - template T8: variable name location of space category is mutated.  \n* Before mutation => "+ 
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
				
			else if(template_index == 9){
					if(flagTemplateEnable[category_index][template_index]){
						if (found != end){
							const clang::DeclRefExpr* Variable39 = Result.Nodes.getNodeAs<clang::DeclRefExpr>("ts9");
							clang::SourceRange loc = Variable39->getSourceRange();

							clang::LangOptions lopt;

							loc = Variable39->getSourceRange();
							locStart = srcMgr.getPresumedLoc(loc.getBegin());
							locEnd = srcMgr.getPresumedLoc(loc.getEnd());

							SourceLocation endLoc = clang::Lexer::getLocForEndOfToken(loc.getEnd(), 0, srcMgr, lopt);

							/// The Rewriter used to insert the override keyword.
							clang::Rewriter rewriter;
							rewriter.setSourceMgr(Result.Context->getSourceManager(),Result.Context->getLangOpts());
							rewriter.InsertText(endLoc,"/ 1000",true,true);

							// Now emit the rewritten buffer.
							std::error_code error_code;
							write_filename="./mutants/T"+to_string(template_index)+input_filename+to_string(mutantNumber)+input_filename_extension;
							llvm::raw_fd_ostream outFile(write_filename, error_code, llvm::sys::fs::F_None);
							rewriter.getEditBuffer(rewriter.getSourceMgr().getMainFileID()).write(outFile); 
							outFile.close();

							mutation_comments = "\n/* Time and space category mutation - template T9: variable name location_x of space category is mutated.  \n* Before mutation => "+ 
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
				
			else if(template_index == 10){
					if(flagTemplateEnable[category_index][template_index]){
						if (found != end){
							const clang::DeclRefExpr* Variable310 = Result.Nodes.getNodeAs<clang::DeclRefExpr>("ts10");
							clang::SourceRange loc = Variable310->getSourceRange();

							clang::LangOptions lopt;

							loc = Variable310->getSourceRange();
							locStart = srcMgr.getPresumedLoc(loc.getBegin());
							locEnd = srcMgr.getPresumedLoc(loc.getEnd());

							SourceLocation endLoc = clang::Lexer::getLocForEndOfToken(loc.getEnd(), 0, srcMgr, lopt);

							/// The Rewriter used to insert the override keyword.
							clang::Rewriter rewriter;
							rewriter.setSourceMgr(Result.Context->getSourceManager(),Result.Context->getLangOpts());
							rewriter.InsertText(endLoc,"/ 1000",true,true);

							// Now emit the rewritten buffer.
							std::error_code error_code;
							write_filename="./mutants/T"+to_string(template_index)+input_filename+to_string(mutantNumber)+input_filename_extension;
							llvm::raw_fd_ostream outFile(write_filename, error_code, llvm::sys::fs::F_None);
							rewriter.getEditBuffer(rewriter.getSourceMgr().getMainFileID()).write(outFile); 
							outFile.close();

							mutation_comments = "\n/* Time and space category mutation - template T10: variable name location_y of space category is mutated.  \n* Before mutation => "+ 
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
				
			else if(template_index == 11){
					if(flagTemplateEnable[category_index][template_index]){
						if (found != end){
							const clang::DeclRefExpr* Variable311 = Result.Nodes.getNodeAs<clang::DeclRefExpr>("ts11");
							clang::SourceRange loc = Variable311->getSourceRange();

							clang::LangOptions lopt;

							loc = Variable311->getSourceRange();
							locStart = srcMgr.getPresumedLoc(loc.getBegin());
							locEnd = srcMgr.getPresumedLoc(loc.getEnd());

							SourceLocation endLoc = clang::Lexer::getLocForEndOfToken(loc.getEnd(), 0, srcMgr, lopt);

							/// The Rewriter used to insert the override keyword.
							clang::Rewriter rewriter;
							rewriter.setSourceMgr(Result.Context->getSourceManager(),Result.Context->getLangOpts());
							rewriter.InsertText(endLoc,"* 1000",true,true);

							// Now emit the rewritten buffer.
							std::error_code error_code;
							write_filename="./mutants/T"+to_string(template_index)+input_filename+to_string(mutantNumber)+input_filename_extension;
							llvm::raw_fd_ostream outFile(write_filename, error_code, llvm::sys::fs::F_None);
							rewriter.getEditBuffer(rewriter.getSourceMgr().getMainFileID()).write(outFile); 
							outFile.close();

							mutation_comments = "\n/* Time and space category mutation - template T11: variable name space of space category is mutated.  \n* Before mutation => "+ 
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
				
			else if(template_index == 12){
					if(flagTemplateEnable[category_index][template_index]){
						if (found != end){
							const clang::DeclRefExpr* Variable312 = Result.Nodes.getNodeAs<clang::DeclRefExpr>("ts12");
							clang::SourceRange loc = Variable312->getSourceRange();

							clang::LangOptions lopt;

							loc = Variable312->getSourceRange();
							locStart = srcMgr.getPresumedLoc(loc.getBegin());
							locEnd = srcMgr.getPresumedLoc(loc.getEnd());

							SourceLocation endLoc = clang::Lexer::getLocForEndOfToken(loc.getEnd(), 0, srcMgr, lopt);

							/// The Rewriter used to insert the override keyword.
							clang::Rewriter rewriter;
							rewriter.setSourceMgr(Result.Context->getSourceManager(),Result.Context->getLangOpts());
							rewriter.InsertText(endLoc,"* 1000",true,true);

							// Now emit the rewritten buffer.
							std::error_code error_code;
							write_filename="./mutants/T"+to_string(template_index)+input_filename+to_string(mutantNumber)+input_filename_extension;
							llvm::raw_fd_ostream outFile(write_filename, error_code, llvm::sys::fs::F_None);
							rewriter.getEditBuffer(rewriter.getSourceMgr().getMainFileID()).write(outFile); 
							outFile.close();

							mutation_comments = "\n/* Time and space category mutation - template T12: variable name latitude of space category is mutated.  \n* Before mutation => "+ 
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
				
			else if(template_index == 13){
					if(flagTemplateEnable[category_index][template_index]){
						if (found != end){
							const clang::DeclRefExpr* Variable313 = Result.Nodes.getNodeAs<clang::DeclRefExpr>("ts13");
							clang::SourceRange loc = Variable313->getSourceRange();

							clang::LangOptions lopt;

							loc = Variable313->getSourceRange();
							locStart = srcMgr.getPresumedLoc(loc.getBegin());
							locEnd = srcMgr.getPresumedLoc(loc.getEnd());

							SourceLocation endLoc = clang::Lexer::getLocForEndOfToken(loc.getEnd(), 0, srcMgr, lopt);

							/// The Rewriter used to insert the override keyword.
							clang::Rewriter rewriter;
							rewriter.setSourceMgr(Result.Context->getSourceManager(),Result.Context->getLangOpts());
							rewriter.InsertText(endLoc,"/ 1000",true,true);

							// Now emit the rewritten buffer.
							std::error_code error_code;
							write_filename="./mutants/T"+to_string(template_index)+input_filename+to_string(mutantNumber)+input_filename_extension;
							llvm::raw_fd_ostream outFile(write_filename, error_code, llvm::sys::fs::F_None);
							rewriter.getEditBuffer(rewriter.getSourceMgr().getMainFileID()).write(outFile); 
							outFile.close();

							mutation_comments = "\n/* Time and space category mutation - template T13: variable name longitude of space category is mutated.  \n* Before mutation => "+ 
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
				
			else if(template_index == 14){
					if(flagTemplateEnable[category_index][template_index]){
						if (found != end){
							const clang::DeclRefExpr* Variable314 = Result.Nodes.getNodeAs<clang::DeclRefExpr>("ts14");
							clang::SourceRange loc = Variable314->getSourceRange();

							clang::LangOptions lopt;

							loc = Variable314->getSourceRange();
							locStart = srcMgr.getPresumedLoc(loc.getBegin());
							locEnd = srcMgr.getPresumedLoc(loc.getEnd());

							SourceLocation endLoc = clang::Lexer::getLocForEndOfToken(loc.getEnd(), 0, srcMgr, lopt);

							/// The Rewriter used to insert the override keyword.
							clang::Rewriter rewriter;
							rewriter.setSourceMgr(Result.Context->getSourceManager(),Result.Context->getLangOpts());
							rewriter.InsertText(endLoc,"* 1000",true,true);

							// Now emit the rewritten buffer.
							std::error_code error_code;
							write_filename="./mutants/T"+to_string(template_index)+input_filename+to_string(mutantNumber)+input_filename_extension;
							llvm::raw_fd_ostream outFile(write_filename, error_code, llvm::sys::fs::F_None);
							rewriter.getEditBuffer(rewriter.getSourceMgr().getMainFileID()).write(outFile); 
							outFile.close();

							mutation_comments = "\n/* Time and space category mutation - template T14: variable name position of space category is mutated.  \n* Before mutation => "+ 
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
				
			else if(template_index == 15){
					if(flagTemplateEnable[category_index][template_index]){
						if (found != end){
							const clang::DeclRefExpr* Variable315 = Result.Nodes.getNodeAs<clang::DeclRefExpr>("ts15");
							clang::SourceRange loc = Variable315->getSourceRange();

							clang::LangOptions lopt;

							loc = Variable315->getSourceRange();
							locStart = srcMgr.getPresumedLoc(loc.getBegin());
							locEnd = srcMgr.getPresumedLoc(loc.getEnd());

							SourceLocation endLoc = clang::Lexer::getLocForEndOfToken(loc.getEnd(), 0, srcMgr, lopt);

							/// The Rewriter used to insert the override keyword.
							clang::Rewriter rewriter;
							rewriter.setSourceMgr(Result.Context->getSourceManager(),Result.Context->getLangOpts());
							rewriter.InsertText(endLoc,"* 1000",true,true);

							// Now emit the rewritten buffer.
							std::error_code error_code;
							write_filename="./mutants/T"+to_string(template_index)+input_filename+to_string(mutantNumber)+input_filename_extension;
							llvm::raw_fd_ostream outFile(write_filename, error_code, llvm::sys::fs::F_None);
							rewriter.getEditBuffer(rewriter.getSourceMgr().getMainFileID()).write(outFile); 
							outFile.close();

							mutation_comments = "\n/* Time and space category mutation - template T15: variable name position_x of space category is mutated.  \n* Before mutation => "+ 
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
				
			else if(template_index == 16){
					if(flagTemplateEnable[category_index][template_index]){
						if (found != end){
							const clang::DeclRefExpr* Variable316 = Result.Nodes.getNodeAs<clang::DeclRefExpr>("ts16");
							clang::SourceRange loc = Variable316->getSourceRange();

							clang::LangOptions lopt;

							loc = Variable316->getSourceRange();
							locStart = srcMgr.getPresumedLoc(loc.getBegin());
							locEnd = srcMgr.getPresumedLoc(loc.getEnd());

							SourceLocation endLoc = clang::Lexer::getLocForEndOfToken(loc.getEnd(), 0, srcMgr, lopt);

							/// The Rewriter used to insert the override keyword.
							clang::Rewriter rewriter;
							rewriter.setSourceMgr(Result.Context->getSourceManager(),Result.Context->getLangOpts());
							rewriter.InsertText(endLoc,"* 1000",true,true);

							// Now emit the rewritten buffer.
							std::error_code error_code;
							write_filename="./mutants/T"+to_string(template_index)+input_filename+to_string(mutantNumber)+input_filename_extension;
							llvm::raw_fd_ostream outFile(write_filename, error_code, llvm::sys::fs::F_None);
							rewriter.getEditBuffer(rewriter.getSourceMgr().getMainFileID()).write(outFile); 
							outFile.close();

							mutation_comments = "\n/* Time and space category mutation - template T16: variable name position_y of space category is mutated.  \n* Before mutation => "+ 
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
				
			else if(template_index == 17){
					if(flagTemplateEnable[category_index][template_index]){
						if (found != end){
							const clang::DeclRefExpr* Variable317 = Result.Nodes.getNodeAs<clang::DeclRefExpr>("ts17");
							clang::SourceRange loc = Variable317->getSourceRange();

							clang::LangOptions lopt;

							loc = Variable317->getSourceRange();
							locStart = srcMgr.getPresumedLoc(loc.getBegin());
							locEnd = srcMgr.getPresumedLoc(loc.getEnd());

							SourceLocation endLoc = clang::Lexer::getLocForEndOfToken(loc.getEnd(), 0, srcMgr, lopt);

							/// The Rewriter used to insert the override keyword.
							clang::Rewriter rewriter;
							rewriter.setSourceMgr(Result.Context->getSourceManager(),Result.Context->getLangOpts());
							rewriter.InsertText(endLoc,"* 1000",true,true);

							// Now emit the rewritten buffer.
							std::error_code error_code;
							write_filename="./mutants/T"+to_string(template_index)+input_filename+to_string(mutantNumber)+input_filename_extension;
							llvm::raw_fd_ostream outFile(write_filename, error_code, llvm::sys::fs::F_None);
							rewriter.getEditBuffer(rewriter.getSourceMgr().getMainFileID()).write(outFile); 
							outFile.close();

							mutation_comments = "\n/* Time and space category mutation - template T17: variable name position_z of space category is mutated.  \n* Before mutation => "+ 
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
				
			else if(template_index == 18){
					if(flagTemplateEnable[category_index][template_index]){
						if (found != end){
							const clang::DeclRefExpr* Variable318 = Result.Nodes.getNodeAs<clang::DeclRefExpr>("ts18");
							clang::SourceRange loc = Variable318->getSourceRange();

							clang::LangOptions lopt;

							loc = Variable318->getSourceRange();
							locStart = srcMgr.getPresumedLoc(loc.getBegin());
							locEnd = srcMgr.getPresumedLoc(loc.getEnd());

							SourceLocation endLoc = clang::Lexer::getLocForEndOfToken(loc.getEnd(), 0, srcMgr, lopt);

							/// The Rewriter used to insert the override keyword.
							clang::Rewriter rewriter;
							rewriter.setSourceMgr(Result.Context->getSourceManager(),Result.Context->getLangOpts());
							rewriter.InsertText(endLoc,"* 1000",true,true);

							// Now emit the rewritten buffer.
							std::error_code error_code;
							write_filename="./mutants/T"+to_string(template_index)+input_filename+to_string(mutantNumber)+input_filename_extension;
							llvm::raw_fd_ostream outFile(write_filename, error_code, llvm::sys::fs::F_None);
							rewriter.getEditBuffer(rewriter.getSourceMgr().getMainFileID()).write(outFile); 
							outFile.close();

							mutation_comments = "\n/* Time and space category mutation - template T18: variable name frame of space category is mutated.  \n* Before mutation => "+ 
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
				
			else if(template_index == 19){
					if(flagTemplateEnable[category_index][template_index]){
						if (found != end){
							const clang::DeclRefExpr* Variable319 = Result.Nodes.getNodeAs<clang::DeclRefExpr>("ts19");
							clang::SourceRange loc = Variable319->getSourceRange();

							clang::LangOptions lopt;

							loc = Variable319->getSourceRange();
							locStart = srcMgr.getPresumedLoc(loc.getBegin());
							locEnd = srcMgr.getPresumedLoc(loc.getEnd());

							SourceLocation endLoc = clang::Lexer::getLocForEndOfToken(loc.getEnd(), 0, srcMgr, lopt);

							/// The Rewriter used to insert the override keyword.
							clang::Rewriter rewriter;
							rewriter.setSourceMgr(Result.Context->getSourceManager(),Result.Context->getLangOpts());
							rewriter.InsertText(endLoc,"* 1000",true,true);

							// Now emit the rewritten buffer.
							std::error_code error_code;
							write_filename="./mutants/T"+to_string(template_index)+input_filename+to_string(mutantNumber)+input_filename_extension;
							llvm::raw_fd_ostream outFile(write_filename, error_code, llvm::sys::fs::F_None);
							rewriter.getEditBuffer(rewriter.getSourceMgr().getMainFileID()).write(outFile); 
							outFile.close();

							mutation_comments = "\n/* Time and space category mutation - template T19: variable name frame_x of space category is mutated.  \n* Before mutation => "+ 
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
				
			else if(template_index == 20){
					if(flagTemplateEnable[category_index][template_index]){
						if (found != end){
							const clang::DeclRefExpr* Variable320 = Result.Nodes.getNodeAs<clang::DeclRefExpr>("ts20");
							clang::SourceRange loc = Variable320->getSourceRange();

							clang::LangOptions lopt;

							loc = Variable320->getSourceRange();
							locStart = srcMgr.getPresumedLoc(loc.getBegin());
							locEnd = srcMgr.getPresumedLoc(loc.getEnd());

							SourceLocation endLoc = clang::Lexer::getLocForEndOfToken(loc.getEnd(), 0, srcMgr, lopt);

							/// The Rewriter used to insert the override keyword.
							clang::Rewriter rewriter;
							rewriter.setSourceMgr(Result.Context->getSourceManager(),Result.Context->getLangOpts());
							rewriter.InsertText(endLoc,"* 1000",true,true);

							// Now emit the rewritten buffer.
							std::error_code error_code;
							write_filename="./mutants/T"+to_string(template_index)+input_filename+to_string(mutantNumber)+input_filename_extension;
							llvm::raw_fd_ostream outFile(write_filename, error_code, llvm::sys::fs::F_None);
							rewriter.getEditBuffer(rewriter.getSourceMgr().getMainFileID()).write(outFile); 
							outFile.close();

							mutation_comments = "\n/* Time and space category mutation - template T20: variable name frame_y of space category is mutated.  \n* Before mutation => "+ 
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
				
			else if(template_index == 21){
					if(flagTemplateEnable[category_index][template_index]){
						if (found != end){
							const clang::DeclRefExpr* Variable321 = Result.Nodes.getNodeAs<clang::DeclRefExpr>("ts21");
							clang::SourceRange loc = Variable321->getSourceRange();

							clang::LangOptions lopt;

							loc = Variable321->getSourceRange();
							locStart = srcMgr.getPresumedLoc(loc.getBegin());
							locEnd = srcMgr.getPresumedLoc(loc.getEnd());

							SourceLocation endLoc = clang::Lexer::getLocForEndOfToken(loc.getEnd(), 0, srcMgr, lopt);

							/// The Rewriter used to insert the override keyword.
							clang::Rewriter rewriter;
							rewriter.setSourceMgr(Result.Context->getSourceManager(),Result.Context->getLangOpts());
							rewriter.InsertText(endLoc,"* 1000",true,true);

							// Now emit the rewritten buffer.
							std::error_code error_code;
							write_filename="./mutants/T"+to_string(template_index)+input_filename+to_string(mutantNumber)+input_filename_extension;
							llvm::raw_fd_ostream outFile(write_filename, error_code, llvm::sys::fs::F_None);
							rewriter.getEditBuffer(rewriter.getSourceMgr().getMainFileID()).write(outFile); 
							outFile.close();

							mutation_comments = "\n/* Time and space category mutation - template T21: variable name frame_z of space category is mutated.  \n* Before mutation => "+ 
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
				
			else if(template_index == 22){
					if(flagTemplateEnable[category_index][template_index]){
						if (found != end){
							const clang::DeclRefExpr* Variable322 = Result.Nodes.getNodeAs<clang::DeclRefExpr>("ts22");
							clang::SourceRange loc = Variable322->getSourceRange();

							clang::LangOptions lopt;

							loc = Variable322->getSourceRange();
							locStart = srcMgr.getPresumedLoc(loc.getBegin());
							locEnd = srcMgr.getPresumedLoc(loc.getEnd());

							SourceLocation endLoc = clang::Lexer::getLocForEndOfToken(loc.getEnd(), 0, srcMgr, lopt);

							/// The Rewriter used to insert the override keyword.
							clang::Rewriter rewriter;
							rewriter.setSourceMgr(Result.Context->getSourceManager(),Result.Context->getLangOpts());
							rewriter.InsertText(endLoc,"/ 1000",true,true);

							// Now emit the rewritten buffer.
							std::error_code error_code;
							write_filename="./mutants/T"+to_string(template_index)+input_filename+to_string(mutantNumber)+input_filename_extension;
							llvm::raw_fd_ostream outFile(write_filename, error_code, llvm::sys::fs::F_None);
							rewriter.getEditBuffer(rewriter.getSourceMgr().getMainFileID()).write(outFile); 
							outFile.close();

							mutation_comments = "\n/* Time and space category mutation - template T22: variable name coordinate of space category is mutated.  \n* Before mutation => "+ 
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
				
			else if(template_index == 23){
					if(flagTemplateEnable[category_index][template_index]){
						if (found != end){
							const clang::DeclRefExpr* Variable323 = Result.Nodes.getNodeAs<clang::DeclRefExpr>("ts23");
							clang::SourceRange loc = Variable323->getSourceRange();

							clang::LangOptions lopt;

							loc = Variable323->getSourceRange();
							locStart = srcMgr.getPresumedLoc(loc.getBegin());
							locEnd = srcMgr.getPresumedLoc(loc.getEnd());

							SourceLocation endLoc = clang::Lexer::getLocForEndOfToken(loc.getEnd(), 0, srcMgr, lopt);

							/// The Rewriter used to insert the override keyword.
							clang::Rewriter rewriter;
							rewriter.setSourceMgr(Result.Context->getSourceManager(),Result.Context->getLangOpts());
							rewriter.InsertText(endLoc,"/ 1000",true,true);

							// Now emit the rewritten buffer.
							std::error_code error_code;
							write_filename="./mutants/T"+to_string(template_index)+input_filename+to_string(mutantNumber)+input_filename_extension;
							llvm::raw_fd_ostream outFile(write_filename, error_code, llvm::sys::fs::F_None);
							rewriter.getEditBuffer(rewriter.getSourceMgr().getMainFileID()).write(outFile); 
							outFile.close();

							mutation_comments = "\n/* Time and space category mutation - template T23: variable name coordinate_x of space category is mutated.  \n* Before mutation => "+ 
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
				
			else if(template_index == 24){
					if(flagTemplateEnable[category_index][template_index]){
						if (found != end){
							const clang::DeclRefExpr* Variable324 = Result.Nodes.getNodeAs<clang::DeclRefExpr>("ts24");
							clang::SourceRange loc = Variable324->getSourceRange();

							clang::LangOptions lopt;

							loc = Variable324->getSourceRange();
							locStart = srcMgr.getPresumedLoc(loc.getBegin());
							locEnd = srcMgr.getPresumedLoc(loc.getEnd());

							SourceLocation endLoc = clang::Lexer::getLocForEndOfToken(loc.getEnd(), 0, srcMgr, lopt);

							/// The Rewriter used to insert the override keyword.
							clang::Rewriter rewriter;
							rewriter.setSourceMgr(Result.Context->getSourceManager(),Result.Context->getLangOpts());
							rewriter.InsertText(endLoc,"/ 1000",true,true);

							// Now emit the rewritten buffer.
							std::error_code error_code;
							write_filename="./mutants/T"+to_string(template_index)+input_filename+to_string(mutantNumber)+input_filename_extension;
							llvm::raw_fd_ostream outFile(write_filename, error_code, llvm::sys::fs::F_None);
							rewriter.getEditBuffer(rewriter.getSourceMgr().getMainFileID()).write(outFile); 
							outFile.close();

							mutation_comments = "\n/* Time and space category mutation - template T24: variable name coordinate_y of space category is mutated.  \n* Before mutation => "+ 
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
				
			else if(template_index == 25){
					if(flagTemplateEnable[category_index][template_index]){
						if (found != end){
							const clang::DeclRefExpr* Variable325 = Result.Nodes.getNodeAs<clang::DeclRefExpr>("ts25");
							clang::SourceRange loc = Variable325->getSourceRange();

							clang::LangOptions lopt;

							loc = Variable325->getSourceRange();
							locStart = srcMgr.getPresumedLoc(loc.getBegin());
							locEnd = srcMgr.getPresumedLoc(loc.getEnd());

							SourceLocation endLoc = clang::Lexer::getLocForEndOfToken(loc.getEnd(), 0, srcMgr, lopt);

							/// The Rewriter used to insert the override keyword.
							clang::Rewriter rewriter;
							rewriter.setSourceMgr(Result.Context->getSourceManager(),Result.Context->getLangOpts());
							rewriter.InsertText(endLoc,"/ 1000",true,true);

							// Now emit the rewritten buffer.
							std::error_code error_code;
							write_filename="./mutants/T"+to_string(template_index)+input_filename+to_string(mutantNumber)+input_filename_extension;
							llvm::raw_fd_ostream outFile(write_filename, error_code, llvm::sys::fs::F_None);
							rewriter.getEditBuffer(rewriter.getSourceMgr().getMainFileID()).write(outFile); 
							outFile.close();

							mutation_comments = "\n/* Time and space category mutation - template T25: variable name coordinate_z of space category is mutated.  \n* Before mutation => "+ 
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
				
			else if(template_index == 26){
					if(flagTemplateEnable[category_index][template_index]){
						if (found != end){
							const clang::DeclRefExpr* Variable326 = Result.Nodes.getNodeAs<clang::DeclRefExpr>("ts26");
							clang::SourceRange loc = Variable326->getSourceRange();

							clang::LangOptions lopt;

							loc = Variable326->getSourceRange();
							locStart = srcMgr.getPresumedLoc(loc.getBegin());
							locEnd = srcMgr.getPresumedLoc(loc.getEnd());

							SourceLocation endLoc = clang::Lexer::getLocForEndOfToken(loc.getEnd(), 0, srcMgr, lopt);

							/// The Rewriter used to insert the override keyword.
							clang::Rewriter rewriter;
							rewriter.setSourceMgr(Result.Context->getSourceManager(),Result.Context->getLangOpts());
							rewriter.InsertText(endLoc,"/ 1000",true,true);

							// Now emit the rewritten buffer.
							std::error_code error_code;
							write_filename="./mutants/T"+to_string(template_index)+input_filename+to_string(mutantNumber)+input_filename_extension;
							llvm::raw_fd_ostream outFile(write_filename, error_code, llvm::sys::fs::F_None);
							rewriter.getEditBuffer(rewriter.getSourceMgr().getMainFileID()).write(outFile); 
							outFile.close();

							mutation_comments = "\n/* Time and space category mutation - template T26: variable name elevation of space category is mutated.  \n* Before mutation => "+ 
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
				
			else if(template_index == 27){
					if(flagTemplateEnable[category_index][template_index]){
						if (found != end){
							const clang::DeclRefExpr* Variable327 = Result.Nodes.getNodeAs<clang::DeclRefExpr>("ts27");
							clang::SourceRange loc = Variable327->getSourceRange();

							clang::LangOptions lopt;

							loc = Variable327->getSourceRange();
							locStart = srcMgr.getPresumedLoc(loc.getBegin());
							locEnd = srcMgr.getPresumedLoc(loc.getEnd());

							SourceLocation endLoc = clang::Lexer::getLocForEndOfToken(loc.getEnd(), 0, srcMgr, lopt);

							/// The Rewriter used to insert the override keyword.
							clang::Rewriter rewriter;
							rewriter.setSourceMgr(Result.Context->getSourceManager(),Result.Context->getLangOpts());
							rewriter.InsertText(endLoc,"/ 1000",true,true);

							// Now emit the rewritten buffer.
							std::error_code error_code;
							write_filename="./mutants/T"+to_string(template_index)+input_filename+to_string(mutantNumber)+input_filename_extension;
							llvm::raw_fd_ostream outFile(write_filename, error_code, llvm::sys::fs::F_None);
							rewriter.getEditBuffer(rewriter.getSourceMgr().getMainFileID()).write(outFile); 
							outFile.close();

							mutation_comments = "\n/* Time and space category mutation - template T27: variable name range of space category is mutated.  \n* Before mutation => "+ 
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
			
		
		template_index = 4;
		if(flagTemplateEnable[category_index][template_index]){
			llvm::sys::fs::create_directory("./mutants/T"+to_string(template_index), true);
			const auto TS4 = declRefExpr(
												isExpansionInMainFile(), 
												to(varDecl(hasName("distance"))),
												unless(hasAncestor(ifStmt())),
												unless(hasAncestor(arraySubscriptExpr())),
												unless(hasAncestor( varDecl(hasType(arrayType()))))
											).bind("ts4");
			MatchFinder MatchFinder34;
			MatchFinder34.addMatcher(TS4, &Handler);
			MatchFinder34.matchAST(Context);
		}
		
		template_index = 5;
		if(flagTemplateEnable[category_index][template_index]){
			llvm::sys::fs::create_directory("./mutants/T"+to_string(template_index), true);
			const auto TS5 = declRefExpr(
												isExpansionInMainFile(), 
												to(varDecl(hasName("dst"))),
												unless(hasAncestor(ifStmt())),
												unless(hasAncestor(arraySubscriptExpr())),
												unless(hasAncestor( varDecl(hasType(arrayType()))))
											).bind("ts5");
			MatchFinder MatchFinder35;
			MatchFinder35.addMatcher(TS5, &Handler);
			MatchFinder35.matchAST(Context);
		}
		
		template_index = 6;
		if(flagTemplateEnable[category_index][template_index]){
			llvm::sys::fs::create_directory("./mutants/T"+to_string(template_index), true);
			const auto TS6 = declRefExpr(
												isExpansionInMainFile(), 
												to(varDecl(hasName("dst_x"))),
												unless(hasAncestor(ifStmt())),
												unless(hasAncestor(arraySubscriptExpr())),
												unless(hasAncestor( varDecl(hasType(arrayType()))))
											).bind("ts6");
			MatchFinder MatchFinder36;
			MatchFinder36.addMatcher(TS6, &Handler);
			MatchFinder36.matchAST(Context);
		}
		
		template_index = 7;
		if(flagTemplateEnable[category_index][template_index]){
			llvm::sys::fs::create_directory("./mutants/T"+to_string(template_index), true);
			const auto TS7 = declRefExpr(
												isExpansionInMainFile(), 
												to(varDecl(hasName("dst_y"))),
												unless(hasAncestor(ifStmt())),
												unless(hasAncestor(arraySubscriptExpr())),
												unless(hasAncestor( varDecl(hasType(arrayType()))))
											).bind("ts7");
			MatchFinder MatchFinder37;
			MatchFinder37.addMatcher(TS7, &Handler);
			MatchFinder37.matchAST(Context);
		}
		
		template_index = 8;
		if(flagTemplateEnable[category_index][template_index]){
			llvm::sys::fs::create_directory("./mutants/T"+to_string(template_index), true);
			const auto TS8 = declRefExpr(
												isExpansionInMainFile(), 
												to(varDecl(hasName("location"))),
												unless(hasAncestor(ifStmt())),
												unless(hasAncestor(arraySubscriptExpr())),
												unless(hasAncestor( varDecl(hasType(arrayType()))))
											).bind("ts8");
			MatchFinder MatchFinder38;
			MatchFinder38.addMatcher(TS8, &Handler);
			MatchFinder38.matchAST(Context);
		}
		
		template_index = 9;
		if(flagTemplateEnable[category_index][template_index]){
			llvm::sys::fs::create_directory("./mutants/T"+to_string(template_index), true);
			const auto TS9 = declRefExpr(
												isExpansionInMainFile(), 
												to(varDecl(hasName("location_x"))),
												unless(hasAncestor(ifStmt())),
												unless(hasAncestor(arraySubscriptExpr())),
												unless(hasAncestor( varDecl(hasType(arrayType()))))
											).bind("ts9");
			MatchFinder MatchFinder39;
			MatchFinder39.addMatcher(TS9, &Handler);
			MatchFinder39.matchAST(Context);
		}
		
		template_index = 10;
		if(flagTemplateEnable[category_index][template_index]){
			llvm::sys::fs::create_directory("./mutants/T"+to_string(template_index), true);
			const auto TS10 = declRefExpr(
												isExpansionInMainFile(), 
												to(varDecl(hasName("location_y"))),
												unless(hasAncestor(ifStmt())),
												unless(hasAncestor(arraySubscriptExpr())),
												unless(hasAncestor( varDecl(hasType(arrayType()))))
											).bind("ts10");
			MatchFinder MatchFinder310;
			MatchFinder310.addMatcher(TS10, &Handler);
			MatchFinder310.matchAST(Context);
		}
		
		template_index = 11;
		if(flagTemplateEnable[category_index][template_index]){
			llvm::sys::fs::create_directory("./mutants/T"+to_string(template_index), true);
			const auto TS11 = declRefExpr(
												isExpansionInMainFile(), 
												to(varDecl(hasName("space"))),
												unless(hasAncestor(ifStmt())),
												unless(hasAncestor(arraySubscriptExpr())),
												unless(hasAncestor( varDecl(hasType(arrayType()))))
											).bind("ts11");
			MatchFinder MatchFinder311;
			MatchFinder311.addMatcher(TS11, &Handler);
			MatchFinder311.matchAST(Context);
		}
		
		template_index = 12;
		if(flagTemplateEnable[category_index][template_index]){
			llvm::sys::fs::create_directory("./mutants/T"+to_string(template_index), true);
			const auto TS12 = declRefExpr(
												isExpansionInMainFile(), 
												to(varDecl(hasName("latitude"))),
												unless(hasAncestor(ifStmt())),
												unless(hasAncestor(arraySubscriptExpr())),
												unless(hasAncestor( varDecl(hasType(arrayType()))))
											).bind("ts12");
			MatchFinder MatchFinder312;
			MatchFinder312.addMatcher(TS12, &Handler);
			MatchFinder312.matchAST(Context);
		}
		
		template_index = 13;
		if(flagTemplateEnable[category_index][template_index]){
			llvm::sys::fs::create_directory("./mutants/T"+to_string(template_index), true);
			const auto TS13 = declRefExpr(
												isExpansionInMainFile(), 
												to(varDecl(hasName("longitude"))),
												unless(hasAncestor(ifStmt())),
												unless(hasAncestor(arraySubscriptExpr())),
												unless(hasAncestor( varDecl(hasType(arrayType()))))
											).bind("ts13");
			MatchFinder MatchFinder313;
			MatchFinder313.addMatcher(TS13, &Handler);
			MatchFinder313.matchAST(Context);
		}
		
		template_index = 14;
		if(flagTemplateEnable[category_index][template_index]){
			llvm::sys::fs::create_directory("./mutants/T"+to_string(template_index), true);
			const auto TS14 = declRefExpr(
												isExpansionInMainFile(), 
												to(varDecl(hasName("position"))),
												unless(hasAncestor(ifStmt())),
												unless(hasAncestor(arraySubscriptExpr())),
												unless(hasAncestor( varDecl(hasType(arrayType()))))
											).bind("ts14");
			MatchFinder MatchFinder314;
			MatchFinder314.addMatcher(TS14, &Handler);
			MatchFinder314.matchAST(Context);
		}
		
		template_index = 15;
		if(flagTemplateEnable[category_index][template_index]){
			llvm::sys::fs::create_directory("./mutants/T"+to_string(template_index), true);
			const auto TS15 = declRefExpr(
												isExpansionInMainFile(), 
												to(varDecl(hasName("position_x"))),
												unless(hasAncestor(ifStmt())),
												unless(hasAncestor(arraySubscriptExpr())),
												unless(hasAncestor( varDecl(hasType(arrayType()))))
											).bind("ts15");
			MatchFinder MatchFinder315;
			MatchFinder315.addMatcher(TS15, &Handler);
			MatchFinder315.matchAST(Context);
		}
		
		template_index = 16;
		if(flagTemplateEnable[category_index][template_index]){
			llvm::sys::fs::create_directory("./mutants/T"+to_string(template_index), true);
			const auto TS16 = declRefExpr(
												isExpansionInMainFile(), 
												to(varDecl(hasName("position_y"))),
												unless(hasAncestor(ifStmt())),
												unless(hasAncestor(arraySubscriptExpr())),
												unless(hasAncestor( varDecl(hasType(arrayType()))))
											).bind("ts16");
			MatchFinder MatchFinder316;
			MatchFinder316.addMatcher(TS16, &Handler);
			MatchFinder316.matchAST(Context);
		}
		
		template_index = 17;
		if(flagTemplateEnable[category_index][template_index]){
			llvm::sys::fs::create_directory("./mutants/T"+to_string(template_index), true);
			const auto TS17 = declRefExpr(
												isExpansionInMainFile(), 
												to(varDecl(hasName("position_z"))),
												unless(hasAncestor(ifStmt())),
												unless(hasAncestor(arraySubscriptExpr())),
												unless(hasAncestor( varDecl(hasType(arrayType()))))
											).bind("ts17");
			MatchFinder MatchFinder317;
			MatchFinder317.addMatcher(TS17, &Handler);
			MatchFinder317.matchAST(Context);
		}
		
		template_index = 18;
		if(flagTemplateEnable[category_index][template_index]){
			llvm::sys::fs::create_directory("./mutants/T"+to_string(template_index), true);
			const auto TS18 = declRefExpr(
												isExpansionInMainFile(), 
												to(varDecl(hasName("frame"))),
												unless(hasAncestor(ifStmt())),
												unless(hasAncestor(arraySubscriptExpr())),
												unless(hasAncestor( varDecl(hasType(arrayType()))))
											).bind("ts18");
			MatchFinder MatchFinder318;
			MatchFinder318.addMatcher(TS18, &Handler);
			MatchFinder318.matchAST(Context);
		}
		
		template_index = 19;
		if(flagTemplateEnable[category_index][template_index]){
			llvm::sys::fs::create_directory("./mutants/T"+to_string(template_index), true);
			const auto TS19 = declRefExpr(
												isExpansionInMainFile(), 
												to(varDecl(hasName("frame_x"))),
												unless(hasAncestor(ifStmt())),
												unless(hasAncestor(arraySubscriptExpr())),
												unless(hasAncestor( varDecl(hasType(arrayType()))))
											).bind("ts19");
			MatchFinder MatchFinder319;
			MatchFinder319.addMatcher(TS19, &Handler);
			MatchFinder319.matchAST(Context);
		}
		
		template_index = 20;
		if(flagTemplateEnable[category_index][template_index]){
			llvm::sys::fs::create_directory("./mutants/T"+to_string(template_index), true);
			const auto TS20 = declRefExpr(
												isExpansionInMainFile(), 
												to(varDecl(hasName("frame_y"))),
												unless(hasAncestor(ifStmt())),
												unless(hasAncestor(arraySubscriptExpr())),
												unless(hasAncestor( varDecl(hasType(arrayType()))))
											).bind("ts20");
			MatchFinder MatchFinder320;
			MatchFinder320.addMatcher(TS20, &Handler);
			MatchFinder320.matchAST(Context);
		}
		
		template_index = 21;
		if(flagTemplateEnable[category_index][template_index]){
			llvm::sys::fs::create_directory("./mutants/T"+to_string(template_index), true);
			const auto TS21 = declRefExpr(
												isExpansionInMainFile(), 
												to(varDecl(hasName("frame_z"))),
												unless(hasAncestor(ifStmt())),
												unless(hasAncestor(arraySubscriptExpr())),
												unless(hasAncestor( varDecl(hasType(arrayType()))))
											).bind("ts21");
			MatchFinder MatchFinder321;
			MatchFinder321.addMatcher(TS21, &Handler);
			MatchFinder321.matchAST(Context);
		}
		
		template_index = 22;
		if(flagTemplateEnable[category_index][template_index]){
			llvm::sys::fs::create_directory("./mutants/T"+to_string(template_index), true);
			const auto TS22 = declRefExpr(
												isExpansionInMainFile(), 
												to(varDecl(hasName("coordinate"))),
												unless(hasAncestor(ifStmt())),
												unless(hasAncestor(arraySubscriptExpr())),
												unless(hasAncestor( varDecl(hasType(arrayType()))))
											).bind("ts22");
			MatchFinder MatchFinder322;
			MatchFinder322.addMatcher(TS22, &Handler);
			MatchFinder322.matchAST(Context);
		}
		
		template_index = 23;
		if(flagTemplateEnable[category_index][template_index]){
			llvm::sys::fs::create_directory("./mutants/T"+to_string(template_index), true);
			const auto TS23 = declRefExpr(
												isExpansionInMainFile(), 
												to(varDecl(hasName("coordinate_x"))),
												unless(hasAncestor(ifStmt())),
												unless(hasAncestor(arraySubscriptExpr())),
												unless(hasAncestor( varDecl(hasType(arrayType()))))
											).bind("ts23");
			MatchFinder MatchFinder323;
			MatchFinder323.addMatcher(TS23, &Handler);
			MatchFinder323.matchAST(Context);
		}
		
		template_index = 24;
		if(flagTemplateEnable[category_index][template_index]){
			llvm::sys::fs::create_directory("./mutants/T"+to_string(template_index), true);
			const auto TS24 = declRefExpr(
												isExpansionInMainFile(), 
												to(varDecl(hasName("coordinate_y"))),
												unless(hasAncestor(ifStmt())),
												unless(hasAncestor(arraySubscriptExpr())),
												unless(hasAncestor( varDecl(hasType(arrayType()))))
											).bind("ts24");
			MatchFinder MatchFinder324;
			MatchFinder324.addMatcher(TS24, &Handler);
			MatchFinder324.matchAST(Context);
		}
		
		template_index = 25;
		if(flagTemplateEnable[category_index][template_index]){
			llvm::sys::fs::create_directory("./mutants/T"+to_string(template_index), true);
			const auto TS25 = declRefExpr(
												isExpansionInMainFile(), 
												to(varDecl(hasName("coordinate_z"))),
												unless(hasAncestor(ifStmt())),
												unless(hasAncestor(arraySubscriptExpr())),
												unless(hasAncestor( varDecl(hasType(arrayType()))))
											).bind("ts25");
			MatchFinder MatchFinder325;
			MatchFinder325.addMatcher(TS25, &Handler);
			MatchFinder325.matchAST(Context);
		}
		
		template_index = 26;
		if(flagTemplateEnable[category_index][template_index]){
			llvm::sys::fs::create_directory("./mutants/T"+to_string(template_index), true);
			const auto TS26 = declRefExpr(
												isExpansionInMainFile(), 
												to(varDecl(hasName("elevation"))),
												unless(hasAncestor(ifStmt())),
												unless(hasAncestor(arraySubscriptExpr())),
												unless(hasAncestor( varDecl(hasType(arrayType()))))
											).bind("ts26");
			MatchFinder MatchFinder326;
			MatchFinder326.addMatcher(TS26, &Handler);
			MatchFinder326.matchAST(Context);
		}
		
		template_index = 27;
		if(flagTemplateEnable[category_index][template_index]){
			llvm::sys::fs::create_directory("./mutants/T"+to_string(template_index), true);
			const auto TS27 = declRefExpr(
												isExpansionInMainFile(), 
												to(varDecl(hasName("range"))),
												unless(hasAncestor(ifStmt())),
												unless(hasAncestor(arraySubscriptExpr())),
												unless(hasAncestor( varDecl(hasType(arrayType()))))
											).bind("ts27");
			MatchFinder MatchFinder327;
			MatchFinder327.addMatcher(TS27, &Handler);
			MatchFinder327.matchAST(Context);
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
	for (int j = 0; j < 28; ++j) //<< Insert print code here - do not delete this comment!!>>
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
	
	timespaceTemplates.push_back("space variables mutation: distance * 1000");
	
	timespaceTemplates.push_back("space variables mutation: dst * 1000");
	
	timespaceTemplates.push_back("space variables mutation: dst_x * 1000");
	
	timespaceTemplates.push_back("space variables mutation: dst_y * 1000");
	
	timespaceTemplates.push_back("space variables mutation: location / 1000");
	
	timespaceTemplates.push_back("space variables mutation: location_x / 1000");
	
	timespaceTemplates.push_back("space variables mutation: location_y / 1000");
	
	timespaceTemplates.push_back("space variables mutation: space * 1000");
	
	timespaceTemplates.push_back("space variables mutation: latitude * 1000");
	
	timespaceTemplates.push_back("space variables mutation: longitude / 1000");
	
	timespaceTemplates.push_back("space variables mutation: position * 1000");
	
	timespaceTemplates.push_back("space variables mutation: position_x * 1000");
	
	timespaceTemplates.push_back("space variables mutation: position_y * 1000");
	
	timespaceTemplates.push_back("space variables mutation: position_z * 1000");
	
	timespaceTemplates.push_back("space variables mutation: frame * 1000");
	
	timespaceTemplates.push_back("space variables mutation: frame_x * 1000");
	
	timespaceTemplates.push_back("space variables mutation: frame_y * 1000");
	
	timespaceTemplates.push_back("space variables mutation: frame_z * 1000");
	
	timespaceTemplates.push_back("space variables mutation: coordinate / 1000");
	
	timespaceTemplates.push_back("space variables mutation: coordinate_x / 1000");
	
	timespaceTemplates.push_back("space variables mutation: coordinate_y / 1000");
	
	timespaceTemplates.push_back("space variables mutation: coordinate_z / 1000");
	
	timespaceTemplates.push_back("space variables mutation: elevation / 1000");
	
	timespaceTemplates.push_back("space variables mutation: range / 1000");
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
	
	flagTemplateEnable[3][4]=true;
	
	flagTemplateEnable[3][5]=true;
	
	flagTemplateEnable[3][6]=true;
	
	flagTemplateEnable[3][7]=true;
	
	flagTemplateEnable[3][8]=true;
	
	flagTemplateEnable[3][9]=true;
	
	flagTemplateEnable[3][10]=true;
	
	flagTemplateEnable[3][11]=true;
	
	flagTemplateEnable[3][12]=true;
	
	flagTemplateEnable[3][13]=true;
	
	flagTemplateEnable[3][14]=true;
	
	flagTemplateEnable[3][15]=true;
	
	flagTemplateEnable[3][16]=true;
	
	flagTemplateEnable[3][17]=true;
	
	flagTemplateEnable[3][18]=true;
	
	flagTemplateEnable[3][19]=true;
	
	flagTemplateEnable[3][20]=true;
	
	flagTemplateEnable[3][21]=true;
	
	flagTemplateEnable[3][22]=true;
	
	flagTemplateEnable[3][23]=true;
	
	flagTemplateEnable[3][24]=true;
	
	flagTemplateEnable[3][25]=true;
	
	flagTemplateEnable[3][26]=true;
	
	flagTemplateEnable[3][27]=true;
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
		  
		  string clang_inc_dir1("-c");
		  ArgumentsAdjuster ardj1 = getInsertArgumentAdjuster(clang_inc_dir1.c_str());
		  Tool.appendArgumentsAdjuster(ardj1);
		  
		  string clang_inc_dir2("-ansi");
		  ArgumentsAdjuster ardj2 = getInsertArgumentAdjuster(clang_inc_dir2.c_str());
		  Tool.appendArgumentsAdjuster(ardj2);
		  
		  string clang_inc_dir3("-pedantic");
		  ArgumentsAdjuster ardj3 = getInsertArgumentAdjuster(clang_inc_dir3.c_str());
		  Tool.appendArgumentsAdjuster(ardj3);
		  
		  string clang_inc_dir4("-Wno-long-long");
		  ArgumentsAdjuster ardj4 = getInsertArgumentAdjuster(clang_inc_dir4.c_str());
		  Tool.appendArgumentsAdjuster(ardj4);
		  
		  string clang_inc_dir5("-fwrapv");
		  ArgumentsAdjuster ardj5 = getInsertArgumentAdjuster(clang_inc_dir5.c_str());
		  Tool.appendArgumentsAdjuster(ardj5);
		  
		  string clang_inc_dir6("-fPIC");
		  ArgumentsAdjuster ardj6 = getInsertArgumentAdjuster(clang_inc_dir6.c_str());
		  Tool.appendArgumentsAdjuster(ardj6);
		  
		  string clang_inc_dir7("-O0");
		  ArgumentsAdjuster ardj7 = getInsertArgumentAdjuster(clang_inc_dir7.c_str());
		  Tool.appendArgumentsAdjuster(ardj7);
		  
		  string clang_inc_dir8("-DMAT_FILE=1");
		  ArgumentsAdjuster ardj8 = getInsertArgumentAdjuster(clang_inc_dir8.c_str());
		  Tool.appendArgumentsAdjuster(ardj8);
		  
		  string clang_inc_dir9("-DINTEGER_CODE=0");
		  ArgumentsAdjuster ardj9 = getInsertArgumentAdjuster(clang_inc_dir9.c_str());
		  Tool.appendArgumentsAdjuster(ardj9);
		  
		  string clang_inc_dir10("-DMT=0");
		  ArgumentsAdjuster ardj10 = getInsertArgumentAdjuster(clang_inc_dir10.c_str());
		  Tool.appendArgumentsAdjuster(ardj10);
		  
		  string clang_inc_dir11("-DCLASSIC_INTERFACE=1");
		  ArgumentsAdjuster ardj11 = getInsertArgumentAdjuster(clang_inc_dir11.c_str());
		  Tool.appendArgumentsAdjuster(ardj11);
		  
		  string clang_inc_dir12("-DALLOCATIONFCN=0");
		  ArgumentsAdjuster ardj12 = getInsertArgumentAdjuster(clang_inc_dir12.c_str());
		  Tool.appendArgumentsAdjuster(ardj12);
		  
		  string clang_inc_dir13("-DONESTEPFCN=0");
		  ArgumentsAdjuster ardj13 = getInsertArgumentAdjuster(clang_inc_dir13.c_str());
		  Tool.appendArgumentsAdjuster(ardj13);
		  
		  string clang_inc_dir14("-DTERMFCN=1");
		  ArgumentsAdjuster ardj14 = getInsertArgumentAdjuster(clang_inc_dir14.c_str());
		  Tool.appendArgumentsAdjuster(ardj14);
		  
		  string clang_inc_dir15("-DMULTI_INSTANCE_CODE=0");
		  ArgumentsAdjuster ardj15 = getInsertArgumentAdjuster(clang_inc_dir15.c_str());
		  Tool.appendArgumentsAdjuster(ardj15);
		  
		  string clang_inc_dir16("-DTID01EQ=1");
		  ArgumentsAdjuster ardj16 = getInsertArgumentAdjuster(clang_inc_dir16.c_str());
		  Tool.appendArgumentsAdjuster(ardj16);
		  
		  string clang_inc_dir17("-DMODEL=b747cl");
		  ArgumentsAdjuster ardj17 = getInsertArgumentAdjuster(clang_inc_dir17.c_str());
		  Tool.appendArgumentsAdjuster(ardj17);
		  
		  string clang_inc_dir18("-DNUMST=2");
		  ArgumentsAdjuster ardj18 = getInsertArgumentAdjuster(clang_inc_dir18.c_str());
		  Tool.appendArgumentsAdjuster(ardj18);
		  
		  string clang_inc_dir19("-DNCSTATES=0");
		  ArgumentsAdjuster ardj19 = getInsertArgumentAdjuster(clang_inc_dir19.c_str());
		  Tool.appendArgumentsAdjuster(ardj19);
		  
		  string clang_inc_dir20("-DHAVESTDIO");
		  ArgumentsAdjuster ardj20 = getInsertArgumentAdjuster(clang_inc_dir20.c_str());
		  Tool.appendArgumentsAdjuster(ardj20);
		  
		  string clang_inc_dir21("-DRT");
		  ArgumentsAdjuster ardj21 = getInsertArgumentAdjuster(clang_inc_dir21.c_str());
		  Tool.appendArgumentsAdjuster(ardj21);
		  
		  string clang_inc_dir22("-DUSE_RTMODEL");
		  ArgumentsAdjuster ardj22 = getInsertArgumentAdjuster(clang_inc_dir22.c_str());
		  Tool.appendArgumentsAdjuster(ardj22);
		  
		  string clang_inc_dir23("-DUNIX");
		  ArgumentsAdjuster ardj23 = getInsertArgumentAdjuster(clang_inc_dir23.c_str());
		  Tool.appendArgumentsAdjuster(ardj23);
		  
		  string clang_inc_dir24("-I/usr/lib/clang/3.8.0/include");
		  ArgumentsAdjuster ardj24 = getInsertArgumentAdjuster(clang_inc_dir24.c_str());
		  Tool.appendArgumentsAdjuster(ardj24);
		  
		  string clang_inc_dir25("-I/home/balaji/clang-llvm/mutationdocker/code/mutator");
		  ArgumentsAdjuster ardj25 = getInsertArgumentAdjuster(clang_inc_dir25.c_str());
		  Tool.appendArgumentsAdjuster(ardj25);
		  
		  string clang_inc_dir26("-I/home/balaji/clang-llvm/mutationdocker/code");
		  ArgumentsAdjuster ardj26 = getInsertArgumentAdjuster(clang_inc_dir26.c_str());
		  Tool.appendArgumentsAdjuster(ardj26);
		  
		  string clang_inc_dir27("-I/home/balaji/mutation/airlib/b747cl_grt_rtw ");
		  ArgumentsAdjuster ardj27 = getInsertArgumentAdjuster(clang_inc_dir27.c_str());
		  Tool.appendArgumentsAdjuster(ardj27);
		  
		  string clang_inc_dir28("-I/home/balaji/mutation/airlib ");
		  ArgumentsAdjuster ardj28 = getInsertArgumentAdjuster(clang_inc_dir28.c_str());
		  Tool.appendArgumentsAdjuster(ardj28);
		  
		  string clang_inc_dir29("-I/usr/local/MATLAB/R2016a/extern/include");
		  ArgumentsAdjuster ardj29 = getInsertArgumentAdjuster(clang_inc_dir29.c_str());
		  Tool.appendArgumentsAdjuster(ardj29);
		  
		  string clang_inc_dir30("-I/usr/local/MATLAB/R2016a/simulink/include");
		  ArgumentsAdjuster ardj30 = getInsertArgumentAdjuster(clang_inc_dir30.c_str());
		  Tool.appendArgumentsAdjuster(ardj30);
		  
		  string clang_inc_dir31("-I/usr/local/MATLAB/R2016a/rtw/c/src");
		  ArgumentsAdjuster ardj31 = getInsertArgumentAdjuster(clang_inc_dir31.c_str());
		  Tool.appendArgumentsAdjuster(ardj31);
		  
		  string clang_inc_dir32("-I/usr/local/MATLAB/R2016a/rtw/c/src/ext_mode/common");
		  ArgumentsAdjuster ardj32 = getInsertArgumentAdjuster(clang_inc_dir32.c_str());
		  Tool.appendArgumentsAdjuster(ardj32);
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
