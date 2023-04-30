#pragma once

#include "../ECS/ECS.h"

#include <SDL_stdinc.h>

namespace sol
{
	class state;
};

class ScriptSystem : public System
{
public:
	ScriptSystem();

	void CreateLuaBindings(sol::state& lua);

	void Update(const float deltaTime, const uint64_t elapsedtime);

};


