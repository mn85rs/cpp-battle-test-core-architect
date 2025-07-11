#include <IO/Commands/CreateMap.hpp>
#include <IO/Commands/March.hpp>
#include <IO/Commands/SpawnHunter.hpp>
#include <IO/Commands/SpawnSwordsman.hpp>
#include <IO/System/CommandParser.hpp>
#include <IO/System/PrintDebug.hpp>
#include <Scenario/IScenario.hpp>
#include <Scenario/ScenarioLoader.hpp>

namespace sw::sc
{
	namespace
	{
		void executeCreateMapCommand(const io::CreateMap& command, sc::IScenario& scenario)
		{
			scenario.createMap(command.width, command.height);
		}

		template <typename T>
		void executeSpawnUnitCommand(const T& command, sc::IScenario& scenario)
		{
			scenario.spawnUnit(
				sc::Coord<sc::Cells>(command.x, command.y),
				[command = command](sc::IUnitCreationContext& ctx) { return sc::createUnit(command, ctx); });
		}

		void executeMarchUnitCommand(const io::March& command, sc::IScenario& scenario)
		{
			scenario.marchUnit(command.unitId, sc::Coord<sc::Cells>(command.targetX, command.targetY));
		}

		template <typename T>
		void processCommand(
			ScenarioLoader::Commands& commands, T& command, void (*commandFunc)(const T&, sc::IScenario&))
		{
			printDebug(std::cout, command);
			commands.emplace_back([command = command, commandFunc](sc::IScenario& scenario)
								  { commandFunc(command, scenario); });
		}
	}

	ScenarioLoader::Commands ScenarioLoader::loadScenario(std::ifstream& file)
	{
		Commands commands;
		io::CommandParser parser;

		parser
			.add<io::CreateMap>([&commands](auto command)
								{ processCommand(commands, command, executeCreateMapCommand); })
			.add<io::SpawnSwordsman>([&commands](auto command)
									 { processCommand(commands, command, executeSpawnUnitCommand); })
			.add<io::SpawnHunter>([&commands](auto command)
								  { processCommand(commands, command, executeSpawnUnitCommand); })
			.add<io::March>([&commands](auto command) { processCommand(commands, command, executeMarchUnitCommand); });

		parser.parse(file);

		return commands;
	}
}
