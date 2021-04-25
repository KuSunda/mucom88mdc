//*****************************************************************************
/**
 * @file mucom88mdc_drv_.c
 * @file mu88md_drv.c
 * @brief mucom88 MegaDrive/Genesis版
 *        68K:SGDK C(FM/DCSG/PCM/1080*2SE)
 *        Z80:PCMドライバ
 * @author Original Yuzo Koshiro
 *         ReProgramed KuSunda
 * @note   .mub FM5/PCM/DCSG3(NOISE) / SE DCSG 3 / PCM 2
 */
//*****************************************************************************

//! ---------------------------------------------------------------------------
//! Include File
//!
#include "mu88md_drv.h"
#include "mu88md_pcm_drv.h"


#include "mu88md_asub.h"

#include "_mu88md_pcm.h"
#include "_mu88md_se.h"

//! ---------------------------------------------------------------------------
//! System Define
#define USE_FM6_CH			(0)		// FM6CHのMMLを処理する
#define USE_FM6_PCM_SWITCH	(0)		// FM6をDAC/FMを動的切り替えを行う。OFFならDAC固定。

#define USE_PCM				(1)		// PCMを有効にする

//! ---------------------------------------------------------------------------
//! Develop Define
//!
#define USE_MUCOM88_DEBUG	(1) 	// DCSG RangeCheck / CUE
#define JOB_PEAK_TIME		(20)

#define OBSOLETE_CODE		(0)		// PC88用のメモ用コード
#define OPTIMIZ_TEST		(1)		// 最適化コード（動作確認用に戻せる設定）


#define PSGOUT_ASM			(1)		// アセンブラ版PSGOUT
#define PSGOUT_ASM2			(1)		// アセンブラ最適化版 FM音源の音色変更

//! ---------------------------------------------------------------------------
//! Prototype
//!
#if USE_MUCOM88_DEBUG
static u8 CUE();
#endif

void Mu88md_SystemAllInit();
void Mu88_CallBackFunc();
static void request_copy_to_driver();
static void mub_request_check();
static void mub_request_start();
static void mub_adress_set();
static void mub_request_stop();
static void mub_request_fadeout();
static void MSTART();
static void START();
static void MSTOP();
static void WORKINIT();
static void PL_SND();
static void DRIVE();
static void AKYOFF();
static void PSGOUT(u8 reg, u8 param);
static void PSGOUT2(u8 reg, u8 param);
static void MONO();
static void FMENT(FM_CH_WORK *ix);
static void FMSUB(FM_CH_WORK *ix);
static void FMSUB0(FM_CH_WORK *ix);
static void FMSUB1(FM_CH_WORK *ix);
static void FMSUB5(FM_CH_WORK *ix);
static u8 FMSUB4(FM_CH_WORK *ix);
static void FMSUB9(FM_CH_WORK *ix, u8 b);
static void PCMGFQ(FM_CH_WORK *ix);
static void FMGFQ(FM_CH_WORK *ix, u8 b);
static void EXMODE(FM_CH_WORK *ix);
static void KEYOFF(FM_CH_WORK *ix);
static void KEYON(FM_CH_WORK *ix);
static void STVOL(FM_CH_WORK *ix);
static void STV1(FM_CH_WORK *ix);
static void STV2(FM_CH_WORK *ix, u8 c);
static inline u8 ConvToneReg2DCSG(u8 ch_no);
static inline u8 ConvVolReg2DCSG(u8 channnel_no);
static void FMINIT(FM_CH_WORK *ix, u8 idx);
static void SSINIT(FM_CH_WORK *ix, u8 ch_no);
static void TO_NML();
static void TNML2(u8 param);
static void TO_EFC();
static void FMSUBA(FM_CH_WORK *ix, u8 cmd_a);
static void FMEND(FM_CH_WORK *ix);

static void FDOUT();
static void FDOFM(FM_CH_WORK *ix);
static void FDO3();

static void SETDEL(FM_CH_WORK *ix);
static void SETCO(FM_CH_WORK *ix);
static void SETVCT(FM_CH_WORK *ix);
static void SETVC2(FM_CH_WORK *ix);
static void SETPEK(FM_CH_WORK *ix);
static void LFOON2(FM_CH_WORK *ix);
static void LFOOFF(FM_CH_WORK *ix);

static void LFORST(FM_CH_WORK *ix);
static void LFORST2(FM_CH_WORK *ix);

static void PLLFO(FM_CH_WORK *ix);
static void PLLFO2(FM_CH_WORK *ix);
static void LFOP5(FM_CH_WORK *ix);
static void LFOP6(FM_CH_WORK *ix);

static void CTLFO(FM_CH_WORK *ix);
static void CTLFO1(FM_CH_WORK *ix);
static void PLS2(FM_CH_WORK *ix);
static void PLSKI2(FM_CH_WORK *ix);

static void FS2(FM_CH_WORK *ix);

static const u16 FNUMB[];
static u8 DETDAT[];
static const u8 OP_SEL[];

static const u8 FMVDAT[];
static const u8 CRYDAT[];

static const u8 ConvToneReg2DCSGTbl[];
static const u8 ConvVolReg2DCSGTbl[];

static void SSGSUB(FM_CH_WORK *ix);
static void SSGENT(FM_CH_WORK *ix);

static void SSSUB_VolRegSet(FM_CH_WORK *ix, u8 vol);

static void DCSGOUT(FM_CH_WORK *ix, u8 a);

static void SSUB0(FM_CH_WORK *ix);
static void SSSUB0(FM_CH_WORK *ix);
static void SOFENV(FM_CH_WORK *ix);
static void SOFEV2(FM_CH_WORK *ix);
static void SOFEV4(FM_CH_WORK *ix);
static void SOFEV9(FM_CH_WORK *ix);
static void SOFEV7(FM_CH_WORK *ix);

static void SSSUB_VolSet(FM_CH_WORK *ix);
static void SSSUB_VolSet00(FM_CH_WORK *ix, u8 vol);
static inline u8 ConvVol2DCSG(u8 vol88);

static void SSSUB8(FM_CH_WORK *ix, u8 cmd);
static void SSUBAB(FM_CH_WORK *ix);
static void SSSUBA(FM_CH_WORK *ix);

static void ENVPST(FM_CH_WORK *ix);
static void ENVPST2(FM_CH_WORK *ix, u8 *hl);
static void OTOSSG(FM_CH_WORK *ix);

static void VOLUPS(FM_CH_WORK *ix);
static void PSGVOL(FM_CH_WORK *ix);
static void PV1(FM_CH_WORK *ix, u8 vol, u8 env);
static void SSGOFF();
static void SSGOFF_FORCE(FM_CH_WORK *ix);
static void SKYOFF(FM_CH_WORK *ix);
static u8 ConvVolReg2DCSG(u8 channnel_no);

static inline u8 *OTOCAL(FM_CH_WORK *ix, u8 a);

static void NOISE(FM_CH_WORK *ix);
static void NOISEW(FM_CH_WORK *ix);

static void SSSUB3(FM_CH_WORK *ix, u8 vol);

static void SSSUB7(FM_CH_WORK *ix);
static void SSSUB6(FM_CH_WORK *ix);
static void SSSUBG(FM_CH_WORK *ix);
static void SSSUBH(FM_CH_WORK *ix);
static void SSSUB9(FM_CH_WORK *ix);

static void SETPT(FM_CH_WORK *ix);

static const u16 TP5BitToDcsg12BItTbl[];
static void SSSUB_ToneRegSet(FM_CH_WORK *ix, u16 hl);
static void SSSUB4(FM_CH_WORK *ix, u16 hl);
static void SSGEND(FM_CH_WORK *ix);
static u16 ConvSSGTone2DCSG(FM_CH_WORK *ix, u16 hl);

static const u8 SSGDAT[0x10][6];

static const u16 SNUMB[]; //;  SSG o1 の、周波数 DATA
static void LoopMubCheck();

static void PCMVOL(FM_CH_WORK *ix);
static void PCMEND(FM_CH_WORK *ix);
static void PCMVOL_MAX(FM_CH_WORK *ix);

static void DCSG_RangeCk(FM_CH_WORK *ix);
static void PLAY_PCM(FM_CH_WORK *ix);
static void OTOPCM(FM_CH_WORK *ix);

static u16 GetHL(u8 h, u8 l);
static u8 *GetDataAdress(u16 hl);
static u8 GetMmlData(u16 hl);

void SwapMubHeaderEndian(MUBHED *src, MUBHED *dst);
static u16 mub_flip16(u16 num);
static u32 mub_flip32(u32 num);

static u16 GetHL(u8 h, u8 l);
static void SetHL(u16 hl, u8 *h, u8 *l);
static u8 GetHfromHL(u16 hl);
static u8 GetLfromHL(u16 hl);

void z80_pcm_driver_load();
static void set_fm6_pcm_use_mode();
static void set_fm6_fm_use_mode();
static void pcm_request_drive();
static void pcm_request_core(u8 ch_no, u8 req_no);
static inline u32 get_pcm_data_adr(u8 pcm_tone_no);
static inline u32 get_pcm_data_size(u8 pcm_tone_no);

//; -----------------------------------------------------------------------------
//; FM　SubCommand Jump
//; -----------------------------------------------------------------------------
static void SECPRC(FM_CH_WORK *ix);
static void NTMEAN(FM_CH_WORK *ix);
#if OBSOLETE_CODE
static void STE2(FM_CH_WORK *ix);
#endif
static void VOLUPF(FM_CH_WORK *ix);
static void MDSET(FM_CH_WORK *ix);
static void HLFOON(FM_CH_WORK *ix);
static void LFOON(FM_CH_WORK *ix);
static void SETDEL(FM_CH_WORK *ix);
static void SETQ(FM_CH_WORK *ix);
static void OTOPST(FM_CH_WORK *ix);
static void VOLPST(FM_CH_WORK *ix);
#if OBSOLETE_CODE
static void VOLDRM(FM_CH_WORK *ix);
static void DVOLSET(FM_CH_WORK *ix);
#endif
static void FRQ_DF(FM_CH_WORK *ix);
static void PVMCHG(FM_CH_WORK *ix);
static void RSKIP(FM_CH_WORK *ix);
static void REPSTF(FM_CH_WORK *ix);
static void REPENF(FM_CH_WORK *ix);
static void REPENF2(FM_CH_WORK *ix);
static void STENV(FM_CH_WORK *ix);
static void W_REG(FM_CH_WORK *ix);

static void STEREO(FM_CH_WORK *ix);
static void STER2(FM_CH_WORK *ix);
static void FLGSET(FM_CH_WORK *ix);
static void RV1(FM_CH_WORK *ix);
static void REVERVE(FM_CH_WORK *ix);
static void REVMOD(FM_CH_WORK *ix);
static void REVSW(FM_CH_WORK *ix);
static void PVMCHG(FM_CH_WORK *ix);
static void HRDENV(FM_CH_WORK *ix);
static void ENVPOD(FM_CH_WORK *ix);

static void NTMEAN(FM_CH_WORK *ix);
static void TIE(FM_CH_WORK *ix);
static void FLGSET(FM_CH_WORK *ix);
static void LFOON3(FM_CH_WORK *ix, u8 a);

static void TSC_LOOPCK();

static void se_system_init();
static void se_request_drive();
static u8 se_drive_core(FM_CH_WORK *ix);
static void SeRequestCk();
static void FMINIT_SE(FM_CH_WORK *ix, u8 ch_no, u8 Req_no);

//! ---------------------------------------------------------------------------
//! --------------------------------------------------------------------------
//! Work
//! --------------------------------------------------------------------------
//! --------------------------------------------------------------------------

//! Channel Work
FM_CH_WORK fm_ch_work[6];
FM_CH_WORK pcm_ch_work[1];
FM_CH_WORK dcsg_ch_work[3];
FM_CH_WORK se_ch_work[3];

// System Work
static MUCOM88_SYS_WORK Mucom88SysWork;
MUCOM88_SYS_WORK *pSys;

// SE System
static MUCOM88_SE_SYS_WORK SeSysWork;
static MUCOM88_SE_SYS_WORK *pSeSys;

// mub/Se/pcm リクエスト
MUCOM88MD_REQ_WORK Mu88md_ReqWork;


//! 使用 FM_CH_WORK 一覧
#define MUB_CH_WORK_NUM (10)
static FM_CH_WORK *ch_work_list_tbl[MUB_CH_WORK_NUM] = {
	&fm_ch_work[0],	  // 1
	&fm_ch_work[1],	  // 2
	&fm_ch_work[2],	  // 3
	&dcsg_ch_work[0], // 6
	&dcsg_ch_work[1], // 7
	&dcsg_ch_work[2], // 8
					  // Rythm
	&fm_ch_work[3],	  // 4
	&fm_ch_work[4],	  // 5
	&fm_ch_work[5],	  // 5
					  // FM6
	&pcm_ch_work[0],  // 9
};

// FMINIT用パラメータ
static const u8 ch_init_type_tbl[MUB_CH_WORK_NUM][3] = {
	// Ch_Kind      mub_idx    set_ch_no
	{CH_TYPE_FM, 0, 0},	  //
	{CH_TYPE_FM, 1, 1},	  //
	{CH_TYPE_FM, 2, 2},	  //
	{CH_TYPE_DCSG, 3, 0}, //
	{CH_TYPE_DCSG, 4, 1}, //
	{CH_TYPE_DCSG, 5, 2}, //
						  // Rythm
	{CH_TYPE_FM, 7, 0},	  //
	{CH_TYPE_FM, 8, 1},	  //
	{CH_TYPE_FM, 9, 2},	  //
						  // FM6
	{CH_TYPE_PCM, 10, 0}, //
};

//!	SE MODE(MODE2) の、デチューンワーク
static u8 DETDAT[] = {
	0, // OP1
	0, // OP2
	0, //   3
	0, //   4
};

//! FMのLR設定を保存している場所
static u8 PALDAT[7] = {
	0xc0, // PMS/AMS/LR DATA  3:Dummy
	0xc0,
	0xc0,
	0x00, // dummy
	0xc0,
	0xc0,
	0xc0,
};

/*****************************************************************************
 * 
 * Programs
 * 
******************************************************************************/

//-----------------------------------------------------------------------------
/**
 * @fn Mu88md_SystemAllInit()
 * @brief Mucom88全体の初期化 
 * @note CallBackにVSync関数を登録する前に四で初期化する
 * 
**/
void Mu88md_SystemAllInit()
{
	memset(&Mucom88SysWork, 0, sizeof(MUCOM88_SYS_WORK));

	pSys = &Mucom88SysWork;
	pSys->MusicNum = 0; // .mubに含まれる曲数

	pSys->Mu_Top = 0;
	pSys->MUSICNUM = 0; // リクエスト曲のINDEX

	pSys->LOOP_MUB = 0;

	pSys->PLSET1 = 0x38; // モード＆Timer初期値
	pSys->PLSET2 = 0x3a; // モード＆Timer初期値

	pSys->FMPORT = 0; // 0:FM1-3 / 4:FM4-6 のどちらを処理中か？

	pSys->PCMFLG = 0;
	pSys->SSGF1 = 0;
	pSys->NEWFNM = 0;

	pSys->PCMNUM = 0;

	pSys->FNUM = 0;
	pSys->FMSUB8 = 0;

	pSys->FPORT = FM_REG_0xa4;		// PC88の自己書き換え

	pSys->DCSG_OCT_UNDER = 0;		// DCSG範囲外の音リクエスト回数

	// ym acsess times for debug 
	pSys->ym_access_num = 0;
	pSys->ym_access_timer = 0;
	pSys->ym_access_max = 0;

	YM2612_reset();
	se_system_init();

#if USE_PCM
	z80_pcm_driver_load(); //!< Z80のPCMドライバーを転送
#endif

#if USE_FM6_PCM_SWITCH == 0
	set_fm6_pcm_use_mode(); //!< FM 6ch DACモードに変更
#endif
}

//-----------------------------------------------------------------------------
/**
 * @fn void Mu88_CallBackFunc()
 * @brief ドライバの最上位関数VSyncから毎フレーム呼び出す
 * @note 
 * 
**/
void Mu88_CallBackFunc()
{

#if USE_MUCOM88_DEBUG
	pSys->ym_access_num = 0;			// 1FrameのYMアクセス数
#endif

	request_copy_to_driver();			// ゲーム側のリクエストをドライバにコピー

	// SEの処理
	se_request_drive();

	// mubの処理
	mub_request_check(); // リクエストのチェック
	if (pSys->SND_STATUS != 0) //! 演奏中か？
	{
		PL_SND();
	}

	pcm_request_drive();	// SE系と、PL_SNDで処理したPCMを演奏

#if USE_MUCOM88_DEBUG
	// YMアクセス数MAXを1秒表示
	pSys->ym_access_timer++;
	if( pSys->ym_access_timer >= JOB_PEAK_TIME ){
		pSys->ym_access_timer = 0;
		pSys->ym_access_max = 0;
	}
	if( pSys->ym_access_max < pSys->ym_access_num ){
		pSys->ym_access_timer = 0;
		pSys->ym_access_max = pSys->ym_access_num;
	}
#endif
}

/**
 * @fn static void request_copy_to_driver()
 * @brief ゲーム側のリクエストをドライバーにコピーする
 * @note ゲーム側は、リクエストを準備しておき、最後に割り込み禁止でリクエストフラグ有り！フラグを有効にする事。サンプルも用意します。
 * @note ゲーム側で作りかけのリクエストで演奏しないように注意する事
**/
static void request_copy_to_driver()
{
		// ゲーム側のリクエスト管理
	if( Mu88md_ReqWork.req_finish == 0 ){
		// リクエストが完了していない
		return;
	}
	Mu88md_ReqWork.req_finish = 0;

	// Mubのリクエストが有れば、ドライバー側のリクエスト処理を準備
	if (Mu88md_ReqWork.mub_req_flag)
	{
		pSys->req_command = Mu88md_ReqWork.mub_req_command;
		pSys->req_param = Mu88md_ReqWork.mub_req_param; // 使っていない

		if (Mu88md_ReqWork.mub_req_command == MUB_REQ_START)
		{
			pSys->Mub_Top_68k = (u8 *)Mu88md_ReqWork.mub_bank;
		}
		Mu88md_ReqWork.mub_req_flag = 0; // リクエスト受付終了
	}

	// PCMのリクエストが有れば、ドライバー側のリクエスト処理を準備
	u32 lp;
	for (lp = 0; lp < 2; lp++)
	{
		if (Mu88md_ReqWork.pcm_req_flag[lp] != 0)
		{
			pSys->pcm_req_wk[lp].prio = Mu88md_ReqWork.pcm_req_prio[lp];
			pSys->pcm_req_wk[lp].req_no = Mu88md_ReqWork.pcm_req_no[lp] + 1;
			Mu88md_ReqWork.pcm_req_flag[lp] = 0;
		}
	}

	// Seリクエスト
	SeRequestCk();

#if USE_MUCOM88_DEBUG
	pSys->NOW_CUE_REQ = Mu88md_ReqWork.cue;
#endif
}

/**
 * @fn static void mub_request_check()
 * @brief MUBリクエストチェック
 * @note
**/
static void mub_request_check()
{
	switch (pSys->req_command)
	{
	case MUB_REQ_NONE: //; NONE
		break;
	case MUB_REQ_START: //; START
		mub_request_start();
		break;
	case MUB_REQ_STOP: //; STOP
		mub_request_stop();
		break;
	case MUB_REQ_FADEOUT: //; FADEOUT
		mub_request_fadeout();
		break;
	}
	pSys->req_command = MUB_REQ_NONE;
}

/**
 * @fn static void mub_request_start()
 * @brief .mubの演奏開始
 * @note
**/
static void mub_request_start()
{
	if (pSys->SND_STATUS != 0)
	{
		MSTOP(); //! 演奏中なら止める
	}
	mub_adress_set();
	MSTART();
	pSys->SND_STATUS = MUB_STATE_PLAY;
}

/**
 * @fn static void mub_adress_set()
 * @brief .mubのヘッダーからデータのアドレスを確定させる
 * @note
**/
static void mub_adress_set()
{
	//	call	SetBank			; hl ワークに設定されている.mubのバンクをセットする
	// ヘッダ部 5Byte
	//-----------------
	// 1byte バイナリに含まれる曲数-1
	// 2byte FM音色データ部オフセット
	// 2byte バイナリサイズ
	//-----------------
	u8 *mub = pSys->Mub_Top_68k;

	pSys->MusicNum = *mub;
	mub++;
	//	pSys->OtoDatOffset_68k = *mub | *(mub + 1) << 8; // FMToneまでのオフセット

	pSys->OtoDatOffset_68k = GetHL(*(mub + 1), *mub);

	mub++; // FM OFSET
	mub++;
	mub++; // DataSize
	mub++;

	pSys->Mu_Top_68k = mub;
}

/**
 * @fn static void mub_request_stop()
 * @brief MUB演奏の停止
 * @note
**/
static void mub_request_stop()
{
	if (pSys->SND_STATUS != 0)
	{
		MSTOP();
	}
}

/**
 * @fn static void mub_request_fadeout()
 * @brief フェードアウトリクエスト
 * @note
**/
static void mub_request_fadeout()
{
	pSys->FDCO_LV = 0x0f;
	pSys->FDCO_WAIT = FADE_SPEED_WAIT;
}

//-----------------------------------------------------------
/**
 * @fn static void FDOUT()
 * @brief フェードアウト処理
 * @note
**/
static void FDOUT()
{
	// FadOutウェイト
	pSys->FDCO_WAIT--;
	if (pSys->FDCO_WAIT != 0)
	{
		return;
	}
	pSys->FDCO_WAIT = FADE_SPEED_WAIT;

	// ボリューム減算処理
	if (pSys->FDCO_LV == 0)
	{
		return;
	}
	pSys->FDCO_LV--;

	//FDO2:
	pSys->TOTALV = pSys->FDCO_LV + 0xf0;

	pSys->FMPORT = 0;
	FDOFM(&fm_ch_work[0]);

	u32 lp;
	for (lp = 0; lp < 3; lp++)
	{
		FM_CH_WORK *ix;
		u8 env_flag;
		u8 vol;

		ix = &dcsg_ch_work[lp];
		env_flag = ix->volume_6 & 0xf0;
		vol = ix->volume_6 & 0x0f;
		PV1(ix, vol, env_flag);
	}

	pSys->FMPORT = 4;
	FDOFM(&fm_ch_work[3]);

	if (pSys->FDCO_LV == 0)
	{
		FDO3(); // To Music Stop
	}
}

/**
 * @fn static void FDOFM()
 * @brief FM 3ch Fadeout Sub
 * @note
**/
static void FDOFM(FM_CH_WORK *ix)
{
	u32 lp;
	for (lp = 0; lp < 3; lp++)
	{
		STVOL(ix);
		ix++;
	}
}

/**
 * @fn static void FDO3()
 * @brief Fadeout Stop & Music Stup
 * @note
**/
static void FDO3()
{
	MSTOP();
	pSys->TOTALV = 0;
}

/**
 * @fn static void MSTART()
 * @brief .mubの演奏開始処理
 * @note
**/
static void MSTART()
{
	pSys->MUSICNUM = 0; /*< .mub内の最初の曲限定 */

	AKYOFF();
	SSGOFF();

	WORKINIT(); //; チャンネルデータのクリアとチャンネルごとの譜面をセット

	START();
}

//!
//! 演奏開始サブ
//!
static void START()
{
	//	ifdef USE_TIERB_DRIVE
	//	call	ENBL
	//	endif

	MONO();
	TO_NML();
}

/**
 * @fn static void MSTOP()
 * @brief .mubの演奏停止
 * @note
**/
static void MSTOP()
{
	AKYOFF();
	SSGOFF();
	pSys->SND_STATUS = 0;
}

//-----------------------------------------------------------------------------
/**
 * @fn static void WORKINIT()
 * @brief 演奏開始前のワーク初期化
 * @note
**/
static void WORKINIT()
{
	pSys->C2NUM = 0; // 使っていない　不要
	pSys->CHNUM = 0; // FMINITのchannnel_noのみ使っていた 不要
	pSys->PVMODE = 0;

	pSys->FLGADR = 0;

	pSys->LOOP_MUB = 0;
	pSys->MUB_LOOPED = 0;

	pSys->DCSG_OCT_UNDER = 0;
	pSys->DCSG_NOISEP = 0;
	pSys->DCSG_NOISEW = 0;

	pSys->FDCO_LV = 0;
	pSys->FDCO_WAIT = 0;
	pSys->TOTALV = 0;

	pSys->DCSG_OCT_UNDER = 0;

	u8 *mu_top = pSys->Mu_Top_68k;

	u8 music_no = pSys->MUSICNUM;
	music_no = 0; // 強制的に0にしておく

	//! .mub内の2曲目以降に設定（未対応）
	while (music_no != 0)
	{
		/*
		mu_top = pSys->Mu_Top_68k;

		mu_top++;					// 次の曲へ（ここは使わない予定）
		mub_top += MAXCH*4;

		nextmub_offset = *(u16*)mub_top;		// TODO 68k aline注意

		mu_top = pSys->Mu_Top_68k + nextmub_offset;

		music_no--;
*/
	}

	pSys->TIMER_B = *mu_top++;
	pSys->TB_TOP_68k = mu_top; // Adress

	//	pSys->CHIDX = 0;

	u32 lp;
	for (lp = 0; lp < MUB_CH_WORK_NUM; lp++)
	{
		FMINIT(ch_work_list_tbl[lp], lp);
	}

	LoopMubCheck();
}

//-----------------------------------------------------------------------------
/**
 * @fn static void PL_SND()
 * @brief ドライバー全体コントロールループ
 * @note Drive/Cue/Fade/一時停止などの制御
**/
static void PL_SND()
{
	// TimerBと効果音モード設定 自己書き換えを行っているが廃止も検討
	PSGOUT(FM_REG_0x27, pSys->PLSET1); /*!< TIMER-OFF DATA */
	PSGOUT(FM_REG_0x27, pSys->PLSET2); /*!< TIMER-ON */

#if 0
	//! TODO Pauseが必要ならここで判定
	ESC_PRC() Pauseチェック
	if(pSys->ESCAPE != 0){
	//! Pause Goto PLSND4
	}
#endif

	DRIVE(); /*!< ドライバメイン */

#if USE_MUCOM88_DEBUG
	// 早送り時にLOOPチェックは保証しない
	u8 cue_flag = 0;
	cue_flag = CUE(); /*!< 早送り */

	if (cue_flag != 0)
	{
		return;
	}
#endif

	FDOUT();	  /*!< FADEOUT処理 */
	TSC_LOOPCK(); /*!< TSC_LOOPCK */
}

/**
 * @fn static void PcmReqOut()
 * @brief デバッグ用の4倍速再生
 * @note void request_copy_to_driver()　で、ゲーム側のPCMリクエストをリクエストワークに登録
 * @note FMENTのPCMChannelで最低優先でリクエストワークに登録
 **/
static void TSC_LOOPCK()
{
	u32 lp;
	u8 endflag = 1;

	for (lp = 0; lp < MUB_CH_WORK_NUM; lp++)
	{
		const FM_CH_WORK *ix = ch_work_list_tbl[lp];
		if ((ix->flag_31 & FM_31_BIT0_LOOPEND) == 0)
		{
			endflag = 0;
		}
	}

	if (endflag == 0)
	{
		// 未loopもしくは曲が終わっていない
		return;
	}

	pSys->MUB_LOOPED = 0x01; // 大ループを1回以上超えた

	if (pSys->LOOP_MUB != 0)
	{
		// loop曲なので
		return;
	}

	// 非Loop曲なので演奏終了
	MSTOP(); // 非LOOP曲が終了
}

/**
 * @fn static void pcm_request_drive()
 * @brief PCMのリクエスト処理
 * @note PCM発声状態のチェック・発声優先のチェック
 * @note void request_copy_to_driver()　で、ゲーム側のPCMリクエストをリクエストワークに登録
 * @note FMENTのPCMChannelで最低優先でリクエストワークに登録
 **/
static void pcm_request_drive()
{
	u16 z80state = Z80_getAndRequestBus(TRUE);

	vu8 *ptr;
	u8 pl_state[2];

	// Z80のstatus取得
	ptr = Z80_COMM_CH0_ADDR;
	pl_state[0] = *ptr;
	ptr = Z80_COMM_CH1_ADDR;
	pl_state[1] = *ptr;

	if (!z80state)
	{
		Z80_releaseBus();
	}

	pSys->pcm_state_z80[0] = pl_state[0];
	pSys->pcm_state_z80[1] = pl_state[1];

	u32 lp;

	if ((pl_state[0] == 0xff) && (pl_state[1] == 0xff))
	{
		// Z80演奏無し
	}

	for (lp = 0; lp < PCM_REQ_CHANNNEL_NUM; lp++)
	{
		if (pSys->pcm_req_wk[lp].req_no != 0)
		{
			if (pl_state[lp] != 0xff)
			{
				// 演奏中
				if (pSys->now_pcm_req_wk[lp].prio < pSys->pcm_req_wk[lp].prio)
				{
					pSys->now_pcm_req_wk[lp].req_no = pSys->pcm_req_wk[lp].req_no;
					pSys->now_pcm_req_wk[lp].prio = pSys->pcm_req_wk[lp].prio;
					// Req実行
					pcm_request_core(lp, pSys->now_pcm_req_wk[lp].req_no);
				}
			}
			else
			{
				// 停止中
				pSys->now_pcm_req_wk[lp].req_no = pSys->pcm_req_wk[lp].req_no;
				pSys->now_pcm_req_wk[lp].prio = pSys->pcm_req_wk[lp].prio;
				pcm_request_core(lp, pSys->now_pcm_req_wk[lp].req_no);
			}

			// リクエスト終了
			pSys->pcm_req_wk[lp].req_no = 0;
		}
	}

#if USE_FM6_PCM_SWITCH == 1
	if (fm_pcm == 0)
	{
		// FM
		set_fm6_fm_use_mode();
	}
	else
	{
		// PCM
		set_fm6_pcm_use_mode();
	}
#endif
}

/**
 * @fn static void pcm_request_core(u8 ch_no, u8 req_no)
 * @brief Z80のメモリに書き込み
 **/
static void pcm_request_core(u8 ch_no, u8 req_no)
{
	vu8 *cmd_ptr;
	vu8 *ptr;
	vu8 *ptr_len;
	u32 wav_adr;
	vu32 len;

	wav_adr = get_pcm_data_adr(req_no - 1);
	len = get_pcm_data_size(req_no - 1);

	// リクエストフラグ
	if (ch_no == 0)
	{
		cmd_ptr = Z80_COMM_CH0_ADDR;
	}
	else
	{
		cmd_ptr = Z80_COMM_CH1_ADDR;
	}

	if (ch_no == 0)
	{
		ptr = Z80_COMM_CH0_BANK_ADDR;
		ptr_len = Z80_COMM_CH0L_LEN_ADDR;
	}
	else
	{
		ptr = Z80_COMM_CH1_BANK_ADDR;
		ptr_len = Z80_COMM_CH1L_LEN_ADDR;
	}

	u16 z80state = Z80_getAndRequestBus(TRUE);

	*cmd_ptr = 0;

	// adr
	*ptr++ = (u8)(wav_adr & 0xff);
	*ptr++ = (u8)((wav_adr & 0xff00) >> 8);
	*ptr = (u8)((wav_adr & 0xff0000) >> 16);

	// len
	*ptr_len++ = (u8)(len & 0xff);
	*ptr_len = (u8)((len & 0xff00) >> 8);

	if (!z80state)
	{
		Z80_releaseBus();
	}
}

/**
 * @fn static void CUE()
 * @brief デバッグ用の4倍速再生
 **/
static u8 CUE()
{
	if (pSys->NOW_CUE_REQ == 0)
	{
		return (0);
	}

	DRIVE();
	DRIVE();
	DRIVE();
	DRIVE();
	return (1);
}

/**
 * @fn static void NoUseChENT(FM_CH_WORK *ix)
 * @brief PCM/Rythm(使えない)チャンネルの処理 曲の終了判定だけ行う
 **/
static void NoUseChENT(FM_CH_WORK *ix)
{
	ix->flag_31 |= FM_31_BIT0_LOOPEND; // loopend flag 立てる
}

//-----------------------------------------------------------------------------
//! DCSG SE Routine
//-----------------------------------------------------------------------------
/**
 * @fn static void SeAllInit()
 * @brief SE管理の初期化
**/
static void se_system_init()
{
	memset(&SeSysWork, 0, sizeof(MUCOM88_SE_SYS_WORK));
	pSeSys = &SeSysWork;
}

/**
 * @fn static void se_request_drive()
 * @brief SEの初期化
**/
static void se_request_drive()
{
	pSys->SSGF1 = 0xff; // Mucom本体をSSG演奏モードで動作させる

	// SE 1CH
	if (pSeSys->PlayState[0] == SE_PLAY_STATE_ON)
	{
		pSys->SeState |= SE_PLAY_BIT1;
		pSys->Base68k_adrs = pSys->Base68k_se_adrs[0];
		if (se_drive_core(&se_ch_work[0]) == 1)
		{
			pSeSys->PlayState[0] = SE_PLAY_STATE_OFF;
			pSys->SeState &= ~SE_PLAY_BIT1;
		}
	}

	// SE 2CH
	if (pSeSys->PlayState[1] == SE_PLAY_STATE_ON)
	{
		pSys->SeState |= SE_PLAY_BIT2;
		pSys->Base68k_adrs = pSys->Base68k_se_adrs[1];
		if (se_drive_core(&se_ch_work[1]) == 1)
		{
			pSeSys->PlayState[1] = SE_PLAY_STATE_OFF;
			pSys->SeState &= ~SE_PLAY_BIT2;
		}
	}

	// SE 3CH(+Noise)
	if (pSeSys->PlayState[2] == SE_PLAY_STATE_ON)
	{
		pSys->SeState |= SE_PLAY_BIT3;
		pSys->Base68k_adrs = pSys->Base68k_se_adrs[2];
		if (se_drive_core(&se_ch_work[2]) == 1)
		{
			pSeSys->PlayState[2] = SE_PLAY_STATE_OFF;
			pSys->SeState &= ~SE_PLAY_BIT3;
		}
	}

	pSys->SSGF1 = 0x00; // Mucom本体をSSG演奏モードで動作させる
}

/**
 * @fn static void se_request_drive()
 * @brief SEの初期化
**/
static u8 se_drive_core(FM_CH_WORK *ix)
{
	SSGENT(ix);
	if ((ix->flag_31 & FM_31_BIT0_LOOPEND) != 0)
	{
		// Se End
		return (1);
	}
	return (0);
}

/**
 * @fn static void SeRequestCk()
 * @brief SEの初期化
**/
static void SeRequestCk()
{
	u32 lp;
	for (lp = 0; lp < 3; lp++)
	{
		// リクエストがあるか？
		u8 req_cmd = Mu88md_ReqWork.Mucom88mdz_SeReqArray[SE_REQ_COMMAND][lp];
		Mu88md_ReqWork.Mucom88mdz_SeReqArray[SE_REQ_COMMAND][lp] = 0;
		if (req_cmd != 0)
		{
			// SEFMINIT
			u8 req_no = Mu88md_ReqWork.Mucom88mdz_SeReqArray[SE_REQ_NO][lp];
			pSeSys->PlayState[lp] = SE_PLAY_STATE_ON; // 演奏中
			FMINIT_SE(&se_ch_work[lp], lp, req_no);
		}
	}
}

/**
 * @fn static void FMINIT_SE(FM_CH_WORK *ix, u8 ch_no)
 * @brief SEのWork初期化
**/
static void FMINIT_SE(FM_CH_WORK *ix, u8 ch_no, u8 Req_no)
{
	memset(ix, 0, sizeof(FM_CH_WORK));

	ix->counter_0 = 1;
	ix->volume_6 = 0;

	u32 tbl_adr = (u32)&Mucom88mdSeTbl;
	u16 *Setbl = (u16 *)(tbl_adr + (Req_no * 2));
	u16 *MmlDat = (u16 *)(tbl_adr + *Setbl);

	pSys->Base68k_se_adrs[ch_no] = (u8 *)MmlDat; // Base68k_adrs

	ix->data_adress_2 = 0; // L Offsetのみです。後で、Mu_Top_68kとタス
	ix->data_adress_3 = 0; // H

	ix->data_top_adress_4 = 0;
	ix->data_top_adress_5 = 0;

	// DCSGレジスタ番号
	SSINIT(ix, ch_no);
}

//-----------------------------------------------------------------------------
/**
 * @fn static void DRIVE()
 * @brief 音源ドライブメイン
**/
static void DRIVE()
{
	pSys->Base68k_adrs = pSys->Mu_Top_68k;

	//! FM 1-3
	pSys->FMPORT = 0;
	FMENT(&fm_ch_work[0]);
	FMENT(&fm_ch_work[1]);
	FMENT(&fm_ch_work[2]);

	//! FM 4-6
	pSys->FMPORT = 4;
	FMENT(&fm_ch_work[3]);
	FMENT(&fm_ch_work[4]);

#if USE_FM6_CH
	FMENT(&fm_ch_work[5]); /*!< PCMを使わないならFMを演奏 */
#endif

	//! DCSG
	pSys->SSGF1 = 0xff;

	pSys->SEMODE = 0x00;
	if (pSys->SeState & SE_PLAY_BIT1)
	{
		pSys->SEMODE = 0x01;
		dcsg_ch_work[0].se_int_41 |= DCSG_41_BIT7_SE_DCSG_OFF;
	}
	SSGENT(&dcsg_ch_work[0]);

	pSys->SEMODE = 0x00;
	if (pSys->SeState & SE_PLAY_BIT2)
	{
		pSys->SEMODE = 0x01;
		dcsg_ch_work[1].se_int_41 |= DCSG_41_BIT7_SE_DCSG_OFF;
	}
	SSGENT(&dcsg_ch_work[1]);

	pSys->SEMODE = 0x00;
	if (pSys->SeState & SE_PLAY_BIT3)
	{
		pSys->SEMODE = 0x01;
		dcsg_ch_work[2].se_int_41 |= DCSG_41_BIT7_SE_DCSG_OFF;
	}
	SSGENT(&dcsg_ch_work[2]);
	pSys->SSGF1 = 0;

	//! PCM
	pSys->PCMFLG = 0xff;
	FMENT(&pcm_ch_work[0]);
	pSys->PCMFLG = 0;
}

//-----------------------------------------------------------------------------
/**
 * @fn static void PSGOUT(u8 reg, u8 param)
 * @brief YMのレジスタへのパラメータ書き込み
 * @note
**/
#if PSGOUT_ASM
// ASM Ver
static void PSGOUT(u8 reg, u8 param)
{
#if USE_MUCOM88_DEBUG
	pSys->ym_access_num++;			// 1FrameのYMアクセス数
#endif
	if ((reg >= 0x30) && (pSys->FMPORT != 0) ){
		WriteFmPortBat1(reg, param);
	}else{
		WriteFmPortBat0(reg, param);
	}
}

#else
// C Lang Ver.
static void PSGOUT(u8 reg, u8 param)
{
#if USE_MUCOM88_DEBUG
	pSys->ym_access_num ++;			// 1FrameのYMアクセス数
#endif

	u16 port = 0;
	if (reg >= 0x30) // FM 1-3/4-6で書き込み先を変更する
	{
		//! FM 4-6 / PCMへの書き込み時はレジスタを変更する
		if ((pSys->FMPORT != 0) || (pSys->PCMFLG != 0)) // FM 4-6(PCM 6)のregisterアドレスを変更
														//		if (pSys->FMPORT != 0)
		{
			port = 1;
		}
	}

	Z80_requestBus(TRUE);
	u8 z80_ym_reg = *Z80_NOW_REG_ADDR; /*!< 現在のZ80側からYMにアクセスするregister番号 */

	YM2612_writeReg(port, reg, param);

	if (z80_ym_reg != 0) // Z80からYMに書き込み直前なら元に戻す
	{
		YM2612_write(0, z80_ym_reg);
	}
	Z80_releaseBus();

}
#endif

//-----------------------------------------------------------------------------
/**
 * @fn static void PSGOUT2(u8 reg, u8 param)
 * @brief YMのレジスタへのパラメータ書き込み
 * @note 音色変更向けの関数
**/
static void PSGOUT2(u8 reg, u8 param)
{
#if USE_MUCOM88_DEBUG
	pSys->ym_access_num++;			// 1FrameのYMアクセス数
#endif
	if ((reg >= 0x30) && (pSys->FMPORT != 0) ){
		WriteFmPortCore1(reg, param);
	}else{
		WriteFmPortCore0(reg, param);
	}
}

// -----------------------------------------------------------------------------
/**
 * @fn static void MONO()
 * @brief All Monoral / H.LFO Off
 * @note
**/
static void MONO()
{
	//! MONO 1-3
	u32 lp;
	pSys->FMPORT = 0;
	for (lp = 0; lp < 3; lp++)
	{
		PSGOUT(FM_REG_0xb4 + lp, 0xc0); // LR両方出力
	}

	//! MONO 4-6
	pSys->FMPORT = 4;
	for (lp = 0; lp < 3; lp++)
	{
		if (lp == 2)
		{
			continue; // PCM TEST
		}
		PSGOUT(FM_REG_0xb4 + lp, 0xc0); // LR両方出力
	}
	pSys->FMPORT = 0;

	//! H.LFO OFF
	PSGOUT(FM_REG_0x22, 0x00); // LFOのクリア

	//! PCM MONO
}

//! ***************************************************************************
//!
//! チャンネル処理
//!
//! ***************************************************************************

/**
 * @fn static void FMENT(FM_CH_WORK *ix)
 * @brief FM/PCMのMML処理先頭
 * @note
**/
static void FMENT(FM_CH_WORK *ix)
{
	pSys->sHL = GetHL(ix->data_adress_3, ix->data_adress_2);
	FMSUB(ix);
	PLLFO(ix);
	SetHL(pSys->sHL, &ix->data_adress_3, &ix->data_adress_2);
}

/**
 * @fn static void SSGENT(FM_CH_WORK *ix)
 * @brief DCSG(SSG)のMML処理先頭
 * @note
**/
static void SSGENT(FM_CH_WORK *ix)
{
	pSys->sHL = GetHL(ix->data_adress_3, ix->data_adress_2);
	SSGSUB(ix);
	PLLFO(ix);
	SetHL(pSys->sHL, &ix->data_adress_3, &ix->data_adress_2);
}

//! ***************************************************************************
//!
//! FM音源(PCM)処理
//!
//! ***************************************************************************

/**
 * @fn static void FMSUB(FM_CH_WORK *ix)
 * @brief FM 譜面処理
 * @note
**/
static void FMSUB(FM_CH_WORK *ix)
{
	ix->counter_0--; // ; Clockの減算
	if (ix->counter_0 == 0)
	{
		FMSUB1(ix); // ; 次の音符
	}
	else if (ix->counter_0 == ix->q_on_18) // ; 'q' スタカートのClock減算 (A - r)
	{
		FMSUB0(ix); // ; To REVERVE/SOFT ENV
	}
	else if (ix->counter_0 > ix->q_on_18)
	{
		return;
	}
	else
	{
		// RET	NC
		FMSUB0(ix); // (ix->counter_0 <= ix->q_on_18)
	}
}

//!- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
 * @fn static void FMSUB0(FM_CH_WORK *ix)
 * @brief リバーブ・ソフトエンベロープ処理
 * @note
**/
static void FMSUB0(FM_CH_WORK *ix)
{
	u8 mml_dat;
	mml_dat = GetMmlData(pSys->sHL); // ;  HL=SOUND DATA ADDRESS

	if (mml_dat == MML_CMD_TIE_SLUR)
	{ // ; COUNT OVER ?  A = 0xfd は終了
		return;
	}
	else if ((ix->flag_33 & FM_33_BIT5_REVERVE_FLAG) == 0) // ; REVERVE FLAG
	{
		KEYOFF(ix); // ; REVERVE FLAG Offの処理
	}
	else
	{
		FS2(ix); // FS2は関数化
	}
}

/**
 * @fn static void FS2(FM_CH_WORK *ix)
 * @brief リバーブ/Soft Envのボリューム設定
 * @note
**/
static void FS2(FM_CH_WORK *ix)
{
	u8 vol;
	vol = ix->volume_6 + ix->dcsg_senv_rr_17; // (IX+17)	; SoftEnve Dummy (6byte last)

	STV2(ix, (vol >> 1));
	ix->flag_31 |= FM_31_BIT6_KEYOFF;
}

//!- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
 * @fn static void FMSUB1(FM_CH_WORK *ix)
 * @brief 次の音符・休符・サブコマンドの解析と処理開始
 * @note
**/
static void FMSUB1(FM_CH_WORK *ix)
{
	u8 mml_cmd;
	ix->flag_31 |= FM_31_BIT6_KEYOFF;
	mml_cmd = GetMmlData(pSys->sHL);

	if (mml_cmd == MML_CMD_TIE_SLUR) // タイ/スラー 0xfd
	{
		//FMSUBE:
		ix->flag_31 &= ~FM_31_BIT6_KEYOFF; // RES KEYOFF FLAG
		pSys->sHL++;
	}

	//! サブコマンド処理(複数コマンドが連続可能性あり)
	do
	{
		mml_cmd = GetMmlData(pSys->sHL); // ; A=DATA

		// データ終了
		if (mml_cmd == 0) // ; データ終了を調べる
		{
			// ;* 00H as end
			ix->flag_31 |= FM_31_BIT0_LOOPEND;
			if ((ix->data_top_adress_4 | ix->data_top_adress_5) == 0)
			{
				FMEND(ix);
				return;
			}
			else
			{
				// HLをループ位置に修正する
				pSys->sHL = GetHL(ix->data_top_adress_5, ix->data_top_adress_4);
				//			FMSUB2(ix);
				mml_cmd = GetMmlData(pSys->sHL); // ; A=DATA ループした時はデータを再取得
			}
		}
		pSys->sHL++;

		// サブコマンドならコマンドを実行
		if (mml_cmd < 0xf0)
		{
			break; // ループを抜けて音符・休符処理に移動
		}
		FMSUBA(ix, mml_cmd); // ; DATA がコマンドなら FMSUBA へ
	} while (1);

	ix->counter_0 = mml_cmd & 0x7f;
	if (mml_cmd & MML_CMD_REST_FLAG) // & 0x80
	{
		//! 休符
		SetHL(pSys->sHL, &ix->data_adress_3, &ix->data_adress_2); /*!< TODO 不要 */

		if (ix->flag_33 & FM_33_BIT4_REVERVE_MODE)
		{
			KEYOFF(ix); //! FS3
		}
		if (ix->flag_33 & FM_33_BIT5_REVERVE_FLAG)
		{
			FS2(ix);
		}
	}
	else
	{
		// 音符
		FMSUB5(ix);
	}
}

//-----------------------------------------------------------------------------
/**
 * @fn static void FMSUB5(FM_CH_WORK *ix)
 * @brief 音符の処理(ノーマル・効果音モードの分岐)
 * @note
**/
static void FMSUB5(FM_CH_WORK *ix)
{
	if (ix->flag_31 & FM_31_BIT6_KEYOFF)
	{
		KEYOFF(ix); //	; 新しい音符ならKEYOFFに飛ぶはず
	}

	if (pSys->PLSET1 == 0x78)
	{
		// NML (!EFFECT)
		if (pSys->FMPORT == 0)
		{
			if (ix->channel_no_8 == 2) // Ch = 3
			{
				EXMODE(ix); //; SEモード
				return;
			}
		}
	}
	FMSUB4(ix);
}

/**
 * @fn static u8 FMSUB4(FM_CH_WORK *ix)
 * @brief 新規発声・継続発声の分岐
 * @return KEYON And 継続音符なら、1を返す
 * @note 今回の音符を発声するか、前回の継続か判定して処理
**/
static u8 FMSUB4(FM_CH_WORK *ix)
{
	u8 new_code;
	new_code = GetMmlData(pSys->sHL++);						  /*!<  A=BLOCK( OCTAVE-1 ) & KEY CODE DATA */
	SetHL(pSys->sHL, &ix->data_adress_3, &ix->data_adress_2); /*!< TODO 別途大本で書き込んでいる */

	if (ix->flag_31 & FM_31_BIT6_KEYOFF) /*!< KEY OFFなら音符処理 */
	{
		FMSUB9(ix, new_code);
		return (0);
	}
	if (ix->befor_code_32 != new_code) /*!< 前回と異なる音符なら処理 */
	{
		FMSUB9(ix, new_code);
		return (0);
	}

	return (1); // SCF(CARRYをセットして終了)  EXMODE向け
}

/**
 * @fn static void FMSUB9(FM_CH_WORK *ix, u8 b)
 * @brief FM/PCMの処理分岐
 * @note 
**/
static void FMSUB9(FM_CH_WORK *ix, u8 new_code)
{
	ix->befor_code_32 = new_code;
	if (pSys->PCMFLG)
	{
		PCMGFQ(ix);
	}
	else
	{
		FMGFQ(ix, new_code);
	}
}

/**
 * @fn static void PCMGFQ(FM_CH_WORK *ix)
 * @brief PCMの音程計算して発声処理に移行する
 * @note ADPCOMの音程を、音符+DT+LFOから計算する。(MDでは不要)
**/
static void PCMGFQ(FM_CH_WORK *ix)
{
	// u8 b = ix->befor_code_32;
	// CODEから再生周波数・再生octaveを計算
	// 結果をDELT_Nに格納

	// PCMのLFO設定(MDでは関係ない)
	if ((ix->flag_31 & FM_31_BIT6_KEYOFF) == 0)
	{
		LFORST(ix);
	}
	LFORST2(ix);

	// 演奏
	PLAY_PCM(ix);
}

/**
 * @fn static u8 FMGFQ(FM_CH_WORK *ix, u8 b)
 * @brief FM音源 FNUMB Write / LFO INIT
 * @param b : 音符 KEY CODE
 * @note 
**/
static void FMGFQ(FM_CH_WORK *ix, u8 b)
{
	//! FM音源のBaseとなるF-Num（68K並び）
	//! <https://github.com/kuma4649/mucomMD2vgm/blob/master/mucomMD2vgm/FNUM_YM2612.txt>
	static const u16 FNUMB[] = {
		0x0289,0x02af,0x02d8,0x0303,0x0331,0x0362,0x0395,0x03cc,0x0405,0x0443,0x0484,0x04c8,		// FNUMB
	};

	u8 code, block;
	//	u8 a, bb;
	block = (b & 0x70) >> 1; // ; GET BLOCK DATA A4-A6 ポート　出力用に合わせる
	code = (b & 0x0f);		 //; GET KEY CODE (C,C+,D ... B)
							 //	code += code;		// FNUMB Table引くためなのでマスク

	u16 fnum;
	fnum = FNUMB[code];
	fnum = fnum | ((u16)block << 8); // A= KEY CODE & FNUM HI

	fnum += GetHL(ix->detune_10, ix->detune_9); // ; GET DETUNE DATA & DETUNE PLUS
	if ((ix->flag_33 & FM_33_BIT6_TTL_TREMOLO_FLAG) == 0)
	{
		SetHL(fnum, &ix->fnum2_data_30, &ix->fnum1_data_29);
		pSys->FNUM = fnum;
	}

	//FMS92:
	if (ix->flag_31 & FM_31_BIT6_KEYOFF)
	{
		LFORST(ix);
	}
	LFORST2(ix);

	fnum += pSys->FMSUB8; /*!< EXMODEで計算されたデータ */

	PSGOUT(FM_REG_0xa4 + ix->channel_no_8, GetHfromHL(fnum));	  // d,e
	PSGOUT(FM_REG_0xa4 + ix->channel_no_8 - 4, GetLfromHL(fnum)); // d,e

	KEYON(ix);

	//	return (0); // 	AND	A キャリーフラグを寝かせる
}

//-----------------------------------------------------------------------------
/**
 * @fn static void EXMODE(FM_CH_WORK *ix)
 * @brief 効果音モードの設定
 * @note SE MODE の DETUNE 設定
**/
static void EXMODE(FM_CH_WORK *ix)
{
	u16 bc = 0;

	bc = (u16)DETDAT[0];
	pSys->FMSUB8 = bc; // 自己書き換え

	u16 ret = 0;

	ret = FMSUB4(ix); //; SET OP1
	if (ret == 1)
	{
		// 同じ音符でキーオンのままならreturn
		return;
	}

	//! 新しい音符なら、EFCTモードDT設定
	u8 *dt_dat = &DETDAT[1];
	u8 reg_a = FM_REG_0xaa;
	do
	{
		u16 fnum = pSys->FNUM;
		u8 dt = *dt_dat++;

		// FSUB6 の部分
		fnum += (u16)dt;

		PSGOUT(reg_a + ix->channel_no_8, GetHfromHL(fnum));		// d,e
		PSGOUT(reg_a + ix->channel_no_8 - 4, GetLfromHL(fnum)); // d,e

		reg_a++;

		KEYON(ix);
	} while (reg_a != FM_REG_0xad);

	pSys->FPORT = FM_REG_0xa4; // 0xa4; // 自己書き換えを戻す（コードをコピーしたので使わない）
	pSys->FMSUB8 = 0;		   // 自己書き換え
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//! KEY-ON / KEY-OFF関係
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 * @fn static void AKYOFF()
 * @brief FMの全チャンネルのキーオフ
 * @note
**/
static void AKYOFF()
{
	u8 lp;
	for (lp = 0; lp < FM_CHANNNEL_NUM; lp++)
	{
		PSGOUT(FM_REG_0x28, lp); /*!< $28 xxxx-yyy x:slot y:channel */
	}
}

//-----------------------------------------------------------------------------
/**
 * @fn static void KEYOFF(FM_CH_WORK *ix)
 * @brief FM/PCMのキーオフ処理
 * @note KEY-OFF ROUTINE
**/
static void KEYOFF(FM_CH_WORK *ix)
{
	if (pSys->PCMFLG != 0)
	{
		PCMEND(ix);
	}
	else
	{
		PSGOUT(FM_REG_0x28, pSys->FMPORT + ix->channel_no_8);
	}
}

//-----------------------------------------------------------------------------
/**
 * @fn static void KEYON(FM_CH_WORK *ix)
 * @brief FMのキーオン処理
 * @note　KEY-ON  ROUTINE
**/
static void KEYON(FM_CH_WORK *ix)
{
	//! KEY ON
	u8 slot = 0xf0;
	if (pSys->FMPORT != 0)
	{
		slot = 0xf4;
	}
	PSGOUT(FM_REG_0x28, slot + ix->channel_no_8); // ;  KEY-ON

	// REVERVE Vol
	if (ix->flag_33 & FM_33_BIT5_REVERVE_FLAG)
	{
		STVOL(ix);
	}
}

#if OBSOLETE_CODE
/*
; -----------------------------------------------------------------------------
;	リズム音源のキーオン

DKEYON:

	LD	A,(READY)
	OR	A
	RET	Z
	LD	D,10H
	LD	A,(RHYTHM)	; GET RETHM PARAMETER
	AND	00111111B
	LD	E,A		; KEY ON
	CALL	PSGOUT
	RET
*/
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// DCSG書き込み関係
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 * @fn static void DCSGOUT(FM_CH_WORK *ix, u8 a)
 * @brief 
 * @param a<=DCSG パラメーター(Vol | Reg / Tone / Reg)
 * @note　SEでの書き込み抑制も考慮
**/
static void DCSGOUT(FM_CH_WORK *ix, u8 a)
{
	// [SE]
	if (ix->se_int_41 & DCSG_41_BIT7_SE_DCSG_OFF)
	{ // ; Bitが立っていたら、SEに割り込まれているので出力しない
		return;
	}

	u16 bus_taken;
	bus_taken = Z80_isBusTaken();
	if (!bus_taken)
		Z80_requestBus(TRUE);

	vu8 *dcsg_io;
	dcsg_io = DCSG_IO_ADRS;
	*dcsg_io = a;

	if (!bus_taken)
		Z80_releaseBus();
}

//-----------------------------------------------------------------------------
/**
 * @fn static void FMSUBA(FM_CH_WORK *ix, u8 cmd_a)
 * @brief FMのサブコマンドのジャンプ
 * @note
**/
static void FMSUBA(FM_CH_WORK *ix, u8 cmd_a)
{
	//!	FM CONTROL COMMAND(s) サブコマンド
	static void (*FMCOM[])(FM_CH_WORK *) =
		{
			OTOPST, /*＊ F0-音色セット    '@' */
			VOLPST, /*＊ F1-VOLUME SET   'v' */
			FRQ_DF, /* F2-DETUNE(周波数ずらし) 'D' */
			SETQ,	/* F3-SET COMMAND 'q' */
			LFOON,	/* F4-LFO SET */
			REPSTF, /* F5-REPEAT START SET  '[' */
			REPENF, /* F6-REPEAT END SET    ']' */
			MDSET,	/* F7-FM音源　モードセット */
			STEREO, /* F8-STEREO MODE */
			FLGSET, /* F9-FLAGSET */
			W_REG,	/* FA-COMMAND OF   'y' */
			VOLUPF, /* FB-VOLUME UP    ')' */
			HLFOON, /* FC-HARD LFO */
			TIE,	/* (CANT USE) */
			RSKIP,	/* FE-REPEAT JUMP'/' */
			SECPRC, /* FF-to second com */
		};

	FMCOM[(cmd_a & 0x0f)](ix);
}

/**
 * @fn static void SECPRC(FM_CH_WORK *ix)
 * @brief Second SubCommand
bc * @note SubCommand FF FM/DCSG
**/
static void SECPRC(FM_CH_WORK *ix)
{
	static void (*FMCOM2[])(FM_CH_WORK *) =
		{
			PVMCHG,	 // ;FFF0-PCM VOLUME MODE
			HRDENV,	 // ;FFF1-HARD ENVE SET 's'
			ENVPOD,	 // ;FFF2-HARD ENVE PERIOD
			REVERVE, // ;FFF3-リバーブ
			REVMOD,	 // ;FFF4-リバーブモード
			REVSW,	 // ;FFF5-リバーブ　スイッチ
		};

	u8 cmd2;
	cmd2 = GetMmlData(pSys->sHL++);

	FMCOM2[(cmd2 & 0x0f)](ix);
}

/**
 * @fn static void NTMEAN(FM_CH_WORK *ix)
 * @brief  未使用
 * @note SubCommand FC DCSG
**/
static void NTMEAN(FM_CH_WORK *ix)
{
	//	RET
}

/**
 * @fn static void TIE(FM_CH_WORK *ix)
 * @brief タイ
 * @note SubCommand FD FM/DCSG
**/
static void TIE(FM_CH_WORK *ix)
{
	ix->flag_31 = ix->flag_31 & (~FM_31_BIT6_KEYOFF);
}

/**
 * @fn static void FLGSET(FM_CH_WORK *ix)
 * @brief イベント フラグセット
 * @note SubCommand F9 FM/DCSG
**/
static void FLGSET(FM_CH_WORK *ix)
{
	pSys->FLGADR = GetMmlData(pSys->sHL++);
}

/**
 * @fn static void REVERVE(FM_CH_WORK *ix)
 * @brief リバーブ
 * @note SubCommand2 FFF3 FM/DCSG
**/
static void REVERVE(FM_CH_WORK *ix)
{
	ix->dcsg_senv_rr_17 = GetMmlData(pSys->sHL++);
	RV1(ix);
}

//!
//! リバーブフラグのセット
//!
static void RV1(FM_CH_WORK *ix)
{
	ix->flag_33 |= FM_33_BIT5_REVERVE_FLAG;
}

/**
 * @fn static void REVSW(FM_CH_WORK *ix)
 * @brief リバーブ
 * @note SubCommand2 FFF5 FM/DCSG
**/
static void REVSW(FM_CH_WORK *ix)
{
	u8 a = GetMmlData(pSys->sHL++);
	if (a != 0)
	{
		RV1(ix);
	}
	else
	{
		ix->flag_33 &= (~FM_33_BIT5_REVERVE_FLAG);
		STVOL(ix);
	}
}

/**
 * @fn static void REVMOD(FM_CH_WORK *ix)
 * @brief リバーブモード
 * @note SubCommand2 FFF4 FM/DCSG
**/
static void REVMOD(FM_CH_WORK *ix)
{
	u8 a = GetMmlData(pSys->sHL++);

	if (a == 0)
	{
		// RM2
		ix->flag_33 &= (~FM_33_BIT4_REVERVE_MODE);
	}
	else
	{
		ix->flag_33 |= FM_33_BIT4_REVERVE_MODE;
	}
}

/**
 * @fn static void PVMCHG(FM_CH_WORK *ix)
 * @brief PCM VMODE CHANGE
 * @note SubCommand2 FFF0 FM6
**/
static void PVMCHG(FM_CH_WORK *ix)
{
	u8 a = GetMmlData(pSys->sHL++);
	pSys->PVMODE = a;
}

/**
 * @fn static void STEREO(FM_CH_WORK *ix)
 * @brief STEREO設定の分岐
 * @note SubCommand F8 FM6
**/
static void STEREO(FM_CH_WORK *ix)
{
	if (pSys->PCMFLG == 0)
	{
		STER2(ix);
	}
	else
	{
		pSys->PCMLR = GetMmlData(pSys->sHL++);
	}

#if OBSOLETE_CODE
	if (pSys->DRMF1 != 0)
	{
		STE2(ix);
	}
#endif
}

/**
 * @fn static void STER2(FM_CH_WORK *ix)
 * @brief FM STEREO設定
**/
static void STER2(FM_CH_WORK *ix)
{
	u8 mml_lr = GetMmlData(pSys->sHL++);
	mml_lr = mml_lr << 6; // TODO 0-1Bitを6-7Bitに移動

	u8 tmp = PALDAT[ix->channel_no_8 + pSys->FMPORT];
	tmp = (tmp & 0x3f) | mml_lr;
	PALDAT[ix->channel_no_8 + pSys->FMPORT] = tmp;

	PSGOUT(FM_REG_0xb4 + ix->channel_no_8, tmp);

}


#if OBSOLETE_CODE
static void STE2(FM_CH_WORK *ix)
{
	u8 a = GetMmlData(pSys->sHL++); /*!< MML処理のためにとりあえず実装 */
}
#endif

/**
 * @fn static void VOLUPF(FM_CH_WORK *ix)
 * @brief VolumeのUP/DOWN
 * @note SubCommand FB FM
**/
static void VOLUPF(FM_CH_WORK *ix)
{
	u8 mml_vol = GetMmlData(pSys->sHL++);
	ix->volume_6 += mml_vol; /// Vol up/down

#if OPTIMIZ_TEST
	if (pSys->PCMFLG == 0){
		STVOL(ix);		// FM
	}else{
		// MD PCMのボリューム実装予定なし
	}
#else
	if (pSys->PCMFLG != 0)
	{
		// 処理なし
	}
	else
	{
		// FM
		STVOL(ix);
	}
#endif
}

/**
 * @fn static void VOLUPF(FM_CH_WORK *ix)
 * @brief 効果音モードのDTセット
 * @note SubCommand F7 FM
**/
static void MDSET(FM_CH_WORK *ix)
{
	TO_EFC(ix);

	DETDAT[0] = GetMmlData(pSys->sHL++);
	DETDAT[1] = GetMmlData(pSys->sHL++);
	DETDAT[2] = GetMmlData(pSys->sHL++);
	DETDAT[3] = GetMmlData(pSys->sHL++);
}

/**
 * @fn static void HLFOON(FM_CH_WORK *ix)
 * @brief HARD LFO SET	
 * @note SubCommand FC FM
 * @note TODO 後半未実装
**/
#define FM_HLFO_ON_BIT (0x08)
static void HLFOON(FM_CH_WORK *ix)
{
	u8 freq = GetMmlData(pSys->sHL++);

	PSGOUT(FM_REG_0x22, freq | FM_HLFO_ON_BIT); // HLFO ON | freq count

	u8 pms = GetMmlData(pSys->sHL++); // PMS
	u8 ams = GetMmlData(pSys->sHL++); // AMS

	u8 amspms = ams << 4 | pms; // AMS+PMS
	u8 ch_idx = ix->channel_no_8 + pSys->FMPORT;

	u8 bit_lr = PALDAT[ch_idx];
	bit_lr &= 0xc0; // AND	11000000B

	PALDAT[ch_idx] = bit_lr | amspms;

	PSGOUT(FM_REG_0xb4 + ix->channel_no_8, (bit_lr | amspms));
}

/**
 * @fn static void LFOON(FM_CH_WORK *ix)
 * @brief SOFT LFO SET(RESET)
 * @note SubCommand F4 FM/DCSG
**/
static void LFOON(FM_CH_WORK *ix)
{
	u8 a = GetMmlData(pSys->sHL++);

	if (a != 0)
	{
		LFOON3(ix, a);
	}
	else
	{
		SETDEL(ix);
		SETCO(ix);
		SETVCT(ix);
		SETPEK(ix);

		ix->flag_31 |= FM_31_BIT7_LFO;
	}
}

/**
 * @fn static void LFOON3(FM_CH_WORK *ix, u8 a)
 * @brief LFO設定のジャンプ分岐
**/
static void LFOON3(FM_CH_WORK *ix, u8 a)
{
	static void (*LFOTBL[])(FM_CH_WORK * ix) = {
		LFOOFF,
		LFOON2,
		SETDEL,
		SETCO,
		SETVC2,
		SETPEK,
	};
	a--;
	LFOTBL[a](ix);
}

/**
 * @fn static void SETDEL(FM_CH_WORK *ix)
 * @brief Set Delay
**/
static void SETDEL(FM_CH_WORK *ix)
{
	u8 a = GetMmlData(pSys->sHL++);
	ix->lfo_delay_keep_19 = a; //; SET DELAY
	ix->lfo_delay_wk_20 = a;
}

/**
 * @fn static void SETCO(FM_CH_WORK *ix)
 * @brief Set LFO Counter
**/
static void SETCO(FM_CH_WORK *ix)
{
	u8 a = GetMmlData(pSys->sHL++);
	ix->lfo_counter_keep_21 = a; //; SET COUNTER
	ix->lfo_counter_wk_22 = a;
}

/**
 * @fn static void SETVCT(FM_CH_WORK *ix)
 * @brief Set LFO 振幅スピード
**/
static void SETVCT(FM_CH_WORK *ix)
{
	u8 e = GetMmlData(pSys->sHL++);
	u8 d = GetMmlData(pSys->sHL++);

	ix->lfo_range_l_keep_23 = e; // ; SET 変化量
	ix->lfo_range_l_wk_25 = e;
	ix->lfo_range_h_keep_24 = d;
	ix->lfo_range_h_wk_26 = d;
}

/**
 * @fn static void SETVC2(FM_CH_WORK *ix)
 * @brief SET LFT 振幅スピード・　PEAK Level / Delayの設定
**/
static void SETVC2(FM_CH_WORK *ix)
{
	SETVCT(ix);
	LFORST(ix);
}

/**
 * @fn static void SETPEK(FM_CH_WORK *ix)
 * @brief Set LFO Peak Level
**/
static void SETPEK(FM_CH_WORK *ix)
{
	u8 a = GetMmlData(pSys->sHL++);

	ix->lfo_peak_level_keep_27 = a;
	ix->lfo_peak_level_wk_28 = a >> 1;
}

/**
 * @fn static void LFOON2(FM_CH_WORK *ix)
 * @brief Set LFO On2
**/
static void LFOON2(FM_CH_WORK *ix)
{
	ix->flag_31 |= FM_31_BIT7_LFO;
}

/**
 * @fn static void LFOOFF(FM_CH_WORK *ix)
 * @brief Set LFO Off
**/
static void LFOOFF(FM_CH_WORK *ix)
{
	ix->flag_31 &= (~FM_31_BIT7_LFO);
}

/**
 * @fn static void SETQ(FM_CH_WORK *ix)
 * @brief スタッカートのクロック設定(qコマンド)
 * @note SubComman F03 FM/DCSG
**/
static void SETQ(FM_CH_WORK *ix)
{
	u8 a = GetMmlData(pSys->sHL++);
	ix->q_on_18 = a;
}

/**
 * @fn static void OTOPST(FM_CH_WORK *ix)
 * @brief 音色設定(FM/PCM)
 * @note SubComman F0 FM
**/
static void OTOPST(FM_CH_WORK *ix)
{
	if (pSys->PCMFLG)
	{
		OTOPCM(ix); /*!< PCM */
	}
#if OBSOLETE_CODE
	else if (pSys->DRMF1)
	{
		// OTODRM();
	}
#endif
	else
	{
		u8 a = GetMmlData(pSys->sHL++);
		ix->tone_no_1 = a;
		STENV(ix);
		STVOL(ix);
	}
}

#if OBSOLETE_CODE
OTODRM:
	LD	A,(HL)
	INC	HL
	LD	(RHYTHM),A	; SET RETHM PARA
	RET
#endif


/**
 * @fn static void OTOPCM(FM_CH_WORK *ix)
 * @brief PCMの音色設定
**/
static void OTOPCM(FM_CH_WORK *ix)
{
	u8 a = GetMmlData(pSys->sHL++);
	pSys->PCMNUM = a;	   // SB2に入れる値
	ix->tone_no_1 = a - 1; // PCMは、@1からMMLを記載するので

	// TONE番号のPCMデータを取得(PCMADRから)
	// 個々の処理はZ80で行う


#if OBSOLETE_CODE
	// PVMODEは実装予定なし
	if (pSys->PVMODE != 0)
	{
		//		LD	(IX+6),E
		// ix->volume_6 = 0;			// PCMの音色ごとのボリューム?
		// TODO Z80側にもたせるかな？
	}
	else
	{
		return;
	}
#endif
}

/**
 * @fn static void VOLPST(FM_CH_WORK *ix)
 * @brief ボリュームのセット
 * @note SubCommand F1 FM
**/
static void VOLPST(FM_CH_WORK *ix)
{
	if (pSys->PCMFLG)
	{
		PCMVOL(ix);
	}
#if OBSOLETE_CODE
	else if (pSys->DRMF1)
	{
		VOLDRM(ix);
	}
#endif
	else
	{
		// FM
		ix->volume_6 = GetMmlData(pSys->sHL++);
		STVOL(ix);
	}
}

/**
 * @fn static void VOLDRM(FM_CH_WORK *ix)
 * @brief Rhythmのボリュームセット
 * @note HLのIncのみ作っておく
**/
#if OBSOLETE_CODE
static void VOLDRM(FM_CH_WORK *ix)
{
	ix->volume_6 = GetMmlData(pSys->sHL++);
	DVOLSET(ix);
}

//!
//! Rhythmのボリューム
//!
static void DVOLSET(FM_CH_WORK *ix)
{
}
#endif


/**
 * @fn static void PCMVOL(FM_CH_WORK *ix)
 * @brief PCMのボリュームセット
 * @note MegaDriveでの設定方法を検討中
**/
static void PCMVOL(FM_CH_WORK *ix)
{
	// PCMの音階からＦＭの１６段階に補正する
	static u8 PcmToFM_TL_Cnv_Tbl[] = {
		0x00,
		0x01,
		0x02,
		0x03,
		0x04,
		0x05,
		0x06,
		0x07,
		0x08,
		0x09,
		0x0a,
		0x0b,
		0x0c,
		0x0d,
		0x0e,
		0x0f,
	};

	u8 e = GetMmlData(pSys->sHL++);

	if (pSys->PVMODE == 0)
	{
		ix->volume_pcm_6 = e;							 // -4 - 251
		ix->volume_6 = PcmToFM_TL_Cnv_Tbl[(e + 4) >> 4]; // 16段階に補正
		// FM 6CHのボリュームをセットする
		STVOL(ix);
	}
	else
	{
		ix->algorithm_7 = e;
	}

	PCMVOL_MAX(ix);
}

/**
 * @fn static void PCMVOL_MAX(FM_CH_WORK *ix);
 * @brief 発売済みタイトルの実装をみた値
 * @note 
 **/
static void PCMVOL_MAX(FM_CH_WORK *ix)
{
	u8 a = ix->channel_no_8 + 0x40;
	u32 lp;
	for (lp = 0; lp < 4; lp++) // 0x42/0x46/0x4a/0x4e
	{
		PSGOUT(a, 0x7f); //; リリース(RR) カットの処理
		a += 4;
	}
}

/**
 * @fn static void FRQ_DF(FM_CH_WORK *ix)
 * @brief Detune値の設定
 * @note SubCommand F02 (FM\DCSG)
 **/
static void FRQ_DF(FM_CH_WORK *ix)
{
	u8 d, e, a;
	u16 de, hl;

	ix->befor_code_32 = 0;

	e = GetMmlData(pSys->sHL++);
	d = GetMmlData(pSys->sHL++);
	a = GetMmlData(pSys->sHL++);

	de = GetHL(d, e);
	if (a != 0)
	{
		hl = GetHL(ix->detune_10, ix->detune_9);
		de += hl;
	}

	ix->detune_9 = GetLfromHL(de);
	ix->detune_10 = GetHfromHL(de);

	//! PCMのDT対応
	if (pSys->PCMFLG != 0)
	{
		//		u16 delt = pSys->DELT_N;		// FNUMとして書き込み済み
		//		delta + de を PCMのDELTAに書き込み

		//	PCMOUT(0x09)		// DELTA L
		//	PCMOUT(0x0a)		// DELTA H
	}
}

/**
 * @fn static void RSKIP(FM_CH_WORK *ix)
 * @brief リピートのスキップ
 * @note SubCommand FE   FM/DCSG
 * @note MDは譜面をROMで持っているのでリピート数をチャンネルデータに4回分保持
 **/
static void RSKIP(FM_CH_WORK *ix)
{
	u8 d, e;
	e = GetMmlData(pSys->sHL);
	d = GetMmlData(pSys->sHL + 1);
	u16 de = GetHL(d, e);

	u8 b = GetMmlData(pSys->sHL + de);

	if ((ix->rep_counter_37[ix->rep_ptr_36 - 1] + 1) == b)
	{
		// Skip
		ix->rep_ptr_36--;
		ix->rep_counter_37[ix->rep_ptr_36] = 0;
		pSys->sHL += de + 4; // Loop末Offset + Counter + COunter Keep
	}
	else
	{
		// no Skip
		pSys->sHL += 2;
	}
}

/**
 * @fn static void REPSTF(FM_CH_WORK *ix)
 * @brief リピートスタートコマンド
 * @note SubCommand F05　FM/DCSG
 * @note MDは譜面をROMで持っているのでリピート数をチャンネルデータに4回分保持
 **/
static void REPSTF(FM_CH_WORK *ix)
{
	// リピート終了地点のオフセット取得（MDでは使わない）
	pSys->sHL++;
	pSys->sHL++; // ; DE as REWRITE ADR OFFSET +1

	ix->rep_counter_37[ix->rep_ptr_36] = 0;
	ix->rep_ptr_36++;
}

/**
 * @fn static void REPENF(FM_CH_WORK *ix)
 * @brief リピート終端　コマンド
 * @note SubCommand F06　FM/DCSG
 * @note MDは譜面をROMで持っているのでリピート数をチャンネルデータに4回分保持
 **/
static void REPENF(FM_CH_WORK *ix)
{
	u8 b = GetMmlData(pSys->sHL);
	ix->rep_counter_37[ix->rep_ptr_36 - 1]++;

	if (ix->rep_counter_37[ix->rep_ptr_36 - 1] == b)
	{
		REPENF2(ix); /*!< リピート終了 */
	}
	else
	{
		//! リピートスタートポイントに戻る
		pSys->sHL++; //; 計算中REPカウンタ（使わないけど）
		pSys->sHL++; //; 比較用REPカウンタ

		u16 hl = GetHL(GetMmlData(pSys->sHL + 1), GetMmlData(pSys->sHL));
		pSys->sHL -= hl;
	}
}

/**
 * @fn static void REPENF2(FM_CH_WORK *ix)
 * @brief リピート終了サブルーチン
 **/
static void REPENF2(FM_CH_WORK *ix)
{
	ix->rep_counter_37[ix->rep_ptr_36 - 1] = 0;
	ix->rep_ptr_36--;

	pSys->sHL += 4; // REP CTR WK(1)/REP CTR(1)/RET OFFSET(2)
}

/**
 * @fn static void STENV(FM_CH_WORK *ix)
 * @brief FM音色セット サブルーチン
 **/


#if !PSGOUT_ASM2
void STENV(FM_CH_WORK *ix)
{
	KEYOFF(ix);

	u8 wreg = ix->channel_no_8 + 0x80;
	u32 lp;
	for (lp = 0; lp < 4; lp++)
	{
		PSGOUT(wreg, 0x0f); //; リリース(RR) カットの処理
		wreg += 4;
	}

	// Tone Dataアドレス取得
	u16 tone_ofs = (u16)ix->tone_no_1;
	tone_ofs *= 25;

	// Z80 0x8051  >> OtoDat
	u8 *oto_68k = GetDataAdress(pSys->OtoDatOffset_68k);
	oto_68k += tone_ofs;
	oto_68k++;

	u8 c, b;
	wreg = 0x30 + ix->channel_no_8;
	for (c = 0; c < 4; c++)
	{
		for (b = 0; b < 6; b++)
		{
			PSGOUT(wreg, *oto_68k++);
			wreg = wreg + 4;
		}
	}
	u8 algo_feed = *oto_68k;
	ix->algorithm_7 = algo_feed & 0x07;
	PSGOUT(0xb0 + ix->channel_no_8, algo_feed);
}

#else
static void STENV(FM_CH_WORK *ix)
{

	Z80_requestBus(TRUE);

	//	KEYOFF(ix);		この関数内に組み込んだ
	if (pSys->PCMFLG != 0)
	{
		PCMEND(ix);
	}
	else
	{
		PSGOUT2(FM_REG_0x28, pSys->FMPORT + ix->channel_no_8);
	}

	u8 wreg = ix->channel_no_8 + 0x80;
	u32 lp;
	for (lp = 0; lp < 4; lp++)
	{
		PSGOUT2(wreg, 0x0f); //; リリース(RR) カットの処理
		wreg += 4;
	}

	// Tone Dataアドレス取得
	u16 tone_ofs = (u16)ix->tone_no_1;
	tone_ofs *= 25;

	// Z80 0x8051  >> OtoDat
	u8 *oto_68k = GetDataAdress(pSys->OtoDatOffset_68k);
	oto_68k += tone_ofs;
	oto_68k++;

#if OPTIMIZ_TEST
	u8 reg_idx;
	wreg = 0x30 + ix->channel_no_8;

		for (reg_idx = 0; reg_idx < 24; reg_idx++)		// 6 Param * 4 OP
		{
			PSGOUT2(wreg, *oto_68k++);
			wreg = wreg + 4;
		}
#else
	u8 c, b;
	wreg = 0x30 + ix->channel_no_8;

	for (c = 0; c < 4; c++)
	{
		for (b = 0; b < 6; b++)
		{
			PSGOUT2(wreg, *oto_68k++);
			wreg = wreg + 4;
		}
	}
#endif

	u8 algo_feed = *oto_68k;
	ix->algorithm_7 = algo_feed & 0x07;
	PSGOUT2(0xb0 + ix->channel_no_8, algo_feed);

	YM2612_writeSafe(0, 0x2a);
	Z80_releaseBus();
}

#endif


/**
 * @fn static void STVOL(FM_CH_WORK *ix)
 * @brief FMのボリュームセット
**/
static void STVOL(FM_CH_WORK *ix)
{
	STV1(ix);
}

/**
 * @fn static void STV1(FM_CH_WORK *ix)
 * @brief FMのボリュームセット
**/
static void STV1(FM_CH_WORK *ix)
{
	u8 vol;
	vol = pSys->TOTALV + ix->volume_6;
	if (vol > 20)
	{
		vol = 0;
	}
	STV2(ix, vol);
}

/**
 * @fn static void STV2(FM_CH_WORK *ix)
 * @brief FMのボリュームセット
**/
static void STV2(FM_CH_WORK *ix, u8 vol_idx)
{
	//!	ボリュームデータ
	static const u8 FMVDAT[] = {
		// ボリュームデータ (FM)
		0x36, 0x33, 0x30, 0x2D,
		0x2A, 0x28, 0x25, 0x22, //;  0,  1,  2,  3
		0x20, 0x1D, 0x1A, 0x18, //;  4,  5,  6,  7
		0x15, 0x12, 0x10, 0x0D, //;  8,  9, 10, 11
		0x0A, 0x08, 0x05, 0x02, //; 12, 13, 14, 15
	};

	//!	ボリュームデータ(TLを設定するオペレーターフラグ)
	static const u8 CRYDAT[] = {
		// キャリア / モジュレータのデータ
		0x08, //
		0x08, //  各ビットがキャリア/モジュレータを表わす
		0x08, //
		0x08, //  Bit=1 が、キャリア
		0x0C, //       0 が、モジュレータ
		0x0E, //
		0x0E, //  Bit0=OP 1 , Bit1=OP 2 ... etc
		0x0F, //
	};

	u8 vol_e;
	vol_e = FMVDAT[vol_idx]; // GET VOLUME DATA　(0-19)

	u8 ch_reg_a;
	ch_reg_a = ix->channel_no_8 + FM_REG_0x40; // GET PORT No.

	u8 crydat = CRYDAT[ix->algorithm_7]; // INPUT ALGOLIZM

#if !PSGOUT_ASM2
	//STVOL2:
	u32 lp;
	for (lp = 0; lp < 4; lp++)
	{
		if (crydat & BIT_0)
		{
			PSGOUT(ch_reg_a, vol_e);
		}
		crydat = crydat >> 1;
		ch_reg_a += 4;
	}
#else
	Z80_requestBus(TRUE);

	u32 lp;
	for (lp = 0; lp < 4; lp++)
	{
		if (crydat & BIT_0)
		{
			PSGOUT2(ch_reg_a, vol_e);
		}
		crydat = crydat >> 1;
		ch_reg_a += 4;
	}

	YM2612_writeSafe(0, 0x2a);
	Z80_releaseBus();

#endif
}

#if OBSOLETE_CODE
/*
; -----------------------------------------------------------------------------	
; Timer-B カウンタ・セット　ルーチン
; IN: E<= TIMER_B COUNTER
; @note TIMERB Driveに対応
STTMB:
	PUSH AF
	PUSH DE

STTMB2:
	LD D,26H            ;Timer-B  D:26h E:Timer-B
	CALL PSGOUT

	LD D,27H
	LD E,78H
	CALL PSGOUT     ;  Timer-B OFF
	LD E,7AH
	CALL PSGOUT     ;  Timer-B ON

;;    LD A,5            ; PC88 サウンド割り込み以下を停止
;;    OUT (0E4H),A

	POP DE
	POP AF

	RET
*/
#endif

//! ***************************************************************************
//!
//! LFO音源処理
//!
//! ***************************************************************************

//-----------------------------------------------------------------------------
/**
 * @fn static void PLLFO(FM_CH_WORK *ix)
 * @brief Software LOF処理のTOP関数(FM&DCSG)
 * @note FSUB/SSUBで設定したFNUMを操作する
**/
static void PLLFO(FM_CH_WORK *ix)
{
	if ((ix->flag_31 & FM_31_BIT7_LFO) == 0)
	{
		return;
	}

	u8 a = GetMmlData(pSys->sHL - 1);
	if (a == 0xf0)
	{ // ;  以前のデータが '&' なら RET
		return;
	}

	if (ix->flag_31 & FM_31_BIT5_LFO_CONTINUE)
	{			   //; LFO CONTINE FLAG
		CTLFO(ix); //; bit 5 = 1 なら LFO 継続
		return;
	}

	//! LFO INITIARIZE
	LFORST(ix);
	LFORST2(ix);
	ix->lfo_counter_wk_22 = ix->lfo_counter_keep_21;
	ix->flag_31 |= FM_31_BIT5_LFO_CONTINUE;

	CTLFO(ix);
}

/**
 * @fn static void CTLFO(FM_CH_WORK *ix)
 * @brief LFO Continue 継続処理
 * @note DlayTimeのカウント処理
**/
static void CTLFO(FM_CH_WORK *ix)
{
	if (ix->lfo_delay_wk_20 == 0)
	{
		CTLFO1(ix);
	}
	else
	{
		ix->lfo_delay_wk_20--; // Delay中
	}
}

/**
 * @fn static void CTLFO1(FM_CH_WORK *ix)
 * @brief LFO Continue 継続処理
 * @note DlayTimeのカウント処理
**/
static void CTLFO1(FM_CH_WORK *ix)
{
	ix->lfo_counter_wk_22--;
	if (ix->lfo_counter_wk_22 != 0)
	{
		return;
	}

	ix->lfo_counter_wk_22 = ix->lfo_counter_keep_21;

#if OPTIMIZ_TEST
	if (ix->lfo_peak_level_wk_28 == 0){ /*!<  GET PEAK LEVEL COUNTER (P.L.C) */
		u16 de = GetHL(ix->lfo_range_h_wk_26, ix->lfo_range_l_wk_25);
		de = 0 - de; //; WAVE 反転　(変化量の反転)
		SetHL(de, &ix->lfo_range_h_wk_26, &ix->lfo_range_l_wk_25);
		ix->lfo_peak_level_wk_28 = ix->lfo_peak_level_keep_27; // P.L.C 再設定
	}
#else
	if (ix->lfo_peak_level_wk_28 != 0) /*!<  GET PEAK LEVEL COUNTER (P.L.C) */
	{
		//		PLLFO1(ix);		//	;  P.L.C > 0 なら PLLFO1
	}
	else
	{
		u16 de = GetHL(ix->lfo_range_h_wk_26, ix->lfo_range_l_wk_25);
		de = 0 - de; //; WAVE 反転　(変化量の反転)
		SetHL(de, &ix->lfo_range_h_wk_26, &ix->lfo_range_l_wk_25);

		ix->lfo_peak_level_wk_28 = ix->lfo_peak_level_keep_27; // P.L.C 再設定
	}
#endif

// PLLFO1:
	ix->lfo_peak_level_wk_28--; //; P.L.C.-1   (変化回数 DEC)
	PLS2(ix);
}

/**
 * @fn static void PLS2(FM_CH_WORK *ix)
 * @brief LFO出力処理の、FM/PCM分岐
 * @note 
**/
static void PLS2(FM_CH_WORK *ix)
{
	if (pSys->PCMFLG == 0)
	{
		PLSKI2(ix); /*!< FM/SSG LFO処理 */
	}
#if OBSOLETE_CODE
	else
	{
		//		u16 hl = GetHL(ix->lfo_range_h_wk_26, ix->lfo_range_l_wk_25); /*!< PCM LFO */
		//		pSys->DELT_N += hl;
		//		PCMOUT(0x09)
		//		PCMOUT(0x0a)
	}
#endif
}

/**
 * @fn static void PLSKI2(FM_CH_WORK *ix)
 * @brief LFOの振幅とfnumを加算して出力
 * @note 
**/
static void PLSKI2(FM_CH_WORK *ix)
{
	u16 hl = GetHL(ix->lfo_range_h_wk_26, ix->lfo_range_l_wk_25); /*!< PLLFO1: で取得した値 */
	u16 de = GetHL(ix->fnum2_data_30, ix->fnum1_data_29);		  /*!< GET FNUM1 / B/FNUM2 */
																  //	hl = (s16)hl + (s16)de;								  /*!< HL= NEW F-NUMBER */
	hl = hl + de;												  /*!< HL= NEW F-NUMBER */
	SetHL(hl, &ix->fnum2_data_30, &ix->fnum1_data_29);			  /*!< SET NEW SET / F-NUM2 */

	if (pSys->SSGF1 == 0)
	{
		//! FOR FM LFO
		pSys->NEWFNM = hl; //LFOP5の処理を前倒し
		LFOP5(ix);		   //; FM音源ならLFOP5に移動
	}
	else
	{
		//! FOR SSG LFO
		u8 a = (ix->befor_code_32 >> 4) & 0x0f; // GET KEY CODE&OCTAVE　(BEFORE CODE)
		hl = hl >> a;
		SSSUB_ToneRegSet(ix, (u16)hl);
	}
}

/**
 * @fn static u16 ConvSSGTone2DCSG(FM_CH_WORK *ix, u16 hl)
 * @brief PC88(SSG)のFnumをDCSGに変換
 *
 * @note  IN:HL<=PSGの音程 H 上位 4Bit　L 下位 8Bit
 * @note 
 * @note  PC88 SSGのTONEをDCSGに変換
 * @note  PC88 12bit
 * @note  B Fine Tune 8Bit   下位音階
 * @note  A Coarse Tune 4Bit 上位音階
 * @note  76543210 76543210
 * @note  ----AAAA BBBB BBBB
 * @note 
 * @note  MegaDrive 10bit
 * @note  A/B Channel
 * @note  C Tone Data
 * @note 
 * @note  76543210 76543210
 * @note  ABBBCCCC --CCCCCC
 * @note 　　　下位     上位
 * @note  88 3816　/ o1 c
 * @note  MD 3420  / o1 c
 * @note  0.89  >>>> DTやLFOの周波数の値は0.9倍ぐらいすると正しくなる？
 * @note 
 * @note  Noise
 * @note  0xe0
 * @note  D2(FB):1bit 0 同期ノイズ 1 ホワイトノイズ
 * @note  D1-D0(NF1/0)
 * @note  00　（N/32)/16   N基本クロック
 * @note  01　（N/32)/32   N基本クロック
 * @note  10　（N/32)/64   N基本クロック
 * @note  11　トーンジェネレーター3(ノイズの音色を連続可変が可能)
 * @note 
 * @note  0xf0
 * @note   A0-A3 アッテネーション
**/
static u16 ConvSSGTone2DCSG(FM_CH_WORK *ix, u16 hl)
{
	u8 byte_1;
	u8 byte_2;
	byte_1 = hl & 0x0f;		   // 下位4BIT
	byte_2 = (hl >> 4) & 0x3f; // 下位4BIT

	return (GetHL(byte_1, byte_2));
}

/**
 * @fn static void LFOP5(FM_CH_WORK *ix)
 * @brief FM LFO
 * @note 
**/
static void LFOP5(FM_CH_WORK *ix)
{
	if ((ix->flag_33 & FM_33_BIT6_TTL_TREMOLO_FLAG) != 0)
	{
		LFOP6(ix);
	}
	else if ((ix->channel_no_8 & BIT_1) == 0) // ;  CH=3?
	{
		PLLFO2(ix); //; NOT CH3 THEN PLLFO2
	}
	else if (pSys->PLSET1 != 0x78)
	{
		PLLFO2(ix); //	; NOT SE MODE
	}
	else
	{
		//!　書き込みOp選択
		static const u8 OP_SEL[] = {
			0xA6, 0xAC, 0xAD, 0xAE //; OP 4,3,1,2
		};

		//		pSys->NEWFNM = hl;	// 事前に処理
		u8 *detdat = &DETDAT[0];
		u8 *op_sel = (u8 *)&OP_SEL[0];

		u32 lp;
		for (lp = 0; lp < 4; lp++)
		{
			u16 hl = pSys->NEWFNM + (u16)*detdat;
			detdat++; // inc hl

			PSGOUT(*op_sel, GetHfromHL(hl));			  // H
			PSGOUT((*op_sel) - 4, GetLfromHL(hl & 0xff)); // l
			op_sel++;
		}
	}
}

/**
 * @fn static void PLLFO2(FM_CH_WORK *ix)
 * @brief LFOの変化周波数を書き込む
 * @note 
**/
static void PLLFO2(FM_CH_WORK *ix)
{
	u16 hl = pSys->NEWFNM;

	PSGOUT(FM_REG_0xa4 + ix->channel_no_8, GetHfromHL(hl));		//;  PORT A4H
	PSGOUT(FM_REG_0xa4 + ix->channel_no_8 - 4, GetLfromHL(hl)); //;  F-NUMBER1 DATA
}

/**
 * @fn static void PLLFO2(FM_CH_WORK *ix)
 * @brief LFOの変化周波数を書き込む/ Soft Env?
 * @note 
**/
static void LFOP6(FM_CH_WORK *ix)
{
	u8 l = pSys->NEWFNM & 0xff; //LFOP5の処理を前倒し

	u8 c = 1;
	u8 a = 0x40 + ix->channel_no_8;

	if (c & BIT_0)
	{
		// static void LFP62(FM_CH_WORK *ix)
		PSGOUT(a, l);
		a += 4;
	}
	if (c & BIT_2)
	{
		// static void LFP62(FM_CH_WORK *ix)
		PSGOUT(a, l);
		a += 4;
	}
	if (c & BIT_1)
	{
		// static void LFP62(FM_CH_WORK *ix)
		PSGOUT(a, l);
		a += 4;
	}
	if (c & BIT_3)
	{
		// static void LFP62(FM_CH_WORK *ix)
		PSGOUT(a, l);
	}
}

#if 0
static void LFP62(FM_CH_WORK *ix)
{
	/*
	LD	D,A
	CALL	PSGOUT
	ADD	A,4
	RET
	*/
}
#endif

/**
 * @fn static void LFORST(FM_CH_WORK *ix)
 * @brief RESET PEAK L.&DELAY
 * @note 
**/
static void LFORST(FM_CH_WORK *ix)
{
	u8 a = ix->lfo_delay_keep_19;
	ix->lfo_delay_wk_20 = a;
	ix->flag_31 &= (~FM_31_BIT5_LFO_CONTINUE);
}

/**
 * @fn static void LFORST2(FM_CH_WORK *ix)
 * @brief LFO PEAK LEVEL / 変化量 再設定
 * @note 
**/
static void LFORST2(FM_CH_WORK *ix)
{
	u8 a;
	a = ix->lfo_peak_level_keep_27;
	ix->lfo_peak_level_wk_28 = a >> 1; // ; LFO PEAK LEVEL 再設定

	a = ix->lfo_range_l_keep_23;
	ix->lfo_range_l_wk_25 = a; // ; 変化量　再設定

	a = ix->lfo_range_h_keep_24;
	ix->lfo_range_h_wk_26 = a;
}

/*******************************************************************************
 * 
 * DCSG音源処理
 * 
********************************************************************************/

// =======================================================================
/**
 * @fn static void SSGSUB(FM_CH_WORK *ix)
 * @brief DCSG音源メイン処理
 * @note 
**/
static void SSGSUB(FM_CH_WORK *ix)
{
	ix->counter_0--;
	if (ix->counter_0 == 0)
	{
		SSSUB7(ix); // 次の音符へ
	}
	else if (ix->counter_0 != ix->q_on_18) // ; 'q' スタカートのClock減算 (A - r)
	{
		SSSUB0(ix); // 発声継続 / To REVERVE/SOFT ENV
	}
	else
	{
		// スタッカートで発声短縮
		u8 a = GetMmlData(pSys->sHL); //HL=SOUND DATA ADD
		if (a == MML_CMD_TIE_SLUR)
		{
			SSUB0(ix);
		}
		else
		{
			SSSUBA(ix); //; TO REREASE
		}
	}
}

/**
 * @fn static void SSUB0(FM_CH_WORK *ix)
 * @brief DCSG音符終端処理（長さ終了）
 * @note 
**/
static void SSUB0(FM_CH_WORK *ix)
{
	ix->flag_31 &= ~FM_31_BIT6_KEYOFF; // TODO DCSGは　SET TIE FLAG　？
	SSSUB0(ix);
}

/**
 * @fn static void SSSUB0(FM_CH_WORK *ix)
 * @brief ソフトウェアエンベロープ処理とボリューム設定
 * @note 
**/
static void SSSUB0(FM_CH_WORK *ix)
{
	if ((ix->volume_6 & DCSG_6_BIT7_SOFT_ENV) == 0)
	{
		// SOFT ENV 無し
		return;
	}
	else
	{
		SOFENV(ix);
		SSSUB_VolRegSet(ix, pSys->SOFEV7_ret);
	}
}

/**
 * @fn static void SSSUB7(FM_CH_WORK *ix)
 * @brief DCSGの次の音符・コマンドの処理
 * @note 
**/
static void SSSUB7(FM_CH_WORK *ix)
{
	u8 mml_cmd = GetMmlData(pSys->sHL);

	if (mml_cmd == MML_CMD_TIE_SLUR) /*!< 0xfdは、タイ/スラー */
	{
		// SSUB1:
		ix->flag_31 &= ~FM_31_BIT6_KEYOFF;
		pSys->sHL++;
	}
	else
	{
		// SSSUBE:
		ix->flag_31 |= FM_31_BIT6_KEYOFF;
	}

	//! サブコマンド
	do
	{
		mml_cmd = GetMmlData(pSys->sHL);
		if (mml_cmd == 0)
		{
			// z
			ix->flag_31 |= FM_31_BIT0_LOOPEND;
			if ((ix->data_top_adress_4 | ix->data_top_adress_5) == 0)
			{
				SSGEND(ix);
				return;
			}
			else
			{
				pSys->sHL = GetHL(ix->data_top_adress_5, ix->data_top_adress_4);
				mml_cmd = GetMmlData(pSys->sHL); // ;INPUT FLAG &LENGTH A=DATA ループした時はデータを再取得
			}
		}
		// nz
		pSys->sHL++;

		// サブコマンドならコマンドを実行
		if (mml_cmd < 0xf0)
		{
			break; // ループを抜けて音符・休符処理に移動
		}
		SSSUB8(ix, mml_cmd); // ; DATA がコマンドなら FMSUBA へ
	} while (1);

	ix->counter_0 = mml_cmd & 0x7f;
	if (mml_cmd & 0x80)
	{
		//! 休符処理
		SSSUBA(ix);
		SETPT(ix);
	}
	else
	{
		SSSUB6(ix);
	}
}

/**
 * @fn static void SSSUB6(FM_CH_WORK *ix)
 * @brief DCSGの F-Num の算出
 * @note SET FINE TUNE & COARSE TUNE
**/
static void SSSUB6(FM_CH_WORK *ix)
{
	// [SE]
	if (pSys->SEMODE == 0x00)
	{
		// [SE]割り込まれていないのでDCSGOUTする
		ix->se_int_41 &= ~DCSG_41_BIT7_SE_DCSG_OFF; // ; DCSG出力抑制フラグをクリア(IX+41 0Bit)
	}

	//SSSUB6skip
	u8 keycode = GetMmlData(pSys->sHL++);
	if ((ix->flag_31 & FM_31_BIT6_KEYOFF) == 0)
	{
		// TIE
		if (ix->befor_code_32 == keycode)
		{
			SETPT(ix);
			return;
		}
	}

	//SSSKIP0:
	ix->befor_code_32 = keycode; // ; STORE KEY CODE & OCTAVE

#if USE_MUCOM88_DEBUG
	DCSG_RangeCk(ix); //		; for Debug DCSGの演奏範囲かチェック
#endif

	u16 fnum = SNUMB[keycode & 0x0f]; //;  GET KEY CODE

	u16 hl = GetHL(ix->detune_10, ix->detune_9); // GET DETUNE DATA
												 // DETUNE PLUS		FNUMにDTが加算(DTはユーザ補正すべき？)
	hl += fnum;

	SetHL(hl, &ix->fnum2_data_30, &ix->fnum1_data_29); // SAVE FOR LFO

	keycode = keycode >> 4; // Octave情報 0xf0
	hl = hl >> keycode;

	SSSUB4(ix, hl);
}

/**
 * @fn static void SSSUB4(FM_CH_WORK *ix, u16 hl)
 * @brief DCSGの周波数セット・ソフトエンベロープ初期化
**/
static void SSSUB4(FM_CH_WORK *ix, u16 hl)
{
	//! DCSGノイズ対応のサブルーチンに変更
	SSSUB_ToneRegSet(ix, hl);

	if ((ix->flag_31 & FM_31_BIT6_KEYOFF) == 0)
	{
		// Z
		SOFENV(ix); // KEYOFFされていないので、エンベロープ継続
		SSSUB9(ix); // POP HL >> SetVol
	}
	else
	{
		// SSSUBF:			; KEYON された時の処理
		if ((ix->dcsg_hard_env_33 & 0x80) != 0)
		{
			// HARD ENV 処理
			SSSUBH(ix);
			SSSUB9(ix);
		}
		else
		{
			SSSUBG(ix); // ソフトウェアエンベロープ初期化
		}
	}
}

#if 0
; ---	HARD ENV. KEY ON	---
/*
	; DCSGはHARD ENVに対応していない
    LD	E,16            ; HARD ENV.では最大ボリューム
    LD	D,(IX+7)
    CALL	PSGOUT		; HARD ENV.KEYON

    ld  a,16
    call    ConvVol2DCSG
    or	a,(IX+7)
    call    DCSGOUT     ; HARD ENV.KEYON

    LD	A,(IX+33)
    AND	00001111B       ; HARD ENV. TYPE
    LD	E,A
    LD	D,0DH
    CALL	PSGOUT      ; MDにHARD ENV.は存在しない
	JR	SSSUBH          ; SOFT ENVEをスキップ
*/
#endif

/**
 * @fn static void SSSUBG(FM_CH_WORK *ix)
 * @brief ソフトウェアエンベロープ KEYON 処理
**/
static void SSSUBG(FM_CH_WORK *ix)
{
	u8 a = ix->volume_6 & 0x0f;
	//	ix->volume_6 = a | 0x90;
	ix->volume_6 = a | (DCSG_6_BIT4_ATTACK | DCSG_6_BIT7_SOFT_ENV);

	ix->dcsg_senv_vol_11 = ix->dcsg_senv_al_12; // ENVE INIT(SOFT ENVのボリュームをRESET)
	ix->flag_31 &= ~FM_31_BIT5_LFO_CONTINUE;	// ; RESET LFO CONTINE FLAG

	SOFEV7(ix);

	SSSUBH(ix); // PEEK DELAY再設定
	SSSUB9(ix); // POP HL >> SetVol
}

/**
 * @fn static void SSSUBH(FM_CH_WORK *ix)
 * @brief LOF PEEK/DELAY 再設定
 * @brief 直後にSSSUB9 を呼ぶこと
**/
static void SSSUBH(FM_CH_WORK *ix)
{
	ix->lfo_peak_level_wk_28 = ix->lfo_peak_level_keep_27 >> 1; // ;  LFO PEAK LEVEL 再設定
	ix->lfo_delay_wk_20 = ix->lfo_delay_keep_19;				// ;  LFO DELAY の、再設定
}

/**
 * @fn static void SSSUB9(FM_CH_WORK *ix)
 * @brief SOFENVで設定したボリューム(pSys->SOFEV7_ret)をDCSGに書き込み
 **/
static void SSSUB9(FM_CH_WORK *ix)
{
	SSSUB3(ix, pSys->SOFEV7_ret);
}

/**
 * @fn static void SSSUB3(FM_CH_WORK *ix, u8 vol)
 * @brief VOLUME OUT PROCESS
 * @brief vol の値をDCSGに書き込み
 * @note ENTRY A: VOLUME DATA
 */
static void SSSUB3(FM_CH_WORK *ix, u8 vol)
{
	if ((ix->dcsg_hard_env_33 & DCSG_33_BIT7_HARDENV) != 0)
	{
		//		KLog("[DCSG]HARD ENV:");
	}
	else
	{
		SSSUB_VolRegSet(ix, vol);
	}
	SETPT(ix); /*!< SET POINTER */
}

/**
 * @fn static void SETPT(FM_CH_WORK *ix)
 * @brief HLの保存(別途処理している)
 * @note 別途保存しているので削除予定
 */
static void SETPT(FM_CH_WORK *ix)
{
	SetHL(pSys->sHL, &ix->data_adress_3, &ix->data_adress_2);
}

/**
 * @fn static const u16 SNUMB[];
 * @brief DCSG用F-Numテーブル
 * @note ENTRY A: VOLUME DATA
 */
static const u16 SNUMB[] = {
	//! kumaさんのmucomMD2vgmのドキュメントを参考
	//! <https://github.com/kuma4649/mucomMD2vgm/blob/master/mucomMD2vgm/FNUM_SN76489.txt>
	0x00d5c, 0x00c9c, 0x00be8, 0x00b3c, 0x00a9c, 0x00a04, /*!< DCSG F-Num */
	0x00974, 0x008ec, 0x0086c, 0x007f0, 0x00780, 0x00714  /*!< DCSG F-Num */
};

/**
 * @fn static void SSSUB_ToneRegSet(FM_CH_WORK *ix, u16 hl)
 * @brief DCSG 周波数の書き込み
 * @param u16 hl:DCSG fnum
 **/
static void SSSUB_ToneRegSet(FM_CH_WORK *ix, u16 hl)
{
	//! SSGノイズ互換のためのwコマンドパラメータ変換テーブル
	//! デフォルトでは変換前後が同じ値なので、PC88の周波数に合わせるなら書き換えが必要
	//! Tpが0の場合、GensKmodでは再生周波数が0Hzと表示される。1からスタートが良い?　要実機確認
	static const u16 TP5BitToDcsg12BItTbl[] =
		{
			1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,			// ノイズ変換テーブル
			16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, //
		};

	if (ix->dcsgvol_reg_7 == DCSG_ATT3_D0H)
	{ //	; F ch.
		if (ix->noise_mode_42 != 0)
		{
			DCSGOUT(ix, DCSG_NOISE4_E0H | 0x07); // ; WhiteNoise Toneモード
			u8 a = ix->noise_freq_43;
			hl = TP5BitToDcsg12BItTbl[a]; // GET FNUM2 :音程上位 0E80Ehの0e
										  // GET FNUM1 :音程下位 0E80Ehのe8
		}
	}

	//! SSSUB_ToneSet:		; PSGのVolumeSet
	u16 hl2 = ConvSSGTone2DCSG(ix, hl); //        ; hl: h上位4Bit l 下位8Bit
	u8 a;
	a = ix->channel_no_8 | GetHfromHL(hl2); // 音程上位
	DCSGOUT(ix, a);
	a = GetLfromHL(hl2); // 音程下位
	DCSGOUT(ix, a);
}

/**
 * @fn static void SSSUB_VolRegSet(FM_CH_WORK *ix, u8 vol)
 * @brief Noise対応のDCSGボリュームセット
 **/
static void SSSUB_VolRegSet(FM_CH_WORK *ix, u8 vol)
{
	u8 a = ix->dcsgvol_reg_7;
	if (a != DCSG_ATT3_D0H)
	{ // ; D/E Ch のボリューム
		SSSUB_VolSet00(ix, vol);
	}
	else if (ix->noise_mode_42 == 0)
	{
		// F ch が通常トーン
		SSSUB_VolSet(ix); // Toneモードなのでノイズのボリュームは0にする
		SSSUB_VolSet00(ix, vol);
	}
	else
	{
		// ; F CH Noiseレジスタに出力
		u8 new_vol = ConvVol2DCSG(vol);
		DCSGOUT(ix, new_vol | 0xf0); // NOISE Vol(出力)
		SSSUB_VolSet00(ix, 0);		 // TONE Vol 0
	}
}

/**
 * @fn static void SSSUB_VolSet(FM_CH_WORK *ix)
 * @brief Noise対応のDCSGボリュームセット
 **/
static void SSSUB_VolSet(FM_CH_WORK *ix) //				; PSGのVolumeSet
{
	DCSGOUT(ix, 0xff); /*!< Vol(出力) Toneモードなのでノイズのボリュームは0に設定 */
}

/**
 * @fn static void SSSUB_VolSet00(FM_CH_WORK *ix, u8 vol)
 * @brief 
 **/
static void SSSUB_VolSet00(FM_CH_WORK *ix, u8 vol)
{
	// new_volは、0が最高音量　fが無音（MD）
	u8 new_vol = ConvVol2DCSG(vol);
	DCSGOUT(ix, new_vol | ix->dcsgvol_reg_7); /*!< Vol(出力) MAX 0 - f MIN(MD) */
}

/**
 * @fn static void DCSG_RangeCk(FM_CH_WORK *ix)
 * @brief DCSG範囲外の低温がリクエストされた回数をカウント
 *           0    1    2    3    4    5    6    7    8    9    a    b
 * '         c    c+   d    d+   e    f    f+   g    g+   a    a+   b
 * 'FNUM_00=$3FF,$3FF,$3FF,$3FF,$3FF,$3FF,$3FF,$3FF,$3FF,$3FF,$3FF,$3FF
 * 'FNUM_00=$3FF,$3FF,$3FF,$3FF,$3FF,$3FF,$3FF,$3FF,$3FF,$3F9,$3C0,$38A
 **/
static void DCSG_RangeCk(FM_CH_WORK *ix)
{
	u8 code;
	code = ix->befor_code_32;

	// Octaveチェック
	if ((code >> 4) >= 2)
	{
		return;
	}

	if ((code & 0x0f) >= 9)
	{ // Check KEY CODE
		return;
	}
	pSys->DCSG_OCT_UNDER++;
}

/**
 * @fn static void SSSUBA(FM_CH_WORK *ix)
 * @brief KEY OFF 時の RR 処理
 **/
static void SSSUBA(FM_CH_WORK *ix)
{
	//　HARD ENV. KEY OFF
	if ((ix->flag_33 & DCSG_33_BIT7_HARDENV) == 0)
	{
		SSUBAB(ix); /*!< SOFT ENV */
	}
	else
	{
		/*
		// HARD ENV
	LD	E,0				; HARD ENVの処理はマスク
	LD	D,(IX+7)
	CALL	PSGOUT		; HARD ENV.KEYOFF
*/
		SSUBAB(ix); //	; NOT HARD ENV.
	}
}

/**
 * @fn static void SSUBAB(FM_CH_WORK *ix)
 * @brief 
 **/
static void SSUBAB(FM_CH_WORK *ix)
{
	if ((ix->flag_33 & FM_33_BIT5_REVERVE_FLAG) == 0)
	{
		//		SSUBAC(ix);
	}
	else
	{
		ix->flag_31 &= ~FM_31_BIT6_KEYOFF;
		SSSUB0(ix);
		return;
	}

	//SSUBAC:
	if ((ix->volume_6 & DCSG_6_BIT7_SOFT_ENV) == 0)
	{
		// SSSUB3
		SSSUB3(ix, 0); //		; SoftEnv処理をして、Volumeをレジスタにセット
	}
	else
	{
		ix->volume_6 = ix->volume_6 & 0x8f;
		SOFEV9(ix);
		SSSUB3(ix, pSys->SOFEV7_ret); //		; SoftEnv処理をして、Volumeをレジスタにセット
	}
}

// -----------------------------------------------------------------------
/**
 * @fn static void SSSUB8(FM_CH_WORK *ix, u8 cmd)
 * @brief DCSG サブコマンド
**/
static void SSSUB8(FM_CH_WORK *ix, u8 cmd)
{
	//! PSG COMMAND TABLE
	static void (*PSGCOM[])(FM_CH_WORK *) =
		{
			OTOSSG, // ;F0-音色セット         '@'              1
			PSGVOL, // ;F1-VOLUME SET
			FRQ_DF, // ;F2-DETUNE
			SETQ,	// ;F3-COMMAND OF        'q'
			LFOON,	// ;F4-LFO                             4
			REPSTF, // ;F5-REPEAT START SET  '['
			REPENF, // ;F6-REPEAT END SET    ']'
			NOISE,	// ;F7-MIX PORT          'P'                  2
			NOISEW, // ;F8-NOIZE PARAMATER   'w'
			FLGSET, // ;F9-FLAGSET
			ENVPST, // ;FA-SOFT ENVELOPE     'E'
			VOLUPS, // ;FB-VOLUME UP    ')'
			NTMEAN, // ;FC-
			TIE,
			RSKIP,
			SECPRC, // ;FF- to sec com
		};

	PSGCOM[cmd & 0x0f](ix);
}

/**
 * @fn static void HRDENV(FM_CH_WORK *ix)
 * @brief HARD ENVE SET
**/
static void HRDENV(FM_CH_WORK *ix)
{
	// TODO 0x0dって何？
	u8 d, e;
	d = 0x0d;
	e = GetMmlData(pSys->sHL++);
	PSGOUT(d, e);
	e |= 0x80;		 //; SET H.E FLAG
	ix->flag_33 = e; // ; H.E MODE
	ix->volume_6 = 16;
}

/**
 * @fn static void ENVPOD(FM_CH_WORK *ix)
 * @brief HARD ENVE PERIOD
 * @note SubCommand2 FFF2 FM
**/
static void ENVPOD(FM_CH_WORK *ix)
{
	// TODO 0x0bって何？
	u8 d, e;
	d = 0x0b;
	e = GetMmlData(pSys->sHL++);
	PSGOUT(d, e);

	e = GetMmlData(pSys->sHL++);
	d++;
	PSGOUT(d, e);
}

/**
 * @fn static void W_REG(FM_CH_WORK *ix)
 * @brief Write REG
 * @note SubCommand2 FA FM
 * @note yコマンド用の直接レジスタ書き込み
**/
static void W_REG(FM_CH_WORK *ix)
{
	u8 d_reg, e_param;
	d_reg = GetMmlData(pSys->sHL++);
	e_param = GetMmlData(pSys->sHL++);
	PSGOUT(d_reg, e_param);
}

/**
 * @fn static void NOISE(FM_CH_WORK *ix)
 * @brief MIX PORT CONTROL
 * @note SubCommand2 F7 DCSG
 * @note yコマンド用の直接レジスタ書き込み
 * @note SSGでは、3チャンネル全てに、発生、ノイズ、トーン、トーン＋ノイズを選択可能
 * @note DCSGではFCh.のみ、Tone/Noiseの切り替えを行う。P0:未対応 P1:トーン P2:ノイズ　P3:未対応(ノイズと等価)
**/
static void NOISE(FM_CH_WORK *ix)
{
	u8 noise_flag = GetMmlData(pSys->sHL++);
	if (ix->dcsgvol_reg_7 == DCSG_ATT3_D0H) // ; F Ch
	{
		//; PC88 MixerControl Reg NOISE_00
		//; Noise:CBA Tone:CBA
		ix->noise_mode_42 = noise_flag & DCSG_42_NOISE;
	}
}

/**
 * @fn static void NOISEW(FM_CH_WORK *ix)
 * @brief ノイズ周波数
 * @note SubCommand2 F8 DCSG
**/
static void NOISEW(FM_CH_WORK *ix)
{
	u8 noise_tone = GetMmlData(pSys->sHL++);

	if (ix->dcsgvol_reg_7 == DCSG_ATT3_D0H) // ; F Ch
	{
		// Fch.ならNoiseToneを反映する
		ix->noise_freq_43 = noise_tone;
	}
}

/**
 * @fn static void ENVPST(FM_CH_WORK *ix)
 * @brief エンベロープ　パラメータセット
 * @note SubCommand2 FA DCSG
**/
static void ENVPST(FM_CH_WORK *ix)
{
	ix->dcsg_senv_al_12 = GetMmlData(pSys->sHL++);
	ix->dcsg_senv_ar_13 = GetMmlData(pSys->sHL++);
	ix->dcsg_senv_dr_14 = GetMmlData(pSys->sHL++);
	ix->dcsg_senv_sr_15 = GetMmlData(pSys->sHL++);
	ix->dcsg_senv_sl_16 = GetMmlData(pSys->sHL++);
	ix->dcsg_senv_rr_17 = GetMmlData(pSys->sHL++);

	ix->volume_6 = ix->volume_6 | 0x90;

	ix->tone_no_1 = 0xff; // 自作エンベロープは0xff for Debug
}

/**
 * @fn static void ENVPST2(FM_CH_WORK *ix)
 * @brief エンベロープ　パラメータセット(DCSG音色)
 * @note SubCommand2 FA DCSG
**/
static void ENVPST2(FM_CH_WORK *ix, u8 *hl)
{
	//    AL,  AR,  DR,  SR,SL, RR
	ix->dcsg_senv_al_12 = *hl++; // AL
	ix->dcsg_senv_ar_13 = *hl++; // AR
	ix->dcsg_senv_dr_14 = *hl++; // DR
	ix->dcsg_senv_sr_15 = *hl++; // SR
	ix->dcsg_senv_sl_16 = *hl++; // SL
	ix->dcsg_senv_rr_17 = *hl++; // RR

	ix->volume_6 = ix->volume_6 | (DCSG_6_BIT7_SOFT_ENV | DCSG_6_BIT4_ATTACK);
}

/*
 * @fn static void OTOSSG(FM_CH_WORK *ix)
 * @brief DCSG音色セット
 * @note SubCommand2 F0 DCSG
**/
static void OTOSSG(FM_CH_WORK *ix)
{
	u8 tone_no = GetMmlData(pSys->sHL++) & 0x0f;
	u8 *hl = OTOCAL(ix, tone_no); // ; HLに音色のアドレス

	ENVPST2(ix, hl); // ; SSGDATをSoftEnvにセット

	// デバッグ用に現在の音色番号を保存
	ix->tone_no_1 = tone_no; /*!< デバッグ用に音色番号を入れる。 */
}

/*
 * @fn static u8 *OTOCAL(FM_CH_WORK *ix, u8 a)
 * @brief DCSG音色データアドレスを取得
 * 
**/
static inline u8 *OTOCAL(FM_CH_WORK *ix, u8 a)
{
	return ((u8 *)&SSGDAT[a][0]);
}

//; -----------------------------------------------------------------------------
//; SSG 音色データ
// 1) Attack Rate = ノートオン(鍵盤を押したとき)からの音量の立ち上がりの傾き
// 2) Decay Rate = 最大音量から持続部分の音量に達するまでの減衰の傾き
// 3) Sustain Level = 音が持続する部分の音量
// 4) Sustain Rate = 音が持続する部分の音量の減衰の傾き
// 5) Release Rate = ノートオフ(鍵盤を離したとき)から音が消えるまでの減衰の傾き
static const u8 SSGDAT[0x10][6] = {
	//    AL,  AR,  DR,  SR,SL, RR
	{255, 255, 255, 255, 0, 255}, // ; 0:E ノーマル
	{255, 255, 255, 200, 0, 10},  // ; 1:コナミ(1)
	{255, 255, 255, 200, 1, 10},  // ; 2:コナミ(2)
	{255, 255, 255, 190, 0, 10},  // ; 3:コナミ+LFO(1)
	{255, 255, 255, 190, 1, 10},  // ; 4:コナミ+LFO(2)
	{255, 255, 255, 170, 0, 10},  // ; 5:コナミ(3)
	{40, 70, 14, 190, 0, 15},	  // ; 6:セガタイプﾟ
	{120, 030, 255, 255, 0, 10},  // ; 7:ストリングタイプ
	{255, 255, 255, 225, 8, 15},  // ; 8:ピアノ・ハープタイプ
	{255, 255, 255, 1, 255, 255}, // ; 9:クローズ　ハイハット
	{255, 255, 255, 200, 8, 255}, // ; 10:オープン　ハイハット
	{255, 255, 255, 220, 20, 8},  // ; 11:シンセタム・シンセキック
	{255, 255, 255, 255, 0, 10},  // ; 12:UFO(DQ Warp)
	{255, 255, 255, 255, 0, 10},  // ; 13:FALLING
	{120, 80, 255, 255, 0, 255},  // ; 14:ホイッスル
	{255, 255, 255, 220, 0, 255}, // ; 15:BOM!
								  //; 12 ENVE INIT > 11にコピーする
								  //; 13
								  //; 14 DECAY
								  //; 15 SUSTAIN
								  //; 16 SUSTAIN LEVEL
								  //; 17 REREASE
};

/**
 * @fn static void VOLUPS(FM_CH_WORK *ix)
 * @brief DCSG ボリューム UP/DOWN
 * @note SubCommand2 FB DCSG
**/
static void VOLUPS(FM_CH_WORK *ix)
{
	u8 add_vol = GetMmlData(pSys->sHL++);

	if ((ix->flag_33 & DCSG_33_BIT7_HARDENV) != 0)
	{
		return;
	}
	else
	{
		u8 vol = (ix->volume_6 & 0x0f) + add_vol;
		if (vol >= 16)
		{
			// ボリューム変更語が範囲外なら処理しない
			return;
		}
		else
		{
			ix->volume_6 = (ix->volume_6 & 0xf0) | vol;
		}
	}
}

//; -----------------------------------------------------------------------------
//; **	PSG VOLUME	** PC88: F Max / MD:0　Max
//; @note SubCommand F01

/**
 * @fn static void PSGVOL(FM_CH_WORK *ix)
 * @brief DCSG ボリューム ( VOLUME PC88: F Max / MD:0　Max)
 * @note SubCommand2 FB DCSG
**/
static void PSGVOL(FM_CH_WORK *ix)
{
	u8 senv_flag = ix->volume_6 & 0xf0;
	ix->flag_33 &= ~DCSG_33_BIT7_HARDENV;

	// PV1
	u8 vol = pSys->TOTALV + GetMmlData(pSys->sHL);
	;
	if (vol > 16)
	{
		vol = 0;
	}
	// PV2
	pSys->sHL++;
	ix->volume_6 = senv_flag | vol;
}

//!
//! FADE用 PV1
//!
static void PV1(FM_CH_WORK *ix, u8 vol, u8 env) //:	; Fadeからここが呼び出される
{
	u8 a = pSys->TOTALV + vol;
	if (a > 16)
	{
		a = 0;
	}

	pSys->sHL++;
	ix->volume_6 = a | env;
}

/**
 * @fn static void SSGOFF()
 * @brief ALL DCSGのキーオフ(A/B/C/Noise)
 * @note SubCommand2 FB DCSG
**/
static void SSGOFF()
{
	u32 lp;

	for (lp = 0; lp < 3; lp++)
	{
		DCSGOUT(&dcsg_ch_work[lp], (dcsg_ch_work[lp].dcsgvol_reg_7 | 0x0f));

		if (dcsg_ch_work[lp].dcsgvol_reg_7 == DCSG_NATT4_F0H)
		{
			DCSGOUT(&dcsg_ch_work[lp], DCSG_NATT4_F0H | 0x0f); // ; Vol(出力/SE割り込み中はキーオフしない)
		}
	}
}

//! DCSG KEYOOF param Table
static const u8 DCSG_KeyOffTbl[] = {
	(DCSG_ATT1_90H | 0x0f), (DCSG_ATT2_B0H | 0x0f), (DCSG_ATT3_D0H | 0x0f), // ; PSG-ABC Vol. OFf
	(DCSG_NATT4_F0H | 0x0f),												// ; Noise Vol. OFf

	DCSG_TONE1_80H, 0x00,	  // ; PSG-A Tone Clear
	DCSG_TONE2_A0H, 0x00,	  // ; PSG-B Tone Clear
	DCSG_TONE3_C0H, 0x00,	  // ; PSG-C Tone Clear
	(DCSG_NOISE4_E0H | 0x07), // ; Noise Param Init
};

/**
 * @fn static void SSGOFF_FORCE(FM_CH_WORK *ix)
 * @brief DCSG ALL SOUND OFF(SEを含めて強制KEYOFF)
**/
static void SSGOFF_FORCE(FM_CH_WORK *ix)
{
	u32 lp;
	for (lp = 0; lp < 4; lp++)
	{
		*((u8 *)DCSG_IO_ADRS) = DCSG_KeyOffTbl[lp];
	}
}

/**
 * @fn static void SKYOFF(FM_CH_WORK *ix)
 * @brief DCSG KEY OFF
**/
static void SKYOFF(FM_CH_WORK *ix)
{
	ix->volume_6 = 0;
	SSSUB_VolRegSet(ix, 0); //	; Noise対応のVolume設定
}

/**
 * @fn static u8 ConvVolReg2DCSG(u8 channnel_no)
 * @brief SSGのチャンネルボリュームReg番号からDCSGのReg番号に変換
**/
static inline u8 ConvVolReg2DCSG(u8 channnel_no)
{
	//! DCSGボリュームレジスタの変換
	static const u8 ConvVolReg2DCSGTbl[] = {
		// 4   5   6   Noise
		DCSG_ATT1_90H, DCSG_ATT2_B0H, DCSG_ATT3_D0H, DCSG_NATT4_F0H};
	return (ConvVolReg2DCSGTbl[channnel_no]);
}

/**
 * @fn static u8 ConvToneReg2DCSG(u8 ch_no)
 * @brief SSGの音程Reg番号からDCSGのReg番号に変換
**/
static inline u8 ConvToneReg2DCSG(u8 ch_no)
{
	//! DCSG音程レジスタの変換テーブル
	static const u8 ConvToneReg2DCSGTbl[] = {
		//;       4   5   6
		DCSG_TONE1_80H, DCSG_TONE2_A0H, DCSG_TONE3_C0H, DCSG_NOISE4_E0H};
	return (ConvToneReg2DCSGTbl[ch_no]);
}

/**
 * @fn static u8 ConvVol2DCSG(u8 vol88)
 * @brief SSGのボリュームからDCSGのボリュームに変換
**/
static inline u8 ConvVol2DCSG(u8 vol88)
{
	//! DCSGボリューム変換テーブル(PC88->DCSG)
	static const u8 ConvVol2DCSGTbl[] = {
		0x0f, 0x0e, 0x0d, 0x0c, 0x0b, 0x0a, 0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00};
	return (ConvVol2DCSGTbl[vol88]);
}

//! ***************************************************************************
//!
//! SOFT ENV処理
//!
//! ***************************************************************************

//; -----------------------------------------------------------------------------
/**
 * @fn static void SOFENV(FM_CH_WORK *ix)
 * @brief SOFT ENVEROPE PROCESS ソフトウェアエンベロープ処理メイン
 * @note soft env
*/
static void SOFENV(FM_CH_WORK *ix)
{
	if ((ix->volume_6 & DCSG_6_BIT4_ATTACK) == 0)
	{
		SOFEV2(ix); // ATTAC FLAGが寝ているので、ATACK完了
	}
	else
	{
		// ATTACK 処理
		u16 a = (u16)ix->dcsg_senv_vol_11 + (u16)ix->dcsg_senv_ar_13;
		if (a >= 0xff)
		{
			a = 0xff;
		}
		ix->dcsg_senv_vol_11 = a;
		// ↑ ATK初期のボリュームに、加速ボリュームを加算
		// ボリュームがffを超えたら次の処理

		if (a != 0xff)
		{
			SOFEV7(ix);
		}
		else
		{
			//			a = ix->volume_6;
			//			a ^= 0x30;		  // XOR	0011 0000 B   ; attack decay をxor
			//			ix->volume_6 = a; // ; TO STATE 2 (DECAY)
			ix->volume_6 = ix->volume_6 ^ (DCSG_6_BIT4_ATTACK | DCSG_6_BIT5_DECAY); // ATK OFF / DECAY ON
			SOFEV7(ix);
		}
	}
}

/**
 * @fn static void SOFEV2(FM_CH_WORK *ix)
 * @brief SOFT ENV DECAY処理
*/
static void SOFEV2(FM_CH_WORK *ix)
{
	if ((ix->volume_6 & DCSG_6_BIT5_DECAY) == 0) // decay flag
	{
		SOFEV4(ix); // SUSTAIN処理
	}
	else
	{
		// ; Decay　処理
		u8 a_vol = ix->dcsg_senv_vol_11;	// 現在のボリューム
		u8 d_decay = ix->dcsg_senv_dr_14;	//
		u8 e_sustain = ix->dcsg_senv_sr_15; //

		if (a_vol < d_decay)
		{
			a_vol = e_sustain; // SOFEV8
		}
		else
		{
			u8 a = a_vol - d_decay;
			if (a < e_sustain)
			{
				//					JR	NC,SOFEV3
				//					SOFEV8:
				a_vol = e_sustain; // CE で通過
			}
		}

		// SOFEV3
		ix->dcsg_senv_vol_11 = a_vol;

		if (a_vol != e_sustain)
		{
			SOFEV7(ix);
		}
		else
		{
			//			ix->volume_6 = ix->volume_6 ^ 0x60; // 0110 0000 B   ; Sustain/ decayをXor
			ix->volume_6 = ix->volume_6 ^ (DCSG_6_BIT5_DECAY | DCSG_6_BIT6_SUSTAIN);
			// 0110 0000 B  Sustain/ decayをXor
			SOFEV7(ix);
		}
	}
}

/**
 * @fn static void SOFEV4(FM_CH_WORK *ix)
 * @brief SUSTAIN処理
**/
static void SOFEV4(FM_CH_WORK *ix)
{
	if ((ix->volume_6 & DCSG_6_BIT6_SUSTAIN) == 0)
	{
		SOFEV9(ix); // Release処理
	}
	else
	{
		// SUSTAIN処理
		//		if (ix->dcsg_senv_vol_11 < ix->dcsg_senv_sl_16)
		if (ix->dcsg_senv_vol_11 <= ix->dcsg_senv_sl_16)
		{
			ix->dcsg_senv_vol_11 = 0;
			// SOFEV5
			ix->volume_6 = ix->volume_6 & 0x8f; //  attack/decay/sustainを寝かせる
												// VolumeとSOFTENVFlagが残る
		}
		else
		{
			ix->dcsg_senv_vol_11 -= ix->dcsg_senv_sl_16;
		}

		SOFEV7(ix); // DCSG出力Volumeを計算
	}
}

#if 0
static void SOFEV5(FM_CH_WORK *ix, u8 a)
{
	ix->dcsg_senv_vol_11 = a;
	if (a != 0)
	{
		SOFEV7(ix);
	}
	else
	{
		ix->volume_6 = ix->volume_6 & 0x8f; //; attack/decay/sustainを寝かせる
													  //; END OF ENVE
		SOFEV7(ix);
	}
}
#endif

/**
 * @fn static void SOFEV9(FM_CH_WORK *ix)
 * @brief Releaseのボリューム減衰処理
 * @brief SOFEV7も呼んでいる
**/
static void SOFEV9(FM_CH_WORK *ix)
{
	if (ix->dcsg_senv_vol_11 > ix->dcsg_senv_rr_17)
	{
		ix->dcsg_senv_vol_11 -= ix->dcsg_senv_rr_17;
	}
	else
	{
		ix->dcsg_senv_vol_11 = 0;
	}

	SOFEV7(ix);
}

/**
 * @fn static void SOFEV7(FM_CH_WORK *ix)
 * @brief DCSG Soft Env 最終ボリュームを計算
 * @return pSys->SOFEV7_ret
 * @note SoftEnvとVolumeから、DCSG出力ボリュームを計算
**/
static void SOFEV7(FM_CH_WORK *ix)
{
	u16 base_vol = (u16)ix->volume_6 & 0x0f;
	u16 hl = 0;

	u32 lp;
	for (lp = 0; lp <= base_vol; lp++)
	{
		hl += (u16)ix->dcsg_senv_vol_11;
	}

	u8 dcsg_vol = GetHfromHL(hl);

	if ((ix->flag_31 & FM_31_BIT6_KEYOFF) != 0)
	{
		pSys->SOFEV7_ret = dcsg_vol;
	}
	else if ((ix->flag_33 & FM_33_BIT5_REVERVE_FLAG) == 0)
	{
		pSys->SOFEV7_ret = dcsg_vol;
	}
	else
	{
		pSys->SOFEV7_ret = (dcsg_vol + ix->dcsg_senv_rr_17) >> 1; // リバーブの音量低下
	}
}

//! -----------------------------------------------------------------------------
//! 演奏終わり

/**
 * @fn static void FMEND(FM_CH_WORK *ix)
 * @brief FMの演奏終了
**/
static void FMEND(FM_CH_WORK *ix)
{
	SetHL(pSys->sHL, &ix->data_adress_3, &ix->data_adress_2);

	if (pSys->PCMFLG != 0)
	{
		PCMEND(ix);
	}
	else
	{
		KEYOFF(ix);
	}
}

/**
 * @fn static void PCMEND(FM_CH_WORK *ix)
 * @brief PCMの演奏停止
 * @note Z80に、演奏停止とボリューム0を設定
**/
static void PCMEND(FM_CH_WORK *ix)
{
	// TODO 無音のリクエスト
	// Volumeリセット
	// LR リセット
}

/**
 * @fn static void SSGEND(FM_CH_WORK *ix)
 * @brief DCSGの演奏停止
**/
static void SSGEND(FM_CH_WORK *ix)
{
	SetHL(pSys->sHL, &ix->data_adress_3, &ix->data_adress_2);
	SKYOFF(ix);
	ix->flag_31 &= ~FM_31_BIT7_LFO;
}

//; -----------------------------------------------------------------------------
/**
 * @fn static void FMINIT(FM_CH_WORK *ix, u8 ch_type, u8 ch_no)
 * @brief FM(DCSG/PCM)コントロールワーク初期化
 * 
**/
static void FMINIT(FM_CH_WORK *ix, u8 idx)
{
	u8 ch_type = ch_init_type_tbl[idx][0];
	u8 mub_idx = ch_init_type_tbl[idx][1];
	u8 set_ch_no = ch_init_type_tbl[idx][2];

	memset(ix, 0, sizeof(FM_CH_WORK));

	ix->counter_0 = 1;
	ix->volume_6 = 0;

	u8 *hl;

	hl = pSys->TB_TOP_68k; // TimerBの下
	hl += mub_idx * 4;	   // 2byte*2

	ix->data_adress_2 = *hl++; // L Offsetのみです。後で、Mu_Top_68kとタス
	ix->data_adress_3 = *hl++; // H

	ix->data_top_adress_4 = *hl++;
	ix->data_top_adress_5 = *hl++;

	switch (ch_type)
	{
	case CH_TYPE_FM:
		// FM
		ix->channel_no_8 = set_ch_no;
		if (mub_idx >= 7)
		{
			ix->fm_port_no = 1;
		}
		break;
	case CH_TYPE_DCSG:
		// DCSG
		SSINIT(ix, set_ch_no);
		break;
	case CH_TYPE_PCM:
		// PCM FM 6ch
		ix->channel_no_8 = set_ch_no; // FM 6
		break;
	}
	pSys->CHNUM++;
}

/**
 * @fn static void LoopMubCheck()
 * @brief この曲がLOOPかなのかチェック
**/
static void LoopMubCheck()
{
	u32 lp;
	pSys->LOOP_MUB = 0;
	for (lp = 0; lp < MUB_CH_WORK_NUM; lp++)
	{
		const FM_CH_WORK *ix = ch_work_list_tbl[lp];
		if ((ix->data_top_adress_4 != 0) || (ix->data_top_adress_5 != 0))
		{
			pSys->LOOP_MUB = 1; // 大loopが存在すればループ曲
		}
	}
}

/**
 * @fn static void SSINIT(FM_CH_WORK *ix, u8 ch_no)
 * @brief FM_CH_WORKのDCSG部分の初期化
 * @note ---   FOR SSG   ---
 * @note チャンネルデータと連動するチャンネル番号
 * @note ---   FOR DCSG   ---
 * @note 出力するパラメータを保持。
**/
static void SSINIT(FM_CH_WORK *ix, u8 ch_no)
{
	ix->dcsgvol_reg_7 = ConvVolReg2DCSG(ch_no); // DCSG:vol_reg_7
	ix->channel_no_8 = ConvToneReg2DCSG(ch_no); // DCSG:chnnnel_no_8
}

//; -----------------------------------------------------------------------------
//; **	CHANGE SE MODE	**
//; PL_SNDの処理を書き換えています。
//; -----------------------------------------------------------------------------

/**
 * @fn static void TO_NML()
 * @brief EFCTモードから、通常モードに移行
**/
static void TO_NML()
{
	pSys->PLSET1 = 0x38;
	TNML2(0x3a);
}

/**
 * @fn static void TNML2(u8 param)
 * @brief EFCTモードから、通常モードに移行
**/
static void TNML2(u8 param)
{
	pSys->PLSET2 = param;
	PSGOUT(FM_REG_0x27, param);
}

/**
 * @fn static void TO_NML()
 * @brief EFCTモードから、通常モードに移行
**/
static void TO_EFC()
{
	pSys->PLSET1 = 0x78;
	TNML2(0x7a);
}

/**
 * @fn static inline u32 get_pcm_data_adr(u8 pcm_tone_no)
 * @brief PCM Adressの取得
 * @param
**/
static inline u32 get_pcm_data_adr(u8 pcm_tone_no)
{
	return ((u32)Mucom88md_pcm_tbl[pcm_tone_no]);
}

/**
 * @fn static inline u32 get_pcm_data_size(u8 pcm_tone_no)
 * @brief PCM Adressの取得
 * @param
**/
static inline u32 get_pcm_data_size(u8 pcm_tone_no)
{
	return ((u32)Mucom88md_pcm_size_tbl[pcm_tone_no]);
}

/**
 * @fn static void PLAY(FM_CH_WORK *ix)
 * @brief ADPCM	PLAY
 * @param IN:(STTADR)<=　再生スタート　アドレス
 * @param    (ENDADR)  <=　再生エンド　アドレス
 * @param    (DELT_N)<=　再生レート
**/
static void PLAY_PCM(FM_CH_WORK *ix)
{
	// PC88では、無音・リピート・波形アドレス・レート・ボリューム・LR・出力
	// MDではPCMのリクエスト (1SHOT PCM	)を Z80に送る

	//	if(ix->channel_no_8 == ){
	if (1)
	{ // TODO PCMを2CH使う時に判定を追加
		pSys->pcm_req_wk[0].prio = 0;
		pSys->pcm_req_wk[0].req_no = ix->tone_no_1 + 1; // 0はリクエストなし
	}
	else
	{
		pSys->pcm_req_wk[1].prio = 0;
		pSys->pcm_req_wk[1].req_no = ix->tone_no_1 + 1; //
	}
}

//! *****************************************************************************
/**
 * @brief 汎用サブルーチン
**/

/**
 * @fn static u16 GetHL(u8 h, u8 l)
 * @brief u8 二つをu16に結合
**/
static inline u16 GetHL(u8 h, u8 l)
{
	u16 hl;
	hl = ((u16)h) << 8 | (u16)l; // Bigエンディアン(68k)
	return (hl);
}

/**
 * @fn static void SetHL(u16 hl, u8 *h, u8 *l)
 * @brief u16をu8に分割格納
**/
static inline void SetHL(u16 hl, u8 *h, u8 *l)
{
	*h = GetHfromHL(hl);
	*l = GetLfromHL(hl);
}

/**
 * @fn static u8 GetHfromHL(u16 hl)
 * @brief u16から上位を取得
**/
static inline u8 GetHfromHL(u16 hl)
{
	return ((hl >> 8) & 0xff);
}

/**
 * @fn static u8 GetLfromHL(u16 hl)
 * @brief u16から下位を取得
**/
static inline u8 GetLfromHL(u16 hl)
{
	return (hl & 0xff);
}

/**
 * @fn static u8 GetMmlData(u16 hl)
 * @brief MMLデータの取得
**/
static inline u8 GetMmlData(u16 hl)
{
	return (*(pSys->Base68k_adrs + hl));
}

/**
 * @fn static u8 *GetDataAdress(u16 hl)
 * @brief offset+68kAdress
 * @note FMの音色セットでのみ使っている
**/
static u8 *GetDataAdress(u16 hl)
{
	return (pSys->Mub_Top_68k + hl);
}

/**
 * @fn void SwapMubHeaderEndian(MUBHED *src, MUBHED *dst)
 * @brief .mubファイルのMUBHEDのヘッダーのワードデータの変換
 * @note 
**/
void SwapMubHeaderEndian(MUBHED *src, MUBHED *dst)
{
	//	Memory Data structure
	dst->magic[0] = src->magic[0];
	dst->magic[1] = src->magic[1];
	dst->magic[2] = src->magic[2];
	dst->magic[3] = src->magic[3];

	dst->dataoffset = mub_flip32(src->dataoffset); // DATA offset
	dst->datasize = mub_flip32(src->datasize);	   // DATA size
	dst->tagdata = mub_flip32(src->tagdata);	   // TAG data file offset (0=none)
	dst->tagsize = mub_flip32(src->tagsize);	   // TAG data size
	dst->pcmdata = mub_flip32(src->pcmdata);	   // PCM data offset (0=none)
	dst->pcmsize = mub_flip32(src->pcmsize);	   // PCM data size
	dst->jumpcount = mub_flip32(src->jumpcount);   // Jump count (for skip)
	dst->jumpline = mub_flip32(src->jumpline);	   // Jump line number (for skip)

	//	Extend data (2.0 Header)
	//
	dst->ext_flags = mub_flip16(src->ext_flags); // Option flags ( MUCOM_FLAG_* )

	dst->ext_system = src->ext_system;						 // build system ( MUCOM_SYSTEM_* )
	dst->ext_target = src->ext_target;						 // playback target ( MUCOM_TARGET_* )
	dst->ext_channel_num = mub_flip16(src->ext_channel_num); // Max channel table num
	dst->ext_fmvoice_num = mub_flip16(src->ext_fmvoice_num); // internal FM voice table num

	dst->ext_player = mub_flip32(src->ext_player); // external player option
	dst->pad1 = mub_flip32(src->pad1);			   // not use (reserved)

	int i;
	for (i = 0; i < MUCOM_FMVOICE_MAX; i++)
	{
		dst->ext_fmvoice[i] = src->ext_fmvoice[i]; // FM voice no.(orginal) table
	}
}

//-----------------------------------------------------------------------------
// エンディアンの変換 16
static u16 mub_flip16(u16 num)
{
	u16 tmp;
	tmp = (num & 0x00ff) << 8;
	tmp |= (num & 0xff00) >> 8;
	return (tmp);
}

//-----------------------------------------------------------------------------
// エンディアンの変換 32
static u32 mub_flip32(u32 num)
{
	u32 param;

	param = (num & 0x000000ff) << 24;
	param |= (num & 0x0000ff00) << 8;
	param |= (num & 0x00ff0000) >> 8;
	param |= (num & 0xff000000) >> 24;
	return (param);
}



//*****************************************************************************
/**
 * @brief Z80用サブルーチン
**/

/**
 * @fn void z80_pcm_driver_load()
 * @brief Z80 DriverをZ80に転送
**/
void z80_pcm_driver_load()
{
	SYS_disableInts();

	// clear z80 memory
	Z80_clear(0, Z80_RAM_LEN, FALSE);

	// upload Z80 driver and reset Z80
	Z80_upload(0, mu88md_pcm_drv, sizeof(mu88md_pcm_drv), TRUE);

	SYS_enableInts();
}

/**
 * @fn static void set_fm6_pcm_use_mode()
 * @brief FM6をpcmモードに設定
**/
static void set_fm6_pcm_use_mode()
{
	u16 bus_taken;
	bus_taken = Z80_isBusTaken();
	if (!bus_taken)
		Z80_requestBus(TRUE);

	YM2612_enableDAC();

	if (!bus_taken)
		Z80_releaseBus();
}

/**
 * @fn static void set_fm6_fm_use_mode()
 * @brief FM6をfmモードに設定
**/
static void set_fm6_fm_use_mode()
{
	u16 bus_taken;
	bus_taken = Z80_isBusTaken();
	if (!bus_taken)
		Z80_requestBus(TRUE);

	YM2612_enableDAC();

	if (!bus_taken)
		Z80_releaseBus();
}
