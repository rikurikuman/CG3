#pragma once
#include <Windows.h>
#include <string>
#include <memory>
#include "Vector2.h"

LRESULT _DefWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

class RWindow
{
private:
	RWindow() {};
	~RWindow() = default;
	RWindow(const RWindow&) {};
	RWindow& operator=(const RWindow&) { return *this; }

	LRESULT(*WindowProc)(HWND, UINT, WPARAM, LPARAM) = _DefWindowProc; //ウィンドウプロシージャへのポインタ

	int32_t windowWidth = 1280; //ウィンドウの幅
	int32_t windowHeight = 720; //ウィンドウの高さ
	std::wstring windowName = L"DirectXGame";

	uint32_t iconID = 0;

	WNDCLASSEX wndClassEx{};
	HWND wndHandle;
	MSG msg{};

	void InitInternal();

public:
	//WindowsAPIの初期化処理
	static void Init();
	static RWindow* GetInstance();

	static void SetWindowSize(const int32_t width, const int32_t height) {
		GetInstance()->windowWidth = width;
		GetInstance()->windowHeight = height;
	}

	static void SetWindowName(const std::wstring name) {
		GetInstance()->windowName = name;
	}

	static void SetIcon(const int32_t id) {
		GetInstance()->iconID = id;
	}

	static int32_t GetWidth() {
		return GetInstance()->windowWidth;
	}

	static int32_t GetHeight() {
		return GetInstance()->windowHeight;
	}

	static WNDCLASSEX GetWindowClassEx() {
		return GetInstance()->wndClassEx;
	}

	static HWND GetWindowHandle() {
		return GetInstance()->wndHandle;
	}

	static MSG GetMessageStructure() {
		return GetInstance()->msg;
	}

	//メッセージ処理
	static void ProcessMessage();

	static Vector2 GetMousePos();

	/// <summary>
	/// マウスカーソル移動
	/// ウィンドウの左上を0, 0として指定する
	/// </summary>
	/// <param name="posX">移動先X座標</param>
	/// <param name="posY">移動先Y座標</param>
	static void SetMousePos(int32_t posX, int32_t posY);

	/// <summary>
	/// マウスカーソル移動
	/// モニターの左上を0, 0として指定する
	/// </summary>
	/// <param name="posX">移動先X座標</param>
	/// <param name="posY">移動先Y座標</param>
	static void SetAbsMousePos(int32_t posX, int32_t posY);

	static void SetMouseHideFlag(bool hide);
};