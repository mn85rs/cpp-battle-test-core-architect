#include <IO/Events/MapCreated.hpp>
#include <IO/Events/MarchEnded.hpp>
#include <IO/Events/MarchStarted.hpp>
#include <IO/Events/UnitAttacked.hpp>
#include <IO/Events/UnitDied.hpp>
#include <IO/Events/UnitMoved.hpp>
#include <IO/Events/UnitSpawned.hpp>
#include <IO/System/EventLog.hpp>
#include <Scenario/IScenario.hpp>
#include <Scenario/ScenarioLoader.hpp>
#include <filesystem>
#include <fstream>

using namespace sw;

void logEvent(EventLog& eventLog, uint64_t tick, const sc::IScenario::Event& event)
{
	std::visit([&eventLog, &tick](const auto& event) { eventLog.log(tick, event); }, event);
}

int main(int argc, char** argv)
{
	try
	{
		if (argc != 2)
		{
			throw std::runtime_error("Error: No file specified in command line argument");
		}

		std::ifstream file(argv[1]);

		if (!file)
		{
			throw std::runtime_error("Error: File not found - " + std::string(argv[1]));
		}

		EventLog eventLog;
		const sc::IScenarioPtr scenario = sc::createScenario();

		scenario->subscribeEvents([&eventLog](uint64_t tick, const sc::IScenario::Event& event)
								  { logEvent(eventLog, tick, event); });

		std::cout << "Commands:\n";

		auto commands = sc::ScenarioLoader::loadScenario(file);

		std::cout << "\n\nEvents:\n";

		for (const auto command : commands)
		{
			command(*scenario);
		}

		scenario->run();
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	return 0;
}
