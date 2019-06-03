//=============================================================================
//
// メイン処理 [main.cpp]
// Author :
//
//=============================================================================
#include "main.h"
#include "model.h"
#include "input.h"
#include "field.h"
#include "shadow.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define CLASS_NAME		"AppClass"			// ウインドウのクラス名
#define WINDOW_NAME		"学籍番号　クラス　出席番号　氏名"		// ウインドウのキャプション名

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void Uninit(void);
void Update(void);
void Draw(void);
#ifdef _DEBUG
void DrawFPS(void);
#endif

//*****************************************************************************
//レンダリングテクスチャ関連
//*****************************************************************************
//プロトタイプ宣言はmain.hへ記載


//*****************************************************************************
// グローバル変数:
//*****************************************************************************
LPDIRECT3D9			g_pD3D = NULL;			// Direct3D オブジェクト
LPDIRECT3DDEVICE9	g_pD3DDevice = NULL;	// Deviceオブジェクト(描画に必要)
#ifdef _DEBUG
LPD3DXFONT			g_pD3DXFont = NULL;		// フォントへのポインタ
int					g_nCountFPS;			// FPSカウンタ
#endif

//****************************************************************************
//レンダリングテクスチャ用インターフェース等
//****************************************************************************

LPDIRECT3DSURFACE9		pRenderDepthBuffer;
LPDIRECT3DTEXTURE9		pRenderTexture;
D3DVIEWPORT9			RenderViewport;

LPDIRECT3DSURFACE9		pDefaultRenderTargetSurface;
LPDIRECT3DSURFACE9		pDefaultDepthBufferSurface;
LPDIRECT3DSURFACE9		pRenderTextureSurface;
D3DVIEWPORT9	DefaultViewport;

//背景ポリゴン
HRESULT InitBackGroundPolygon(void);
void UninitBackGroundPolygon(void);
void DrawBackGroundPolygon();
LPDIRECT3DTEXTURE9		g_pD3DBackGround = NULL;	// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pD3DVtxBuffBackGround = NULL;	// 頂点バッファへのポインタ





//=============================================================================
// メイン関数
//=============================================================================
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);	// 無くても良いけど、警告が出る（未使用宣言）
	UNREFERENCED_PARAMETER(lpCmdLine);		// 無くても良いけど、警告が出る（未使用宣言）

	DWORD dwExecLastTime;
	DWORD dwFPSLastTime;
	DWORD dwCurrentTime;
	DWORD dwFrameCount;

	WNDCLASSEX wcex =
	{
		sizeof(WNDCLASSEX),
		CS_CLASSDC,
		WndProc,
		0,
		0,
		hInstance,
		NULL,
		LoadCursor(NULL, IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		NULL,
		CLASS_NAME,
		NULL
	};
	HWND hWnd;
	MSG msg;
	
	// ウィンドウクラスの登録
	RegisterClassEx(&wcex);

	// ウィンドウの作成
	hWnd = CreateWindowEx(0,
						CLASS_NAME,
						WINDOW_NAME,
						WS_OVERLAPPEDWINDOW,
						CW_USEDEFAULT,
						CW_USEDEFAULT,
						SCREEN_WIDTH + GetSystemMetrics(SM_CXDLGFRAME) * 2,
						SCREEN_HEIGHT + GetSystemMetrics(SM_CXDLGFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION),
						NULL,
						NULL,
						hInstance,
						NULL);

	// 初期化処理(ウィンドウを作成してから行う)
	if(FAILED(Init(hInstance, hWnd, TRUE)))
	{
		return -1;
	}

	//フレームカウント初期化
	timeBeginPeriod(1);				// 分解能を設定
	dwExecLastTime = 
	dwFPSLastTime = timeGetTime();
	dwCurrentTime =
	dwFrameCount = 0;

	// ウインドウの表示(初期化処理の後に呼ばないと駄目)
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	
	// メッセージループ
	while(1)
	{
        if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
			{// PostQuitMessage()が呼ばれたらループ終了
				break;
			}
			else
			{
				// メッセージの翻訳とディスパッチ
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
        }
		else
		{
			dwCurrentTime = timeGetTime();
			if((dwCurrentTime - dwFPSLastTime) >= 500)	// 0.5秒ごとに実行
			{
#ifdef _DEBUG
				g_nCountFPS = dwFrameCount * 1000 / (dwCurrentTime - dwFPSLastTime);
#endif
				dwFPSLastTime = dwCurrentTime;
				dwFrameCount = 0;
			}

			if((dwCurrentTime - dwExecLastTime) >= (1000 / 60))
			{
				dwExecLastTime = dwCurrentTime;

				// 更新処理
				Update();

				// 描画処理
				Draw();

				dwFrameCount++;
			}
		}
	}
	
	// ウィンドウクラスの登録を解除
	UnregisterClass(CLASS_NAME, wcex.hInstance);

	// 終了処理
	Uninit();

	timeEndPeriod(1);				// 分解能を戻す

	return (int)msg.wParam;
}

//=============================================================================
// プロシージャ
//=============================================================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		switch(wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(hWnd);
			break;
		}
		break;

	default:
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	D3DPRESENT_PARAMETERS d3dpp;
    D3DDISPLAYMODE d3ddm;

	// Direct3Dオブジェクトの作成
	g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if(g_pD3D == NULL)
	{
		return E_FAIL;
	}

	// 現在のディスプレイモードを取得
    if(FAILED(g_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
	{
		return E_FAIL;
	}

	// デバイスのプレゼンテーションパラメータの設定
	ZeroMemory(&d3dpp, sizeof(d3dpp));							// ワークをゼロクリア
	d3dpp.BackBufferCount			= 1;						// バックバッファの数
	d3dpp.BackBufferWidth			= SCREEN_WIDTH;				// ゲーム画面サイズ(幅)
	d3dpp.BackBufferHeight			= SCREEN_HEIGHT;			// ゲーム画面サイズ(高さ)
	d3dpp.BackBufferFormat			= d3ddm.Format;				// バックバッファフォーマットはディスプレイモードに合わせて使う
	d3dpp.SwapEffect				= D3DSWAPEFFECT_DISCARD;	// 映像信号に同期してフリップする
	d3dpp.Windowed					= bWindow;					// ウィンドウモード
	d3dpp.EnableAutoDepthStencil	= TRUE;						// デプスバッファ（Ｚバッファ）とステンシルバッファを作成
	d3dpp.AutoDepthStencilFormat	= D3DFMT_D24S8;				// デプスバッファとして16bitを使う

	if(bWindow)
	{// ウィンドウモード
		d3dpp.FullScreen_RefreshRateInHz = 0;								// リフレッシュレート
		d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;	// インターバル
	}
	else
	{// フルスクリーンモード
		d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;			// リフレッシュレート
		d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_DEFAULT;		// インターバル
	}

	// デバイスオブジェクトの生成
	// [デバイス作成制御]<描画>と<頂点処理>をハードウェアで行なう
	if(FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, 
									D3DDEVTYPE_HAL, 
									hWnd, 
									D3DCREATE_HARDWARE_VERTEXPROCESSING, 
									&d3dpp, &g_pD3DDevice)))
	{
		// 上記の設定が失敗したら
		// [デバイス作成制御]<描画>をハードウェアで行い、<頂点処理>はCPUで行なう
		if(FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, 
										D3DDEVTYPE_HAL, 
										hWnd, 
										D3DCREATE_SOFTWARE_VERTEXPROCESSING, 
										&d3dpp, &g_pD3DDevice)))
		{
			// 上記の設定が失敗したら
			// [デバイス作成制御]<描画>と<頂点処理>をCPUで行なう
			if(FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, 
											D3DDEVTYPE_REF,
											hWnd, 
											D3DCREATE_SOFTWARE_VERTEXPROCESSING, 
											&d3dpp, &g_pD3DDevice)))
			{
				// 初期化失敗
				return E_FAIL;
			}
		}
	}

	// レンダーステートパラメータの設定
    g_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);				// 裏面をカリング
	g_pD3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);						// Zバッファを使用
	g_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);				// αブレンドを行う
	g_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);		// αソースカラーの指定
	g_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);	// αデスティネーションカラーの指定

	// サンプラーステートパラメータの設定
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);	// テクスチャアドレッシング方法(U値)を設定
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);	// テクスチャアドレッシング方法(V値)を設定
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);	// テクスチャ縮小フィルタモードを設定
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);	// テクスチャ拡大フィルタモードを設定

	// テクスチャステージステートの設定
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);	// アルファブレンディング処理
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);	// 最初のアルファ引数
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);	// ２番目のアルファ引数

#ifdef _DEBUG
	// 情報表示用フォントを設定
	D3DXCreateFont(g_pD3DDevice, 18, 0, 0, 0, FALSE, SHIFTJIS_CHARSET,
					OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Terminal", &g_pD3DXFont);
#endif

	// 入力処理の初期化
	InitInput(hInstance, hWnd);

	//
	InitField();
	InitWall();

	// モデルの初期化
	InitModel();


	//レンダリングテクスチャ関連
	InitBackGroundPolygon();
	InitRendertexture();




	return S_OK;
}


//=============================================================================
// レンダリングテクスチャ作成処理
void InitRendertexture()
{

	//レンダリングテクスチャ用ビューポート
	RenderViewport.X = 0;
	RenderViewport.Y = 0;
	RenderViewport.Width = 800;
	RenderViewport.Height = 600;
	RenderViewport.MinZ = 0.0f;
	RenderViewport.MaxZ = 1.0f;


	//デプスバッファ作成(2のべき乗サイズ)
	g_pD3DDevice->CreateDepthStencilSurface(1024, 1024,		//サイズ
											D3DFMT_D24S8,	//ピクセルフォーマット
											D3DMULTISAMPLE_NONE,//marutisanpuringu OFF
											0,
											FALSE,
											&pRenderDepthBuffer,
											NULL);

	//レンダーターゲット作成(2のべき乗サイズ)
	D3DXCreateTexture(g_pD3DDevice,
						1024,1024,		//サイズ
						1,				//みっぷマップ数
						D3DUSAGE_RENDERTARGET,	//siyou目的
						D3DFMT_X8R8G8B8,
						D3DPOOL_DEFAULT,
						&pRenderTexture);


}



//=============================================================================
// レンダーターゲット切り替え処理
void ChangeRenderTarget()
{

		//レンダーターゲットを切り替え

		//デフォルトビューポート保存
		g_pD3DDevice->GetViewport( &DefaultViewport );

		//デフォルトのサーフェースを保存
		g_pD3DDevice->GetRenderTarget(0, &pDefaultRenderTargetSurface );
		g_pD3DDevice->GetDepthStencilSurface( &pDefaultDepthBufferSurface );
		//レンダリングテクスチャのサーフェース取得
		pRenderTexture->GetSurfaceLevel( 0, &pRenderTextureSurface );

		//新しいレンダリングターゲットをセット
		g_pD3DDevice->SetRenderTarget( 0, pRenderTextureSurface );
		//新しいデプスバッファをセット
		g_pD3DDevice->SetDepthStencilSurface( pRenderDepthBuffer );

		//ビューポートの切り替え
		g_pD3DDevice->SetViewport( &RenderViewport );

		//クリア
		g_pD3DDevice->Clear(0, NULL,  D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_XRGB(130, 0, 0), 1.0f, 0);

}

//=============================================================================
// レンダーターゲット復帰処理
void RecoverRenderTarget()
{

		//レンダーターゲットの復帰
		g_pD3DDevice->SetRenderTarget( 0, pDefaultRenderTargetSurface );	
		g_pD3DDevice->SetDepthStencilSurface( pDefaultDepthBufferSurface );	

		//サーフェースの解放
		pDefaultRenderTargetSurface->Release();
		pDefaultDepthBufferSurface->Release();
		pRenderTextureSurface->Release();

		//ビューポートの復帰
		g_pD3DDevice->SetViewport( &DefaultViewport );


}

//=============================================================================
// 終了処理
//=============================================================================
void Uninit(void)
{
#ifdef _DEBUG
	if(g_pD3DXFont != NULL)
	{// 情報表示用フォントの開放
		g_pD3DXFont->Release();
		g_pD3DXFont = NULL;
	}
#endif
	if(g_pD3DDevice != NULL)
	{// デバイスの開放
		g_pD3DDevice->Release();
		g_pD3DDevice = NULL;
	}

	if(g_pD3D != NULL)
	{// Direct3Dオブジェクトの開放
		g_pD3D->Release();
		g_pD3D = NULL;
	}

	// 入力処理の終了処理
	UninitInput();

	//
	UninitField();
	UninitWall();

	//
	UninitBackGroundPolygon();

	// モデルの終了処理
	UninitModel();
}

//=============================================================================
// 更新処理
//=============================================================================
void Update(void)
{
	// 入力更新
	UpdateInput();

	// モデルの更新処理
	UpdateModel();

	//
	UpdateField();
	UpdateWall();

	

}

//=============================================================================
// 描画処理
//=============================================================================
void Draw(void)
{
	// バックバッファ＆Ｚバッファのクリア
	g_pD3DDevice->Clear(0, NULL, (D3DCLEAR_ZBUFFER), D3DCOLOR_RGBA(0, 0, 0, 0), 1.0f, 0);

	// Direct3Dによる描画の開始
	if(SUCCEEDED(g_pD3DDevice->BeginScene()))
	{

		//レンダリングターゲットの切り替え
		ChangeRenderTarget();

		// モデルの描画処理
		DrawModel();

		//
		DrawField();
		DrawWall();

		#ifdef _DEBUG
			// FPS表示
			DrawFPS();
		#endif


		//レンダリングターゲットの復帰
		RecoverRenderTarget();

		//レンダリングした背景を表示
		DrawBackGroundPolygon();

	
		// Direct3Dによる描画の終了
		g_pD3DDevice->EndScene();
	}

	// バックバッファとフロントバッファの入れ替え
	g_pD3DDevice->Present(NULL, NULL, NULL, NULL);
}


//=============================================================================
// レンダリングテクスチャ背景描画
//=============================================================================
HRESULT	InitBackGroundPolygon()
{

	// オブジェクトの頂点バッファを生成
    g_pD3DDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * NUM_VERTEX,	// 頂点データ用に確保するバッファサイズ(バイト単位)
												D3DUSAGE_WRITEONLY,			// 頂点バッファの使用法　
												FVF_VERTEX_2D,				// 使用する頂点フォーマット
												D3DPOOL_MANAGED,			// リソースのバッファを保持するメモリクラスを指定
												&g_pD3DVtxBuffBackGround,		// 頂点バッファインターフェースへのポインタ
												NULL);						// NULLに設定

	{//頂点バッファの中身を埋める
		VERTEX_2D *pVtx;

		// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
		g_pD3DVtxBuffBackGround->Lock(0, 0, (void**)&pVtx, 0);

		// 頂点座標の設定
		pVtx[0].vtx = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		pVtx[1].vtx = D3DXVECTOR3(1024.0f, 0.0f, 0.0f);
		pVtx[2].vtx = D3DXVECTOR3(0.0f, 1024.0f, 0.0f);
		pVtx[3].vtx = D3DXVECTOR3(1024.0f, 1024.0f, 0.0f);

		pVtx[0].rhw = 
		pVtx[1].rhw = 
		pVtx[2].rhw = 
		pVtx[3].rhw = 1.0f;


		// 反射光の設定
		pVtx[0].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[1].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[2].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[3].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		// テクスチャ座標の設定
		pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

		// 頂点データをアンロックする
		g_pD3DVtxBuffBackGround->Unlock();
	}

	return S_OK;

}

void	UninitBackGroundPolygon()
{

	if(g_pD3DVtxBuffBackGround != NULL)
	{// 頂点バッファの開放
		g_pD3DVtxBuffBackGround->Release();
		g_pD3DVtxBuffBackGround = NULL;
	}


}	
//=============================================================================
// レンダリングテクスチャ背景描画
//=============================================================================
void	DrawBackGroundPolygon()
{

	// 頂点フォーマットの設定
	g_pD3DDevice->SetFVF(FVF_VERTEX_2D);

	// テクスチャの設定
	g_pD3DDevice->SetTexture( 0, pRenderTexture );

	//
	g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// 頂点バッファをデバイスのデータストリームにバインド
    g_pD3DDevice->SetStreamSource(0, g_pD3DVtxBuffBackGround, 0, sizeof(VERTEX_2D));

	g_pD3DDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

	// ポリゴンの描画
	g_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, NUM_POLYGON);

	g_pD3DDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );


}

//=============================================================================
// デバイスの取得
//=============================================================================
LPDIRECT3DDEVICE9 GetDevice(void)
{
	return g_pD3DDevice;
}

#ifdef _DEBUG
//=============================================================================
// FPS表示
//=============================================================================
void DrawFPS(void)
{
	RECT rect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
	char str[256];

	wsprintf(str, "FPS:%d\n", g_nCountFPS);

	// テキスト描画
	g_pD3DXFont->DrawText(NULL, str, -1, &rect, DT_LEFT, D3DCOLOR_ARGB(0xff, 0xff, 0xff, 0xff));
}
#endif

