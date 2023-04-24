#pragma once
#include <mutex>
#include "Core/Windows/KakaWin.h"

namespace Kaka
{
	class Timer
	{
	public:
		Timer();

		void Reset();
		float UpdateDeltaTime();
		float PeekDeltaTime() const;
		float GetTotalTime();
		float GetFPS();

	private:
		LARGE_INTEGER frequency;
		LARGE_INTEGER startTime;
		LARGE_INTEGER lastFrameTime;
		float deltaTime;
		float totalTime = 0.0f;
		float fps = 0.0f;
		std::mutex mutex;
	};
}
