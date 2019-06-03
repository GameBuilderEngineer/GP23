//=======================================================================================
//
//	  メッシュ クラス
//
//=======================================================================================
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS
#include <stdio.h>
#include <d3d9.h>			// DirectX Graphics 関連のヘッダ
#include <d3dx9.h>			// DirectX Graphics 関連のヘッダ
#include "define.h"
#include "common.h"

#include "Mesh.h"

//---------------------------------------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------------------------------------
CMesh::CMesh()
{
	m_pD3DMesh = NULL;
	m_dwNumMaterial = 0;
	m_pMaterial = NULL;
	m_ppTexture = NULL;
}

//---------------------------------------------------------------------------------------
// デストラクタ
//---------------------------------------------------------------------------------------
CMesh::~CMesh()
{
	Finalize();
}

//---------------------------------------------------------------------------------------
// メッシュ初期化
//---------------------------------------------------------------------------------------
bool CMesh::Initialize(LPCTSTR pszFName)
{
	TCHAR			szMsg[MAX_PATH + 32];
	TCHAR			szDir[_MAX_DIR];
	TCHAR			szCurrentDir[_MAX_PATH];

	LPD3DXBUFFER	pD3DXMtrlBuffer = NULL;

	// フォルダ名を抽出
	_tsplitpath(pszFName, NULL, szDir, NULL, NULL);

	// Ｘファイルからメッシュデータを読み込む
	if (FAILED(D3DXLoadMeshFromX(pszFName, D3DXMESH_SYSTEMMEM, g_pD3DDevice,
		NULL, &pD3DXMtrlBuffer, NULL, &m_dwNumMaterial, &m_pD3DMesh))) {
		_stprintf(szMsg, _T("Xファイル(%s)の読み込みに失敗しました。"), pszFName);
		MessageBox(NULL, szMsg, NULL, MB_OK);
		return false;
	}

	// 法線が無い場合は強制的に追加
	if ((m_pD3DMesh->GetFVF() & D3DFVF_NORMAL) == 0) {
		LPD3DXMESH pMeshTmp = m_pD3DMesh;
		pMeshTmp->CloneMeshFVF(pMeshTmp->GetOptions(), pMeshTmp->GetFVF() | D3DFVF_NORMAL,
			g_pD3DDevice, &m_pD3DMesh);
		SAFE_RELEASE(pMeshTmp);
		D3DXComputeNormals(m_pD3DMesh, NULL);
	}

	// カレントディレクトリを変更
	if (szDir[0]) {
		GetCurrentDirectory(_MAX_PATH, szCurrentDir);
		SetCurrentDirectory(szDir);
	}

	// マテリアル読み込み
	D3DXMATERIAL* pD3DMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();
	m_pMaterial = new D3DMATERIAL9[m_dwNumMaterial];
	m_ppTexture = new LPDIRECT3DTEXTURE9[m_dwNumMaterial];
	for (DWORD i = 0; i < m_dwNumMaterial; i++) { 
		m_pMaterial[i] = pD3DMaterials[i].MatD3D;
		m_pMaterial[i].Ambient = m_pMaterial[i].Diffuse;
		m_ppTexture[i] = NULL;
		if (pD3DMaterials[i].pTextureFilename && pD3DMaterials[i].pTextureFilename[0]) {
			// テクスチャファイルを読み込む
			if (FAILED(D3DXCreateTextureFromFileA(g_pD3DDevice,
				pD3DMaterials[i].pTextureFilename, &m_ppTexture[i]))) {
				_stprintf(szMsg, _T("テクスチャ(%hs)の読み込みに失敗しました。"),
					pD3DMaterials[i].pTextureFilename);
				MessageBox(NULL, szMsg, NULL, MB_OK);
			}
		}
	}

	// カレントディレクトリを元に戻す
	if (szDir[0])
		SetCurrentDirectory(szCurrentDir);

	pD3DXMtrlBuffer->Release();

	return true;
}

//---------------------------------------------------------------------------------------
// メッシュ解放
//---------------------------------------------------------------------------------------
void CMesh::Finalize()
{
	// テクスチャ オブジェクトを解放
	if (m_ppTexture) {
		for (DWORD i = 0; i < m_dwNumMaterial; i++) {
			SAFE_RELEASE(m_ppTexture[i]);
		}
		SAFE_DELETE_ARRAY(m_ppTexture);
	}
	SAFE_DELETE_ARRAY(m_pMaterial);

	SAFE_RELEASE(m_pD3DMesh);	// メッシュ オブジェクトを解放
}

//---------------------------------------------------------------------------------------
// メッシュ描画
//---------------------------------------------------------------------------------------
void CMesh::Draw(D3DXMATRIX& world)
{
	// ワールド マトリックス設定
	g_pD3DDevice->SetTransform(D3DTS_WORLD, &world);

	for (DWORD i = 0; i < m_dwNumMaterial; i++) {
		g_pD3DDevice->SetMaterial(&m_pMaterial[i]);
		g_pD3DDevice->SetTexture(0, m_ppTexture[i]);	// テクスチャを設定
		m_pD3DMesh->DrawSubset(i);						// 描画を実行
	}
}

//=======================================================================================
//	End of File
//=======================================================================================