#ifndef LLVM_TRANSFORMS_UTILS_LIQUID_FIXPOINTMANAGER_H
#define LLVM_TRANSFORMS_UTILS_LIQUID_FIXPOINTMANAGER_H

#include <string>
#include <vector>
#include "llvm/Transforms/LiquidFixpointBuilder/ResultType.h"
#include "llvm/ADT/SmallString.h"

namespace liquid {
	class FixpointResponse
	{
	public:
		bool IsSafe;
		std::string OutputStreamText;
		std::string ErrorStreamText;
	};

	class FixpointManager
	{
	private:
		const std::string ExecutablePath;
		static const ResultType executeCommand(const std::string& Executable, const std::vector<std::string>& parameters, int& returnCode, std::string& outputString, std::string& errString);
		static const ResultType writeDataToTempFile(const std::string& data, std::string& fullPath);
		static 	const ResultType readFileData(const llvm::SmallString<1024>& outputStreamFileName, std::string& outputData);

	public:
		FixpointManager(const std::string& executablePath) : ExecutablePath(executablePath) {}
		const ResultType Execute(const std::string& constraints, FixpointResponse& response);
	};
}

#endif
