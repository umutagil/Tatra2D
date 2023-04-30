#pragma once

struct AnimationComponent
{
	AnimationComponent(const unsigned numFrames = 0, const int frameRateSpeed = 0, const bool isLoop = true)
		: numFrames(numFrames)
		, currentFrame(1)
		, frameRateSpeed(frameRateSpeed)
		, isLoop(isLoop)
		, accumulatedTime(0)
	{
	}

	unsigned numFrames;
	int frameRateSpeed;
	bool isLoop;
	unsigned currentFrame;
	float accumulatedTime;
};

