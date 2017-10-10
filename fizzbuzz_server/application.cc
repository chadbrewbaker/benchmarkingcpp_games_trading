#include "application.h"
#include <assert.h>
#include <string>
#include <fstream>

namespace Demo {

namespace {

int ReadInt(const char* input, size_t len)
{
	return std::atoi(input);
}
// template <typename RetType>
// RetType ReadInt(const char* input, size_t len)
// {
// 	RetType n = 0;
// 	while (len--)
// 	{
// 		assert(*input >= '0' && *input <= '9');
// 		n = n * 10 + *input - '0';
// 		input++;
// 	}

// 	return n;
// }

std::string MakeFizzBuzz(uint64_t number)
{
	std::string output;
	if (number % 3 == 0)
	{
		output += "Fizz";
	}
	if (number % 5 == 0)
	{
		output += "Buzz";
	}
	if (output.empty())
	{
		output = std::to_string(number);
	}
	return output;
}

}


Application::~Application()
{
	mTimingLog.DumpLog();
}

void Application::HandleRequest(const char* recvBuf, size_t recvSize, IConnection* conn)
{
	TimeCapture timeCapture(mTimingLog);		// <--- START PROCESSING TS
	uint64_t number = ReadInt(recvBuf, recvSize);
	timeCapture.CaptureFinishParsingTSC();		// <--- FINISH PARSING TS

	std::string output = MakeFizzBuzz(number);
	timeCapture.CaptureFinishProcessingTSC();	// <--- FINISH PROCESSING TS

	conn->Send(output);
	timeCapture.CaptureFinishSendTSC();			// <--- FINISH SENDING TS

	timeCapture.SetRequest(number);
}

void Application::SimulationRun(const char* simulationFile)
{
	std::vector<SimulatedNetworkPacket> packets;
	packets.reserve(1000000);

	std::ifstream infile(simulationFile);
	if (infile)
	{
		std::string number;
		int delayNs = 0;
		while (infile >> number >> delayNs)
		{
			packets.emplace_back(number, delayNs);
		}
		infile.close();
	}

	mSimServer.Run(packets, this);
}

}
