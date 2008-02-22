// This is an automatically generated file
#include <iostream>
#include <rose.h>
#include <vector>
#include "compass.h"
#include "checkers.h"

void
buildCheckers( std::vector<Compass::TraversalBase*> &retVal, Compass::Parameters &params, Compass::OutputObject &output )
{

  retVal.push_back( new CompassAnalyses::AllocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstraction::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::AssignmentOperatorCheckSelf::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::AssignmentReturnConstThis::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::AsynchronousSignalHandler::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::AvoidUsingTheSameHandlerForMultipleSignals::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::BooleanIsHas::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::BufferOverflowFunctions::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::ByteByByteStructureComparison::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::CharStarForString::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::CommaOperator::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::ComputationalFunctions::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::ConstCast::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::ConstructorDestructorCallsVirtualFunction::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::ConstStringLiterals::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::ControlVariableTestAgainstFunction::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::CopyConstructorConstArg::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::CppCallsSetjmpLongjmp::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::CyclomaticComplexity::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::DataMemberAccess::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::DeepNesting::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::DefaultCase::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::DefaultConstructor::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::DiscardAssignment::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::DoNotCallPutenvWithAutoVar::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::DoNotDeleteThis::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::DoNotUseCstyleCasts::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::DuffsDevice::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::DynamicCast::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::EmptyInsteadOfSize::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::EnumDeclarationNamespaceClassScope::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::ExplicitCharSign::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::ExplicitCopy::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::ExplicitTestForNonBooleanValue::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::FloatForLoopCounter::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::FloatingPointExactComparison::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::FopenFormatParameter::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::ForbiddenFunctions::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::ForLoopConstructionControlStmt::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::ForLoopCppIndexVariableDeclaration::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::FriendDeclarationModifier::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::FunctionCallAllocatesMultipleResources::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::FunctionDefinitionPrototype::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::FunctionDocumentation::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::InductionVariableUpdate::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::InternalDataSharing::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::LocalizedVariables::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::LocPerFunction::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::LowerRangeLimit::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::MagicNumber::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::MallocReturnValueUsedInIfStmt::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::MultiplePublicInheritance::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::NameAllParameters::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::NameConsistency::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::NewDelete::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::NoExceptions::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::NoExitInMpiCode::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::NoGoto::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::NonAssociativeRelationalOperators::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::NonmemberFunctionInterfaceNamespace::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::NonStandardTypeRefArgs::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::NonStandardTypeRefReturns::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::NonVirtualRedefinition::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::NoOverloadAmpersand::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::NoRand::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::NoSecondTermSideEffects::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::NoSideEffectInSizeof::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::NoTemplateUsage::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::NoVfork::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::NullDeref::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::OneLinePerDeclaration::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::OperatorOverloading::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::OtherArgument::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::PlaceConstantOnTheLhs::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::PreferAlgorithms::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::PreferFseekToRewind::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::PreferSetvbufToSetbuf::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::ProtectVirtualMethods::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::PushBack::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::RightShiftMask::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::SetPointersToNull::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::SingleParameterConstructorExplicitModifier::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::SizeOfPointer::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::StringTokenToIntegerConverter::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::SubExpressionEvaluationOrder::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::TernaryOperator::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::Time_tDirectManipulation::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::TypeTypedef::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::UnaryMinus::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::UninitializedDefinition::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::UpperRangeLimit::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::VariableNameEqualsDatabaseName::Traversal(params, &output) );

  retVal.push_back( new CompassAnalyses::VoidStar::Traversal(params, &output) );


  return;
} //buildCheckers()

