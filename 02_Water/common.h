//=======================================================================================
//
//	  グローバル変数関連
//
//=======================================================================================

#ifndef _H_COMMON
#define _H_COMMON

#ifdef _GLOBAL
#define extern
#endif
extern LPDIRECT3D9					g_pD3D;					// Direct3D8 オブジェクト
extern LPDIRECT3DDEVICE9			g_pD3DDevice;			// Direct3DDevice8 オブジェクト
extern D3DPRESENT_PARAMETERS		g_D3DPresentParam;		// PRESENT PARAMETERS
extern LPD3DXFONT					g_pD3DXFont;			// D3DXFont オブジェクト

extern HWND							g_hWnd;					// ウィンドウハンドル
extern int							g_FPS;					// フレーム数カウント用
extern DWORD						g_dwTicks;				// タイムカウンタ

extern TCHAR						g_szDebug[1024];		// デバッグ用ストリングデータ

extern bool							g_bWire;				// ワイヤーフレーム描画
extern int							g_nStatus;				// フェーズ

extern float						g_fAngle;				// カメラ角度
extern float						g_fPos;					// カメラ高さ

#endif

//=======================================================================================
//	End of File
//=======================================================================================
