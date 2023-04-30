#pragma once

#include "../ECS/ECS.h"

namespace sol
{
	class state;
};

class ScriptSystem : public System
{
public:
	ScriptSystem();

	void CreateLuaBindings(sol::state& lua);

	void Update(const float deltaTime, const double elapsedtime);

};


