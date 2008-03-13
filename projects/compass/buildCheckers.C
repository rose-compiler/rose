// This is an automatically generated file
#include <iostream>
#include <rose.h>
#include <vector>
#include "compass.h"
#include "checkers.h"

//AS(1/18/08) Read in the selecting of rules
std::map<std::string, bool > 
readFile( std::string filename){
  std::map<std::string, bool > checkersToUse;
  std::ifstream* file_op = Compass::openFile( filename );

  std::string current_word;
  bool is_first_word = true;
  //read file
  char dummyString[2000];

  while((*file_op) >> current_word){
	//First word denotes what the regular expression should operate
	//upon. Second word denotes the regular expression

	if(  current_word.substr(0,1) == std::string("#") ){
	  //Skip rest of the line if a "#" character is found. This denotes a 
	  //comment
	  file_op->getline(dummyString,2000);

	}else{
	  if(current_word.substr(0,2) == "+:" ){
		checkersToUse[current_word.substr(2)] = true;
	  }else if(current_word.substr(0,2) == "-:" ){
		checkersToUse[current_word.substr(2)] = false;
	  }
	}
  }

  return checkersToUse;
}



void
buildCheckers( std::vector<Compass::TraversalBase*> &retVal, Compass::Parameters &params, Compass::OutputObject &output )
{


    try {
        CompassAnalyses::AllocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstraction::Traversal *traversal;
        traversal = new CompassAnalyses::AllocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstraction::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker AllocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstraction: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::AssignmentOperatorCheckSelf::Traversal *traversal;
        traversal = new CompassAnalyses::AssignmentOperatorCheckSelf::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker AssignmentOperatorCheckSelf: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::AssignmentReturnConstThis::Traversal *traversal;
        traversal = new CompassAnalyses::AssignmentReturnConstThis::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker AssignmentReturnConstThis: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::AsynchronousSignalHandler::Traversal *traversal;
        traversal = new CompassAnalyses::AsynchronousSignalHandler::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker AsynchronousSignalHandler: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::AvoidUsingTheSameHandlerForMultipleSignals::Traversal *traversal;
        traversal = new CompassAnalyses::AvoidUsingTheSameHandlerForMultipleSignals::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker AvoidUsingTheSameHandlerForMultipleSignals: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::BooleanIsHas::Traversal *traversal;
        traversal = new CompassAnalyses::BooleanIsHas::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker BooleanIsHas: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::BufferOverflowFunctions::Traversal *traversal;
        traversal = new CompassAnalyses::BufferOverflowFunctions::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker BufferOverflowFunctions: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::ByteByByteStructureComparison::Traversal *traversal;
        traversal = new CompassAnalyses::ByteByByteStructureComparison::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker ByteByByteStructureComparison: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::CharStarForString::Traversal *traversal;
        traversal = new CompassAnalyses::CharStarForString::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker CharStarForString: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::CommaOperator::Traversal *traversal;
        traversal = new CompassAnalyses::CommaOperator::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker CommaOperator: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::ComputationalFunctions::Traversal *traversal;
        traversal = new CompassAnalyses::ComputationalFunctions::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker ComputationalFunctions: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::ConstCast::Traversal *traversal;
        traversal = new CompassAnalyses::ConstCast::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker ConstCast: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::ConstructorDestructorCallsVirtualFunction::Traversal *traversal;
        traversal = new CompassAnalyses::ConstructorDestructorCallsVirtualFunction::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker ConstructorDestructorCallsVirtualFunction: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::ConstStringLiterals::Traversal *traversal;
        traversal = new CompassAnalyses::ConstStringLiterals::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker ConstStringLiterals: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::ControlVariableTestAgainstFunction::Traversal *traversal;
        traversal = new CompassAnalyses::ControlVariableTestAgainstFunction::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker ControlVariableTestAgainstFunction: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::CopyConstructorConstArg::Traversal *traversal;
        traversal = new CompassAnalyses::CopyConstructorConstArg::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker CopyConstructorConstArg: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::CppCallsSetjmpLongjmp::Traversal *traversal;
        traversal = new CompassAnalyses::CppCallsSetjmpLongjmp::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker CppCallsSetjmpLongjmp: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::CyclomaticComplexity::Traversal *traversal;
        traversal = new CompassAnalyses::CyclomaticComplexity::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker CyclomaticComplexity: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::DataMemberAccess::Traversal *traversal;
        traversal = new CompassAnalyses::DataMemberAccess::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker DataMemberAccess: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::DeepNesting::Traversal *traversal;
        traversal = new CompassAnalyses::DeepNesting::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker DeepNesting: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::DefaultCase::Traversal *traversal;
        traversal = new CompassAnalyses::DefaultCase::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker DefaultCase: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::DefaultConstructor::Traversal *traversal;
        traversal = new CompassAnalyses::DefaultConstructor::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker DefaultConstructor: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::DiscardAssignment::Traversal *traversal;
        traversal = new CompassAnalyses::DiscardAssignment::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker DiscardAssignment: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::DoNotCallPutenvWithAutoVar::Traversal *traversal;
        traversal = new CompassAnalyses::DoNotCallPutenvWithAutoVar::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker DoNotCallPutenvWithAutoVar: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::DoNotDeleteThis::Traversal *traversal;
        traversal = new CompassAnalyses::DoNotDeleteThis::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker DoNotDeleteThis: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::DoNotUseCstyleCasts::Traversal *traversal;
        traversal = new CompassAnalyses::DoNotUseCstyleCasts::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker DoNotUseCstyleCasts: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::DuffsDevice::Traversal *traversal;
        traversal = new CompassAnalyses::DuffsDevice::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker DuffsDevice: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::DynamicCast::Traversal *traversal;
        traversal = new CompassAnalyses::DynamicCast::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker DynamicCast: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::EmptyInsteadOfSize::Traversal *traversal;
        traversal = new CompassAnalyses::EmptyInsteadOfSize::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker EmptyInsteadOfSize: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::EnumDeclarationNamespaceClassScope::Traversal *traversal;
        traversal = new CompassAnalyses::EnumDeclarationNamespaceClassScope::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker EnumDeclarationNamespaceClassScope: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::ExplicitCharSign::Traversal *traversal;
        traversal = new CompassAnalyses::ExplicitCharSign::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker ExplicitCharSign: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::ExplicitCopy::Traversal *traversal;
        traversal = new CompassAnalyses::ExplicitCopy::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker ExplicitCopy: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::ExplicitTestForNonBooleanValue::Traversal *traversal;
        traversal = new CompassAnalyses::ExplicitTestForNonBooleanValue::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker ExplicitTestForNonBooleanValue: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::FloatForLoopCounter::Traversal *traversal;
        traversal = new CompassAnalyses::FloatForLoopCounter::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker FloatForLoopCounter: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::FloatingPointExactComparison::Traversal *traversal;
        traversal = new CompassAnalyses::FloatingPointExactComparison::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker FloatingPointExactComparison: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::FopenFormatParameter::Traversal *traversal;
        traversal = new CompassAnalyses::FopenFormatParameter::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker FopenFormatParameter: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::ForbiddenFunctions::Traversal *traversal;
        traversal = new CompassAnalyses::ForbiddenFunctions::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker ForbiddenFunctions: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::ForLoopConstructionControlStmt::Traversal *traversal;
        traversal = new CompassAnalyses::ForLoopConstructionControlStmt::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker ForLoopConstructionControlStmt: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::ForLoopCppIndexVariableDeclaration::Traversal *traversal;
        traversal = new CompassAnalyses::ForLoopCppIndexVariableDeclaration::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker ForLoopCppIndexVariableDeclaration: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::FriendDeclarationModifier::Traversal *traversal;
        traversal = new CompassAnalyses::FriendDeclarationModifier::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker FriendDeclarationModifier: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::FunctionCallAllocatesMultipleResources::Traversal *traversal;
        traversal = new CompassAnalyses::FunctionCallAllocatesMultipleResources::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker FunctionCallAllocatesMultipleResources: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::FunctionDefinitionPrototype::Traversal *traversal;
        traversal = new CompassAnalyses::FunctionDefinitionPrototype::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker FunctionDefinitionPrototype: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::FunctionDocumentation::Traversal *traversal;
        traversal = new CompassAnalyses::FunctionDocumentation::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker FunctionDocumentation: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::InductionVariableUpdate::Traversal *traversal;
        traversal = new CompassAnalyses::InductionVariableUpdate::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker InductionVariableUpdate: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::InternalDataSharing::Traversal *traversal;
        traversal = new CompassAnalyses::InternalDataSharing::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker InternalDataSharing: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::LocalizedVariables::Traversal *traversal;
        traversal = new CompassAnalyses::LocalizedVariables::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker LocalizedVariables: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::LocPerFunction::Traversal *traversal;
        traversal = new CompassAnalyses::LocPerFunction::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker LocPerFunction: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::LowerRangeLimit::Traversal *traversal;
        traversal = new CompassAnalyses::LowerRangeLimit::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker LowerRangeLimit: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::MagicNumber::Traversal *traversal;
        traversal = new CompassAnalyses::MagicNumber::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker MagicNumber: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::MallocReturnValueUsedInIfStmt::Traversal *traversal;
        traversal = new CompassAnalyses::MallocReturnValueUsedInIfStmt::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker MallocReturnValueUsedInIfStmt: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::MultiplePublicInheritance::Traversal *traversal;
        traversal = new CompassAnalyses::MultiplePublicInheritance::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker MultiplePublicInheritance: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::NameAllParameters::Traversal *traversal;
        traversal = new CompassAnalyses::NameAllParameters::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker NameAllParameters: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::NameConsistency::Traversal *traversal;
        traversal = new CompassAnalyses::NameConsistency::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker NameConsistency: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::NewDelete::Traversal *traversal;
        traversal = new CompassAnalyses::NewDelete::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker NewDelete: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::NoExceptions::Traversal *traversal;
        traversal = new CompassAnalyses::NoExceptions::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker NoExceptions: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::NoExitInMpiCode::Traversal *traversal;
        traversal = new CompassAnalyses::NoExitInMpiCode::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker NoExitInMpiCode: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::NoGoto::Traversal *traversal;
        traversal = new CompassAnalyses::NoGoto::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker NoGoto: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::NonAssociativeRelationalOperators::Traversal *traversal;
        traversal = new CompassAnalyses::NonAssociativeRelationalOperators::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker NonAssociativeRelationalOperators: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::NonmemberFunctionInterfaceNamespace::Traversal *traversal;
        traversal = new CompassAnalyses::NonmemberFunctionInterfaceNamespace::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker NonmemberFunctionInterfaceNamespace: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::NonStandardTypeRefArgs::Traversal *traversal;
        traversal = new CompassAnalyses::NonStandardTypeRefArgs::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker NonStandardTypeRefArgs: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::NonStandardTypeRefReturns::Traversal *traversal;
        traversal = new CompassAnalyses::NonStandardTypeRefReturns::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker NonStandardTypeRefReturns: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::NonVirtualRedefinition::Traversal *traversal;
        traversal = new CompassAnalyses::NonVirtualRedefinition::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker NonVirtualRedefinition: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::NoOverloadAmpersand::Traversal *traversal;
        traversal = new CompassAnalyses::NoOverloadAmpersand::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker NoOverloadAmpersand: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::NoRand::Traversal *traversal;
        traversal = new CompassAnalyses::NoRand::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker NoRand: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::NoSecondTermSideEffects::Traversal *traversal;
        traversal = new CompassAnalyses::NoSecondTermSideEffects::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker NoSecondTermSideEffects: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::NoSideEffectInSizeof::Traversal *traversal;
        traversal = new CompassAnalyses::NoSideEffectInSizeof::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker NoSideEffectInSizeof: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::NoTemplateUsage::Traversal *traversal;
        traversal = new CompassAnalyses::NoTemplateUsage::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker NoTemplateUsage: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::NoVfork::Traversal *traversal;
        traversal = new CompassAnalyses::NoVfork::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker NoVfork: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::NullDeref::Traversal *traversal;
        traversal = new CompassAnalyses::NullDeref::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker NullDeref: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::OneLinePerDeclaration::Traversal *traversal;
        traversal = new CompassAnalyses::OneLinePerDeclaration::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker OneLinePerDeclaration: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::OperatorOverloading::Traversal *traversal;
        traversal = new CompassAnalyses::OperatorOverloading::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker OperatorOverloading: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::OtherArgument::Traversal *traversal;
        traversal = new CompassAnalyses::OtherArgument::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker OtherArgument: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::PlaceConstantOnTheLhs::Traversal *traversal;
        traversal = new CompassAnalyses::PlaceConstantOnTheLhs::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker PlaceConstantOnTheLhs: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::PreferAlgorithms::Traversal *traversal;
        traversal = new CompassAnalyses::PreferAlgorithms::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker PreferAlgorithms: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::PreferFseekToRewind::Traversal *traversal;
        traversal = new CompassAnalyses::PreferFseekToRewind::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker PreferFseekToRewind: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::PreferSetvbufToSetbuf::Traversal *traversal;
        traversal = new CompassAnalyses::PreferSetvbufToSetbuf::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker PreferSetvbufToSetbuf: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::ProtectVirtualMethods::Traversal *traversal;
        traversal = new CompassAnalyses::ProtectVirtualMethods::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker ProtectVirtualMethods: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::PushBack::Traversal *traversal;
        traversal = new CompassAnalyses::PushBack::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker PushBack: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::RightShiftMask::Traversal *traversal;
        traversal = new CompassAnalyses::RightShiftMask::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker RightShiftMask: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::SetPointersToNull::Traversal *traversal;
        traversal = new CompassAnalyses::SetPointersToNull::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker SetPointersToNull: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::SingleParameterConstructorExplicitModifier::Traversal *traversal;
        traversal = new CompassAnalyses::SingleParameterConstructorExplicitModifier::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker SingleParameterConstructorExplicitModifier: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::SizeOfPointer::Traversal *traversal;
        traversal = new CompassAnalyses::SizeOfPointer::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker SizeOfPointer: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::StringTokenToIntegerConverter::Traversal *traversal;
        traversal = new CompassAnalyses::StringTokenToIntegerConverter::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker StringTokenToIntegerConverter: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::SubExpressionEvaluationOrder::Traversal *traversal;
        traversal = new CompassAnalyses::SubExpressionEvaluationOrder::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker SubExpressionEvaluationOrder: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::TernaryOperator::Traversal *traversal;
        traversal = new CompassAnalyses::TernaryOperator::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker TernaryOperator: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::Time_tDirectManipulation::Traversal *traversal;
        traversal = new CompassAnalyses::Time_tDirectManipulation::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker Time_tDirectManipulation: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::TypeTypedef::Traversal *traversal;
        traversal = new CompassAnalyses::TypeTypedef::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker TypeTypedef: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::UnaryMinus::Traversal *traversal;
        traversal = new CompassAnalyses::UnaryMinus::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker UnaryMinus: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::UninitializedDefinition::Traversal *traversal;
        traversal = new CompassAnalyses::UninitializedDefinition::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker UninitializedDefinition: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::UpperRangeLimit::Traversal *traversal;
        traversal = new CompassAnalyses::UpperRangeLimit::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker UpperRangeLimit: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::VariableNameEqualsDatabaseName::Traversal *traversal;
        traversal = new CompassAnalyses::VariableNameEqualsDatabaseName::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker VariableNameEqualsDatabaseName: " << e.what() << std::endl;
    }

    try {
        CompassAnalyses::VoidStar::Traversal *traversal;
        traversal = new CompassAnalyses::VoidStar::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << "error initializing checker VoidStar: " << e.what() << std::endl;
    }


  //AS(1/18/2008) Remove the rules that has been deselected from the retVal
  std::string ruleFile = Compass::parseString(params["Compass.RuleSelection"]);

  std::map<std::string, bool > ruleSelection = readFile(ruleFile);

  std::vector<Compass::TraversalBase*> ruleDeselected;
  for( std::vector<Compass::TraversalBase*>::reverse_iterator iItr = retVal.rbegin();
	  iItr != retVal.rend(); iItr++ )
  {
	std::map<std::string, bool >::iterator isRuleSelected = ruleSelection.find((*iItr)->getName() );   
      
	if( isRuleSelected == ruleSelection.end() ){
         std::cerr << "Error: It has not been selected if " + (*iItr)->getName() + " should be run." 
		           << std::endl;
		 exit(1);
	}
	if( isRuleSelected->second == false  )
	  ruleDeselected.push_back(*iItr);
  }

   for( std::vector<Compass::TraversalBase*>::iterator iItr = ruleDeselected.begin();
	  iItr != ruleDeselected.end(); iItr++ )
  {
	retVal.erase(std::find(retVal.begin(),retVal.end(),*iItr));
  }



  return;
} //buildCheckers()

