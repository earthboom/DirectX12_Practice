#include "pch.h"
#include "Timer.h"

void Timer::Init()
{
	::QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&_frequency));
	::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&_prevCount));	//���� �������� CPU Ŭ��
}

void Timer::Update()
{
	uint64 currentCount;
	::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currentCount));	//���� �������� CPU Ŭ��

	_deltaTime = (currentCount - _prevCount) / static_cast<float>(_frequency);
	_prevCount = currentCount;


	//������Ʈ �� _frameCount�� 1�� ����
	++_frameCount;
	_frameTime += _deltaTime; //������ ���� �ð��� _deltaTime�� ��� ������.

	if (_frameTime > 1.0f)	//1�ʸ� �ʰ��ϸ�
	{
		_fps = static_cast<uint32>(_frameCount / _frameTime);	//1�� ���� ����� ������ ����

		_frameTime = 0.0f;
		_frameCount = 0;
	}
}
