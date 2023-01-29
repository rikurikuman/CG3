#pragma once
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <wrl.h>
#include "Vector3.h"
#include "Vector2.h"
#include <Xinput.h>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "xinput.lib")

class RInput {
public:
	//入力情報管理機能初期化
	static void Init();
	static RInput* GetInstance();

	//入力情報の更新
	static void Update();

	//キーが押されているか
	static bool GetKey(unsigned char key);
	//キーが離れた瞬間か
	static bool GetKeyUp(unsigned char key);
	//キーが押された瞬間か
	static bool GetKeyDown(unsigned char key);

	//マウスの状態を得る
	static inline DIMOUSESTATE2 GetMouseState() {
		return GetInstance()->mouseState;
	}
	//前回のマウスの状態を得る
	static inline DIMOUSESTATE2 GetOldMouseState() {
		return GetInstance()->oldMouseState;
	}

	//マウスクリックされているか
	static bool GetMouseClick(int buttonNum);
	//マウスクリックが離れた瞬間か
	static bool GetMouseClickUp(int buttonNum);
	//マウスクリックがされた瞬間か
	static bool GetMouseClickDown(int buttonNum);

	//マウスの位置を取得する
	static Vector2 GetMousePos();

	//前回のマウスの位置を取得する
	static Vector2 GetOldMousePos();

	//マウスの移動量を取得する
	static Vector3 GetMouseMove();

	//パッドに接続されてるか
	bool GetPadConnect();

	//パッドのボタンが押されているか
	bool GetPadButton(UINT button);

	//パッドのボタンが離れた瞬間か
	bool GetPadButtonUp(UINT button);

	//パッドのボタンが押された瞬間か
	bool GetPadButtonDown(UINT button);

	//パッドの左スティック
	Vector2 GetPadLStick();

	//パッドの右スティック
	Vector2 GetPadRStick();

	//左トリガーを押し込んだ瞬間か
	bool GetLTriggerDown();

	//右トリガーを押し込んだ瞬間か
	bool GetRTriggerDown();

	//左スティックを上に倒した瞬間か
	bool GetLStickUp();

	//左スティックを下に倒した瞬間か
	bool GetLStickDown();

private:
	RInput() {};
	~RInput() = default;
	RInput(const RInput& a) {};
	RInput& operator=(const RInput&) { return *this; }

	Microsoft::WRL::ComPtr<IDirectInput8> directInput = nullptr;
	Microsoft::WRL::ComPtr<IDirectInputDevice8> keyboard = nullptr;
	Microsoft::WRL::ComPtr<IDirectInputDevice8> mouse = nullptr;
	BYTE keyState[256] = {};
	BYTE oldKeyState[256] = {};
	DIMOUSESTATE2 mouseState = {};
	DIMOUSESTATE2 oldMouseState = {};
	Vector2 mousePos;
	Vector2 oldMousePos;
	XINPUT_STATE xInputState;
	XINPUT_STATE oldXInputState;
	bool isConnectPad = false;

	void InitInternal();
};