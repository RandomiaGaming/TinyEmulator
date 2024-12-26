#include "EZProfiler.h"
#include <iostream>

EZ::Profiler::Profiler(LONGLONG interval) {
	_interval = interval;
	_frameCount = 0;
	_lastLogTicks = 0;
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&_lastLogTicks));
}
void EZ::Profiler::Tick() {
	_frameCount++;
	if (_frameCount > _interval) {
		LONGLONG _ticksNow;
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&_ticksNow));
		LONGLONG elapsedTicks = _ticksNow - _lastLogTicks;
		LONGLONG TPF = elapsedTicks / _frameCount;
		LONGLONG FPS = (10000000 * _frameCount) / elapsedTicks;
		std::cout << "FPS: " << FPS << " TPF: " << TPF << std::endl;
		_lastLogTicks = _ticksNow;
		_frameCount = 0;
	}
}
EZ::Profiler::~Profiler() {
	_interval = 0;
	_frameCount = 0;
	_lastLogTicks = 0;
}