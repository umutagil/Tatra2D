#include "AnimationComponent.h"
#include <SDL.h>

AnimationComponent::AnimationComponent(const unsigned numFrames, const int frameRateSpeed, const bool isLoop)
	: numFrames(numFrames)
	, currentFrame(1)
	, frameRateSpeed(frameRateSpeed)
	, isLoop(isLoop)
	, accumulatedTime(0)
{
}
