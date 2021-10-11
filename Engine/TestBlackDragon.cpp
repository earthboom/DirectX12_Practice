#include "pch.h"
#include "TestBlackDragon.h"
#include "Input.h"
#include "Animator.h"

void TestBlackDragon::Update()
{
	if (INPUT->GetButton(KEY_TYPE::KEY_1))
	{
		int32 count = GetAnimator()->GetAnimCount();
		int32 currentIndex = GetAnimator()->GetCurrentClipIndex();

		int32 index = (currentIndex + 1) % count;
		GetAnimator()->Play(index);
	}

	if (INPUT->GetButton(KEY_TYPE::KEY_2))
	{
		int32 count = GetAnimator()->GetAnimCount();
		int32 currentIndex = GetAnimator()->GetCurrentClipIndex();

		int32 index = (currentIndex - 1) % count;
		GetAnimator()->Play(index);
	}
}
