//必要なヘッダーのインクルード
#include <windows.h>
#include <d3dx9.h>
//必要なライブラリファイルのロード
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
//定数、マクロ定義
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }
#define THING_AMOUNT 2
//データ型定義
struct THING
{	
	LPD3DXMESH pMesh;
	D3DMATERIAL9* pMeshMaterials;
	LPDIRECT3DTEXTURE9* pMeshTextures; 
	DWORD dwNumMaterials;
	D3DXVECTOR3 vPosition;
	D3DXMATRIX mWorld;
	THING()
	{
		ZeroMemory(this,sizeof(THING));
	}
};
//グローバル
LPDIRECT3D9 pD3d;
LPDIRECT3DDEVICE9 pDevice; 
FLOAT g_fCameraX=2,g_fCameraY=5.0f,g_fCameraZ=-3.0f,
			g_fCameraHeading=0,g_fCameraPitch=0;
THING Thing[THING_AMOUNT];
D3DXVECTOR3 g_vLightDirection(0,-1,1);
//プロトタイプ
LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
HRESULT InitD3d(HWND);
HRESULT InitThing(THING *,LPSTR,D3DXVECTOR3*);
HRESULT CreateVolume(LPD3DXMESH pMesh, D3DXVECTOR3 );
VOID Render();
VOID RenderThing(THING*);

HRESULT RenderVolumeToStencil(LPDIRECT3DDEVICE9,D3DXMATRIX*);
VOID RenderShadow(LPDIRECT3DDEVICE9 );

VOID FreeDx();
//
//INT WINAPI WinMain( HINSTANCE hInst,HINSTANCE hPrevInst,LPSTR szStr,INT iCmdShow)
//アプリケーションのエントリー関数
INT WINAPI WinMain( HINSTANCE hInst,HINSTANCE hPrevInst,LPSTR szStr,INT iCmdShow)
{
	HWND hWnd=NULL;
	MSG msg;
	// ウィンドウの初期化
	static char szAppName[] = "シャドーボリュームの影 矢印キーでカメラ回転、A,D,でカメラ左右移動　W,Cでカメラ前後移動";     
     WNDCLASSEX  wndclass;

     wndclass.cbSize           = sizeof (wndclass);
     wndclass.style              = CS_HREDRAW | CS_VREDRAW;
     wndclass.lpfnWndProc    = WndProc;
     wndclass.cbClsExtra      = 0;
     wndclass.cbWndExtra     = 0;
     wndclass.hInstance        = hInst;
     wndclass.hIcon             = LoadIcon (NULL, IDI_APPLICATION);
     wndclass.hCursor          = LoadCursor (NULL, IDC_ARROW);
     wndclass.hbrBackground = (HBRUSH) GetStockObject (BLACK_BRUSH);
     wndclass.lpszMenuName = NULL;
     wndclass.lpszClassName = szAppName;
     wndclass.hIconSm         = LoadIcon (NULL, IDI_APPLICATION);

     RegisterClassEx (&wndclass);

     hWnd = CreateWindow (szAppName,szAppName,WS_OVERLAPPEDWINDOW,   
                    0,0,800,600,NULL,NULL,hInst,NULL);	

    ShowWindow (hWnd,SW_SHOW);
    UpdateWindow (hWnd);
	// ダイレクト３Dの初期化関数を呼ぶ
	if(FAILED(InitD3d(hWnd)))
	{
		return 0;
	}
	// メッセージループ
    ZeroMemory( &msg, sizeof(msg) );
    while( msg.message!=WM_QUIT )
     {
		if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
         {
			TranslateMessage( &msg );
             DispatchMessage( &msg );
         }
         else
		{		
			Render();		
		}                    
	}
	// メッセージループから抜けたらオブジェクトを全て開放する
	FreeDx();
	// OSに戻る（アプリケーションを終了する）
     return (INT)msg.wParam;
}

//
//LRESULT CALLBACK WndProc(HWND hWnd,UINT iMsg,WPARAM wParam,LPARAM lParam)
// ウィンドウプロシージャ関数
LRESULT CALLBACK WndProc(HWND hWnd,UINT iMsg,WPARAM wParam,LPARAM lParam)
{	
	switch(iMsg)
	{
		case WM_DESTROY:
			PostQuitMessage(0);
		break;
		case WM_KEYDOWN:
		switch((CHAR)wParam)
		{
			case VK_ESCAPE:
				PostQuitMessage(0);
			break;
			// カメラの移動
			case 'A':
				g_fCameraX-=0.1f;  
			break;
			case 'D':
				g_fCameraX+=0.1f;         
			break;
			case 'Q':
				g_fCameraY-=0.1f;         
			break;
			case 'E':
				g_fCameraY+=0.1f;         
			break;
			case 'W':
				g_fCameraZ+=0.1f;         
			break;
			case 'C':
				g_fCameraZ-=0.1f;         
			break;
			// カメラの回転
			case VK_LEFT:
				g_fCameraHeading-=0.1f;
				break;
			case VK_RIGHT:
				g_fCameraHeading+=0.1f;
			break;
			case VK_UP:
				g_fCameraPitch-=0.1f;
			break;
			case VK_DOWN:
				g_fCameraPitch+=0.1f;
			break;
		}
		break;	
	}
	return DefWindowProc (hWnd, iMsg, wParam, lParam);	
}
//
//HRESULT InitD3d(HWND hWnd)
//ダイレクト3Dの初期化関数
HRESULT InitD3d(HWND hWnd)
{
	// 「Direct3D」オブジェクトの作成
	if( NULL == ( pD3d = Direct3DCreate9( D3D_SDK_VERSION ) ) )
	{
		MessageBox(0,"Direct3Dの作成に失敗しました","",MB_OK);
        return E_FAIL;
	}
	// 「DIRECT3Dデバイス」オブジェクトの作成
	D3DPRESENT_PARAMETERS d3dpp; 
	ZeroMemory( &d3dpp, sizeof(d3dpp) );   
	d3dpp.BackBufferFormat =D3DFMT_UNKNOWN;
	d3dpp.BackBufferCount=1;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;	
	d3dpp.Windowed = true;
    d3dpp.EnableAutoDepthStencil = true;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	d3dpp.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;//ダブルステンシル

	if( FAILED( pD3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,    
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,&d3dpp, &pDevice ) ) )
	{
		if( FAILED( pD3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, hWnd,           
			D3DCREATE_SOFTWARE_VERTEXPROCESSING,&d3dpp, &pDevice ) ) )			
		{			
			MessageBox(0,"DIRECT3Dデバイスの作成に失敗しました",NULL,MB_OK);			
			return E_FAIL;			
		}    
	}		  
	// Xファイル毎にメッシュを作成する
	InitThing(&Thing[0],"Field.x",&D3DXVECTOR3(0,-0.5,0));
	InitThing(&Thing[1],"RobotA_still.x",&D3DXVECTOR3(0,0.12,0));
	//カリングは行わない
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	// Zバッファー処理を有効にする
    pDevice->SetRenderState( D3DRS_ZENABLE, true );  
	// ライトを有効にする
	pDevice->SetRenderState( D3DRS_LIGHTING, true );
	// アンビエントライト（環境光）を設定する
	pDevice->SetRenderState( D3DRS_AMBIENT, 0x00111111 );
	// スペキュラ（鏡面反射）を有効にする
	pDevice->SetRenderState(D3DRS_SPECULARENABLE,true);
		
	return S_OK;
}
//
//HRESULT InitThing(THING *pThing,LPSTR szXFileName,D3DXVECTOR3* pvPosition)
//Thingを初期化する
HRESULT InitThing(THING *pThing,LPSTR szXFileName,D3DXVECTOR3* pvPosition)
{
	// メッシュの初期位置
	memcpy(&pThing->vPosition,pvPosition,sizeof(D3DXVECTOR3));
	// Xファイルからメッシュをロードする	
	LPD3DXBUFFER pD3DXMtrlBuffer = NULL;

	if( FAILED( D3DXLoadMeshFromX( szXFileName, D3DXMESH_SYSTEMMEM, 
            pDevice, NULL, &pD3DXMtrlBuffer, NULL,
			&pThing->dwNumMaterials, &pThing->pMesh ) ) )
    {
            MessageBox(NULL, "Xファイルの読み込みに失敗しました",szXFileName, MB_OK);
            return E_FAIL;   
    }
	D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();
	pThing->pMeshMaterials = new D3DMATERIAL9[pThing->dwNumMaterials];
    pThing->pMeshTextures  = new LPDIRECT3DTEXTURE9[pThing->dwNumMaterials];

	for( DWORD i=0; i<pThing->dwNumMaterials; i++ )
	{ 
		pThing->pMeshMaterials[i] = d3dxMaterials[i].MatD3D;		
        pThing->pMeshMaterials[i].Ambient = pThing->pMeshMaterials[i].Diffuse;
        pThing->pMeshTextures[i] = NULL;
        if( d3dxMaterials[i].pTextureFilename != NULL && 
            lstrlen(d3dxMaterials[i].pTextureFilename) > 0 )
        {       
            if( FAILED( D3DXCreateTextureFromFile( pDevice, 
                                                d3dxMaterials[i].pTextureFilename, 
                                                &pThing->pMeshTextures[i] ) ) )
            {      
                MessageBox(NULL, "テクスチャの読み込みに失敗しました", NULL, MB_OK);
			}
        }
    }
	pD3DXMtrlBuffer->Release();
    
	return S_OK;
}
//
//VOID Render()
//シーン全体のレンダリング
VOID Render()
{
	pDevice->SetRenderState( D3DRS_ZENABLE,true );
    pDevice->SetRenderState( D3DRS_STENCILENABLE,false );
    pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,false );

	if(FAILED(CreateVolume(Thing[1].pMesh,g_vLightDirection)))		
	{
		MessageBox(0,"ボリューム作成失敗","",MB_OK);
		return;
	}
	// ビュートランスフォーム（視点座標変換）
	D3DXMATRIXA16 mView,mCameraPosition,mHeading,mPitch;
     D3DXVECTOR3 vEyePt( g_fCameraX,g_fCameraY,g_fCameraZ ); //カメラ（視点）位置
     D3DXVECTOR3 vLookatPt( g_fCameraX-2.0f,g_fCameraY-5.0f,g_fCameraZ+3.0f );//注視位置
     D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );//上方位置      
	D3DXMatrixIdentity(&mView);
	D3DXMatrixRotationY(&mHeading,g_fCameraHeading);
	D3DXMatrixRotationX(&mPitch,g_fCameraPitch);
	D3DXMatrixLookAtLH( &mCameraPosition, &vEyePt, &vLookatPt, &vUpVec );
	D3DXMatrixMultiply(&mView,&mView,&mHeading);
	D3DXMatrixMultiply(&mView,&mView,&mPitch);
	D3DXMatrixMultiply(&mView,&mView,&mCameraPosition);
     pDevice->SetTransform( D3DTS_VIEW, &mView );
	// プロジェクショントランスフォーム（射影変換）
     D3DXMATRIXA16 mProj;
     D3DXMatrixPerspectiveFovLH( &mProj, D3DX_PI/4, (FLOAT)800/(FLOAT)600, 0.1f, 1000.0f );
     pDevice->SetTransform( D3DTS_PROJECTION, &mProj ); 
	// ライトをあてる 白色で鏡面反射ありに設定	
     D3DLIGHT9 light;
     ZeroMemory( &light, sizeof(D3DLIGHT9) );
     light.Type       = D3DLIGHT_DIRECTIONAL;
     light.Diffuse.r  = 1.0f;
     light.Diffuse.g  = 1.0f;
     light.Diffuse.b  = 1.0f;    
	light.Specular.r=1.0f;
	light.Specular.g=1.0f;
	light.Specular.b=1.0f;
	light.Direction=g_vLightDirection;
	light.Position=D3DXVECTOR3(0,10,0);
     light.Range       = 200.0f;
     pDevice->SetLight( 0, &light );
     pDevice->LightEnable( 0, true );	
	//ライトを回転
	D3DXMATRIX mRotation;
	D3DXMatrixRotationY(&mRotation,D3DXToRadian(2));//回転速度は適当に2度/フレーム
	D3DXVec3TransformCoord(&g_vLightDirection,&g_vLightDirection,&mRotation);

	pDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER
		| D3DCLEAR_STENCIL, D3DCOLOR_XRGB(0,255,0), 1.0f, 0 );

	if( SUCCEEDED( pDevice->BeginScene() ) )
	{		
		RenderThing(&Thing[0]);	
		RenderThing(&Thing[1]);
		RenderVolumeToStencil(pDevice,&Thing[1].mWorld);		
		RenderShadow(pDevice);		

		pDevice->EndScene();
	}	
	pDevice->Present( NULL, NULL, NULL, NULL );	
}
//
//VOID RenderThing(THING* pThing)
//Thingごとのレンダリング。ここではRender関数のサブ関数
VOID RenderThing(THING* pThing)
{
	//ワールドトランスフォーム（絶対座標変換）
	D3DXMATRIXA16 mWorld,mPosition;
	D3DXMatrixIdentity(&mWorld);
	D3DXMatrixTranslation(&mPosition,pThing->vPosition.x,pThing->vPosition.y,
		pThing->vPosition.z);
	D3DXMatrixMultiply(&mWorld,&mWorld,&mPosition);
     pDevice->SetTransform( D3DTS_WORLD, &mWorld );
	pThing->mWorld=mWorld;   
	
	// レンダリング
	for( DWORD i=0; i<pThing->dwNumMaterials; i++ )
     {
		 pDevice->SetMaterial( &pThing->pMeshMaterials[i] );
          pDevice->SetTexture( 0,pThing->pMeshTextures[i] ); 
          pThing->pMesh->DrawSubset( i );
     }		
}


//
//VOID FreeDx()
// 作成したDirectXオブジェクトの開放
VOID FreeDx()
{
	for(DWORD i=0;i<THING_AMOUNT;i++)
	{
		for(DWORD k=0;k<Thing[i].dwNumMaterials;k++)
		{		
			SAFE_RELEASE( Thing[i].pMeshTextures[k] );
		}
		SAFE_RELEASE( Thing[i].pMesh );
	}
	SAFE_RELEASE( pDevice );
	SAFE_RELEASE( pD3d );
}