//=======================================================================================
//
//	  定数定義、マクロ定義
//
//=======================================================================================

#ifndef _H_DEFINE
#define _H_DEFINE

//-------- マクロ
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x)			if(x){ x->Release(); x=NULL; }
#endif
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(x)	if(x){ delete[] x; x=NULL; }
#endif

//-------- 定数定義
#define CLASS_NAME		_T("NewPaddle")		// ウィンドウクラス名
#define CAPTION_NAME	_T("水たまり改")	// キャプション名

#define SCREEN_WIDTH	1024		// スクリーンの幅
#define SCREEN_HEIGHT	768			// スクリーンの高さ
#define FRAME_RATE		(1000/60)	// フレームレート

#define ASPECT_RATIO	((float)SCREEN_WIDTH/SCREEN_HEIGHT)	// アスペクト比
#define NEAR_CLIP		1.0f								// ニアクリップを行う距離
#define FAR_CLIP		30000.0f							// ファークリップを行う距離

#endif

//=======================================================================================
//	End of File
//=======================================================================================