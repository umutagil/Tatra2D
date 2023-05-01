#include "BaseController.h"

#include "Game.h"
#include "../ECS/ECS.h"

BaseController::BaseController(Game& game, Registry& registry)
	: game(game)
	, registry(registry)
{
}
