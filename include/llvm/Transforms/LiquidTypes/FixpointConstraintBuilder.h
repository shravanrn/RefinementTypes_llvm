#ifndef LLVM_TRANSFORMS_UTILS_LIQUID_FIXPOINT_H
#define LLVM_TRANSFORMS_UTILS_LIQUID_FIXPOINT_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <sstream>

namespace liquid {

	enum FixpointBaseType {
		INT, BOOL
	};
	static const char* FixpointBaseTypeStrings[] = { "int", "bool" };

	class Qualifier {
	public:
		const std::string Name;
		std::vector<FixpointBaseType> ParamTypes;
		std::vector<std::string> ParamNames;
		const std::string QualifierString;

		Qualifier(std::string name,
			std::vector<FixpointBaseType> paramTypes,
			std::vector<std::string> paramNames,
			std::string qualifierString) : Name(name), ParamTypes(paramTypes), ParamNames(paramNames), QualifierString(qualifierString) {}
	};

	class Binder {
	public:
		const unsigned int Id;
		const std::string Name;
		const FixpointBaseType Type;
		const std::vector<std::string> Qualifiers;

		Binder(unsigned int id,
			std::string name,
			FixpointBaseType type,
			std::vector<std::string> qualifiers) : Id(id), Name(name), Type(type), Qualifiers(qualifiers) {}
	};

	class Constraint {
	public:
		const unsigned int Id;
		const std::string Name;
		const FixpointBaseType Type;
		const std::vector<std::string> Qualifiers;
		const std::vector<std::string> TargetQualifiers;
		const std::vector<unsigned int> BinderReferences;

		Constraint(unsigned int id,
			std::string name,
			FixpointBaseType type,
			std::vector<std::string> qualifiers,
			std::vector<std::string> targetQualifiers,
			std::vector<unsigned int> binderReferences) : Id(id), Name(name), Type(type), Qualifiers(qualifiers), TargetQualifiers(targetQualifiers), BinderReferences(binderReferences) {}
	};

	class WellFormednessConstraint {
	public:
		const unsigned int Id;
		const FixpointBaseType Type;
		const std::vector<unsigned int> BinderReferences;

		WellFormednessConstraint(unsigned int id,
			FixpointBaseType type,
			std::vector<unsigned int> binderReferences) : Id(id), Type(type), BinderReferences(binderReferences) {}
	};

	class FixpointConstraintBuilder {
	private:

		bool failed = false;
		std::string failureReason;
		void setFailure(std::string reason);

		std::vector<std::unique_ptr<WellFormednessConstraint>> wellFormednessConstraints;

		std::map<std::string, Qualifier*> qualifierNameMapping;
		std::vector<std::unique_ptr<Qualifier>> qualifiers;

		unsigned int freshRefinementId = 0;
		unsigned int getFreshRefinementId();

		unsigned int freshBinderId = 1;
		unsigned int getFreshBinderId();
		std::map<std::string, Binder*> binderNameMapping;
		std::vector<std::unique_ptr<Binder>> binders;
		//any context specific binders
		//for example
		//
		//bool a; 
		//if (a)
		//{
		//	//here we have context information about the binder a stating "a is true"
		//}
		std::map<std::string, Binder*> binderInformationNameMapping;
		std::vector<std::unique_ptr<Binder>> binderInformation;
		std::vector<unsigned int> getEnvironmentBinderIds(const std::map<std::string, Binder*>& source, const std::vector<std::string>& names);
		std::vector<unsigned int> getEnvironmentBinderIds(const std::map<std::string, std::unique_ptr<Binder>>& source, const std::vector<std::string>& names);

		std::map<std::string, std::unique_ptr<Binder>> futureBindersMapping;
		bool isFutureBinderTypeValidIfExists(std::string& name, FixpointBaseType type, std::string& failureString);
		void validateNoUninstantiatedFutureBinders();

		unsigned int freshConstraintId = 0;
		unsigned int getFreshConstraintId();
		std::map<std::string, Constraint*> constraintNameMapping;
		std::vector<std::unique_ptr<Constraint>> constraints;

		//used by the user of this class to create a unique name at any time
		unsigned int freshNameState = 0;

	public:
		unsigned int GetFreshNameSuffix();
		void AddQualifierIfNew(std::string name, std::vector<FixpointBaseType> paramTypes, std::vector<std::string> paramNames, std::string qualifierString);
		bool DoesBinderExist(std::string name);
		void CreateBinder(std::string name, FixpointBaseType type, std::vector<std::string> environmentBinders, std::vector<std::string> binderInformation);
		void CreateBinderWithQualifiers(std::string name, FixpointBaseType type, std::vector<std::string> binderQualifiers);
		//Create a binder `name` while checking that this binder is created normally by a subsequent call to CreateBinder or CreateBinderWithQualifiers
		void CreateFutureBinder(std::string name, FixpointBaseType type);
		void AddBinderInformation(std::string name, std::string binderName, std::vector<std::string> binderQualifiers);
		void AddConstraintForAssignment(std::string constraintName, std::string targetName, std::string assignedExpression, std::vector<std::string> environmentBinders, std::vector<std::string> futureBinders, std::vector<std::string> binderInformation);

		bool ToStringOrFailure(std::string& output);
	};
}

#endif