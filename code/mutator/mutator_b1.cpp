// Clang includes
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/Decl.h"
#include "clang/AST/Type.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Frontend/CompilerInstance.h"	
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/ArgumentsAdjusters.h"
#include <iostream>
#include <iomanip>
#include "clang/Rewrite/Frontend/Rewriters.h"
#include "clang/Rewrite/Core/RewriteBuffer.h"
#include "clang/Rewrite/Core/Rewriter.h"

/*
#include "function_definition_lister.h"
#include "llvm/Support/CommandLine.h"
/home/balaji/clang-llvm/llvm/include/llvm/Support/CommandLine.h

using namespace clang::tooling;
using namespace llvm;
using namespace clang::ast_matchers;

#include <home/balaji/mutation/airlib/b747cl_grt_rtw>
#include "home/balaji/mutation/airlib"
#include "usr/local/MATLAB/R2016a/extern/include"
#include "usr/local/MATLAB/R2016a/simulink/include"
#include "usr/local/MATLAB/R2016a/rtw/c/src"
#include "usr/local/MATLAB/R2016a/rtw/c/src/ext_mode/common"

#include "extern/include"
#include "simulink/include"
#include "src"
#include "src/ext_mode/common"
*/
// LLVM includes
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"

// Standard includes
#include <memory>
#include <string>
#include <vector>

using namespace llvm;
using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::tooling;

static llvm::cl::OptionCategory flt_cat("func-decl-list-am options");

static llvm::cl::opt<bool> verbose_compiler(
  "vc",
  llvm::cl::desc("pass -v to compiler instance (default false)"),
  llvm::cl::cat(flt_cat),
  llvm::cl::init(false)
);





namespace Mutator {

/// Callback class for clang-variable matches.
class MatchHandler : public clang::ast_matchers::MatchFinder::MatchCallback {
 public:
  using MatchResult = clang::ast_matchers::MatchFinder::MatchResult;

  /// Handles the matched variable.
  ///
  /// Checks if the name of the matched variable is either empty or prefixed
  /// with `clang_` else emits a diagnostic and FixItHint.
  void run(const MatchResult& Result) {

    
clang::SourceManager &srcMgr = Result.Context->getSourceManager();





if(const clang::VarDecl* var = Result.Nodes.getNodeAs<clang::VarDecl>("clang")) {
      if(var->isFunctionOrMethodVarDecl()) {
        std::cout << std::setw(20) << std::left << "Local Variable: " << var->getName().str() << "\t\t";
        std::cout << ((clang::CXXMethodDecl*)(var->getParentFunctionOrMethod()))->getQualifiedNameAsString() << "\t";
        std::cout << "--" << srcMgr.getFilename(var->getLocation()).str();
        std::cout << "\n";
      }
      if(var->hasExternalStorage()) {
        std::cout << std::setw(20) << std::left << "External Variable: " << var->getName().str() << "\t\t";
        std::cout << "--" << srcMgr.getFilename(var->getLocation()).str();
        std::cout << "\n";
      }
      else if(var->hasGlobalStorage()) {
        std::cout << std::setw(20) << std::left << "Global Variable: " << var->getName().str() << "\t\t";
        std::cout << "--" << srcMgr.getFilename(var->getLocation()).str();
        std::cout << "\n";
      }
}
    const clang::VarDecl* Variable =
        Result.Nodes.getNodeAs<clang::VarDecl>("clang");

    //clang::SourceManager &srcMgr = Result.Context->getSourceManager();

    clang::SourceRange loc = Variable->getSourceRange();
    clang::PresumedLoc locStart = srcMgr.getPresumedLoc(loc.getBegin());
    clang::PresumedLoc locEnd = srcMgr.getPresumedLoc(loc.getEnd());

    //std::string r = "hihi";
    std::cout << "\n\n ***  Start print  *** \n\n" << std::endl;
    std::cout << Variable->getName().str() << std::endl;

    //std::cout << loc.str() << std::endl;
    std::cout << locStart.getLine() << std::endl;
    std::cout << locStart.getColumn() << std::endl;

    std::cout << locEnd.getLine() << std::endl;
    std::cout << locEnd.getColumn() << std::endl;

    /// The `Rewriter` used to insert the `override` keyword.
    clang::Rewriter rewriter;
    rewriter.setSourceMgr(Result.Context->getSourceManager(),Result.Context->getLangOpts());
    //rewriter.InsertText(loc.getBegin(),"float_Balaji", true, true); //not working
    //rewriter.ReplaceText(loc, "float_Balaji");
    rewriter.ReplaceText(loc.getBegin(),5,"real_Tmutated"); 

    // Now emit the rewritten buffer.
   // Rewriter.getEditBuffer(SM.getMainFileID()).write(llvm::outs()); --> this will output to screen as what you got.
    std::error_code error_code;
    llvm::raw_fd_ostream outFile("output.txt", error_code, llvm::sys::fs::F_None);
    rewriter.getEditBuffer(rewriter.getSourceMgr().getMainFileID()).write(outFile); // --> this will write the result to outFile
    outFile.close();




    //errs() << "\n Found " << "";

    //rewriter.getEditBuffer(rewriter.getSourceMgr().getMainFileID()).write(errs());

    //rewriter.ReplaceText(loc, "float_Balaji");
    //rewriter.ReplaceText(sloc,5,"float_Balaji");

//std::cout << Variable->getLocation().str() << std::endl;
    std::cout << "\n\n ***  End print  *** \n\n" << std::endl;
    //printf("%s\n",r.c_str());

    const llvm::StringRef Name = Variable->getName();

    if (Name.empty() || Name.startswith("clang_")) return;

    clang::DiagnosticsEngine& Engine = Result.Context->getDiagnostics();

    const unsigned ID =
        Engine.getCustomDiagID(clang::DiagnosticsEngine::Warning,
                               "found mutating variable");

    /// Hint to the user to prefix the variable with 'clang_'.
    const clang::FixItHint FixIt =
        clang::FixItHint::CreateInsertion(Variable->getLocation(), "precision & accuracy mutation");

    Engine.Report(Variable->getLocation(), ID).AddFixItHint(FixIt);


  }
};  // namespace Mutator



/// Dispatches the ASTMatcher.
class Consumer : public clang::ASTConsumer {
 public:
  /// Creates the matcher for clang variables and dispatches it on the TU.
  void HandleTranslationUnit(clang::ASTContext& Context) override {
    using namespace clang::ast_matchers;  // NOLINT(build/namespaces)

	
    const auto Matcher = declaratorDecl(
	isExpansionInMainFile(), 
	hasType(asString("int_T"))
	).bind("clang");

 	 /*
    // clang-format off
    const auto Matcher = varDecl(
      isExpansionInMainFile(),
      hasType(isConstQualified()),                              // const
      hasInitializer(
        hasType(cxxRecordDecl(
          isLambda(),                                           // lambda
          has(functionTemplateDecl(                             // auto
            has(cxxMethodDecl(
              isNoThrow(),                                      // noexcept
              hasBody(compoundStmt(hasDescendant(gotoStmt())))  // goto
      )))))))).bind("clang");
    // clang-format on
	*/
    MatchHandler Handler;
    MatchFinder MatchFinder;
    MatchFinder.addMatcher(Matcher, &Handler);
    MatchFinder.matchAST(Context);
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
    llvm::errs() << "Processing " << Filename << "\n\n";
    return true;
  }

  void EndSourceFileAction() override {
    if (!RewriteOption) return;
    const auto File = Rewriter.getSourceMgr().getMainFileID();
    Rewriter.getEditBuffer(File).write(llvm::outs());
    llvm::errs() << "\nFinished processing file ...\n";
  }

 private:
  /// Whether to rewrite the source code. Forwarded to the `Consumer`.
  bool RewriteOption;

  /// A `clang::Rewriter` to rewrite source code. Forwarded to the `Consumer`.
  clang::Rewriter Rewriter;
};
}  // namespace Mutator

namespace {
llvm::cl::OptionCategory ToolCategory("clang-variables options");
llvm::cl::extrahelp MoreHelp(R"(
  Finds all Const Lambdas, that take an Auto parameter, are declared Noexcept
  and have a Goto statement inside, e.g.:

  const auto lambda = [] (auto) noexcept {
    bool done = true;
    flip: done = !done;
    if (!done) goto flip;
  }
)");

llvm::cl::extrahelp
    CommonHelp(clang::tooling::CommonOptionsParser::HelpMessage);
}  // namespace

auto main(int argc, const char* argv[]) -> int {
  using namespace clang::tooling;

  CommonOptionsParser OptionsParser(argc, argv, ToolCategory);
  ClangTool Tool(OptionsParser.getCompilations(),
                 OptionsParser.getSourcePathList());
//MATLAB_ROOT := /usr/local/MATLAB/R2016a
//START_DIR := /home/balaji/clang-llvm/cppdocker/code/mutator

//#-c -ansi -pedantic -Wno-long-long -fwrapv -fPIC -O0 -DMAT_FILE=1 -DINTEGER_CODE=0 -DMT=0 -DCLASSIC_INTERFACE=1 -DALLOCATIONFCN=0 -DONESTEPFCN=0 -DTERMFCN=1 -DMULTI_INSTANCE_CODE=0 -DTID01EQ=1 -DMODEL=b747cl -DNUMST=2 -DNCSTATES=0 -DHAVESTDIO -DRT -DUSE_RTMODEL -DUNIX -I$(START_DIR)/b747cl_grt_rtw -I$(START_DIR) -I$(MATLAB_ROOT)/extern/include -I$(MATLAB_ROOT)/simulink/include -I$(MATLAB_ROOT)/rtw/c/src -I$(MATLAB_ROOT)/rtw/c/src/ext_mode/common -Iextern/include -Isimulink/include -Isrc -Isrc/ext_mode/common 

  // add header search paths to compiler

	std::string clang_inc_dir1("-c");
	std::string clang_inc_dir2("-ansi");
	std::string clang_inc_dir3("-pedantic");
	std::string clang_inc_dir4("-Wno-long-long");
	std::string clang_inc_dir5("-Wgnu-include-next");
	std::string clang_inc_dir6("-fwrapv");
	std::string clang_inc_dir7("-fPIC");
	std::string clang_inc_dir8("-O0");
	std::string clang_inc_dir9("-DMAT_FILE=1");
	std::string clang_inc_dir10("-DINTEGER_CODE=0");
	std::string clang_inc_dir11("-DMT=0");
	std::string clang_inc_dir12("-DCLASSIC_INTERFACE=1");
	std::string clang_inc_dir13("-DALLOCATIONFCN=0");
	std::string clang_inc_dir14("-DONESTEPFCN=0");
	std::string clang_inc_dir15("-DTERMFCN=1");
	std::string clang_inc_dir16("-DMULTI_INSTANCE_CODE=0");
	std::string clang_inc_dir17("-DTID01EQ=1");
	std::string clang_inc_dir18("-DMODEL=b747cl");
	std::string clang_inc_dir19("-DNUMST=2");
	std::string clang_inc_dir20("-DNCSTATES=0");
	std::string clang_inc_dir21("-DHAVESTDIO");
	std::string clang_inc_dir22("-DRT");
	std::string clang_inc_dir23("-DUSE_RTMODEL");
	std::string clang_inc_dir24("-DUNIX");
	std::string clang_inc_dir25("-I/usr/lib/clang/3.8.0/include");
	std::string clang_inc_dir26("-I/home/balaji/clang-llvm/cppdocker/code/mutator");
	std::string clang_inc_dir27("-I/home/balaji/clang-llvm/cppdocker/code");
	std::string clang_inc_dir28("-I/usr/local/MATLAB/R2016a/extern/include");
	std::string clang_inc_dir29("-I/usr/local/MATLAB/R2016a/simulink/include");
	std::string clang_inc_dir30("-I/usr/local/MATLAB/R2016a/rtw/c/src");
	std::string clang_inc_dir31("-I/usr/local/MATLAB/R2016a/rtw/c/src/ext_mode/common");



//-I$(START_DIR)/b747cl_grt_rtw -I$(START_DIR) -I$(MATLAB_ROOT)/extern/include -I$(MATLAB_ROOT)/simulink/include -I$(MATLAB_ROOT)/rtw/c/src -I$(MATLAB_ROOT)/rtw/c/src/ext_mode/common

  //std::string clang_inc_dir1("-I/src/ext_mode/common"); //clang_inc_dir1="-I/path1/to/headers"
  //std::string clang_inc_dir2("-I/simulink/include");
  //std::string clang_inc_dir3("-I/extern/include");
  ArgumentsAdjuster ardj1 = getInsertArgumentAdjuster(clang_inc_dir1.c_str());
  ArgumentsAdjuster ardj2 = getInsertArgumentAdjuster(clang_inc_dir2.c_str());
  ArgumentsAdjuster ardj3 = getInsertArgumentAdjuster(clang_inc_dir3.c_str());
  ArgumentsAdjuster ardj4 = getInsertArgumentAdjuster(clang_inc_dir4.c_str());
  ArgumentsAdjuster ardj5 = getInsertArgumentAdjuster(clang_inc_dir5.c_str());
  ArgumentsAdjuster ardj6 = getInsertArgumentAdjuster(clang_inc_dir6.c_str());
  ArgumentsAdjuster ardj7 = getInsertArgumentAdjuster(clang_inc_dir7.c_str());
  ArgumentsAdjuster ardj8 = getInsertArgumentAdjuster(clang_inc_dir8.c_str());
  ArgumentsAdjuster ardj9 = getInsertArgumentAdjuster(clang_inc_dir9.c_str());
  ArgumentsAdjuster ardj10 = getInsertArgumentAdjuster(clang_inc_dir10.c_str());
  ArgumentsAdjuster ardj11 = getInsertArgumentAdjuster(clang_inc_dir11.c_str());
  ArgumentsAdjuster ardj12 = getInsertArgumentAdjuster(clang_inc_dir12.c_str());
  ArgumentsAdjuster ardj13 = getInsertArgumentAdjuster(clang_inc_dir13.c_str());
  ArgumentsAdjuster ardj14 = getInsertArgumentAdjuster(clang_inc_dir14.c_str());
  ArgumentsAdjuster ardj15 = getInsertArgumentAdjuster(clang_inc_dir15.c_str());
  ArgumentsAdjuster ardj16 = getInsertArgumentAdjuster(clang_inc_dir16.c_str());
  ArgumentsAdjuster ardj17 = getInsertArgumentAdjuster(clang_inc_dir17.c_str());
  ArgumentsAdjuster ardj18 = getInsertArgumentAdjuster(clang_inc_dir18.c_str());
  ArgumentsAdjuster ardj19 = getInsertArgumentAdjuster(clang_inc_dir19.c_str());
  ArgumentsAdjuster ardj20 = getInsertArgumentAdjuster(clang_inc_dir20.c_str());
  ArgumentsAdjuster ardj21 = getInsertArgumentAdjuster(clang_inc_dir21.c_str());
  ArgumentsAdjuster ardj22 = getInsertArgumentAdjuster(clang_inc_dir22.c_str());
  ArgumentsAdjuster ardj23 = getInsertArgumentAdjuster(clang_inc_dir23.c_str());
  ArgumentsAdjuster ardj24 = getInsertArgumentAdjuster(clang_inc_dir24.c_str());
  ArgumentsAdjuster ardj25 = getInsertArgumentAdjuster(clang_inc_dir25.c_str());
  ArgumentsAdjuster ardj26 = getInsertArgumentAdjuster(clang_inc_dir26.c_str());
  ArgumentsAdjuster ardj27 = getInsertArgumentAdjuster(clang_inc_dir27.c_str());
  ArgumentsAdjuster ardj28 = getInsertArgumentAdjuster(clang_inc_dir28.c_str());
  ArgumentsAdjuster ardj29 = getInsertArgumentAdjuster(clang_inc_dir29.c_str());
  ArgumentsAdjuster ardj30 = getInsertArgumentAdjuster(clang_inc_dir30.c_str());
  ArgumentsAdjuster ardj31 = getInsertArgumentAdjuster(clang_inc_dir31.c_str());

  Tool.appendArgumentsAdjuster(ardj1);
  Tool.appendArgumentsAdjuster(ardj2);
  Tool.appendArgumentsAdjuster(ardj3);
  Tool.appendArgumentsAdjuster(ardj4);
  Tool.appendArgumentsAdjuster(ardj5);
  Tool.appendArgumentsAdjuster(ardj6);
  Tool.appendArgumentsAdjuster(ardj7);
  Tool.appendArgumentsAdjuster(ardj8);
  Tool.appendArgumentsAdjuster(ardj9);
  Tool.appendArgumentsAdjuster(ardj10);
  Tool.appendArgumentsAdjuster(ardj11);
  Tool.appendArgumentsAdjuster(ardj12);
  Tool.appendArgumentsAdjuster(ardj13);
  Tool.appendArgumentsAdjuster(ardj14);
  Tool.appendArgumentsAdjuster(ardj15);
  Tool.appendArgumentsAdjuster(ardj16);
  Tool.appendArgumentsAdjuster(ardj17);
  Tool.appendArgumentsAdjuster(ardj18);
  Tool.appendArgumentsAdjuster(ardj19);
  Tool.appendArgumentsAdjuster(ardj20);
  Tool.appendArgumentsAdjuster(ardj21);
  Tool.appendArgumentsAdjuster(ardj22);
  Tool.appendArgumentsAdjuster(ardj23);
  Tool.appendArgumentsAdjuster(ardj24);
  Tool.appendArgumentsAdjuster(ardj25);
  Tool.appendArgumentsAdjuster(ardj26);
  Tool.appendArgumentsAdjuster(ardj27);
  Tool.appendArgumentsAdjuster(ardj28);
  Tool.appendArgumentsAdjuster(ardj29);
  Tool.appendArgumentsAdjuster(ardj30);
  Tool.appendArgumentsAdjuster(ardj31);

if(verbose_compiler){
  ArgumentsAdjuster ardj32 = getInsertArgumentAdjuster("-v");
  Tool.appendArgumentsAdjuster(ardj32);
}

  const auto Action = newFrontendActionFactory<Mutator::Action>();
  return Tool.run(Action.get());
}
