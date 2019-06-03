//=============================================================================
//
// モデル処理 [model.cpp]
// Author : 
//
//=============================================================================
#include "model.h"
#include "input.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_CAR		"data/MODEL/ball.x"						// 読み込むモデル名
#define TEXTURE_FILENAME "data/TEXTURE/カービー.dds"
#define TEXTURE_FILENAME2 "data/TEXTURE/すたふぃー.dds"
#define	POS_CAMERA_P_X	(0.0f)											// カメラ視点の初期位置(X座標)
#define	POS_CAMERA_P_Y	(2.0f)											// カメラ視点の初期位置(Y座標)
#define	POS_CAMERA_P_Z	(-3.0f)										// カメラ視点の初期位置(Z座標)
#define	VIEW_ANGLE		(D3DXToRadian(45.0f))							// ビュー平面の視野角
#define	VIEW_ASPECT		((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT)	// ビュー平面のアスペクト比	
#define	VIEW_NEAR_Z		(1.0f)											// ビュー平面のNearZ値
#define	VIEW_FAR_Z		(1000.0f)										// ビュー平面のFarZ値

#define	VALUE_MOVE		(5.0f)											// 移動量
#define	VALUE_ROTATE	(D3DX_PI * 0.02f)								// 回転量

#define	NUM_LIGHT		(3)												// ライトの数
#define DELTA_ANGLE 0.05f

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void InitCamera(void);
void InitLight(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
LPDIRECT3DTEXTURE9	g_pD3DTextureModel;		// テクスチャへのポインタ
LPDIRECT3DTEXTURE9	g_pD3DTextureModel2;	// テクスチャ2へのポインタ
LPD3DXMESH			g_pD3DXMeshModel;		// メッシュ情報へのポインタ
LPD3DXBUFFER		g_pD3DXBuffMatModel;	// マテリアル情報へのポインタ
DWORD				g_nNumMatModel;			// マテリアル情報の数

D3DXVECTOR3			g_posModel;				// モデルの位置
D3DXVECTOR3			g_rotModel;				// モデルの向き(回転)
D3DXVECTOR3			g_sclModel;				// モデルの大きさ(スケール)

D3DXVECTOR3			g_posCameraP;			// カメラの視点
D3DXVECTOR3			g_posCameraR;			// カメラの注視点
D3DXVECTOR3			g_vecCameraU;			// カメラの上方向ベクトル

D3DXMATRIX			g_mtxView;				// ビューマトリックス
D3DXMATRIX			g_mtxProjection;		// プロジェクションマトリックス
D3DXMATRIX			g_mtxWorld;				// ワールドマトリックス

D3DLIGHT9			g_aLight[NUM_LIGHT];	// ライト情報
//D3DXQUATERNION		quaternion(-1.0f, -1.0f, 0.0f,1);	// クォータニオン
D3DXQUATERNION		qtnLight[NUM_LIGHT];

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitModel(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// カメラの初期化
	InitCamera();

	// ライトの初期化
	InitLight();

	// 位置・回転・スケールの初期設定
	g_posModel = D3DXVECTOR3(0.0f, 2.0f, 0.0f);
	g_rotModel = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_sclModel = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

	g_pD3DTextureModel = NULL;
	g_pD3DTextureModel2 = NULL;
	g_pD3DXMeshModel = NULL;
	g_pD3DXBuffMatModel = NULL;
	g_nNumMatModel = 0;

	// Xファイルの読み込み
	if(FAILED(D3DXLoadMeshFromX(MODEL_CAR,				// 読み込むモデルファイル名(Xファイル)
								D3DXMESH_SYSTEMMEM,		// メッシュの作成オプションを指定
								pDevice,				// IDirect3DDevice9インターフェイスへのポインタ
								NULL,					// 隣接性データを含むバッファへのポインタ
								&g_pD3DXBuffMatModel,	// マテリアルデータを含むバッファへのポインタ
								NULL,					// エフェクトインスタンスの配列を含むバッファへのポインタ
								&g_nNumMatModel,		// D3DXMATERIAL構造体の数
								&g_pD3DXMeshModel)))	// ID3DXMeshインターフェイスへのポインタのアドレス
	{
		return E_FAIL;
	}

#if 1
	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,					// デバイスへのポインタ
								TEXTURE_FILENAME,		// ファイルの名前
								&g_pD3DTextureModel);	// 読み込むメモリー
	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,					// デバイスへのポインタ
								TEXTURE_FILENAME2,		// ファイルの名前
								&g_pD3DTextureModel2);	// 読み込むメモリー
#endif
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	//ステージ0の設定
	pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);				//ステージカラー
	pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_CURRENT);				//ARG1カラー
	pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);				//ARG2カラー
	pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);			//ステージアルファ
	pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_CURRENT);				//ARG2アルファ
	pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);				//ARG2アルファ
	//ステージ1の設定
	pDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 0);						//UV座標の設定をステージ０の設定と共通にする。
	pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE);				//ステージカラー
	pDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);				//ARG1カラー
	pDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_TEXTURE);				//ARG2カラー
	pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_MODULATE);			//ステージアルファ
	pDevice->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_CURRENT);				//ARG2アルファ
	pDevice->SetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);				//ARG2アルファ
#if 0
	//ステージ1の設定																				//ステージ1の設定
	pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_BLENDTEXTUREALPHA);		//ステージカラー
	pDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);				//ARG2カラー
	pDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);				//ARG1カラー
	pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);			//ステージアルファ
	pDevice->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_CURRENT);				//ARG1アルファ
	pDevice->SetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);				//ARG2アルファ
#endif

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitModel(void)
{
	if(g_pD3DTextureModel != NULL)
	{// テクスチャの開放
		g_pD3DTextureModel->Release();
		g_pD3DTextureModel = NULL;
	}
	if(g_pD3DTextureModel2 != NULL)
	{// テクスチャの開放
		g_pD3DTextureModel2->Release();
		g_pD3DTextureModel2 = NULL;
	}

	if(g_pD3DXMeshModel != NULL)
	{// メッシュの開放
		g_pD3DXMeshModel->Release();
		g_pD3DXMeshModel = NULL;
	}

	if(g_pD3DXBuffMatModel != NULL)
	{// マテリアルの開放
		g_pD3DXBuffMatModel->Release();
		g_pD3DXBuffMatModel = NULL;
	}
}
float value = 0.01;
//=============================================================================
// 更新処理
//=============================================================================
void UpdateModel(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	
	
	g_aLight[0].Diffuse.r += value;
	g_aLight[1].Diffuse.g += value;
	g_aLight[2].Diffuse.b += value;
	
	if (g_aLight[0].Diffuse.r > 1.0f)
	{
		value *= -1;
	}
	if (g_aLight[0].Diffuse.r < 0.0f)
	{
		value *= -1;
	}

	D3DXQUATERNION qtnDelta;
	//sin(1/2θ)*(0,1,0)
	qtnDelta.x = 0;
	qtnDelta.y = sin(-DELTA_ANGLE / 2.0f);
	qtnDelta.z = 0;
	qtnDelta.w = cos(-DELTA_ANGLE / 2.0f);
	for (int i = 0; i < NUM_LIGHT; i++)
	{
		qtnLight[i] *= qtnDelta;
		D3DXVECTOR3 vecDir;
		// ライト2の方向の設定
		vecDir = D3DXVECTOR3(qtnLight[i].x, qtnLight[i].y, qtnLight[i].z);
		D3DXVec3Normalize((D3DXVECTOR3*)&g_aLight[i].Direction, &vecDir);
	}




	// ライトをレンダリングパイプラインに設定
	for (int i = 0; i < NUM_LIGHT; i++)
	{
		pDevice->SetLight(i, &g_aLight[i]);
	}
	g_rotModel.y += D3DXToRadian( 0.1f );

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawModel(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	D3DXMATRIX mtxScl, mtxRot, mtxTranslate;
	D3DXMATERIAL *pD3DXMat;
	D3DMATERIAL9 matDef;

	// ビューマトリックスの初期化
	D3DXMatrixIdentity(&g_mtxView);

	// ビューマトリックスの作成
	D3DXMatrixLookAtLH(&g_mtxView, 
						&g_posCameraP,		// カメラの視点
						&g_posCameraR,		// カメラの注視点
						&g_vecCameraU);		// カメラの上方向ベクトル

	// ビューマトリックスの設定
	pDevice->SetTransform(D3DTS_VIEW, &g_mtxView);


	// プロジェクションマトリックスの初期化
	D3DXMatrixIdentity(&g_mtxProjection);

	// プロジェクションマトリックスの作成
	D3DXMatrixPerspectiveFovLH(&g_mtxProjection,
								VIEW_ANGLE,			// ビュー平面の視野角
								VIEW_ASPECT,		// ビュー平面のアスペクト比
								VIEW_NEAR_Z,		// ビュー平面のNearZ値
								VIEW_FAR_Z);		// ビュー平面のFarZ値

	// プロジェクションマトリックスの設定
	pDevice->SetTransform(D3DTS_PROJECTION, &g_mtxProjection);

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&g_mtxWorld);

	// スケールを反映
	D3DXMatrixScaling(&mtxScl, g_sclModel.x, g_sclModel.y, g_sclModel.z);
	D3DXMatrixMultiply(&g_mtxWorld, &g_mtxWorld, &mtxScl);

	// 回転を反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, g_rotModel.y, g_rotModel.x, g_rotModel.z);
	D3DXMatrixMultiply(&g_mtxWorld, &g_mtxWorld, &mtxRot);

	// 移動を反映
	D3DXMatrixTranslation(&mtxTranslate, g_posModel.x, g_posModel.y, g_posModel.z);
	D3DXMatrixMultiply(&g_mtxWorld, &g_mtxWorld, &mtxTranslate);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &g_mtxWorld);

	//
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);


	// 現在のマテリアルを取得
	pDevice->GetMaterial(&matDef);

	// マテリアル情報に対するポインタを取得
	pD3DXMat = (D3DXMATERIAL*)g_pD3DXBuffMatModel->GetBufferPointer();

	for(int nCntMat = 0; nCntMat < (int)g_nNumMatModel; nCntMat++)
	{
		// マテリアルの設定
		pDevice->SetMaterial(&pD3DXMat[nCntMat].MatD3D);

		// テクスチャの設定
		pDevice->SetTexture(0, g_pD3DTextureModel);

		// テクスチャ2の設定
		pDevice->SetTexture(1, g_pD3DTextureModel2);

		// 描画
		g_pD3DXMeshModel->DrawSubset(nCntMat);
	}

	//ステージ１のテクスチャを外す
	pDevice->SetTexture(1, NULL);

	// マテリアルをデフォルトに戻す
	pDevice->SetMaterial(&matDef);
}

//=============================================================================
// カメラの初期化処理
//=============================================================================
void InitCamera(void)
{
	g_posCameraP = D3DXVECTOR3(POS_CAMERA_P_X, POS_CAMERA_P_Y, POS_CAMERA_P_Z);
#if 1
	g_posCameraR = D3DXVECTOR3(0.0f, 2.0f, 0.0f);
#else
	g_posCameraR = g_posModel;
#endif
	g_vecCameraU = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
}

//=============================================================================
// ライトの初期化処理
//=============================================================================
void InitLight(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	D3DXVECTOR3 vecDir;

	// D3DLIGHT9構造体を0でクリアする
	ZeroMemory(&g_aLight[0], sizeof(D3DLIGHT9));

	// ライト0のタイプの設定
	g_aLight[0].Type = D3DLIGHT_DIRECTIONAL;

	// ライト0の拡散光の設定
	g_aLight[0].Diffuse = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);

	// ライト0の環境光の設定
	g_aLight[0].Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	// ライト0の方向の設定
	//vecDir = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	qtnLight[0] = D3DXQUATERNION(0.0f, 1.0f, 0.0f, 1.0f);
	vecDir = D3DXVECTOR3(qtnLight[0].x, qtnLight[0].y, qtnLight[0].z);
	D3DXVec3Normalize((D3DXVECTOR3*)&g_aLight[0].Direction, &vecDir);

	// ライト0をレンダリングパイプラインに設定
	pDevice->SetLight(0, &g_aLight[0]);

	// ライト0を使用使用状態に
	pDevice->LightEnable(0, TRUE);

	
	// D3DLIGHT9構造体を0でクリアする
	ZeroMemory(&g_aLight[1], sizeof(D3DLIGHT9));

	// ライト1のタイプの設定
	g_aLight[1].Type = D3DLIGHT_DIRECTIONAL;

	// ライト1の拡散光の設定
	g_aLight[1].Diffuse = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);

	// ライト1の環境光の設定
	g_aLight[1].Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

    // ライト1の方向の設定
	//vecDir = D3DXVECTOR3(1.0f, -1.0f, 0.0f);
	qtnLight[1] = D3DXQUATERNION(1.0f, -1.0f, 0.0f, 1.0f);
	vecDir = D3DXVECTOR3(qtnLight[1].x, qtnLight[1].y, qtnLight[1].z);
	D3DXVec3Normalize((D3DXVECTOR3*)&g_aLight[1].Direction, &vecDir);

	// ライト1をレンダリングパイプラインに設定
	pDevice->SetLight(1, &g_aLight[1]);

	// ライト1の設定
	pDevice->LightEnable(1, TRUE);

	// ライティングモードをON
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	// D3DLIGHT9構造体を0でクリアする
	ZeroMemory(&g_aLight[2], sizeof(D3DLIGHT9));

	// ライト2のタイプの設定
	g_aLight[2].Type = D3DLIGHT_DIRECTIONAL;

	// ライト2の拡散光の設定
	g_aLight[2].Diffuse = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);

	// ライト2の環境光の設定
	g_aLight[2].Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

    // ライト2の方向の設定
	//vecDir = D3DXVECTOR3(-1.0f, -1.0f, 0.0f);
	qtnLight[2] = D3DXQUATERNION(-1.0f, -1.0f, 0.0f, 1.0f);
	vecDir = D3DXVECTOR3(qtnLight[2].x, qtnLight[2].y, qtnLight[2].z);
	D3DXVec3Normalize((D3DXVECTOR3*)&g_aLight[2].Direction, &vecDir);

	// ライト2をレンダリングパイプラインに設定
	pDevice->SetLight(2, &g_aLight[2]);

	// ライト2の設定
	pDevice->LightEnable(2, TRUE);

	// ライティングモードをON
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
}

