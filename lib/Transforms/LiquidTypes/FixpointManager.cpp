#include "llvm/Transforms/LiquidTypes/FixpointManager.h"
#include <memory>
//#include <errno.h>
//#include <stdio.h>
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Program.h"
#include "llvm/Support/Process.h"
#include "llvm/Support/MemoryBuffer.h"
//#include "llvm/ADT/SmallVector.h"

using namespace std::literals::string_literals;
using namespace llvm;

#if !defined(_MSC_VER) && !defined(__MINGW32__)
#include <unistd.h>
#else
#include <io.h>
#endif



namespace liquid {

	const ResultType FixpointManager::executeCommand(const std::string& Executable, const std::vector<std::string>& parameters, int& returnCode, std::string& outputString, std::string& errString) {
		
		const char ** argv = new const char* [parameters.size() + 2];
		{
			argv[0] = Executable.c_str();

			for (unsigned int i = 0; i < parameters.size(); i++)
			{
				argv[i + 1] = parameters[i].c_str();
			}

			argv[parameters.size() + 1] = nullptr;
		}

		llvm::SmallString<1024> outputStreamFileName, errStreamFileName;
		{
			std::error_code failedCreate = llvm::sys::fs::createTemporaryFile("Fixpoint_out", "fq", outputStreamFileName);
			if (failedCreate)
			{
				return ResultType::Error("Error occurred creating temporary file - " + failedCreate.message());
			}

			std::error_code failedCreate2 = llvm::sys::fs::createTemporaryFile("Fixpoint_err", "fq", errStreamFileName);
			if (failedCreate2)
			{
				return ResultType::Error("Error occurred creating temporary file - " + failedCreate2.message());
			}
		}

		llvm::sys::ProcessInfo PI1;
		{
			std::string Error;
			bool ExecutionFailed;
			StringRef outputStreamFileName_str = outputStreamFileName.str(), errStreamFileName_str = errStreamFileName.str();
			const llvm::StringRef *Redirects[] = { nullptr, &outputStreamFileName_str, &errStreamFileName_str };

			PI1 = llvm::sys::ExecuteNoWait(Executable, argv, /*env*/ nullptr, Redirects, /* memoryLimit */ 0, &Error, &ExecutionFailed);

			if (ExecutionFailed)
			{
				return ResultType::Error("Error executing program - " + Executable + " - " + Error);
			}
		}

		llvm::sys::ProcessInfo WaitResult;

		{
			std::string Error2;
			WaitResult = llvm::sys::Wait(PI1, 0, true, &Error2);

			if ((WaitResult.ReturnCode == -1 || WaitResult.ReturnCode == -2) && !Error2.empty())
			{
				return ResultType::Error("Error waiting for executing program - " + Executable + " - " + Error2);
			}
		}

		{
			ResultType outRes = readFileData(outputStreamFileName, outputString);
			if (!outRes.Succeeded) { return outRes; }

			ResultType errRes = readFileData(errStreamFileName, errString);
			if (!errRes.Succeeded) { return errRes; }
		}

		returnCode = WaitResult.ReturnCode;
		return ResultType::Success();
	}

	const ResultType FixpointManager::writeDataToTempFile(const std::string& data, std::string& fullPath)
	{
		llvm::SmallString<1024> fileName;
		int fd;

		{
			std::error_code failedCreate = llvm::sys::fs::createTemporaryFile("Fixpoint", "fq", fd, fileName);
			if (failedCreate)
			{
				return ResultType::Error("Error occurred creating temporary file - " + failedCreate.message());
			}
		}

		{
			llvm::raw_fd_ostream Out(fd, /*shouldClose=*/true);
			Out << data;
			Out.close();

			if (Out.has_error()) {
				return ResultType::Error("Error occurred writing to temporary file");
			}
		}

		fullPath = fileName.str().str();
		return ResultType::Success();
	}

	const ResultType FixpointManager::readFileData(const llvm::SmallString<1024>& outputStreamFileName, std::string& outputData)
	{
		outputData = ""s;

		int outputFd;
		std::error_code failedOpen = llvm::sys::fs::openFileForRead(outputStreamFileName, outputFd);
		if (failedOpen)
		{
			return ResultType::Error("Error occurred opening temporary file - " + failedOpen.message());
		}

		std::string targetFileName;
		ErrorOr<std::unique_ptr<MemoryBuffer>> Buffer = MemoryBuffer::getOpenFile(outputFd, targetFileName, -1);
		std::error_code failedRead = Buffer.getError();

		if (failedRead)
		{
			return ResultType::Error("Error occurred reading temporary file - " + failedOpen.message());
		}

		std::error_code failClose = sys::Process::SafelyCloseFileDescriptor(outputFd);

		if (failClose)
		{
			return ResultType::Error("Error closing temporary file - " + failClose.message());
		}

		outputData = ""s + Buffer.get()->getBuffer().str();
		return ResultType::Success();
	}

	const ResultType FixpointManager::Execute(const std::string& constraints, FixpointResponse& response)
	{
		std::string tempFileName;
		auto const writeConstraintsRes = writeDataToTempFile(constraints, tempFileName);
		if (!writeConstraintsRes.Succeeded) { return writeConstraintsRes; }

		auto const commandToRun = ExecutablePath;
		const std::vector<std::string> parameters = {
			tempFileName,
			"--save"s,
			"--eliminate=some"s,
			"--minimalsol"s
		};

		int returnCode;
		std::string outputText, errText;

		auto const executeRes = executeCommand(commandToRun, parameters, returnCode, outputText, errText);
		if (!executeRes.Succeeded) { return executeRes; }

		response.IsSafe = (returnCode == 0);
		response.OutputStreamText = outputText;
		response.ErrorStreamText = errText;
		return ResultType::Success();
	}
}
