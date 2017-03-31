#include "llvm/Transforms/LiquidTypes/FixpointConstraintBuilder.h"
#include "llvm/Transforms/LiquidTypes/RefinementUtils.h"
#include <regex>
#include <sstream>
namespace liquid {

	void FixpointConstraintBuilder::setFailure(std::string reason)
	{
		if (!failed)
		{
			failed = true;
			failureReason = reason;
		}
	}

	unsigned int FixpointConstraintBuilder::getFreshBinderId()
	{
		auto copy = freshBinderId;
		freshBinderId++;
		return copy;
	}

	unsigned int FixpointConstraintBuilder::getFreshRefinementId()
	{
		auto copy = freshRefinementId;
		freshRefinementId++;
		return copy;
	}

	unsigned int FixpointConstraintBuilder::getFreshConstraintId()
	{
		auto copy = freshConstraintId;
		freshConstraintId++;
		return copy;
	}

	unsigned int FixpointConstraintBuilder::GetFreshNameSuffix()
	{
		auto copy = freshNameState;
		freshNameState++;
		return copy;
	}

	std::vector<unsigned int> FixpointConstraintBuilder::getEnvironmentBinderIds(const std::map<std::string, Binder*>& source, const std::vector<std::string>& names)
	{
		std::vector<unsigned int> environmentBinderReferences;

		for (auto& name : names)
		{
			auto targetBinderTuple = source.find(name);
			if (targetBinderTuple == source.end())
			{
				setFailure("Could not find binder with name : " + name);
			}
			else
			{
				environmentBinderReferences.push_back(targetBinderTuple->second->Id);
			}
		}

		return environmentBinderReferences;
	}

	std::vector<unsigned int> FixpointConstraintBuilder::getEnvironmentBinderIds(const std::map<std::string, std::unique_ptr<Binder>>& source, const std::vector<std::string>& names)
	{
		std::vector<unsigned int> environmentBinderReferences;

		for (auto& name : names)
		{
			auto targetBinderTuple = source.find(name);
			if (targetBinderTuple == source.end())
			{
				setFailure("Could not find binder with name : " + name);
			}
			else
			{
				environmentBinderReferences.push_back(targetBinderTuple->second->Id);
			}
		}

		return environmentBinderReferences;
	}

	bool FixpointConstraintBuilder::isFutureBinderTypeValidIfExists(std::string& name, FixpointBaseType type, std::string& failureString)
	{
		auto futureBinderFound = futureBindersMapping.find(name);
		if (futureBinderFound != futureBindersMapping.end())
		{
			auto& binder = futureBinderFound->second;
			if (binder->Type != type)
			{
				setFailure("Duplicate future binder calls of "
					+ name
					+ " with different types: "
					+ FixpointBaseTypeStrings[binder->Type]
					+ ", "
					+ FixpointBaseTypeStrings[type]
				);

				return false;
			}
		}

		return true;
	}

	bool FixpointConstraintBuilder::DoesBinderExist(std::string name)
	{
		auto binderFound = binderNameMapping.find(name);
		auto found = (binderFound != binderNameMapping.end());
		return found;
	}

	void FixpointConstraintBuilder::AddQualifierIfNew(std::string name, std::vector<FixpointBaseType> paramTypes, std::vector<std::string> paramNames, std::string qualifierString)
	{
		if (paramNames.size() != paramTypes.size())
		{
			setFailure("Qualifier " + name + " does not have equal number of param names and types");
			return;
		}

		auto qualifierObj = std::make_unique<Qualifier>(name, paramTypes, paramNames, qualifierString);

		auto findCondition = [&qualifierObj](std::unique_ptr<Qualifier>& qCurr) {
			return (qCurr->ParamTypes == qualifierObj->ParamTypes) && (qCurr->ParamNames == qCurr->ParamNames) && (qCurr->QualifierString == qualifierObj->QualifierString);
		};

		if (std::find_if(qualifiers.begin(), qualifiers.end(), findCondition) != qualifiers.end())
		{
			//an equivalent qualifier has already been added
			return;
		}

		if (qualifierNameMapping.find(name) != qualifierNameMapping.end())
		{
			setFailure("Multiple qualifiers with name " + name);
			return;
		}

		qualifierNameMapping[qualifierObj->Name] = qualifierObj.get();
		qualifiers.push_back(std::move(qualifierObj));
	}

	void FixpointConstraintBuilder::CreateBinder(std::string name, FixpointBaseType type, std::vector<std::string> environmentBinders, std::vector<std::string> binderInformation)
	{
		auto binderFound = binderNameMapping.find(name);
		if (binderFound != binderNameMapping.end())
		{
			setFailure("Multiple binders with name " + name);
			return;
		}

		//remove any future binders that were expected with this name
		unsigned int binderId;
		std::string failureReason;

		if (!isFutureBinderTypeValidIfExists(name, type, failureReason))
		{
			setFailure(failureReason);
			return;
		}
		else
		{
			auto futureBinderFound = futureBindersMapping.find(name);
			if (futureBinderFound != futureBindersMapping.end())
			{
				binderId = futureBinderFound->second->Id;
				futureBindersMapping.erase(name);
			}
			else
			{
				binderId = getFreshBinderId();
			}
		}

		auto environmentBinderIds = getEnvironmentBinderIds(binderNameMapping, environmentBinders);
		auto environmentBinderInfoIds = getEnvironmentBinderIds(binderInformationNameMapping, binderInformation);
		auto allBinderIds = RefinementUtils::vectorAppend(environmentBinderIds, environmentBinderInfoIds);

		auto refineId = getFreshRefinementId();
		auto wellformednessConstraint = std::make_unique<WellFormednessConstraint>(refineId, type, allBinderIds);
		wellFormednessConstraints.push_back(std::move(wellformednessConstraint));

		std::string freshRefinement = "$k" + std::to_string(refineId);

		std::vector<std::string> binderQualifiers;
		binderQualifiers.push_back(freshRefinement);


		auto binder = std::make_unique<Binder>(binderId, name, type, binderQualifiers);
		binderNameMapping[name] = binder.get();
		binders.push_back(std::move(binder));
	}

	void FixpointConstraintBuilder::CreateBinderWithQualifiers(std::string name, FixpointBaseType type, std::vector<std::string> binderQualifiers)
	{
		if (binderNameMapping.find(name) != binderNameMapping.end())
		{
			setFailure("Multiple binders with name " + name);
			return;
		}

		//remove any future binders that were expected with this name
		unsigned int binderId;
		std::string failureReason;

		if (!isFutureBinderTypeValidIfExists(name, type, failureReason))
		{
			setFailure(failureReason);
			return;
		}
		else
		{
			auto futureBinderFound = futureBindersMapping.find(name);
			if (futureBinderFound != futureBindersMapping.end())
			{
				binderId = futureBinderFound->second->Id;
				futureBindersMapping.erase(name);
			}
			else
			{
				binderId = getFreshBinderId();
			}
		}

		unsigned int i = 0;
		for (auto& qualifier : binderQualifiers)
		{
			std::string fullname = name + "_" + std::to_string(i);
			AddQualifierIfNew(fullname, { type }, { "__value" }, qualifier);
			i++;
		}

		auto binder = std::make_unique<Binder>(binderId, name, type, binderQualifiers);
		binderNameMapping[name] = binder.get();
		binders.push_back(std::move(binder));
	}

	void FixpointConstraintBuilder::CreateFutureBinder(std::string name, FixpointBaseType type)
	{
		std::string failureString;
		if (!isFutureBinderTypeValidIfExists(name, type, failureString))
		{
			setFailure(failureString);
		}
		else
		{
			auto binderId = getFreshBinderId();
			std::vector<std::string> qualifiers;
			auto newBinder = std::make_unique<Binder>(binderId, name, type, qualifiers);
			futureBindersMapping[name] = std::move(newBinder);
		}
	}


	void FixpointConstraintBuilder::AddBinderInformation(std::string name, std::string binderName, std::vector<std::string> binderQualifiers)
	{
		auto binderFound = binderNameMapping.find(binderName);
		if (binderFound == binderNameMapping.end())
		{
			setFailure("Cannot finder binder with name " + binderName);
			return;
		}

		if (binderInformationNameMapping.find(name) != binderInformationNameMapping.end())
		{
			setFailure("Multiple binder infomation records with name " + name);
			return;
		}

		auto binderInfoId = getFreshBinderId();
		auto type = binderFound->second->Type;

		unsigned int i = 0;
		for (auto& qualifier : binderQualifiers)
		{
			std::string fullname = name + "_" + std::to_string(i);
			AddQualifierIfNew(fullname, { type }, { "__value" }, qualifier);
			i++;
		}

		auto binderInfo = std::make_unique<Binder>(binderInfoId, binderName, type, binderQualifiers);
		binderInformationNameMapping[name] = binderInfo.get();
		binderInformation.push_back(std::move(binderInfo));
	}

	void FixpointConstraintBuilder::AddConstraintForAssignment(std::string constraintName, std::string targetName, std::string assignedExpression, std::vector<std::string> environmentBinders, std::vector<std::string> futureBinders, std::vector<std::string> binderInformation)
	{
		auto constraintId = getFreshConstraintId();

		auto targetBinderTuple = binderNameMapping.find(targetName);
		if (targetBinderTuple == binderNameMapping.end())
		{
			setFailure("Target binder " + targetName + "not found");
			return;
		}

		auto targetBinder = targetBinderTuple->second;

		std::vector<std::string> qualifiers;
		qualifiers.push_back(assignedExpression);

		auto environmentBinderIds = getEnvironmentBinderIds(binderNameMapping, environmentBinders);
		auto environmentBinderInfoIds = getEnvironmentBinderIds(binderInformationNameMapping, binderInformation);
		auto futureBindersIds = getEnvironmentBinderIds(futureBindersMapping, futureBinders);

		auto tmp = RefinementUtils::vectorAppend(environmentBinderIds, environmentBinderInfoIds);
		auto allBinderIds = RefinementUtils::vectorAppend(tmp, futureBindersIds);

		auto constraint = std::make_unique<Constraint>(constraintId, constraintName, targetBinder->Type, qualifiers, targetBinder->Qualifiers, allBinderIds);
		constraintNameMapping[constraintName] = constraint.get();
		constraints.push_back(std::move(constraint));
	}

	void FixpointConstraintBuilder::validateNoUninstantiatedFutureBinders()
	{
		if (futureBindersMapping.size() > 0)
		{
			std::string missingBinderList = "";
			for (auto& futureBinder : futureBindersMapping)
			{
				missingBinderList += futureBinder.first + " ";
			}

			setFailure("Missing future binder instantiations : " + missingBinderList);
		}
	}

	bool FixpointConstraintBuilder::ToStringOrFailure(std::string& output)
	{
		validateNoUninstantiatedFutureBinders();

		if (failed)
		{
			output = failureReason;
		}
		else
		{
			std::stringstream outputBuff;
			for (auto& qualifier : qualifiers)
			{
				//rename __value to v as fixpoint chokes on variables with this format in qualifiers
				auto modifiedQualifierString = std::regex_replace(qualifier->QualifierString, std::regex("\\_\\_value"), "v");

				outputBuff << "qualif Q"
					<< qualifier->Name
					<< "(";

				auto size = qualifier->ParamNames.size();
				for (size_t i = 0u; i < size; i++)
				{
					if (i != 0) { outputBuff << ", "; }
					outputBuff << std::regex_replace(qualifier->ParamNames[i], std::regex("\\_\\_value"), "v")
						<< ":" << FixpointBaseTypeStrings[qualifier->ParamTypes[i]];
				}

				outputBuff << "): ("
					<< modifiedQualifierString
					<< ")\n";
			}

			outputBuff << "\n";

			for (auto& binderSourceIndex : { 0,1 })
			{
				auto binderSource = &binders;
				if (binderSourceIndex == 1) { binderSource = &binderInformation; }

				for (auto& binder : *binderSource)
				{
					outputBuff << "bind "
						<< binder->Id
						<< " "
						<< binder->Name
						<< " : { __value : "
						<< FixpointBaseTypeStrings[binder->Type]
						<< " | "
						<< RefinementUtils::stringJoin(" && ", binder->Qualifiers)
						<< " }\n";
				}
			}

			outputBuff << "\n";

			for (auto& constraint : constraints)
			{
				outputBuff << "constraint:\n"
					<< "  env ["
					<< RefinementUtils::stringJoin(";", constraint->BinderReferences)
					<< "]\n"
					<< "  lhs { __value : "
					<< FixpointBaseTypeStrings[constraint->Type]
					<< " | "
					<< RefinementUtils::stringJoin(" && ", constraint->Qualifiers)
					<< " }\n"
					<< "  rhs { __value : "
					<< FixpointBaseTypeStrings[constraint->Type]
					<< " | "
					<< RefinementUtils::stringJoin(" && ", constraint->TargetQualifiers)
					<< " }\n"
					<< "  id "
					<< constraint->Id
					<< " tag []\n\n";
			}

			for (auto& wellFormednessConstraint : wellFormednessConstraints)
			{
				outputBuff << "wf:\n"
					<< "  env ["
					<< RefinementUtils::stringJoin(";", wellFormednessConstraint->BinderReferences)
					<< "]\n"
					<< "  reft { __value : "
					<< FixpointBaseTypeStrings[wellFormednessConstraint->Type]
					<< " | $k"
					<< wellFormednessConstraint->Id
					<< " }\n\n";
			}

			output = outputBuff.str();
		}

		return !failed;
	}

}