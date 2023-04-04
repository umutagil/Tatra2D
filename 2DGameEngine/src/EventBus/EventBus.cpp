#include "EventBus.h"

#include "../Logger/Logger.h"

EventBus::EventBus()
{
	Logger::Log("Event bus constructor called.");
}

EventBus::~EventBus()
{
	Logger::Log("Event bus destructor called.");
}

void EventBus::Reset()
{
	subscribers.clear();
}

