#pragma once

struct AnimationComponent
{
	AnimationComponent(const unsigned numFrames = 0, const int frameRateSpeed = 0, const bool isLoop = true);

	unsigned numFrames;
	int frameRateSpeed;
	bool isLoop;
	unsigned currentFrame;
	float accumulatedTime;
};

