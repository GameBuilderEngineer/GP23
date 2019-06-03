//=============================================================================
//
// 地面処理 [field.cpp]
// Author : 
//
//=============================================================================
#include "field.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	TEXTURE_FIELD	"data/TEXTURE/field004.jpg"	// 読み込むテクスチャファイル名
#define	TEXTURE_WALL	"data/TEXTURE/wall004.jpg"	// 読み込むテクスチャファイル名

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexField(LPDIRECT3DDEVICE9 pDevice);
HRESULT MakeVertexWall(LPDIRECT3DDEVICE9 pDevice);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
LPDIRECT3DTEXTURE9		g_pD3DTextureField = NULL;	// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pD3DVtxBuffField = NULL;	// 頂点バッファへのポインタ
D3DXMATRIX				g_mtxWorldField;			// ワールドマトリックス
D3DXVECTOR3				g_posField;					// 現在の位置
D3DXVECTOR3				g_rotField;					// 現在の向き


LPDIRECT3DTEXTURE9		g_pD3DTextureWall = NULL;	// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pD3DVtxBuffWall = NULL;	// 頂点バッファへのポインタ
D3DXMATRIX				g_mtxWorldWall;				// ワールドマトリックス
D3DXVECTOR3				g_posWall;					// 現在の位置
D3DXVECTOR3				g_rotWall;					// 現在の向き

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitField(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice(); 
	HRESULT hr;

	// 位置、向きの初期設定
	g_posField = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_rotField = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,					// デバイスへのポインタ
								TEXTURE_FIELD,			// ファイルの名前
								&g_pD3DTextureField);	// 読み込むメモリー

	// 頂点情報の作成
	hr = MakeVertexField(pDevice);



	return hr;
}

HRESULT InitWall(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice(); 
	HRESULT hr;

	// 位置、向きの初期設定
	g_posWall = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_rotWall = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,					// デバイスへのポインタ
								TEXTURE_WALL,			// ファイルの名前
								&g_pD3DTextureWall);	// 読み込むメモリー

	// 頂点情報の作成
	hr = MakeVertexWall(pDevice);



	return hr;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitField(void)
{
	if(g_pD3DTextureField != NULL)
	{// テクスチャの開放
		g_pD3DTextureField->Release();
		g_pD3DTextureField = NULL;
	}

	if(g_pD3DVtxBuffField != NULL)
	{// 頂点バッファの開放
		g_pD3DVtxBuffField->Release();
		g_pD3DVtxBuffField = NULL;
	}
}

void UninitWall(void)
{

	if(g_pD3DTextureWall != NULL)
	{// テクスチャの開放
		g_pD3DTextureWall->Release();
		g_pD3DTextureWall = NULL;
	}

	if(g_pD3DVtxBuffWall != NULL)
	{// 頂点バッファの開放
		g_pD3DVtxBuffWall->Release();
		g_pD3DVtxBuffWall = NULL;
	}



}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateField(void)
{
}

void UpdateWall(void)
{
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawField(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice(); 
	D3DXMATRIX mtxRot, mtxTranslate;

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&g_mtxWorldField);

	// 回転を反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, g_rotField.y, g_rotField.x, g_rotField.z);
	D3DXMatrixMultiply(&g_mtxWorldField, &g_mtxWorldField, &mtxRot);

	// 移動を反映
	D3DXMatrixTranslation(&mtxTranslate, g_posField.x, g_posField.y, g_posField.z);
	D3DXMatrixMultiply(&g_mtxWorldField, &g_mtxWorldField, &mtxTranslate);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &g_mtxWorldField);

	// 頂点バッファをデバイスのデータストリームにバインド
    pDevice->SetStreamSource(0, g_pD3DVtxBuffField, 0, sizeof(VERTEX_3D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	// テクスチャの設定
	pDevice->SetTexture(0, g_pD3DTextureField);

	//
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, NUM_POLYGON);
}


void DrawWall(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice(); 
	D3DXMATRIX mtxRot, mtxTranslate;

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&g_mtxWorldWall);

	// 回転を反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, g_rotWall.y, g_rotWall.x, g_rotWall.z);
	D3DXMatrixMultiply(&g_mtxWorldField, &g_mtxWorldField, &mtxRot);

	// 移動を反映
	D3DXMatrixTranslation(&mtxTranslate, g_posWall.x, g_posWall.y, g_posWall.z);
	D3DXMatrixMultiply(&g_mtxWorldWall, &g_mtxWorldWall, &mtxTranslate);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &g_mtxWorldWall);

	// 頂点バッファをデバイスのデータストリームにバインド
    pDevice->SetStreamSource(0, g_pD3DVtxBuffWall, 0, sizeof(VERTEX_3D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	// テクスチャの設定
	pDevice->SetTexture(0, g_pD3DTextureWall);

	//
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);


	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 8);
}


//=============================================================================
// 頂点の作成
//=============================================================================
HRESULT MakeVertexField(LPDIRECT3DDEVICE9 pDevice)
{
	// オブジェクトの頂点バッファを生成
    if(FAILED(pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * NUM_VERTEX,	// 頂点データ用に確保するバッファサイズ(バイト単位)
												D3DUSAGE_WRITEONLY,			// 頂点バッファの使用法　
												FVF_VERTEX_3D,				// 使用する頂点フォーマット
												D3DPOOL_MANAGED,			// リソースのバッファを保持するメモリクラスを指定
												&g_pD3DVtxBuffField,		// 頂点バッファインターフェースへのポインタ
												NULL)))						// NULLに設定
	{
        return E_FAIL;
	}

	{//頂点バッファの中身を埋める
		VERTEX_3D *pVtx;

		// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
		g_pD3DVtxBuffField->Lock(0, 0, (void**)&pVtx, 0);

		// 頂点座標の設定
		pVtx[0].vtx = D3DXVECTOR3(-100.0f, 0.0f, 100.0f);
		pVtx[1].vtx = D3DXVECTOR3(100.0f, 0.0f, 100.0f);
		pVtx[2].vtx = D3DXVECTOR3(-100.0f, 0.0f, -100.0f);
		pVtx[3].vtx = D3DXVECTOR3(100.0f, 0.0f, -100.0f);

		// 法線ベクトルの設定
		pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		pVtx[1].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		pVtx[2].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		pVtx[3].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

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
		g_pD3DVtxBuffField->Unlock();
	}

	return S_OK;
}


HRESULT MakeVertexWall(LPDIRECT3DDEVICE9 pDevice)
{
	// オブジェクトの頂点バッファを生成
    if(FAILED(pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * 10,	// 頂点データ用に確保するバッファサイズ(バイト単位)
												D3DUSAGE_WRITEONLY,			// 頂点バッファの使用法　
												FVF_VERTEX_3D,				// 使用する頂点フォーマット
												D3DPOOL_MANAGED,			// リソースのバッファを保持するメモリクラスを指定
												&g_pD3DVtxBuffWall,		// 頂点バッファインターフェースへのポインタ
												NULL)))						// NULLに設定
	{
        return E_FAIL;
	}

	{//頂点バッファの中身を埋める
		VERTEX_3D *pVtx;

		// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
		g_pD3DVtxBuffWall->Lock(0, 0, (void**)&pVtx, 0);

		// 頂点座標の設定
		pVtx[0].vtx = D3DXVECTOR3(-100.0f, 0.0f, 100.0f);
		pVtx[1].vtx = D3DXVECTOR3(-100.0f, 70.0f, 100.0f);
		pVtx[2].vtx = D3DXVECTOR3(100.0f, 0.0f, 100.0f);
		pVtx[3].vtx = D3DXVECTOR3(100.0f, 70.0f,  100.0f);
		pVtx[4].vtx = D3DXVECTOR3(100.0f, 0.0f, -100.0f);
		pVtx[5].vtx = D3DXVECTOR3(100.0f, 70.0f, -100.0f);
		pVtx[6].vtx = D3DXVECTOR3(-100.0f, 0.0f, -100.0f);
		pVtx[7].vtx = D3DXVECTOR3(-100.0f, 70.0f, -100.0f);
		pVtx[8].vtx = D3DXVECTOR3(-100.0f, 0.0f, 100.0f);
		pVtx[9].vtx = D3DXVECTOR3(-100.0f, 70.0f, 100.0f);

		// 法線ベクトルの設定 光源計算しない
		pVtx[0].nor = pVtx[1].nor = 
		pVtx[2].nor = pVtx[3].nor = 
		pVtx[4].nor = pVtx[5].nor = 
		pVtx[6].nor = pVtx[7].nor = 
		pVtx[8].nor = pVtx[9].nor = D3DXVECTOR3(0.0f, 0.0f, 0.0f);


		// 反射光の設定
		pVtx[0].diffuse = pVtx[1].diffuse = 
		pVtx[2].diffuse = pVtx[3].diffuse = 
		pVtx[4].diffuse = pVtx[5].diffuse = 
		pVtx[6].diffuse = pVtx[7].diffuse = 
		pVtx[8].diffuse = pVtx[9].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);


		// テクスチャ座標の設定
		pVtx[0].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[1].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(1.0f, 1.0f);
		pVtx[3].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[4].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[5].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[6].tex = D3DXVECTOR2(1.0f, 1.0f);
		pVtx[7].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[8].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[9].tex = D3DXVECTOR2(0.0f, 0.0f);


		// 頂点データをアンロックする
		g_pD3DVtxBuffWall->Unlock();
	}

	return S_OK;
}







