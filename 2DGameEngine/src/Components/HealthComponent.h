#pragma once

struct HealthComponent
{
	HealthComponent(const int health = 100)
		: health(health)
	{
	}

	int health;
};


