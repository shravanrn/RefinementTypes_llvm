#ifndef LLVM_TRANSFORMS_UTILS_LIQUID_FIXPOINTCONSTRAINTBUILDER_H
#define LLVM_TRANSFORMS_UTILS_LIQUID_FIXPOINTCONSTRAINTBUILDER_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <sstream>

#include "llvm/Transforms/LiquidTypes/ResultType.h"
#include "llvm/Transforms/LiquidTypes/Counter.h"
#include "llvm/Transforms/LiquidTypes/FixpointType.h"

namespace liquid {

	class Qualifier {
	public:
		const std::string Name;
		std::vector<FixpointType> ParamTypes;
		std::vector<std::string> ParamNames;
		const std::string QualifierString;

		Qualifier(std::string name,
			std::vector<FixpointType> paramTypes,
			std::vector<std::string> paramNames,
			std::string qualifierString) : Name(name), ParamTypes(paramTypes), ParamNames(paramNames), QualifierString(qualifierString) {}
	};

	class Binder {
	public:
		const unsigned int Id;
		const std::string Name;
		const FixpointType Type;
		const std::vector<std::string> Qualifiers;

		Binder(unsigned int id,
			std::string name,
			FixpointType type,
			std::vector<std::string> qualifiers) : Id(id), Name(name), Type(type), Qualifiers(qualifiers) {}
	};

	class Constraint {
	public:
		const unsigned int Id;
		const std::string Name;
		const FixpointType Type;
		const std::vector<std::string> Qualifiers;
		const std::vector<std::string> TargetQualifiers;
		const std::vector<unsigned int> BinderReferences;

		Constraint(unsigned int id,
			std::string name,
			FixpointType type,
			std::vector<std::string> qualifiers,
			std::vector<std::string> targetQualifiers,
			std::vector<unsigned int> binderReferences) : Id(id), Name(name), Type(type), Qualifiers(qualifiers), TargetQualifiers(targetQualifiers), BinderReferences(binderReferences) {}
	};

	class WellFormednessConstraint {
	public:
		const unsigned int Id;
		const FixpointType Type;
		const std::vector<unsigned int> BinderReferences;

		WellFormednessConstraint(unsigned int id,
			FixpointType type,
			std::vector<unsigned int> binderReferences) : Id(id), Type(type), BinderReferences(binderReferences) {}
	};

	class UninterpretedFunction {
	public:
		const unsigned int Id;
		const std::string Name;
		const std::vector<std::string> ParameterTypes;
		const std::string ReturnType;

		UninterpretedFunction(unsigned int id,
			std::string name,
			std::vector<std::string> parameterTypes,
			std::string returnType) : Id(id), Name(name), ParameterTypes(parameterTypes), ReturnType(returnType) {}
	};

	class FixpointConstraintBuilder {
	private:

		Counter freshRefinementId;
		Counter freshBinderId;
		Counter freshConstraintId;
		Counter freshUninterpretedFunctionId;
		Counter freshNameId;

		std::map<std::string, std::unique_ptr<WellFormednessConstraint>> wellFormednessConstraintsMapping;
		std::vector<std::string> wellFormednessConstraintsOrder;

		std::map<std::string, std::unique_ptr<Qualifier>> qualifierNameMapping;
		std::vector<std::string> qualifierNameOrder;

		std::map<std::string, std::unique_ptr<Binder>> binderNameMapping;
		std::vector<std::string> binderNameOrder;

		std::map<std::string, std::unique_ptr<Binder>> futureBindersMapping;

		std::map<std::string, std::unique_ptr<Constraint>> constraintNameMapping;
		std::vector<std::string> constraintNameOrder;

		std::map<std::string, std::unique_ptr<UninterpretedFunction>> uninterpretedFunctionsNameMapping;
		std::vector<std::string> uninterpretedFunctionsNameOrder;

		ResultType getEnvironmentBinderIds(const std::vector<std::string>& names, std::vector<unsigned int>& environmentBinderReferences);
		ResultType isFutureBinderTypeValidIfExists(std::string& name, FixpointType type);
		ResultType validateNoUninstantiatedFutureBinders();
		ResultType validateBinder(std::string uniqueName, FixpointType type, unsigned int& binderId);

	public:
		Counter* GetFreshName();
		ResultType AddQualifierIfNew(std::string name, std::vector<FixpointType> paramTypes, std::vector<std::string> paramNames, std::string qualifierString);
		bool DoesBinderExist(std::string name);
		std::string GetBinderAddress(std::string name);
		ResultType CreateBinderWithUnknownType(std::string uniqueName, std::string binderName, FixpointType type, std::vector<std::string> binderNames);
		ResultType CreateBinderWithUnknownType(std::string name, FixpointType type, std::vector<std::string> binderNames);
		ResultType CreateBinderWithConstraints(std::string uniqueName, std::string binderName, FixpointType type, std::vector<std::string> binderQualifiers);
		ResultType CreateBinderWithConstraints(std::string name, FixpointType type, std::vector<std::string> binderQualifiers);
		ResultType AddConstraint(std::string constraintName, FixpointType type, std::vector<std::string> constraints, std::string assignedExpression, std::vector<std::string> environmentBinders);
		ResultType AddConstraintForAssignment(std::string constraintName, std::string targetBinderName, std::string assignedExpression, std::vector<std::string> environmentBinders);
		ResultType CreateFutureBinder(std::string name, FixpointType type);
		ResultType AddUninterpretedFunctionDefinitionIfNew(std::string functionName, std::vector<std::string> parameterTypes, std::string returnType);
		ResultType ToStringOrFailure(std::string& output);
	};
}

#endif