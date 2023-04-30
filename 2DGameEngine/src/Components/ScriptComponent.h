#pragma once

#include <sol/sol.hpp>
struct ScriptComponent
{
	ScriptComponent(sol::function func = sol::lua_nil) 
		: func(func)
	{
	}

	sol::function func;
};




