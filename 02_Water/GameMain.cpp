//=======================================================================================
//
//	  ゲームメイン、描画モジュール
//
//=======================================================================================
//-------- インクルードヘッダ
#define STRICT
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS
#include <windows.h>		// Windowsプログラムにはこれを付ける
#include <tchar.h>			// 汎用テキスト マッピング
#include <stdio.h>			// 文字列処理で必要
#include <d3d9.h>			// DirectX Graphics 関連のヘッダー
#include <d3dx9.h>			// DirectX Graphics 関連のヘッダー

#include "define.h"
#include "common.h"
#include "proto.h"

#include "Mesh.h"

extern CMesh	g_mesh;

//---------------------------------------------------------------------------------------
// ゲームメイン処理（メインループからコールされる）
//---------------------------------------------------------------------------------------
void GameMain()
{
	TCHAR	str[256];

	g_szDebug[0] = _T('\0');		// デバッグストリング初期化

	// ＦＰＳを画面に描画するための文字列を作成
	_stprintf(str, _T("FPS = %d\n"), g_FPS);
	lstrcat(g_szDebug, str);

	//----- ここにゲーム本体処理
	switch (g_nStatus) {
	case 0:
		g_fAngle = D3DXToRadian(45.0f);
		g_fPos   = 7.0f;
		g_dwTicks = 0;

		g_nStatus = 1;
		// THRU
	case 1:
		g_dwTicks++;
		if (GetAsyncKeyState(VK_LEFT ) & 0x8000) g_fAngle += D3DXToRadian(1);
		if (GetAsyncKeyState(VK_RIGHT) & 0x8000) g_fAngle -= D3DXToRadian(1);
		if (GetAsyncKeyState(VK_UP   ) & 0x8000) g_fPos   += 1.0f;
		if (GetAsyncKeyState(VK_DOWN ) & 0x8000) g_fPos   -= 1.0f;

		ExecWave();						// 波の更新
		break;
	}
	_stprintf(str, _T("PosY  = %.0f\nAngle = %d\n"), g_fPos, (int)D3DXToDegree(g_fAngle));
	lstrcat(g_szDebug, str);
}

//---------------------------------------------------------------------------------------
// 描画処理（RenderGraphics からコールされる）
//---------------------------------------------------------------------------------------
void DrawMain()
{
	//----- ここに描画処理
	// ビューマトリックス設定
	D3DXMATRIX	matView, matRotate;
	D3DXMatrixRotationY(&matRotate, g_fAngle);
	D3DXVECTOR3 vecView(0.0f, g_fPos, -70.0f);
	D3DXVec3TransformCoord(&vecView, &vecView, &matRotate);
	D3DXMatrixLookAtLH(&matView,
		&vecView,								// 視点座標
		&D3DXVECTOR3( 0.0f,  0.0f,  0.0f),		// 注視点座標
		&D3DXVECTOR3( 0.0f,  1.0f,  0.0f));		// アップベクトル
	g_pD3DDevice->SetTransform(D3DTS_VIEW, &matView);

	D3DXMATRIX matrix;
	D3DXMatrixTranslation(&matrix, 0.0f, -2.0f, 0.0f);
	g_mesh.Draw(matrix);

	DrawWave();							// 波の描画

	// デバッグストリング描画
	RECT	rcStr = {0, 0, 640, 480};
	g_pD3DXFont->DrawText(NULL, g_szDebug, -1, &rcStr, DT_LEFT, D3DCOLOR_ARGB(255, 255, 255, 255));
}

//=======================================================================================
//	End of File
//=======================================================================================
