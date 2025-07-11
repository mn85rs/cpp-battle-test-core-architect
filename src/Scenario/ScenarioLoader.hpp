#pragma once

#include <functional>
#include <vector>
#include <fstream>

namespace sw::sc
{
	struct IScenario;

	struct ScenarioLoader
	{
		using Command = std::function<void(sc::IScenario&)>;
		using Commands = std::vector<Command>;

		static Commands loadScenario(std::ifstream& file);
	};
}