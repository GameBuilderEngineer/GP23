//�K�v�ȃw�b�_�[�̃C���N���[�h
#include <windows.h>
#include <d3dx9.h>
//���b�V���̒��_�^�B�i���Ӂ@�{�T���v���̃��b�V���p�Ȃ̂ŁA�Ⴆ�΃X�L�����b�V���̏ꍇ�Ƃ͈قȂ�j
struct MESH_VERTEX
{ 
	D3DXVECTOR3 Pos;
	D3DXVECTOR3 Norm;
	//D3DXVECTOR2 Tex;
}; 
//�O���[�o��
DWORD g_dwVerticesNum=0;
D3DXVECTOR3 g_pVertices[30000];
//
//HRESULT CreateVolume(LPD3DXMESH pMesh, D3DXVECTOR3 vLightDir)
//�V���h�[�́h�{�����[���h���쐬
HRESULT CreateVolume(LPD3DXMESH pMesh, D3DXVECTOR3 vLightDir)
{
	g_dwVerticesNum=0;

	D3DXVECTOR3 vVertex0;
	D3DXVECTOR3 vVertex1;
	D3DXVECTOR3 vVertex2;
	D3DXVECTOR3 vVertex3;
	D3DXVECTOR3 vVertex4;	
    MESH_VERTEX *pVertices;
	WORD       *pIndices;

	DWORD dwPolyNum = pMesh->GetNumFaces();
    WORD *pEdges = new WORD[dwPolyNum*4];
	
    pMesh->LockVertexBuffer( 0L, (LPVOID*)&pVertices );
    pMesh->LockIndexBuffer( 0L, (LPVOID*)&pIndices );
	
    DWORD dwEdgesNum=0;
    // �S�|���S���ɂ��ă��C�g�x�N�g���Ƃ̊p�x�𒲂ׂāA�֊s�ƂȂ�|���S���𒊏o
    for( DWORD i = 0; i < dwPolyNum; i++ )
    {
        WORD wPolyIndex0 = pIndices[3*i+0];
        WORD wPolyIndex1 = pIndices[3*i+1];
        WORD wPolyIndex2 = pIndices[3*i+2];         
		vVertex0 = pVertices[wPolyIndex0].Pos;        
		vVertex1 = pVertices[wPolyIndex1].Pos;
		vVertex2 = pVertices[wPolyIndex2].Pos;
	
		D3DXVECTOR3 vNormal;
        D3DXVECTOR3 vCross1(vVertex2-vVertex1);
        D3DXVECTOR3 vCross2(vVertex1-vVertex0);        
        D3DXVec3Cross( &vNormal, &vCross1, &vCross2 );

        if( D3DXVec3Dot( &vNormal, &vLightDir ) >= 0 )
        {			
			pEdges[2*dwEdgesNum] = wPolyIndex0;
			pEdges[2*dwEdgesNum+1] = wPolyIndex1;	
			
			pEdges[2*dwEdgesNum+2] = wPolyIndex1;
			pEdges[2*dwEdgesNum+3] = wPolyIndex2;		
			
			pEdges[2*dwEdgesNum+4] = wPolyIndex2;
			pEdges[2*dwEdgesNum+5] = wPolyIndex0;
			
			dwEdgesNum+=3; 
        }
    }
	//�֊s�Ƃ��ă��X�g�A�b�v���ꂽ�|���S���������L�΂��i�����œK����15�P�ʐL�΂��j
    for( DWORD i = 0; i < dwEdgesNum; i++ )
    {
         vVertex1 = pVertices[pEdges[2*i]].Pos;
         vVertex2 = pVertices[pEdges[2*i+1]].Pos;
         vVertex3 = vLightDir*15-vVertex1;
         vVertex4 = vLightDir*15-vVertex2;

        g_pVertices[g_dwVerticesNum] = vVertex1;
        g_pVertices[g_dwVerticesNum+1] = vVertex2;
        g_pVertices[g_dwVerticesNum+2] = vVertex3;

        g_pVertices[g_dwVerticesNum+3] = vVertex2;
        g_pVertices[g_dwVerticesNum+4] = vVertex4;
        g_pVertices[g_dwVerticesNum+5] = vVertex3;
		g_dwVerticesNum+=6;
    }
    pMesh->UnlockVertexBuffer();
    pMesh->UnlockIndexBuffer();
	delete pEdges;

	return S_OK;
}
//
//HRESULT RenderVolumeToStencil(LPDIRECT3DDEVICE9 pDevice,D3DXMATRIX* pmWorld)
//�{�����[�����h�X�e���V���o�b�t�@�Ɂh��������
HRESULT RenderVolumeToStencil(LPDIRECT3DDEVICE9 pDevice,D3DXMATRIX* pmWorld)
{
    pDevice->SetRenderState( D3DRS_ZWRITEENABLE, false );
    pDevice->SetRenderState( D3DRS_STENCILENABLE,true );
    pDevice->SetRenderState( D3DRS_SHADEMODE,D3DSHADE_FLAT );

    pDevice->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_ALWAYS );
    pDevice->SetRenderState( D3DRS_STENCILZFAIL,D3DSTENCILOP_KEEP );
    pDevice->SetRenderState( D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP );

    pDevice->SetRenderState( D3DRS_STENCILREF,  0x1 );
    pDevice->SetRenderState( D3DRS_STENCILMASK,  0xffffffff );
    pDevice->SetRenderState( D3DRS_STENCILWRITEMASK, 0xffffffff );
    pDevice->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_INCR );

    pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, true );
    pDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ZERO );
    pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
    //�܂��͕Жʂ��X�e���V���ɏ�������
	pDevice->SetTransform( D3DTS_WORLD, pmWorld  );    
	pDevice->SetFVF( D3DFVF_XYZ | D3DFVF_TEX1  );
    pDevice->DrawPrimitiveUP( D3DPT_TRIANGLELIST, g_dwVerticesNum/3,g_pVertices, sizeof(D3DXVECTOR3) );
    //���̗��ʂ��X�e���V���ɏ�������     
	pDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );     
	pDevice->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_DECR );   
	pDevice->SetFVF( D3DFVF_XYZ | D3DFVF_TEX1  );
    pDevice->DrawPrimitiveUP( D3DPT_TRIANGLELIST, g_dwVerticesNum/3,g_pVertices, sizeof(D3DXVECTOR3) );

    // �����_�����O�X�e�[�g�����̏�Ԃɖ߂�
    pDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_GOURAUD );
    pDevice->SetRenderState( D3DRS_CULLMODE,  D3DCULL_CCW );
    pDevice->SetRenderState( D3DRS_ZWRITEENABLE,  true );
    pDevice->SetRenderState( D3DRS_STENCILENABLE, false );
    pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, false );
	return S_OK;
}
//
//VOID RenderShadow(LPDIRECT3DDEVICE9 pDevice)
//�V���h�E��ʏ�̃t���[���o�b�t�@�Ƀ����_�����O
VOID RenderShadow(LPDIRECT3DDEVICE9 pDevice)
{	
	pDevice->SetRenderState( D3DRS_STENCILREF, 0x1 );
    pDevice->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_LESSEQUAL );
    pDevice->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_KEEP );

    pDevice->SetRenderState( D3DRS_ZENABLE,false );
    pDevice->SetRenderState( D3DRS_STENCILENABLE,true );
    pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,true );
    pDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
    pDevice->SetRenderState( D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA );
    // �������|���S������ʂ̍őO�ʂɕ`��B���̔|���S�����X�e���V���Ő؂������������V���h�E�ƂȂ�
	struct COLOR_POINT
	{
		D3DXVECTOR4 vCoord;
		DWORD dwColor;
	};
	BYTE bAlpha=150;
	COLOR_POINT ColorPoint[4];
	ColorPoint[0].vCoord=D3DXVECTOR4(0,0,0,1);
	ColorPoint[0].dwColor=D3DCOLOR_ARGB(bAlpha,10,10,10);
	ColorPoint[1].vCoord=D3DXVECTOR4(800,0,0,1);
	ColorPoint[1].dwColor=D3DCOLOR_ARGB(bAlpha,10,10,10);
	ColorPoint[2].vCoord=D3DXVECTOR4(0,600,0,1);
	ColorPoint[2].dwColor=D3DCOLOR_ARGB(bAlpha,10,10,10);
	ColorPoint[3].vCoord=D3DXVECTOR4(800,600,0,1);
	ColorPoint[3].dwColor=D3DCOLOR_ARGB(bAlpha,10,10,10);

	pDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE );	
	pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2, ColorPoint, sizeof(COLOR_POINT));
}