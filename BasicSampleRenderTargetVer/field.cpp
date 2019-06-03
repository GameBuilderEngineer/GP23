//=============================================================================
//
// �n�ʏ��� [field.cpp]
// Author : 
//
//=============================================================================
#include "field.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	TEXTURE_FIELD	"data/TEXTURE/field004.jpg"	// �ǂݍ��ރe�N�X�`���t�@�C����
#define	TEXTURE_WALL	"data/TEXTURE/wall004.jpg"	// �ǂݍ��ރe�N�X�`���t�@�C����

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT MakeVertexField(LPDIRECT3DDEVICE9 pDevice);
HRESULT MakeVertexWall(LPDIRECT3DDEVICE9 pDevice);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
LPDIRECT3DTEXTURE9		g_pD3DTextureField = NULL;	// �e�N�X�`���ւ̃|�C���^
LPDIRECT3DVERTEXBUFFER9 g_pD3DVtxBuffField = NULL;	// ���_�o�b�t�@�ւ̃|�C���^
D3DXMATRIX				g_mtxWorldField;			// ���[���h�}�g���b�N�X
D3DXVECTOR3				g_posField;					// ���݂̈ʒu
D3DXVECTOR3				g_rotField;					// ���݂̌���


LPDIRECT3DTEXTURE9		g_pD3DTextureWall = NULL;	// �e�N�X�`���ւ̃|�C���^
LPDIRECT3DVERTEXBUFFER9 g_pD3DVtxBuffWall = NULL;	// ���_�o�b�t�@�ւ̃|�C���^
D3DXMATRIX				g_mtxWorldWall;				// ���[���h�}�g���b�N�X
D3DXVECTOR3				g_posWall;					// ���݂̈ʒu
D3DXVECTOR3				g_rotWall;					// ���݂̌���

//=============================================================================
// ����������
//=============================================================================
HRESULT InitField(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice(); 
	HRESULT hr;

	// �ʒu�A�����̏����ݒ�
	g_posField = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_rotField = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	// �e�N�X�`���̓ǂݍ���
	D3DXCreateTextureFromFile(pDevice,					// �f�o�C�X�ւ̃|�C���^
								TEXTURE_FIELD,			// �t�@�C���̖��O
								&g_pD3DTextureField);	// �ǂݍ��ރ������[

	// ���_���̍쐬
	hr = MakeVertexField(pDevice);



	return hr;
}

HRESULT InitWall(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice(); 
	HRESULT hr;

	// �ʒu�A�����̏����ݒ�
	g_posWall = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_rotWall = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	// �e�N�X�`���̓ǂݍ���
	D3DXCreateTextureFromFile(pDevice,					// �f�o�C�X�ւ̃|�C���^
								TEXTURE_WALL,			// �t�@�C���̖��O
								&g_pD3DTextureWall);	// �ǂݍ��ރ������[

	// ���_���̍쐬
	hr = MakeVertexWall(pDevice);



	return hr;
}

//=============================================================================
// �I������
//=============================================================================
void UninitField(void)
{
	if(g_pD3DTextureField != NULL)
	{// �e�N�X�`���̊J��
		g_pD3DTextureField->Release();
		g_pD3DTextureField = NULL;
	}

	if(g_pD3DVtxBuffField != NULL)
	{// ���_�o�b�t�@�̊J��
		g_pD3DVtxBuffField->Release();
		g_pD3DVtxBuffField = NULL;
	}
}

void UninitWall(void)
{

	if(g_pD3DTextureWall != NULL)
	{// �e�N�X�`���̊J��
		g_pD3DTextureWall->Release();
		g_pD3DTextureWall = NULL;
	}

	if(g_pD3DVtxBuffWall != NULL)
	{// ���_�o�b�t�@�̊J��
		g_pD3DVtxBuffWall->Release();
		g_pD3DVtxBuffWall = NULL;
	}



}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateField(void)
{
}

void UpdateWall(void)
{
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawField(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice(); 
	D3DXMATRIX mtxRot, mtxTranslate;

	// ���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&g_mtxWorldField);

	// ��]�𔽉f
	D3DXMatrixRotationYawPitchRoll(&mtxRot, g_rotField.y, g_rotField.x, g_rotField.z);
	D3DXMatrixMultiply(&g_mtxWorldField, &g_mtxWorldField, &mtxRot);

	// �ړ��𔽉f
	D3DXMatrixTranslation(&mtxTranslate, g_posField.x, g_posField.y, g_posField.z);
	D3DXMatrixMultiply(&g_mtxWorldField, &g_mtxWorldField, &mtxTranslate);

	// ���[���h�}�g���b�N�X�̐ݒ�
	pDevice->SetTransform(D3DTS_WORLD, &g_mtxWorldField);

	// ���_�o�b�t�@���f�o�C�X�̃f�[�^�X�g���[���Ƀo�C���h
    pDevice->SetStreamSource(0, g_pD3DVtxBuffField, 0, sizeof(VERTEX_3D));

	// ���_�t�H�[�}�b�g�̐ݒ�
	pDevice->SetFVF(FVF_VERTEX_3D);

	// �e�N�X�`���̐ݒ�
	pDevice->SetTexture(0, g_pD3DTextureField);

	//
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// �|���S���̕`��
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, NUM_POLYGON);
}


void DrawWall(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice(); 
	D3DXMATRIX mtxRot, mtxTranslate;

	// ���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&g_mtxWorldWall);

	// ��]�𔽉f
	D3DXMatrixRotationYawPitchRoll(&mtxRot, g_rotWall.y, g_rotWall.x, g_rotWall.z);
	D3DXMatrixMultiply(&g_mtxWorldField, &g_mtxWorldField, &mtxRot);

	// �ړ��𔽉f
	D3DXMatrixTranslation(&mtxTranslate, g_posWall.x, g_posWall.y, g_posWall.z);
	D3DXMatrixMultiply(&g_mtxWorldWall, &g_mtxWorldWall, &mtxTranslate);

	// ���[���h�}�g���b�N�X�̐ݒ�
	pDevice->SetTransform(D3DTS_WORLD, &g_mtxWorldWall);

	// ���_�o�b�t�@���f�o�C�X�̃f�[�^�X�g���[���Ƀo�C���h
    pDevice->SetStreamSource(0, g_pD3DVtxBuffWall, 0, sizeof(VERTEX_3D));

	// ���_�t�H�[�}�b�g�̐ݒ�
	pDevice->SetFVF(FVF_VERTEX_3D);

	// �e�N�X�`���̐ݒ�
	pDevice->SetTexture(0, g_pD3DTextureWall);

	//
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);


	// �|���S���̕`��
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 8);
}


//=============================================================================
// ���_�̍쐬
//=============================================================================
HRESULT MakeVertexField(LPDIRECT3DDEVICE9 pDevice)
{
	// �I�u�W�F�N�g�̒��_�o�b�t�@�𐶐�
    if(FAILED(pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * NUM_VERTEX,	// ���_�f�[�^�p�Ɋm�ۂ���o�b�t�@�T�C�Y(�o�C�g�P��)
												D3DUSAGE_WRITEONLY,			// ���_�o�b�t�@�̎g�p�@�@
												FVF_VERTEX_3D,				// �g�p���钸�_�t�H�[�}�b�g
												D3DPOOL_MANAGED,			// ���\�[�X�̃o�b�t�@��ێ����郁�����N���X���w��
												&g_pD3DVtxBuffField,		// ���_�o�b�t�@�C���^�[�t�F�[�X�ւ̃|�C���^
												NULL)))						// NULL�ɐݒ�
	{
        return E_FAIL;
	}

	{//���_�o�b�t�@�̒��g�𖄂߂�
		VERTEX_3D *pVtx;

		// ���_�f�[�^�͈̔͂����b�N���A���_�o�b�t�@�ւ̃|�C���^���擾
		g_pD3DVtxBuffField->Lock(0, 0, (void**)&pVtx, 0);

		// ���_���W�̐ݒ�
		pVtx[0].vtx = D3DXVECTOR3(-100.0f, 0.0f, 100.0f);
		pVtx[1].vtx = D3DXVECTOR3(100.0f, 0.0f, 100.0f);
		pVtx[2].vtx = D3DXVECTOR3(-100.0f, 0.0f, -100.0f);
		pVtx[3].vtx = D3DXVECTOR3(100.0f, 0.0f, -100.0f);

		// �@���x�N�g���̐ݒ�
		pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		pVtx[1].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		pVtx[2].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		pVtx[3].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

		// ���ˌ��̐ݒ�
		pVtx[0].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[1].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[2].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[3].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		// �e�N�X�`�����W�̐ݒ�
		pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

		// ���_�f�[�^���A�����b�N����
		g_pD3DVtxBuffField->Unlock();
	}

	return S_OK;
}


HRESULT MakeVertexWall(LPDIRECT3DDEVICE9 pDevice)
{
	// �I�u�W�F�N�g�̒��_�o�b�t�@�𐶐�
    if(FAILED(pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * 10,	// ���_�f�[�^�p�Ɋm�ۂ���o�b�t�@�T�C�Y(�o�C�g�P��)
												D3DUSAGE_WRITEONLY,			// ���_�o�b�t�@�̎g�p�@�@
												FVF_VERTEX_3D,				// �g�p���钸�_�t�H�[�}�b�g
												D3DPOOL_MANAGED,			// ���\�[�X�̃o�b�t�@��ێ����郁�����N���X���w��
												&g_pD3DVtxBuffWall,		// ���_�o�b�t�@�C���^�[�t�F�[�X�ւ̃|�C���^
												NULL)))						// NULL�ɐݒ�
	{
        return E_FAIL;
	}

	{//���_�o�b�t�@�̒��g�𖄂߂�
		VERTEX_3D *pVtx;

		// ���_�f�[�^�͈̔͂����b�N���A���_�o�b�t�@�ւ̃|�C���^���擾
		g_pD3DVtxBuffWall->Lock(0, 0, (void**)&pVtx, 0);

		// ���_���W�̐ݒ�
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

		// �@���x�N�g���̐ݒ� �����v�Z���Ȃ�
		pVtx[0].nor = pVtx[1].nor = 
		pVtx[2].nor = pVtx[3].nor = 
		pVtx[4].nor = pVtx[5].nor = 
		pVtx[6].nor = pVtx[7].nor = 
		pVtx[8].nor = pVtx[9].nor = D3DXVECTOR3(0.0f, 0.0f, 0.0f);


		// ���ˌ��̐ݒ�
		pVtx[0].diffuse = pVtx[1].diffuse = 
		pVtx[2].diffuse = pVtx[3].diffuse = 
		pVtx[4].diffuse = pVtx[5].diffuse = 
		pVtx[6].diffuse = pVtx[7].diffuse = 
		pVtx[8].diffuse = pVtx[9].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);


		// �e�N�X�`�����W�̐ݒ�
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


		// ���_�f�[�^���A�����b�N����
		g_pD3DVtxBuffWall->Unlock();
	}

	return S_OK;
}







