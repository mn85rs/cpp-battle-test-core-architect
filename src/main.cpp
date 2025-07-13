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
#include <Scenario/Utils/EventDispatcher.hpp>
#include <filesystem>
#include <fstream>

using namespace sw;

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

		sc::IEventsDispatcherPtr eventsDispatcher = sc::createEventDispatcher();

		EventLog eventLog;
		const sc::IScenarioPtr scenario = sc::createScenario(*eventsDispatcher);

		ed::setEventsHandler(
			*eventsDispatcher, [&eventLog, &scenario](const auto& event) { eventLog.log(scenario->getTick(), event); });

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
