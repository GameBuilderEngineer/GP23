//=======================================================================================
//
//	  [ DXGrph9_simple ] : DirectX Graphics9 一番シンプルなプログラム
//
//=======================================================================================

//-------- インクルードヘッダ
#define STRICT
#include <windows.h>		// Windowsプログラムにはこれを付ける
#include <tchar.h>			// 汎用テキスト マッピング
#include <d3d9.h>			// DirectX Graphics 関連のヘッダー
#include <d3dx9.h>			// DirectX Graphics 関連のヘッダー
#include <mmsystem.h>		// 時間管理で使用
#include <stdlib.h>			// 乱数管理で使用

#define _GLOBAL				// common.h の中で意味を持つ

#include "define.h"
#include "common.h"
#include "proto.h"

#pragma comment(lib, "winmm.lib")

//---------------------------------------------------------------------------------------
//	メイン
//---------------------------------------------------------------------------------------
int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPTSTR lpCmdLine, int iCmdShow)
{
	MSG		msg;
	DWORD	dwExecLastTime;		// FPS計測用
	DWORD	dwFPSLastTime;
	DWORD	dwCurrentTime;
	DWORD	dwFrameCount;

	srand(GetTickCount());

	// ウィンドウクラスの登録
	WNDCLASSEX wc = {
		sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW, WindowProc,
		0, 0, hInstance, LoadIcon(hInstance, IDI_APPLICATION), LoadCursor(NULL, IDC_ARROW),
		(HBRUSH)GetStockObject(WHITE_BRUSH), NULL, CLASS_NAME, NULL};
	if (RegisterClassEx(&wc) == 0) return false;	// ウィンドウクラスを登録

	// ウィンドウを作成
	g_hWnd = CreateWindow(
		CLASS_NAME, CAPTION_NAME, WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		SCREEN_WIDTH  + GetSystemMetrics(SM_CXFIXEDFRAME) * 2,
		SCREEN_HEIGHT + GetSystemMetrics(SM_CYFIXEDFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION),
		NULL, NULL, hInstance, NULL);
	if (g_hWnd == NULL) return false;
	ShowWindow(g_hWnd, iCmdShow);		// ウィンドウを表示
	UpdateWindow(g_hWnd);				// ウィンドウをアップデート

	// グラフィック環境の初期化
	bool	bWindow = false;
	if (IDYES == MessageBox(g_hWnd, _T("ウィンドウモードで実行しますか？"), _T("画面モード"), MB_YESNO))
		bWindow = true;
	if (FAILED(InitializeGraphics(g_hWnd, bWindow))) return false;	// ここで初期化

	// 各種初期化
	timeBeginPeriod(1);									// タイマーの分解能を最小にセット
	dwFPSLastTime = dwExecLastTime = timeGetTime();		// 現在のシステムタイマーを取得
	dwExecLastTime -= FRAME_RATE;
	dwFrameCount = 0;
	g_FPS = 0;
	g_bWire = false;

	// メッセージループ
	msg.message = WM_NULL;
	while (WM_QUIT != msg.message) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {		// メッセージをチェック
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			// この辺でフレーム数カウント
			dwCurrentTime = timeGetTime();					// 現在のタイマ値を取得
			if (dwCurrentTime - dwFPSLastTime >= 500) {		// 0.5 秒ごとに計測
				// フレーム数を計算
				g_FPS = dwFrameCount * 1000 / (dwCurrentTime - dwFPSLastTime);
				dwFPSLastTime = dwCurrentTime;	// タイマ値を更新
				dwFrameCount = 0;				// フレームカウンタをリセット
			}

			// この辺で時間管理
			while (dwCurrentTime - dwExecLastTime >= FRAME_RATE) {	// 一定時間が経過したら…
				dwExecLastTime += FRAME_RATE;						// タイマ値を更新
				GameMain();											// ゲームメイン処理
			}
			RenderGraphics();									// レンダリング処理を実行
			dwFrameCount++;										// フレームカウントを +1
			Sleep(1);				// いったん Windows に制御を戻す
		}
	}

	// グラフィック環境のクリーンアップ
	CleanupGraphics();
	timeEndPeriod(1);				// タイマーの分解能を元に戻す

	return msg.wParam;
}

//---------------------------------------------------------------------------------------
//	ウィンドウプロシージャ
//---------------------------------------------------------------------------------------
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_DESTROY:				//----- ウィンドウ破棄指示がきた
		PostQuitMessage(0);				// システムにスレッドの終了を要求
		return 0;
	case WM_KEYDOWN:				//----- キーボードが押された
		switch (wParam) {
		case VK_ESCAPE:					// [ESC]キーが押された
			DestroyWindow(hWnd);		// ウィンドウを破棄するよう指示する
			return 0;
		case 'W':						// [W]キーが押された
			g_bWire = !g_bWire;			// ワイヤーフレーム モードを反転
			return 0;
		}
		break;
	default:
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//=======================================================================================
//	End of File
//=======================================================================================
