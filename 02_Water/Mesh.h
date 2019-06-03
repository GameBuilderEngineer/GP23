//=======================================================================================
//
//	  メッシュ クラス定義
//
//=======================================================================================
#pragma once
#include <windows.h>		// Windowsプログラムにはこれを付ける
#include <tchar.h>			// 汎用テキスト マッピング
#include <d3d9.h>			// DirectX Graphics 関連のヘッダ
#include <d3dx9.h>			// DirectX Graphics 関連のヘッダ

class CMesh
{
private:
	LPD3DXMESH			m_pD3DMesh;			// D3DXMesh オブジェクト
	DWORD				m_dwNumMaterial;	// マテリアル数
	D3DMATERIAL9*		m_pMaterial;		// マテリアル
	LPDIRECT3DTEXTURE9*	m_ppTexture;		// テクスチャ

public:
	CMesh();								// コンストラクタ
	virtual ~CMesh();						// デストラクタ
	bool Initialize(LPCTSTR pszFName);		// メッシュ初期化
	void Finalize();						// メッシュ解放
	void Draw(D3DXMATRIX& world);			// メッシュ描画
};

//=======================================================================================
//	End of File
//=======================================================================================