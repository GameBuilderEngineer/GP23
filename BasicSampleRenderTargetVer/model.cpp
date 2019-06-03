//=============================================================================
//
// ���f������ [model.cpp]
// Author : 
//
//=============================================================================
#include "model.h"
#include "input.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_CAR		"data/MODEL/ball.x"						// �ǂݍ��ރ��f����
#define TEXTURE_FILENAME "data/TEXTURE/�J�[�r�[.dds"
#define TEXTURE_FILENAME2 "data/TEXTURE/�����ӂ��[.dds"
#define	POS_CAMERA_P_X	(0.0f)											// �J�������_�̏����ʒu(X���W)
#define	POS_CAMERA_P_Y	(2.0f)											// �J�������_�̏����ʒu(Y���W)
#define	POS_CAMERA_P_Z	(-3.0f)										// �J�������_�̏����ʒu(Z���W)
#define	VIEW_ANGLE		(D3DXToRadian(45.0f))							// �r���[���ʂ̎���p
#define	VIEW_ASPECT		((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT)	// �r���[���ʂ̃A�X�y�N�g��	
#define	VIEW_NEAR_Z		(1.0f)											// �r���[���ʂ�NearZ�l
#define	VIEW_FAR_Z		(1000.0f)										// �r���[���ʂ�FarZ�l

#define	VALUE_MOVE		(5.0f)											// �ړ���
#define	VALUE_ROTATE	(D3DX_PI * 0.02f)								// ��]��

#define	NUM_LIGHT		(3)												// ���C�g�̐�
#define DELTA_ANGLE 0.05f

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void InitCamera(void);
void InitLight(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
LPDIRECT3DTEXTURE9	g_pD3DTextureModel;		// �e�N�X�`���ւ̃|�C���^
LPDIRECT3DTEXTURE9	g_pD3DTextureModel2;	// �e�N�X�`��2�ւ̃|�C���^
LPD3DXMESH			g_pD3DXMeshModel;		// ���b�V�����ւ̃|�C���^
LPD3DXBUFFER		g_pD3DXBuffMatModel;	// �}�e���A�����ւ̃|�C���^
DWORD				g_nNumMatModel;			// �}�e���A�����̐�

D3DXVECTOR3			g_posModel;				// ���f���̈ʒu
D3DXVECTOR3			g_rotModel;				// ���f���̌���(��])
D3DXVECTOR3			g_sclModel;				// ���f���̑傫��(�X�P�[��)

D3DXVECTOR3			g_posCameraP;			// �J�����̎��_
D3DXVECTOR3			g_posCameraR;			// �J�����̒����_
D3DXVECTOR3			g_vecCameraU;			// �J�����̏�����x�N�g��

D3DXMATRIX			g_mtxView;				// �r���[�}�g���b�N�X
D3DXMATRIX			g_mtxProjection;		// �v���W�F�N�V�����}�g���b�N�X
D3DXMATRIX			g_mtxWorld;				// ���[���h�}�g���b�N�X

D3DLIGHT9			g_aLight[NUM_LIGHT];	// ���C�g���
//D3DXQUATERNION		quaternion(-1.0f, -1.0f, 0.0f,1);	// �N�H�[�^�j�I��
D3DXQUATERNION		qtnLight[NUM_LIGHT];

//=============================================================================
// ����������
//=============================================================================
HRESULT InitModel(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// �J�����̏�����
	InitCamera();

	// ���C�g�̏�����
	InitLight();

	// �ʒu�E��]�E�X�P�[���̏����ݒ�
	g_posModel = D3DXVECTOR3(0.0f, 2.0f, 0.0f);
	g_rotModel = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_sclModel = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

	g_pD3DTextureModel = NULL;
	g_pD3DTextureModel2 = NULL;
	g_pD3DXMeshModel = NULL;
	g_pD3DXBuffMatModel = NULL;
	g_nNumMatModel = 0;

	// X�t�@�C���̓ǂݍ���
	if(FAILED(D3DXLoadMeshFromX(MODEL_CAR,				// �ǂݍ��ރ��f���t�@�C����(X�t�@�C��)
								D3DXMESH_SYSTEMMEM,		// ���b�V���̍쐬�I�v�V�������w��
								pDevice,				// IDirect3DDevice9�C���^�[�t�F�C�X�ւ̃|�C���^
								NULL,					// �אڐ��f�[�^���܂ރo�b�t�@�ւ̃|�C���^
								&g_pD3DXBuffMatModel,	// �}�e���A���f�[�^���܂ރo�b�t�@�ւ̃|�C���^
								NULL,					// �G�t�F�N�g�C���X�^���X�̔z����܂ރo�b�t�@�ւ̃|�C���^
								&g_nNumMatModel,		// D3DXMATERIAL�\���̂̐�
								&g_pD3DXMeshModel)))	// ID3DXMesh�C���^�[�t�F�C�X�ւ̃|�C���^�̃A�h���X
	{
		return E_FAIL;
	}

#if 1
	// �e�N�X�`���̓ǂݍ���
	D3DXCreateTextureFromFile(pDevice,					// �f�o�C�X�ւ̃|�C���^
								TEXTURE_FILENAME,		// �t�@�C���̖��O
								&g_pD3DTextureModel);	// �ǂݍ��ރ������[
	// �e�N�X�`���̓ǂݍ���
	D3DXCreateTextureFromFile(pDevice,					// �f�o�C�X�ւ̃|�C���^
								TEXTURE_FILENAME2,		// �t�@�C���̖��O
								&g_pD3DTextureModel2);	// �ǂݍ��ރ������[
#endif
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	//�X�e�[�W0�̐ݒ�
	pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);				//�X�e�[�W�J���[
	pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_CURRENT);				//ARG1�J���[
	pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);				//ARG2�J���[
	pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);			//�X�e�[�W�A���t�@
	pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_CURRENT);				//ARG2�A���t�@
	pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);				//ARG2�A���t�@
	//�X�e�[�W1�̐ݒ�
	pDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 0);						//UV���W�̐ݒ���X�e�[�W�O�̐ݒ�Ƌ��ʂɂ���B
	pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE);				//�X�e�[�W�J���[
	pDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);				//ARG1�J���[
	pDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_TEXTURE);				//ARG2�J���[
	pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_MODULATE);			//�X�e�[�W�A���t�@
	pDevice->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_CURRENT);				//ARG2�A���t�@
	pDevice->SetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);				//ARG2�A���t�@
#if 0
	//�X�e�[�W1�̐ݒ�																				//�X�e�[�W1�̐ݒ�
	pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_BLENDTEXTUREALPHA);		//�X�e�[�W�J���[
	pDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);				//ARG2�J���[
	pDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);				//ARG1�J���[
	pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);			//�X�e�[�W�A���t�@
	pDevice->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_CURRENT);				//ARG1�A���t�@
	pDevice->SetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);				//ARG2�A���t�@
#endif

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitModel(void)
{
	if(g_pD3DTextureModel != NULL)
	{// �e�N�X�`���̊J��
		g_pD3DTextureModel->Release();
		g_pD3DTextureModel = NULL;
	}
	if(g_pD3DTextureModel2 != NULL)
	{// �e�N�X�`���̊J��
		g_pD3DTextureModel2->Release();
		g_pD3DTextureModel2 = NULL;
	}

	if(g_pD3DXMeshModel != NULL)
	{// ���b�V���̊J��
		g_pD3DXMeshModel->Release();
		g_pD3DXMeshModel = NULL;
	}

	if(g_pD3DXBuffMatModel != NULL)
	{// �}�e���A���̊J��
		g_pD3DXBuffMatModel->Release();
		g_pD3DXBuffMatModel = NULL;
	}
}
float value = 0.01;
//=============================================================================
// �X�V����
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
	//sin(1/2��)*(0,1,0)
	qtnDelta.x = 0;
	qtnDelta.y = sin(-DELTA_ANGLE / 2.0f);
	qtnDelta.z = 0;
	qtnDelta.w = cos(-DELTA_ANGLE / 2.0f);
	for (int i = 0; i < NUM_LIGHT; i++)
	{
		qtnLight[i] *= qtnDelta;
		D3DXVECTOR3 vecDir;
		// ���C�g2�̕����̐ݒ�
		vecDir = D3DXVECTOR3(qtnLight[i].x, qtnLight[i].y, qtnLight[i].z);
		D3DXVec3Normalize((D3DXVECTOR3*)&g_aLight[i].Direction, &vecDir);
	}




	// ���C�g�������_�����O�p�C�v���C���ɐݒ�
	for (int i = 0; i < NUM_LIGHT; i++)
	{
		pDevice->SetLight(i, &g_aLight[i]);
	}
	g_rotModel.y += D3DXToRadian( 0.1f );

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawModel(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	D3DXMATRIX mtxScl, mtxRot, mtxTranslate;
	D3DXMATERIAL *pD3DXMat;
	D3DMATERIAL9 matDef;

	// �r���[�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&g_mtxView);

	// �r���[�}�g���b�N�X�̍쐬
	D3DXMatrixLookAtLH(&g_mtxView, 
						&g_posCameraP,		// �J�����̎��_
						&g_posCameraR,		// �J�����̒����_
						&g_vecCameraU);		// �J�����̏�����x�N�g��

	// �r���[�}�g���b�N�X�̐ݒ�
	pDevice->SetTransform(D3DTS_VIEW, &g_mtxView);


	// �v���W�F�N�V�����}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&g_mtxProjection);

	// �v���W�F�N�V�����}�g���b�N�X�̍쐬
	D3DXMatrixPerspectiveFovLH(&g_mtxProjection,
								VIEW_ANGLE,			// �r���[���ʂ̎���p
								VIEW_ASPECT,		// �r���[���ʂ̃A�X�y�N�g��
								VIEW_NEAR_Z,		// �r���[���ʂ�NearZ�l
								VIEW_FAR_Z);		// �r���[���ʂ�FarZ�l

	// �v���W�F�N�V�����}�g���b�N�X�̐ݒ�
	pDevice->SetTransform(D3DTS_PROJECTION, &g_mtxProjection);

	// ���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&g_mtxWorld);

	// �X�P�[���𔽉f
	D3DXMatrixScaling(&mtxScl, g_sclModel.x, g_sclModel.y, g_sclModel.z);
	D3DXMatrixMultiply(&g_mtxWorld, &g_mtxWorld, &mtxScl);

	// ��]�𔽉f
	D3DXMatrixRotationYawPitchRoll(&mtxRot, g_rotModel.y, g_rotModel.x, g_rotModel.z);
	D3DXMatrixMultiply(&g_mtxWorld, &g_mtxWorld, &mtxRot);

	// �ړ��𔽉f
	D3DXMatrixTranslation(&mtxTranslate, g_posModel.x, g_posModel.y, g_posModel.z);
	D3DXMatrixMultiply(&g_mtxWorld, &g_mtxWorld, &mtxTranslate);

	// ���[���h�}�g���b�N�X�̐ݒ�
	pDevice->SetTransform(D3DTS_WORLD, &g_mtxWorld);

	//
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);


	// ���݂̃}�e���A�����擾
	pDevice->GetMaterial(&matDef);

	// �}�e���A�����ɑ΂���|�C���^���擾
	pD3DXMat = (D3DXMATERIAL*)g_pD3DXBuffMatModel->GetBufferPointer();

	for(int nCntMat = 0; nCntMat < (int)g_nNumMatModel; nCntMat++)
	{
		// �}�e���A���̐ݒ�
		pDevice->SetMaterial(&pD3DXMat[nCntMat].MatD3D);

		// �e�N�X�`���̐ݒ�
		pDevice->SetTexture(0, g_pD3DTextureModel);

		// �e�N�X�`��2�̐ݒ�
		pDevice->SetTexture(1, g_pD3DTextureModel2);

		// �`��
		g_pD3DXMeshModel->DrawSubset(nCntMat);
	}

	//�X�e�[�W�P�̃e�N�X�`�����O��
	pDevice->SetTexture(1, NULL);

	// �}�e���A�����f�t�H���g�ɖ߂�
	pDevice->SetMaterial(&matDef);
}

//=============================================================================
// �J�����̏���������
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
// ���C�g�̏���������
//=============================================================================
void InitLight(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	D3DXVECTOR3 vecDir;

	// D3DLIGHT9�\���̂�0�ŃN���A����
	ZeroMemory(&g_aLight[0], sizeof(D3DLIGHT9));

	// ���C�g0�̃^�C�v�̐ݒ�
	g_aLight[0].Type = D3DLIGHT_DIRECTIONAL;

	// ���C�g0�̊g�U���̐ݒ�
	g_aLight[0].Diffuse = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);

	// ���C�g0�̊����̐ݒ�
	g_aLight[0].Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	// ���C�g0�̕����̐ݒ�
	//vecDir = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	qtnLight[0] = D3DXQUATERNION(0.0f, 1.0f, 0.0f, 1.0f);
	vecDir = D3DXVECTOR3(qtnLight[0].x, qtnLight[0].y, qtnLight[0].z);
	D3DXVec3Normalize((D3DXVECTOR3*)&g_aLight[0].Direction, &vecDir);

	// ���C�g0�������_�����O�p�C�v���C���ɐݒ�
	pDevice->SetLight(0, &g_aLight[0]);

	// ���C�g0���g�p�g�p��Ԃ�
	pDevice->LightEnable(0, TRUE);

	
	// D3DLIGHT9�\���̂�0�ŃN���A����
	ZeroMemory(&g_aLight[1], sizeof(D3DLIGHT9));

	// ���C�g1�̃^�C�v�̐ݒ�
	g_aLight[1].Type = D3DLIGHT_DIRECTIONAL;

	// ���C�g1�̊g�U���̐ݒ�
	g_aLight[1].Diffuse = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);

	// ���C�g1�̊����̐ݒ�
	g_aLight[1].Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

    // ���C�g1�̕����̐ݒ�
	//vecDir = D3DXVECTOR3(1.0f, -1.0f, 0.0f);
	qtnLight[1] = D3DXQUATERNION(1.0f, -1.0f, 0.0f, 1.0f);
	vecDir = D3DXVECTOR3(qtnLight[1].x, qtnLight[1].y, qtnLight[1].z);
	D3DXVec3Normalize((D3DXVECTOR3*)&g_aLight[1].Direction, &vecDir);

	// ���C�g1�������_�����O�p�C�v���C���ɐݒ�
	pDevice->SetLight(1, &g_aLight[1]);

	// ���C�g1�̐ݒ�
	pDevice->LightEnable(1, TRUE);

	// ���C�e�B���O���[�h��ON
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	// D3DLIGHT9�\���̂�0�ŃN���A����
	ZeroMemory(&g_aLight[2], sizeof(D3DLIGHT9));

	// ���C�g2�̃^�C�v�̐ݒ�
	g_aLight[2].Type = D3DLIGHT_DIRECTIONAL;

	// ���C�g2�̊g�U���̐ݒ�
	g_aLight[2].Diffuse = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);

	// ���C�g2�̊����̐ݒ�
	g_aLight[2].Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

    // ���C�g2�̕����̐ݒ�
	//vecDir = D3DXVECTOR3(-1.0f, -1.0f, 0.0f);
	qtnLight[2] = D3DXQUATERNION(-1.0f, -1.0f, 0.0f, 1.0f);
	vecDir = D3DXVECTOR3(qtnLight[2].x, qtnLight[2].y, qtnLight[2].z);
	D3DXVec3Normalize((D3DXVECTOR3*)&g_aLight[2].Direction, &vecDir);

	// ���C�g2�������_�����O�p�C�v���C���ɐݒ�
	pDevice->SetLight(2, &g_aLight[2]);

	// ���C�g2�̐ݒ�
	pDevice->LightEnable(2, TRUE);

	// ���C�e�B���O���[�h��ON
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
}

