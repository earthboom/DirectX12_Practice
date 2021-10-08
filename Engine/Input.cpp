#include "pch.h"
#include "Input.h"
#include "Engine.h"

void Input::Init(HWND hwnd)
{
	_hwnd = hwnd;	//window handle를 받음.
	_states.resize(KEY_TYPE_COUNT, KEY_STATE::NONE);
}

void Input::Update()
{
	HWND hwnd = ::GetActiveWindow();	//현재 활성화된 윈도우 핸들을 받음.

	if (_hwnd != hwnd)	// 초기화 떄 받은 윈도우 핸들과 현재 활성화된 윈도우가 다르면 작동하지 않음.
	{
		for (uint32 key = 0; key < KEY_TYPE_COUNT; ++key)
			_states[key] = KEY_STATE::NONE;

		return;
	}

	
	BYTE asciiKeys[KEY_TYPE_COUNT] = {};
	if (::GetKeyboardState(asciiKeys) == false)	//256개의 각 카의 상태를 한 번에 받아올 수 있음.
		return;

	for (uint32 key = 0; key < KEY_TYPE_COUNT; ++key)
	{
		//키가 눌러있으면 true
		if (asciiKeys[key] & 0x80)
		{
			KEY_STATE& state = _states[key];

			//이전 프레임에서 키를 누른 상태라면 PRESS
			if (state == KEY_STATE::PRESS || state == KEY_STATE::DOWN)
				state = KEY_STATE::PRESS;
			else
				state = KEY_STATE::DOWN;
		}
		else
		{
			KEY_STATE& state = _states[key];

			//이전 프레임에 키를 누른 상태이면 UP
			if (state == KEY_STATE::PRESS || state == KEY_STATE::DOWN)
				state = KEY_STATE::UP;
			else
				state = KEY_STATE::NONE;
		}
	}

	// 현재 창에서 마우스 위치를 얻음.
	::GetCursorPos(&_mousePos);
	::ScreenToClient(GEngine->GetWindow().hwnd, &_mousePos);
}
