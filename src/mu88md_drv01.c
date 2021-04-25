//*****************************************************************************
/**
 * @file mu88md_drv_req.c
 * @brief ドライバ本体外の関数
 * @note ゲームからのリクエスト関係
 * @note デバッグ表示関係
 */
//*****************************************************************************

//! ---------------------------------------------------------------------------
//! Include File
//!
#include "mu88md_drv.h"

#include "_mu88md_mub.h"
#include "_mu88md_se.h"

//! ---------------------------------------------------------------------------
//! Forword
void Mu88md_ReqWorkInit();
void Mu88md_FrameReset();
void Mu88md_FrameFinish();

void Mu88md_MubReq(u8 mub_no);
void Mu88md_MubStop();
void Mu88md_MubFadeOut();
void Mu88md_PcmReq(u8 req_no, u8 ch_no);
void Mu88md_DebugQueReq(u8 param);
void Mu88md_SePackReq(u8 set_no);
void Mu88md_SeReq(u8 se_no, u8 channel);


#if DEBUG_LOG_USE
void KLog_U1h(char *str, u32 num);
void KLog_U2h(char *str, u32 num, u32 num2);
#endif

//*****************************************************************************
/**
 * @note ゲームからのリクエスト
**/
//*****************************************************************************

/*
 * @fn void Mu88md_ReqWorkInit()
 * @brief リクエストシステムの初期化（未使用）
*/
void Mu88md_ReqWorkInit()
{
	memset(&Mu88md_ReqWork, 0, sizeof(MUCOM88MD_REQ_WORK));
}

/**
 * @fn void Mu88md_FrameReset()
 * @brief ゲームloopの最初でリクエストのワークを初期化
 * @note クリア個所の整理をする
*/
void Mu88md_FrameReset()
{
	Mu88md_ReqWork.req_finish = 0;		// リクエストなし
	memset(&Mu88md_ReqWork, 0, sizeof(MUCOM88MD_REQ_WORK));
}

/**
 * @fn void Mu88md_FrameFinish()
 * @brief ゲームloopの最後でリクエスト登録完了フラグを立てる
 * @note クリア個所の整理をする
*/
void Mu88md_FrameFinish()
{
	SYS_disableInts();
	Mu88md_ReqWork.req_finish = 1;
	SYS_enableInts();
}

/**
* @fn void Mu88md_MubReq(u8 mub_no)
* @brief Mubの演奏リクエスト登録
* @param[in] mub_no リクエストmubのID
*/
void Mu88md_MubReq(u8 mub_no)
{
	MUBHED mc68k_mub_head;

	// リクエストを登録
	Mu88md_ReqWork.mub_req_flag = 1;
	Mu88md_ReqWork.mub_req_no = mub_no;

	// 譜面本体のアドレスを取得
	MUBHED *mubhead = (MUBHED *)Mucom88md_mub_tbl[mub_no];
	SwapMubHeaderEndian(mubhead, &mc68k_mub_head);
	u8 *mubp = (u8 *)mubhead;
	mubp = mubp + mc68k_mub_head.dataoffset;

	Mu88md_ReqWork.mub_bank = (u32)mubp;
	Mu88md_ReqWork.mub_req_command = MUB_REQ_START;
	Mu88md_ReqWork.mub_req_param = 0; // 用途未定
}

/**
 * @fn void Mu88md_MubStop()
 * @brief Mubの演奏停止クエスト登録
*/
void Mu88md_MubStop()
{
	// リクエストを登録
	Mu88md_ReqWork.mub_req_flag = 1;
	Mu88md_ReqWork.mub_req_command = MUB_REQ_STOP;
}

/**
 * @fn void Mu88md_MubFadeOut()
 * @brief Mubのフェードアウトリクエスト
*/
void Mu88md_MubFadeOut()
{
	// リクエストを登録
	Mu88md_ReqWork.mub_req_flag = 1;
	Mu88md_ReqWork.mub_req_command = MUB_REQ_FADEOUT;
}

/**
 * @fn Mu88md_PcmReq(u8 req_no, u8 ch_no) 
 * @brief PCMの発声リクエスト登録
 * @param[in] req_no PCMのリクエストNo.
 * @param[in] ch_no 発声Channel 0 - 1
*/
void Mu88md_PcmReq(u8 req_no, u8 ch_no)
{
	Mu88md_ReqWork.pcm_req_flag[ch_no] = 1;
	Mu88md_ReqWork.pcm_req_no[ch_no] = req_no;
	Mu88md_ReqWork.pcm_req_prio[ch_no] = 1;
}

/**
 * @fn void Mu88md_MubReq(u8 mub_no)
 * @brief デバッグ用Mubの5倍速早送りリクエストを登録
**/
void Mu88md_DebugQueReq(u8 param)
{
	Mu88md_ReqWork.cue = param;
}

/**
 * @fn void Mu88md_SePackReq(u8 set_no)
 * @brief 指定のSEセットをまとめてリクエストする
 * @param[in] set_no リクエストSEセットのID
**/
void Mu88md_SePackReq(u8 set_no)
{
	u32	lp;
	const SE_REQ_TBL *ptr = &Mucom88md_SeSetTbl[set_no];

	for( lp = 0 ; lp < 3 ; lp++){
		if( ptr->req_no[lp] != MUB_SEID_NONE ){
			Mu88md_SeReq(ptr->req_no[lp], lp);
		}
	}
}

/**
 * @fn void Mu88md_SeReq(u8 se_no, u8 channel)
 * @brief 指定のSEを指定のCHANNELにリクエスト
 * @param[in] se_no リクエストSEのID
 * @param[in] channnel 0-2のDCSGチャンネルID
**/
void Mu88md_SeReq(u8 se_no, u8 channel)
{
	Mu88md_ReqWork.Mucom88mdz_SeReqArray[SE_REQ_COMMAND][channel] = SE_REQ_NML;
	Mu88md_ReqWork.Mucom88mdz_SeReqArray[SE_REQ_NO][channel] = se_no;
}

//*****************************************************************************
/**
 * @note デバッグ用サブルーチン
**/
#if DEBUG_LOG_USE
void KLog_U1h(char *str, u32 num);
void KLog_U2h(char *str, u32 num, u32 num2);
#endif

#if DEBUG_LOG_USE
/**
 * @fn static void KLog_U1h(char *str, u32 num)
 * @brief パラメータ付きLog出力
**/
void KLog_U1h(char *str, u32 num)
{
	char tmp[16];
	char tmp2[32];

	memset(tmp, 0, 16);
	memset(tmp2, 0, 32);
	if (num < 255)
	{
		intToHex(num, tmp, 2);
	}
	else if (num < 65535)
	{
		intToHex(num, tmp, 4);
	}
	else
	{
		intToHex(num, tmp, 8);
	}
	strcpy(tmp2, str);
	strcat(tmp2, "0x");
	strcat(tmp2, tmp);

	KLog(tmp2);
}
#endif



#if DEBUG_LOG_USE
/**
 * @fn static void KLog_U2h(char *str, u32 num, u32 num2)
 * @brief 2パラメータ付きLog出力
**/
void KLog_U2h(char *str, u32 num, u32 num2)
{
	char tmp[16];
	char tmp2[32];

	memset(tmp, 0, 16);
	memset(tmp2, 0, 32);
	if (num < 255)
	{
		intToHex(num, tmp, 2);
	}
	else if (num < 65535)
	{
		intToHex(num, tmp, 4);
	}
	else
	{
		intToHex(num, tmp, 8);
	}
	strcpy(tmp2, str);
	strcat(tmp2, "0x");
	strcat(tmp2, tmp);

	memset(tmp, 0, 16);
	if (num2 < 255)
	{
		intToHex(num2, tmp, 2);
	}
	else if (num2 < 65535)
	{
		intToHex(num2, tmp, 4);
	}
	else
	{
		intToHex(num2, tmp, 8);
	}
	strcat(tmp2, ",0x");
	strcat(tmp2, tmp);

	KLog(tmp2);
}
#endif

//***************************************************************************
// End of file
//***************************************************************************