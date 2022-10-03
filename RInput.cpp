#include "RInput.h"
#include <memory>
#include <cassert>
#include "RWindow.h"

using namespace std;
using namespace Microsoft::WRL;

RInput* RInput::GetInstance() {
	static RInput instance;
	return &instance;
}

void RInput::Init() {
	GetInstance()->InitInternal();
}

void RInput::InitInternal()
{
	HRESULT result;

	result = DirectInput8Create(
		RWindow::GetWindowClassEx().hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&directInput, nullptr
	);
	assert(SUCCEEDED(result));

	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(result));
	result = directInput->CreateDevice(GUID_SysMouse, &mouse, NULL);
	assert(SUCCEEDED(result));

	//���̓f�[�^�`���̃Z�b�g
	result = keyboard->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(result));
	result = mouse->SetDataFormat(&c_dfDIMouse2);
	assert(SUCCEEDED(result));

	//�r�����䃌�x���̃Z�b�g
	result = keyboard->SetCooperativeLevel(
		RWindow::GetWindowHandle(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE // | DISCL_NOWINKEY //������A�A�N�e�B�u�E�B���h�E,��L���Ȃ�,Win�L�[����
	);
	assert(SUCCEEDED(result));
	result = mouse->SetCooperativeLevel(
		RWindow::GetWindowHandle(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE //������A�A�N�e�B�u�E�B���h�E,��L���Ȃ�
	);
	assert(SUCCEEDED(result));
}

void RInput::Update()
{
	RInput* instance = GetInstance();

	//���̎擾�J�n
	instance->keyboard->Acquire();
	instance->mouse->Acquire();

	//�S�L�[�̓��͏�Ԃ��擾����
	for (int i = 0; i < 256; i++) {
		instance->oldKeyState[i] = instance->keyState[i];
	}
	instance->oldMouseState = instance->mouseState;
	instance->keyboard->GetDeviceState(sizeof(keyState), instance->keyState);
	instance->mouse->GetDeviceState(sizeof(mouseState), &instance->mouseState);

	instance->oldMousePos = instance->mousePos;
	instance->mousePos = RWindow::GetInstance()->GetMousePos();
}

bool RInput::GetKey(unsigned char key)
{
	return GetInstance()->keyState[key];
}

bool RInput::GetKeyUp(unsigned char key)
{
	return !GetInstance()->keyState[key] && GetInstance()->oldKeyState[key];
}

bool RInput::GetKeyDown(unsigned char key)
{
	return GetInstance()->keyState[key] && !GetInstance()->oldKeyState[key];
}

bool RInput::GetMouseClick(int buttonNum)
{
	return (GetInstance()->mouseState.rgbButtons[buttonNum] & 0x80) != 0;
}

bool RInput::GetMouseClickUp(int buttonNum)
{
	return (GetInstance()->mouseState.rgbButtons[buttonNum] & 0x80) == 0 && (GetInstance()->oldMouseState.rgbButtons[buttonNum] & 0x80) != 0;
}

bool RInput::GetMouseClickDown(int buttonNum)
{
	return (GetInstance()->mouseState.rgbButtons[buttonNum] & 0x80) != 0 && (GetInstance()->oldMouseState.rgbButtons[buttonNum] & 0x80) == 0;
}

Vector2 RInput::GetMousePos()
{
	return GetInstance()->mousePos;
}

Vector2 RInput::GetOldMousePos()
{
	return GetInstance()->oldMousePos;
}

Vector3 RInput::GetMouseMove() {
	return Vector3((float)GetInstance()->mouseState.lX, (float)GetInstance()->mouseState.lY, (float)GetInstance()->mouseState.lZ);
}