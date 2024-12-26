#pragma once
#include <Windows.h>

namespace EZ {
	class Profiler {
	public:
		Profiler(LONGLONG interval = 120);
		void Tick();
		~Profiler();

	private:
		LONGLONG _interval;
		LONGLONG _frameCount;
		LONGLONG _lastLogTicks;
	};
}