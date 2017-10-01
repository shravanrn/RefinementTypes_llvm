#include "llvm/Transforms/LiquidFixpointBuilder/FixpointConstraintBuilder.h"
#include "llvm/Transforms/LiquidFixpointBuilder/RefinementUtils.h"
#include <memory>
#include <regex>
#include <cassert>

using namespace std::literals::string_literals;

namespace liquid {

	Counter* FixpointConstraintBuilder::GetFreshName()
	{
		return &freshNameId;
	}

	ResultType FixpointConstraintBuilder::getEnvironmentBinderIds(
		const std::vector<std::string>& names,
		std::vector<unsigned int>& environmentBinderReferences)
	{
		auto sources = { &binderNameMapping, &futureBindersMapping };
		for (auto& name : names)
		{
			bool found = false;
			for (auto& source : sources)
			{
				auto targetBinderTuple = source->find(name);
				if (targetBinderTuple != source->end())
				{
					environmentBinderReferences.push_back(targetBinderTuple->second->Id);
					found = true;
					break;
				}
			}

			if (!found)
			{
				return ResultType::Error("Could not find binder with name : " + name);
			}
		}

		std::sort(environmentBinderReferences.begin(), environmentBinderReferences.end());
		return ResultType::Success();
	}

	ResultType FixpointConstraintBuilder::isFutureBinderTypeValidIfExists(std::string& name, FixpointType type)
	{
		auto futureBinderFound = futureBindersMapping.find(name);
		if (futureBinderFound != futureBindersMapping.end())
		{
			auto& binder = futureBinderFound->second;
			if (binder->Type != type)
			{
				return ResultType::Error("Duplicate future binder calls of "s
					+ name
					+ " with different types: "s
					+ binder->Type.ToString()
					+ ", "s
					+ type.ToString()
				);
			}
		}

		return ResultType::Success();
	}

	ResultType FixpointConstraintBuilder::AddQualifierIfNew(std::string name, std::vector<FixpointType> paramTypes, std::vector<std::string> paramNames, std::string qualifierString)
	{
		if (paramNames.size() != paramTypes.size())
		{
			return ResultType::Error("Qualifier "s + name + " does not have equal number of param names and types"s);
		}

		if (RefinementUtils::ContainsKey(qualifierNameMapping, name))
		{
			return ResultType::Success();
		}

		qualifierNameMapping[name] = std::make_unique<Qualifier>(name, paramTypes, paramNames, qualifierString);
		qualifierNameOrder.push_back(name);
		return ResultType::Success();
	}

	bool FixpointConstraintBuilder::DoesBinderExist(std::string name)
	{
		auto binderFound = binderNameMapping.find(name);
		auto found = (binderFound != binderNameMapping.end());
		return found;
	}

	std::string FixpointConstraintBuilder::GetBinderAddress(std::string name)
	{
		auto binderFound = binderNameMapping.find(name);
		assert(binderFound != binderNameMapping.end());
		return std::to_string(binderFound->second->Id);
	}

	ResultType FixpointConstraintBuilder::validateBinder(std::string uniqueName, FixpointType type, unsigned int& binderId)
	{
		auto binderFound = binderNameMapping.find(uniqueName);
		if (binderFound != binderNameMapping.end())
		{
			return ResultType::Error("Multiple binders with name " + uniqueName);
		}

		//remove any future binders that were expected with this name
		auto futureBinderTypeRes = isFutureBinderTypeValidIfExists(uniqueName, type);

		if (!futureBinderTypeRes.Succeeded)
		{
			return futureBinderTypeRes;
		}

		auto futureBinderFound = futureBindersMapping.find(uniqueName);
		if (futureBinderFound != futureBindersMapping.end())
		{
			binderId = futureBinderFound->second->Id;
			futureBindersMapping.erase(uniqueName);
		}
		else
		{
			binderId = freshBinderId.GetNextId();
		}

		return ResultType::Success();
	}

	ResultType FixpointConstraintBuilder::CreateBinderWithUnknownType(std::string uniqueName, std::string binderName, FixpointType type, std::vector<std::string> binderNames)
	{
		auto wfcFound = wellFormednessConstraintsMapping.find(uniqueName);
		if (wfcFound != wellFormednessConstraintsMapping.end())
		{
			return ResultType::Error("Multiple constraints with name " + uniqueName);
		}

		unsigned int binderId;
		ResultType validateRes = validateBinder(uniqueName, type, binderId);
		if (!validateRes.Succeeded) { return validateRes; }

		std::vector<unsigned int> allBinderIds;
		auto binderRes = getEnvironmentBinderIds(binderNames, allBinderIds);
		if (!binderRes.Succeeded) { return binderRes; }

		auto refineId = freshRefinementId.GetNextId();
		wellFormednessConstraintsMapping[uniqueName] = std::make_unique<WellFormednessConstraint>(refineId, type, allBinderIds);
		wellFormednessConstraintsOrder.push_back(uniqueName);

		std::string freshRefinement = "$k" + std::to_string(refineId);

		std::vector<std::string> binderQualifiers{ freshRefinement };
		binderNameMapping[uniqueName] = std::make_unique<Binder>(binderId, binderName, type, binderQualifiers);
		binderNameOrder.push_back(uniqueName);
		return ResultType::Success();
	}

	ResultType FixpointConstraintBuilder::CreateBinderWithUnknownType(std::string name, FixpointType type, std::vector<std::string> binderNames)
	{
		return CreateBinderWithUnknownType(name, name, type, binderNames);
	}

	ResultType FixpointConstraintBuilder::CreateBinderWithConstraints(std::string uniqueName, std::string binderName, FixpointType type, std::vector<std::string> binderQualifiers)
	{
		unsigned int binderId;
		ResultType validateRes = validateBinder(uniqueName, type, binderId);
		if (!validateRes.Succeeded) { return validateRes; }

		binderNameMapping[uniqueName] = std::make_unique<Binder>(binderId, binderName, type, binderQualifiers);
		binderNameOrder.push_back(uniqueName);
		return ResultType::Success();
	}

	ResultType FixpointConstraintBuilder::CreateBinderWithConstraints(std::string name, FixpointType type, std::vector<std::string> binderQualifiers)
	{
		return CreateBinderWithConstraints(name, name, type, binderQualifiers);
	}

	ResultType FixpointConstraintBuilder::CreateFutureBinder(std::string name, FixpointType type)
	{
		auto futureBinderTypeRes = isFutureBinderTypeValidIfExists(name, type);

		if (!futureBinderTypeRes.Succeeded)
		{
			return futureBinderTypeRes;
		}

		auto binderId = freshBinderId.GetNextId();
		futureBindersMapping[name] = std::make_unique<Binder>(binderId, name, type, std::vector<std::string>());
		return ResultType::Success();
	}

	ResultType FixpointConstraintBuilder::AddConstraint(std::string constraintName, FixpointType type, std::vector<std::string> constraints, std::string assignedExpression, std::vector<std::string> environmentBinders)
	{
		if (RefinementUtils::ContainsKey(constraintNameMapping, constraintName))
		{
			return ResultType::Error("Constraint "s + constraintName + " already exists"s);
		}

		auto constraintId = freshConstraintId.GetNextId();

		std::vector<unsigned int> allBinderIds;
		auto binderRes = getEnvironmentBinderIds(environmentBinders, allBinderIds);
		if (!binderRes.Succeeded) { return binderRes; }

		std::vector<std::string> assignedExprVec{ assignedExpression };

		constraintNameMapping[constraintName] = std::make_unique<Constraint>(constraintId, constraintName, type, assignedExprVec, constraints, allBinderIds);
		constraintNameOrder.push_back(constraintName);
		return ResultType::Success();
	}

	ResultType FixpointConstraintBuilder::AddConstraintForAssignment(std::string constraintName, std::string targetBinderName, std::string assignedExpression, std::vector<std::string> environmentBinders)
	{
		auto targetBinderTuple = binderNameMapping.find(targetBinderName);
		if (targetBinderTuple == binderNameMapping.end())
		{
			return ResultType::Error("Target binder " + targetBinderName + "not found");
		}

		auto& targetBinder = targetBinderTuple->second;

		return AddConstraint(constraintName, targetBinder->Type, targetBinder->Qualifiers, assignedExpression, environmentBinders);
	}

	ResultType FixpointConstraintBuilder::AddUninterpretedFunctionDefinitionIfNew(std::string functionName, std::vector<std::string> parameterTypes, std::string returnType)
	{
		if (!RefinementUtils::ContainsKey(uninterpretedFunctionsNameMapping, functionName))
		{
			auto functionId = freshUninterpretedFunctionId.GetNextId();
			uninterpretedFunctionsNameMapping[functionName] = std::make_unique<UninterpretedFunction>(functionId, functionName, parameterTypes, returnType);
			uninterpretedFunctionsNameOrder.push_back(functionName);
		}

		return ResultType::Success();
	}

	ResultType FixpointConstraintBuilder::validateNoUninstantiatedFutureBinders()
	{
		if (futureBindersMapping.size() > 0)
		{
			std::string missingBinderList = "";
			for (auto& futureBinder : futureBindersMapping)
			{
				missingBinderList += futureBinder.first + " ";
			}

			return ResultType::Error("Missing future binder instantiations : "s + missingBinderList);
		}

		return ResultType::Success();
	}

	ResultType FixpointConstraintBuilder::ToStringOrFailure(std::string& output)
	{
		auto futBindersRes = validateNoUninstantiatedFutureBinders();

		if (!futBindersRes.Succeeded)
		{
			return futBindersRes;
		}

		std::stringstream outputBuff;
		for (auto& qualifierName : qualifierNameOrder)
		{
			auto& qualifier = qualifierNameMapping[qualifierName];

			//rename __value to v as fixpoint chokes on variables with this format in qualifiers
			auto modifiedQualifierString = std::regex_replace(qualifier->QualifierString, std::regex("\\_\\_value"), "v"s);

			outputBuff << "qualif Q"s
				<< qualifier->Name
				<< "("s;

			auto size = qualifier->ParamNames.size();
			for (size_t i = 0u; i < size; i++)
			{
				if (i != 0) { outputBuff << ", "s; }
				outputBuff << std::regex_replace(qualifier->ParamNames[i], std::regex("\\_\\_value"), "v"s)
					<< ":"s << qualifier->ParamTypes[i].ToString();
			}

			outputBuff << "): ("s
				<< modifiedQualifierString
				<< ")\n"s;
		}

		outputBuff << "\n"s;

		for (auto& uninterpretedFunctionName : uninterpretedFunctionsNameOrder)
		{
			auto& uninterpretedFunction = uninterpretedFunctionsNameMapping[uninterpretedFunctionName];

			outputBuff << "constant "s
				<< uninterpretedFunction->Name
				<< " : func("s
				<< uninterpretedFunction->Id
				<< ", ["s;

			for (auto& uFParam : uninterpretedFunction->ParameterTypes)
			{
				outputBuff << uFParam << "; "s;
			}

			outputBuff << uninterpretedFunction->ReturnType
				<< "])\n"s;
		}

		outputBuff << "\n"s;

		for (auto& binderName : binderNameOrder)
		{
			auto& binder = binderNameMapping[binderName];

			outputBuff << "bind "s
				<< binder->Id
				<< " "s
				<< binder->Name
				<< " : { __value : "s
				<< binder->Type.ToString()
				<< " | "s
				<< RefinementUtils::StringJoin(" && "s, binder->Qualifiers)
				<< " }\n"s;
		}

		outputBuff << "\n"s;

		for (auto& constraintName : constraintNameOrder)
		{
			auto& constraint = constraintNameMapping[constraintName];

			outputBuff << "constraint:\n"s
				<< "  env ["s
				<< RefinementUtils::StringJoin(";"s, constraint->BinderReferences)
				<< "]\n"s
				<< "  lhs { __value : "s
				<< constraint->Type.ToString()
				<< " | "s
				<< RefinementUtils::StringJoin(" && "s, constraint->Qualifiers)
				<< " }\n"s
				<< "  rhs { __value : "s
				<< constraint->Type.ToString()
				<< " | "s
				<< RefinementUtils::StringJoin(" && "s, constraint->TargetQualifiers)
				<< " }\n"s
				<< "  id "s
				<< constraint->Id
				<< " tag []\n"s
				<< "  //"
				<< constraint->Name
				<< "\n\n"s;
		}

		for (auto& wellFormednessConstraintName : wellFormednessConstraintsOrder)
		{
			auto& wellFormednessConstraint = wellFormednessConstraintsMapping[wellFormednessConstraintName];

			outputBuff << "wf:\n"s
				<< "  env ["s
				<< RefinementUtils::StringJoin(";", wellFormednessConstraint->BinderReferences)
				<< "]\n"s
				<< "  reft { __value : "s
				<< wellFormednessConstraint->Type.ToString()
				<< " | $k"s
				<< wellFormednessConstraint->Id
				<< " }\n\n"s;
		}

		output = outputBuff.str();
		return ResultType::Success();
	}
}
