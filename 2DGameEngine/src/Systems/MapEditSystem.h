#pragma once

#include "../ECS/ECS.h"

class EventBus;
class MouseButtonEvent;
class MouseMotionEvent;
class SceneManager;

class MapEditSystem : public System
{
public:
	MapEditSystem(SceneManager& sceneManager);

	void SubscribeToEvents(EventBus& eventBus);

private:
	void OnMouseMoved(MouseMotionEvent& event);
	void OnMouseButtonClicked(MouseButtonEvent& event);

private:
	SceneManager& sceneManager;
};

