//=======================================================================================
//
//	  DirectX Graphics9 モジュール
//
//=======================================================================================
//-------- インクルードヘッダ
#define STRICT
#include <windows.h>		// Windowsプログラムにはこれを付ける
#include <tchar.h>			// 汎用テキスト マッピング
#include <stdio.h>			// 文字列処理で必要
#include <d3d9.h>			// DirectX Graphics 関連のヘッダ
#include <d3dx9.h>			// DirectX Graphics 関連のヘッダ

#include "define.h"
#include "common.h"
#include "proto.h"

#include "Mesh.h"

//-------- ライブラリのリンク（こう記述しておけば別途リンク設定が不要となる）
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dxguid.lib")

CMesh	g_mesh;

//---------------------------------------------------------------------------------------
// グラフィック環境の初期化
//---------------------------------------------------------------------------------------
HRESULT InitializeGraphics(HWND hWnd, bool bWindow)
{
	// デバイス情報テーブル
	struct TCreateDevice {
		D3DDEVTYPE type;			// デバイスの種類
		DWORD      behavior;		// デバイスの動作
	};
	const int c_nMaxDevice = 3;
	const TCreateDevice device[c_nMaxDevice] = {
	    { D3DDEVTYPE_HAL, D3DCREATE_HARDWARE_VERTEXPROCESSING },	// ハードウェアシェーダーを使う
	    { D3DDEVTYPE_HAL, D3DCREATE_SOFTWARE_VERTEXPROCESSING },	// HAL
	    { D3DDEVTYPE_REF, D3DCREATE_SOFTWARE_VERTEXPROCESSING },	// REF
	};
	int		nDev;			// デバイス種別カウンタ

	// Direct3Dオブジェクトを生成
	if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL) {
		MessageBox(hWnd, _T("Direct3Dオブジェクトの作成に失敗しました"), _T("error"), MB_OK | MB_ICONERROR);
		return E_FAIL;
	}
	// Direct3D 初期化パラメータの設定
	ZeroMemory(&g_D3DPresentParam, sizeof(D3DPRESENT_PARAMETERS));
	g_D3DPresentParam.BackBufferWidth        = SCREEN_WIDTH;			// ゲーム画面サイズ
	g_D3DPresentParam.BackBufferHeight       = SCREEN_HEIGHT;
	g_D3DPresentParam.Windowed               = bWindow;					// ウィンドウモードか?
	g_D3DPresentParam.SwapEffect             = D3DSWAPEFFECT_DISCARD;
	g_D3DPresentParam.EnableAutoDepthStencil = true;					// Ｚバッファ有効
	g_D3DPresentParam.AutoDepthStencilFormat = D3DFMT_D24S8;			// Ｚバッファフォーマット
	if (bWindow) {
		// ウィンドウモード
		g_D3DPresentParam.BackBufferFormat           = D3DFMT_UNKNOWN;	// バックバッファ
		g_D3DPresentParam.FullScreen_RefreshRateInHz = 0;				// リフレッシュレート
		g_D3DPresentParam.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;	// インターバル
	} else {
		// フルスクリーンモード
		g_D3DPresentParam.BackBufferFormat           = D3DFMT_X8R8G8B8;			// バックバッファ
		g_D3DPresentParam.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;	// リフレッシュレート
		g_D3DPresentParam.PresentationInterval       = D3DPRESENT_INTERVAL_DEFAULT;	// インターバル
	}

	// デバイスオブジェクトを作成
	for (nDev = 0; nDev < c_nMaxDevice; nDev++) {
		if(SUCCEEDED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, device[nDev].type, hWnd,	// デバイスを作成
					device[nDev].behavior, &g_D3DPresentParam, &g_pD3DDevice))) break;
	}
	if (nDev >= c_nMaxDevice) {
		MessageBox(hWnd, _T("デバイスの作成に失敗しました"), _T("error"), MB_OK | MB_ICONERROR);
		return E_FAIL;
	}

	// 情報表示用フォントを設定
	D3DXCreateFont(g_pD3DDevice, 18, 0, 0, 0, FALSE, SHIFTJIS_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, _T("Terminal"), &g_pD3DXFont);

	//-------- ここから３Ｄ関連

	// ライティング設定
	D3DLIGHT9 light	= {D3DLIGHT_DIRECTIONAL};
	light.Diffuse	= D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	light.Specular	= D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
	light.Ambient	= D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.0f);
	light.Direction	= D3DXVECTOR3(-5.0f, -1.0f, 1.0f);
	g_pD3DDevice->SetLight(0, &light);
	g_pD3DDevice->LightEnable(0, TRUE);
	g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	g_pD3DDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);
	g_pD3DDevice->SetRenderState(D3DRS_AMBIENT, 0x00808080);

	// プロジェクションマトリックス設定
	D3DXMATRIX	matProj;
	D3DXMatrixPerspectiveFovLH(&matProj, D3DXToRadian(30), ASPECT_RATIO, NEAR_CLIP, FAR_CLIP);
	g_pD3DDevice->SetTransform(D3DTS_PROJECTION, &matProj);

	// αブレンド設定
	g_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	g_pD3DDevice->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
	g_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// テクスチャ ステージ ステートの設定
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE); // 入力1と入力2を掛け合わせる。
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE); // 入力1：テクスチャ
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE); // 入力2：ディフューズ(基本色)
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	g_pD3DDevice->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL);
	g_pD3DDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
	g_pD3DDevice->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_MATERIAL);
	g_pD3DDevice->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL);

	g_pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);

	//-------- ゲーム用オブジェクトの初期化
	InitWave();								// 波の初期化
	g_mesh.Initialize(_T("data/land2.x"));

	return S_OK;
}

//---------------------------------------------------------------------------------------
// グラフィック環境のクリーンアップ
//---------------------------------------------------------------------------------------
HRESULT CleanupGraphics()
{
	//-------- ゲーム用オブジェクトの後始末
	g_mesh.Finalize();
	SAFE_RELEASE(g_pD3DXFont);			// D3DXFont オブジェクトを解放
	SAFE_RELEASE(g_pD3DDevice);			// Direct3DDevice オブジェクトを解放
	SAFE_RELEASE(g_pD3D);				// Direct3D オブジェクトを解放
	return S_OK;
}

//---------------------------------------------------------------------------------------
// グラフィック環境のレンダリング
//---------------------------------------------------------------------------------------
HRESULT RenderGraphics()
{
	// ビューポートをクリア
//	g_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(30, 65, 90), 1.0f, 0);
	g_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,
		D3DCOLOR_XRGB(128, 192, 255), 1.0f, 0);

	if (SUCCEEDED(g_pD3DDevice->BeginScene())) {	// シーン開始

		if (g_bWire)
			g_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);	// ワイヤーフレーム

		DrawMain();									// 描画処理

		g_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);			// 面

		g_pD3DDevice->EndScene();					// シーン終了
	}

	// バックバッファに描画した内容を表示
	if (FAILED(g_pD3DDevice->Present(NULL, NULL, NULL, NULL))) {
		g_pD3DDevice->Reset(&g_D3DPresentParam);
	}
	return S_OK;
}

//=======================================================================================
//	End of File
//=======================================================================================