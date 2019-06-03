//=============================================================================
//
// 背景関連処理 [field.h]
// Author : 
//
//=============================================================================
#ifndef _FIELD_H_
#define _FIELD_H_

#include "main.h"

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitField(void);
void UninitField(void);
void UpdateField(void);
void DrawField(void);

HRESULT InitWall(void);
void UninitWall(void);
void UpdateWall(void);
void DrawWall(void);





#endif
