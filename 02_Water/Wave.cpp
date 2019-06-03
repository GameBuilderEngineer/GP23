//=======================================================================================
//
//	  �g�֘A���W���[��
//
//=======================================================================================
//-------- �C���N���[�h�w�b�_
#define STRICT
#include <windows.h>		// Windows�v���O�����ɂ͂����t����
#include <tchar.h>			// �ėp�e�L�X�g �}�b�s���O
#include <stdio.h>			// �����񏈗��ŕK�v
#include <d3d9.h>			// DirectX Graphics �֘A�̃w�b�_
#include <d3dx9.h>			// DirectX Graphics �֘A�̃w�b�_

#include "define.h"
#include "common.h"
#include "proto.h"

//-------- ���_�t�H�[�}�b�g
typedef struct {
	D3DXVECTOR3	pos;
	D3DXVECTOR3	nor;
} VERTEX;

#define FVF_VERTEX	(D3DFVF_XYZ|D3DFVF_NORMAL)//|D3DFVF_DIFFUSE|D3DFVF_SPECULAR)

#define MAX_WAVE	100
#define MAX_INDEX	((MAX_WAVE+1)*(MAX_WAVE-1)*2-2)

VERTEX	vertex[3][MAX_WAVE][MAX_WAVE];
int		num;
WORD	index[MAX_INDEX];

#define MAX_NORMAL_X	((MAX_WAVE-1)*2)
#define MAX_NORMAL_Y	(MAX_WAVE-1)

D3DXVECTOR3	normal[MAX_NORMAL_Y][MAX_NORMAL_X];

//---------------------------------------------------------------------------------------
//  �g������
//---------------------------------------------------------------------------------------
HRESULT InitWave()
{
	int	i, j, k = 0;
	for (j = 0; j < MAX_WAVE; j++) {
		for (i = 0; i < MAX_WAVE; i++) {
			// ���_�l��ݒ�
			vertex[0][j][i].pos.x = (i - (MAX_WAVE / 2)) * 1.0f;
			vertex[0][j][i].pos.y = 0.0f;
			vertex[0][j][i].pos.z = (((MAX_WAVE - 1) - j) - (MAX_WAVE / 2)) * 1.0f;
			vertex[0][j][i].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
			vertex[2][j][i] = vertex[1][j][i] = vertex[0][j][i];
			// �C���f�b�N�X�l��ݒ�
			if (j < MAX_WAVE - 1) {
				index[k++] = (j + 1) * MAX_WAVE + i;
				index[k++] = j * MAX_WAVE + i;
			}
		}
		// �k�ގO�p�`��ݒ�
		if (j < MAX_WAVE - 2) {
			index[k++] = j * MAX_WAVE + MAX_WAVE - 1;
			index[k++] = (j + 2) * MAX_WAVE;
		}
	}
	// ���݂̒��_
	num = 0;
	// �g�𔭐�
	vertex[num][rand() % MAX_WAVE][rand() % MAX_WAVE].pos.y = 2.0f;
	return S_OK;
}

//---------------------------------------------------------------------------------------
// �g�X�V
//---------------------------------------------------------------------------------------
void ExecWave()
{
	// �ŏ���10�b�ԁA�g�𐶐�
	if (g_dwTicks < 10 * 1000 && (g_dwTicks % (5 * 1000)) == 0) {
		vertex[num][rand() % MAX_WAVE][rand() % MAX_WAVE].pos.y += 2.0f;
	}
	// ���O�A����̃t���[�����_�擾
	int prev = (num + 2) % 3;
	int next = (num + 1) % 3;
	// �g�̑��x�A���Ԃ̍��݁A�O���b�h��
	const float c = 0.4f;
	const float dt = 0.4f;
	const float h = 1.0f;
	const float cth = (c * c * dt * dt) / (h * h);

	// �g�̍������X�V
	for (int j = 1; j < MAX_WAVE - 1; j++)
	{
		for (int i = 1; i < MAX_WAVE - 1; i++)
		{
			vertex[next][j][i].pos.y =
				cth *
				(vertex[num][j][i + 1].pos.y +
					vertex[num][j][i - 1].pos.y +
					vertex[num][j + 1][i].pos.y +
					vertex[num][j - 1][i].pos.y) +
					(2-4*cth)*vertex[num][j][i].pos.y -
						vertex[prev][j][i].pos.y;
		}
	}



	//-------------------�����֌v�Z�������쐬

	



	// �|���S���@���̍X�V
	for (int j = 0; j < MAX_WAVE - 1; j++) {
		for (int i = 0; i < MAX_WAVE - 1; i++) {
			D3DXVECTOR3& p0 = vertex[next][j + 1][i].pos;
			D3DXVECTOR3& p1 = vertex[next][j][i].pos;
			D3DXVECTOR3& p2 = vertex[next][j + 1][i + 1].pos;
			D3DXVECTOR3& n0 = normal[j][i * 2];
			D3DXVec3Cross(&n0, &(p1 - p0), &(p2 - p1));
			D3DXVec3Normalize(&n0, &n0);
			D3DXVECTOR3& q0 = vertex[next][j][i].pos;
			D3DXVECTOR3& q1 = vertex[next][j][i + 1].pos;
			D3DXVECTOR3& q2 = vertex[next][j + 1][i + 1].pos;
			D3DXVECTOR3& n1 = normal[j][i * 2 + 1];
			D3DXVec3Cross(&n1, &(q1 - q0), &(q2 - q1));
			D3DXVec3Normalize(&n1, &n1);
		}
	}
	//�@���ӂ̖@���Ƃ̕��ω�
	for (int j = 0; j < MAX_WAVE; j++) {
		for (int i = 0; i < MAX_WAVE; i++) {
			D3DXVECTOR3& n0 = normal[(j + MAX_NORMAL_Y - 1) % MAX_NORMAL_Y][(i * 2 + MAX_NORMAL_X - 2) % MAX_NORMAL_X];
			D3DXVECTOR3& n1 = normal[(j + MAX_NORMAL_Y - 1) % MAX_NORMAL_Y][(i * 2 + MAX_NORMAL_X - 1) % MAX_NORMAL_X];
			D3DXVECTOR3& n2 = normal[(j + MAX_NORMAL_Y - 1) % MAX_NORMAL_Y][i * 2];
			D3DXVECTOR3& n3 = normal[j][(i * 2 + MAX_NORMAL_X - 1) % MAX_NORMAL_X];
			D3DXVECTOR3& n4 = normal[j][i * 2];
			D3DXVECTOR3& n5 = normal[j][(i * 2 + 1) % MAX_NORMAL_X];
			D3DXVec3Normalize(&vertex[next][j][i].nor, &(n0 + n1 + n2 + n3 + n4 + n5));
		}
	}


	// �Q�ƃt���[�����X�V
	num = next;
}

//---------------------------------------------------------------------------------------
// �g�`��
//---------------------------------------------------------------------------------------
void DrawWave()
{
	// ���[���h�}�g���b�N�X�ݒ�
	D3DXMATRIX	matWorld;
	D3DXMatrixIdentity(&matWorld);
	g_pD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);

	// �}�e���A���ݒ�
	D3DMATERIAL9 material;
	material.Diffuse.r = 0.25f;
	material.Diffuse.g = 0.8f;
	material.Diffuse.b = 1.0f;
	material.Diffuse.a = 0.5f;
	material.Ambient = material.Diffuse;
	material.Specular = material.Diffuse;
	material.Power = 1.0f;
	material.Emissive.r = 0.0f;
	material.Emissive.g = 0.0f;
	material.Emissive.b = 0.0f;
	material.Emissive.a = 0.0f;
	g_pD3DDevice->SetMaterial(&material);
	g_pD3DDevice->SetTexture(0, NULL);

	// ���_�t�H�[�}�b�g�t���O��ݒ�
	g_pD3DDevice->SetFVF(FVF_VERTEX);

	// �`������s
	g_pD3DDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLESTRIP, 0, MAX_WAVE * MAX_WAVE,
										 (MAX_WAVE + 1) * (MAX_WAVE - 1) * 2 - 4,
										 index, D3DFMT_INDEX16,
										 vertex[num], sizeof(VERTEX));
}

//=======================================================================================
//	End of File
//=======================================================================================
