#include "AnimationSystem.h"

#include "../Components/SpriteComponent.h"
#include "../Components/AnimationComponent.h"

AnimationSystem::AnimationSystem()
{
	RequireComponent<SpriteComponent>();
	RequireComponent<AnimationComponent>();
}

void AnimationSystem::Update(const float deltaTime)
{
	for (Entity entity : GetSystemEntities()) {
		AnimationComponent& animation = entity.GetComponent<AnimationComponent>();
		SpriteComponent& sprite = entity.GetComponent<SpriteComponent>();

		animation.accumulatedTime += deltaTime;
		const unsigned passedFrameCount = static_cast<unsigned>(animation.accumulatedTime * animation.frameRateSpeed);

		animation.currentFrame = animation.isLoop
							   ? passedFrameCount % animation.numFrames
							   : std::min(passedFrameCount, animation.numFrames);

		sprite.srcRect.x = animation.currentFrame * sprite.width;
	}
}


