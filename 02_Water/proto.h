//=======================================================================================
//
//	  プロトタイプ宣言
//
//=======================================================================================

#ifndef _H_PROTO
#define _H_PROTO

//----- DXGrph9.cpp
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);
HRESULT InitializeGraphics(HWND hWnd, bool bWindow);
HRESULT CleanupGraphics();
HRESULT RenderGraphics();

//----- GameMain.cpp
void GameMain();
void DrawMain();

//----- Wave.cpp
HRESULT InitWave();
void ExecWave();
void DrawWave();

#endif

//=======================================================================================
//	End of File
//=======================================================================================
