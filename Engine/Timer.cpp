#include "pch.h"
#include "Timer.h"

void Timer::Init()
{
	::QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&_frequency));
	::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&_prevCount));	//이전 프레임의 CPU 클럭
}

void Timer::Update()
{
	uint64 currentCount;
	::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currentCount));	//현재 프레임의 CPU 클럭

	_deltaTime = (currentCount - _prevCount) / static_cast<float>(_frequency);
	_prevCount = currentCount;


	//업데이트 당 _frameCount를 1씩 갱신
	++_frameCount;
	_frameTime += _deltaTime; //프레임 간격 시간인 _deltaTime을 계속 더해줌.

	if (_frameTime > 1.0f)	//1초를 초과하면
	{
		_fps = static_cast<uint32>(_frameCount / _frameTime);	//1초 동안 실행된 프레임 개수

		_frameTime = 0.0f;
		_frameCount = 0;
	}
}
