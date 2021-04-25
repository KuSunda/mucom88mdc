/**
 * @fn mucom88md_test.c
 * @brief mucom88md のサウンドテストモード
 * 
**/
#include <genesis.h>
#include "mu88md_drv.h"
#include "mu88md_test.h"

#include "_mu88md_mub.h"
#include "_mu88md_pcm.h"
#include "_mu88md_se.h"

/**
 * @note Defines
**/
#define USE_MUCOM_TESTMODE (1)			// TestModeを有効にする

enum
{
	BGM_TEST = 0,
	SE_TEST,
	PCM_TEST,
	PCM2_TEST,
	MUCOM_TEST_NUM
};

typedef struct _MUCOM88_TEST_WORK
{
	u8 rno[4];
	s8 ctr[4];
	s16 sel_no[MUCOM_TEST_NUM];
	s16 sel_no_max[MUCOM_TEST_NUM];

	s16 SoundTestCursor;
	s16 SoundTestCursorOld;

	u8  disp_mode;

} MUCOM88_TEST_WORK;

/**
 * @note 変数
**/
static MUCOM88_TEST_WORK Mucom88TestWk;
static MUCOM88_TEST_WORK *Mwk;

static JOY_WORK JoyData[MD_JOY_NUM];

/**
 * @note Forward
**/
// TestMode
void Mucom88md_SoundTestInit();
void Mucom88md_SoundTestUpdate();
void soundtest_main();

void Mucom88mdz_Init();
void Mu88md_FrameReset();

void Mucom88mdz_CopyStatus68kToZ80();

void Mucom88mdz_SeReqArrayClear();
void Mu88md_SeReq(u8 se_no, u8 channel);
void Mu88md_SePackReq(u8 set_no);

static void musom88_test_menu_disp();
static void mucom88_test_cursor_disp();
static void mucom88_test_statebase_disp();
static void mucom88_test_chdat_disp();

// Request Sub
void Mu88md_DebugQueReq(u8 param);

void Mu88md_PcmReq(u8 mub_no, u8 ch_no);

// Debug
void Mu88md_MubStop();
void Mu88md_MubFadeOut();
void Mu88md_PcmReq(u8 req_no, u8 ch_no);
void Mu88md_DebugQueReq(u8 param);
void Mu88md_MubReq(u8 mub_no);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Debug
void Dbg_SetPrintPos(u16 x, u16 y);
void Dbg_PrintChar(char c, u16 state);
void Dbg_Print8(u8 val, u16 state);
void Dbg_Print16(u16 val, u16 state);
void Dbg_Print32(u32 valo, u16 state);
void Dbg_PrintString(const char *str, u16 state);

void JoySystemInitilize();
void JoyDataUpdate();

static void set_joydata(u16 port, u16 joytype);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
extern MUCOM88_SYS_WORK *pSys;


#if USE_MUCOM_TESTMODE
//*****************************************************************************
/**
 * @note Programs
**/

/**
 * @note テストモードの初期化
**/
void Mucom88md_SoundTestInit()
{
	memset(&Mucom88TestWk, 0, sizeof(MUCOM88_TEST_WORK));
	Mwk = &Mucom88TestWk;

	Mwk->sel_no_max[0] = MUB_NUM;
	Mwk->sel_no_max[1] = MUCOM88MD_SE_NUM; //MUB_SE_SET_NUM;
	Mwk->sel_no_max[2] = PCM_ID_NUM;
	Mwk->sel_no_max[3] = PCM_ID_NUM;

}

/**
 * @note テストモードのアップデート
**/
void Mucom88md_SoundTestUpdate()
{
	switch (Mwk->rno[0])
	{
	case 0:
		Mwk->rno[0]++; // Z80側のドライバー初期化待ち
		break;
	case 1:
		Mwk->rno[0]++;

		// TESTモードの表示
		mucom88_test_statebase_disp();
		mucom88_test_cursor_disp();

		break;
	case 2:
		soundtest_main();

		if (JoyData[P1].on & SHOT_X){
			Mwk->disp_mode ^= 0x01;
		}

		if (Mwk->disp_mode == 0){
			mucom88_test_chdat_disp();
			musom88_test_menu_disp();
		}

		break;
	}
}

/**
 * @note テストモードの操作
**/
void soundtest_main()
{
	Mwk->SoundTestCursorOld = Mwk->SoundTestCursor;

	if (JoyData[P1].on & UP)
	{
		Mwk->SoundTestCursor--;
		if (Mwk->SoundTestCursor < 0)
		{
			Mwk->SoundTestCursor = MUCOM_TEST_NUM - 1;
		}
		mucom88_test_cursor_disp();
	}
	else if (JoyData[P1].on & DOWN)
	{
		Mwk->SoundTestCursor++;
		if (Mwk->SoundTestCursor >= MUCOM_TEST_NUM)
		{
			Mwk->SoundTestCursor = 0;
		}
		mucom88_test_cursor_disp();
	}

	if (JoyData[P1].on & LEFT)
	{
		Mwk->sel_no[Mwk->SoundTestCursor]--;
		if (Mwk->sel_no[Mwk->SoundTestCursor] < 0)
		{
			Mwk->sel_no[Mwk->SoundTestCursor] = Mwk->sel_no_max[Mwk->SoundTestCursor] - 1;
		}
	}
	else if (JoyData[P1].on & RIGHT)
	{
		Mwk->sel_no[Mwk->SoundTestCursor]++;
		if (Mwk->sel_no[Mwk->SoundTestCursor] >= Mwk->sel_no_max[Mwk->SoundTestCursor])
		{
			Mwk->sel_no[Mwk->SoundTestCursor] = 0;
		}
	}

	// 演奏 --------------------------------------------------
	if (JoyData[P1].on & SHOT_A)
	{
		switch (Mwk->SoundTestCursor)
		{
		case 0:
			// BGM
			Mu88md_MubReq(Mwk->sel_no[0]);		// [Mu88md] BGM Req
			break;
		case 1:
			Mu88md_SeReq(Mwk->sel_no[1], 2);	// [Mu88md] Se Req
			break;
		case 2:
			Mu88md_PcmReq(Mwk->sel_no[2], 0);	// [Mu88md] PCM 1 Req
			break;
		case 3:
			Mu88md_PcmReq(Mwk->sel_no[3], 1);	// [Mu88md] PCM 2 Req
			break;
		}
	}
	// 停止 --------------------------------------------------
	else if (JoyData[P1].on & SHOT_B)
	{
		switch (Mwk->SoundTestCursor)
		{
		case 0:
			Mu88md_MubStop();					// [Mu88md] Bgm Stop
			break;
		case 1:
			//			Mucom88mdz_SetSeAllStop();
			break;
		case 2:
			//			Mucom88mdZ_SetPcmStop();
			break;
		}
	}
	// フェード -----------------------------------------
	else if (JoyData[P1].on & SHOT_C)
	{
		// FADE(BGM Only)
		switch (Mwk->SoundTestCursor)
		{
		case 0:
			Mu88md_MubFadeOut();				// [Mu88md] Bgm Fade Out
			break;
		case 1:
			break;
		case 2:
			break;
		}
	}

	// Debug
	if (JoyData[P1].now & SHOT_Z)
	{
		// FADE(BGM Only)
		switch (Mwk->SoundTestCursor)
		{
		case 0:
			Mu88md_DebugQueReq(1);				// [Mu88md] Debug 早送り
			break;
		case 1:
			break;
		case 2:
			break;
		}
	}
	else
	{
		Mu88md_DebugQueReq(0);
	}
}
#endif

//*****************************************************************************
// 演奏のステータス表示

/**
 * @note 固定情報の描画
**/
static void mucom88_test_statebase_disp()
{
	static char *channnel_name_tbl[CHANNEL_WK_NUM] = {
		"A",
		"B",
		"C",

		"D",
		"E",
		"F",

		"G",

		"H",
		"I",
		"J",

		"K",

		"D",
		"E",
		"F"};

	u16 x = 1;
	u16 y = 9;

	SYS_disableInts();

	for (int i = 0; i < CHANNEL_WK_NUM; i++)
	{
		if ((i == CH_RYTM) || (i == CH_SE_SSG1) || (i == CH_SE_SSG2) || (i == CH_SE_SSG3))
		{
			continue;
		}

		Dbg_SetPrintPos(x, y);
		Dbg_PrintString(channnel_name_tbl[i], 0);

		y += 1;
	}

	x = 1;
	y = 0;

	Dbg_SetPrintPos(x, y);
	Dbg_PrintString("MUCOM88MD TEST", 0);

	x = 3;
	y = 8;
	Dbg_SetPrintPos(x, y);
	Dbg_PrintString("@  Vo DT   Adr  Key LFO REV ", 0);

	x = 1;
	y = 24;

	Dbg_SetPrintPos(x, y);
	Dbg_PrintString("UD:SEL BGM/SE LR:SEL No", 0);

	y = 25;
	Dbg_SetPrintPos(x, y);
	Dbg_PrintString("A:PLAY B:STOP C:FADE(Mub) Z:CUE", 0);

	SYS_enableInts();
}


/**
 * @note 変化するステータスを描画する
**/
static void musom88_test_menu_disp()
{
	static char *snd_state_tbl[] = {
		"STOP/",
		"PLAY/",
	};

	static char *loop_mub_tbl[] = {
		"LoopOff/", // Loop有り
		"LoopOn /", // Loop無し
	};

	static char *looped_tbl[] = {
		"------",
		"Looped",
	};

	Dbg_SetPrintPos(1, 2);
	Dbg_PrintString("Mub :", 0);

	Dbg_SetPrintPos(6, 2);
	Dbg_PrintString(Mucom88md_mub_name_tbl[Mwk->sel_no[0]], 0);

	Dbg_SetPrintPos(1, 3);
	Dbg_PrintString("Se  :", 0);

	Dbg_SetPrintPos(6, 3);
	Dbg_Print8(Mwk->sel_no[1], 0);

	Dbg_SetPrintPos(1, 4);
	Dbg_PrintString("Pcm1:", 0);
	Dbg_SetPrintPos(6, 4);
	Dbg_Print8(Mwk->sel_no[2], 0);

	Dbg_SetPrintPos(1, 5);
	Dbg_PrintString("Pcm2:", 0);
	Dbg_SetPrintPos(6, 5);
	Dbg_Print8(Mwk->sel_no[3], 0);

	Dbg_SetPrintPos(1, 6);
	Dbg_PrintString(snd_state_tbl[pSys->SND_STATUS], 0);

	Dbg_SetPrintPos(6, 6);
	Dbg_PrintString(loop_mub_tbl[pSys->LOOP_MUB], 0);

	Dbg_SetPrintPos(14, 6);
	Dbg_PrintString(looped_tbl[pSys->MUB_LOOPED], 0);

	//	Dbg_SetPrintPos(8, 4); // STOP PLAY
	//	Dbg_PrintString(snd_state_tbl[Mucom88mdz_Z80State.z80_PcmState], 0);

	//	u16 x;
	//	x = 27;
	//	Dbg_SetPrintPos(x, 22);
	//	Dbg_PrintString("OCT:", 0);
	//	Dbg_Print8(Mucom88mdz_Z80State.z80_OctUnderNum, 0);

	// LOOP曲か？
	//	Dbg_SetPrintPos(1, 6);
	//	Dbg_PrintString(loop_mub_tbl[Mucom88mdz_GetLoopMubFlag()], 0);

	// 1回ループしたか？
	//	Dbg_SetPrintPos(1, 7);
	//	Dbg_PrintString(looped_tbl[Mucom88mdz_GetLooped()], 0);

	// FLAGSETの値
	//	Dbg_SetPrintPos(1, 8);
	//	Dbg_PrintString("FLAGSET:", 0);
	//	Dbg_SetPrintPos(9, 8);
	//	Dbg_Print8(Mucom88mdz_GetFlag(), 0);

}

/**
 * @note カーソル表示
**/
static void mucom88_test_cursor_disp()
{
	// 曲/SEの選択
	Dbg_SetPrintPos(0, 2 + Mwk->SoundTestCursorOld);
	Dbg_PrintString(" ", 0);

	Dbg_SetPrintPos(0, 2 + Mwk->SoundTestCursor);
	Dbg_PrintString(">", 0);
}

/**
 * @note チャンネル毎のステータス表示
**/
static void mucom88_test_chdat_disp()
{
	static char *tone_name_tbl[CHANNEL_WK_NUM] = {
		"c ",
		"c#",
		"d ",
		"d#",
		"e ",
		"f ",
		"f#",
		"g ",
		"g#",
		"a ",
		"a#",
		"b ",
	};

	static char *octave_name_tbl[9] = {
		"1", // 0
		"2", // 1
		"3", // 2
		"4", // 3
		"5", // 4
		"6", // 5
		"7", // 6
		"8", // 7
		"9", // 8
	};

	static FM_CH_WORK *ch_wk[] = {
		&fm_ch_work[0],
		&fm_ch_work[1],
		&fm_ch_work[2],

		&dcsg_ch_work[0], // DCSG
		&dcsg_ch_work[1], // DCSG
		&dcsg_ch_work[2], // DCSG

		&fm_ch_work[0], // Rythm

		&fm_ch_work[3],
		&fm_ch_work[4],
		&fm_ch_work[5],

		&pcm_ch_work[0], // PCM

		&fm_ch_work[0], // dmy SE
		&fm_ch_work[0], // dmy SE
		&fm_ch_work[0], // dmy SE
	};

	u16 x = 3;
	u16 y = 9;

	FM_CH_WORK *pWk;

	u32 lp;
	for (lp = 0; lp < CHANNEL_WK_NUM; lp++)
	{
		pWk = ch_wk[lp];

		// 非表示Channel
		if ((lp == CH_RYTM) || (lp == CH_SE_SSG1) || (lp == CH_SE_SSG2) || (lp == CH_SE_SSG3))
		{
			continue;
		}

		// 空白Channel
#if 0
		if (lp == CH_FM6){
			y++;
			continue;
		}
#endif
		// TONE No
		Dbg_SetPrintPos(x, y);
		Dbg_Print8(pWk->tone_no_1, 0);

		// Volume
		Dbg_SetPrintPos(x + 3, y);
		Dbg_Print8(pWk->volume_6&0x0f, 0);

		// DT
		Dbg_SetPrintPos(x + 6, y);
		u16 dt = (((u16)pWk->detune_10)<<8) | pWk->detune_9;
		Dbg_Print16(dt, 0);

		// ASDR
		Dbg_SetPrintPos(x + 11, y);
		u16 adr = (((u16)pWk->data_adress_3)<<8)|pWk->data_adress_2;
		Dbg_Print16(adr, 0);

		// 音符
		Dbg_SetPrintPos(x + 16, y);
		Dbg_PrintString(octave_name_tbl[pWk->befor_code_32 >> 4], 0);
		Dbg_SetPrintPos(x + 17, y);
		Dbg_PrintString(tone_name_tbl[(pWk->befor_code_32 & 0x0f)], 0);

		// LFO
		Dbg_SetPrintPos(x + 20, y);
		u8 lfo = (pWk->flag_31 && FM_31_BIT7_LFO);
		Dbg_Print8(lfo, 0);

		// リバーブ
		Dbg_SetPrintPos(x + 24, y);
		u8 rev = (pWk->flag_33 && FM_33_BIT5_REVERVE_FLAG);
		Dbg_Print8(rev, 0);

		y += 1;
	}

	Dbg_SetPrintPos(9, 4);
	Dbg_Print8(pSys->pcm_state_z80[0], 0);
	Dbg_SetPrintPos(9, 5);
	Dbg_Print8(pSys->pcm_state_z80[1], 0);
}


//*****************************************************************************
//*****************************************************************************
/**
 * @note デバッグ用のサブルーチン
**/
//*****************************************************************************
//*****************************************************************************

static u16 posX;
static u16 posY;
static char *hex = "0123456789ABCDEF";

//=============================================================================
//! @fn void void Dbg_SetPrintPos(u16 x, u16 y)
//! @brief Plan_Aへの文字列表示　座標セット
//! @param[in] u16 pos_x
//! @param[in] u16 pos_y
//! @note 基本的にアプリからはマクロを使う
//=============================================================================
void Dbg_SetPrintPos(u16 x, u16 y)
{
	posX = x;
	posY = y;
}

//=============================================================================
//! @fn void Dbg_PrintChar(char c, u16 state)
//! @brief Plan_Aへの文字列表示　座標セット
//! @param[in] char c 文字コード
//! @param[in] u16 state 0:PAL0 other:PAL1
//! @note 基本的にアプリからはマクロを使う
//=============================================================================
void Dbg_PrintChar(char c, u16 state)
{
	char temp[2];
	temp[0] = c;
	temp[1] = 0;
	VDP_setTextPalette(state ? 1 : 0);
	VDP_drawText(temp, posX, posY);
	posX += 2;
}

//=============================================================================
//! @fn void Dbg_Print8(u8 val, u16 state)
//! @brief Plan_Aへの　1Byteの文字列表示
//! @param[in] u8 表示数値
//! @param[in] u16 state 0:PAL0 other:PAL1
//! @note 基本的にアプリからはマクロを使う
//=============================================================================
void Dbg_Print8(u8 val, u16 state)
{
	char temp[8];
	temp[0] = hex[(val >> 4) & 15];
	temp[1] = hex[val & 15];
	temp[2] = 0;
	VDP_setTextPalette(state ? 1 : 0);
	VDP_drawText(temp, posX, posY);
	//    posX += 8;
	posX += 2;
}

//=============================================================================
//! @fn void Dbg_Print16(u8 val, u16 state)
//! @brief Plan_Aへの　2Byteの文字列表示
//! @param[in] u16 表示数値
//! @param[in] u16 state 0:PAL0 other:PAL1
//! @note 基本的にアプリからはマクロを使う
//=============================================================================
void Dbg_Print16(u16 val, u16 state)
{
	char temp[8];

	temp[0] = hex[val >> 12];
	temp[1] = hex[(val >> 8) & 15];
	temp[2] = hex[(val >> 4) & 15];
	temp[3] = hex[val & 15];
	temp[4] = 0;
	VDP_setTextPalette(state ? 1 : 0);
	VDP_drawText(temp, posX, posY);
	//    posX += 8;
	posX += 4;
}

//=============================================================================
//! @fn void Dbg_Print32(u8 val, u16 state)
//! @brief Plan_Aへの　4Byteの文字列表示
//! @param[in] u32 表示数値
//! @param[in] u16 state 0:PAL0 other:PAL1
//! @note 基本的にアプリからはマクロを使う
//=============================================================================
void Dbg_Print32(u32 valo, u16 state)
{
	char temp[16];
	u16 val;

	val = 0;
	val = valo >> 16;

	temp[0] = hex[val >> 12];
	temp[1] = hex[(val >> 8) & 15];
	temp[2] = hex[(val >> 4) & 15];
	temp[3] = hex[val & 15];

	val = valo & 0xffff;

	temp[4] = hex[val >> 12];
	temp[5] = hex[(val >> 8) & 15];
	temp[6] = hex[(val >> 4) & 15];
	temp[7] = hex[val & 15];

	temp[8] = 0;
	VDP_setTextPalette(state ? 1 : 0);
	VDP_drawText(temp, posX, posY);

	posX += 8;
}

//=============================================================================
//! @fn void Dg_PrintString(const char *str, u16 state)
//! @brief Plan_Aへの文字列表示
//! @param[in] const char *str 表示文字列
//! @param[in] u16 state 0:PAL0 other:PAL1
//! @note 基本的にアプリからはマクロを使う
//=============================================================================
void Dbg_PrintString(const char *str, u16 state)
{
	u16 len;

	len = strlen(str);

	VDP_setTextPalette(state ? 1 : 0);
	if (state == 0)
	{
		VDP_drawText(str, posX, posY);
	}
	else
	{
		VDP_clearText(posX, posY, len);
	}
	posX += len;
}

//=============================================================================
//! @fn void JoySystemInitilize()
//! @brief Joyの初期化
//! @note 適当な実装です
//=============================================================================
void JoySystemInitilize()
{
	u16 i;

	for (i = 0; i < JOY_PORT_NUM; i++)
	{
		JoyData[i].porttype = JOY_getPortType(i);

		switch (JoyData[i].porttype)
		{
		case PORT_TYPE_MENACER:
			JOY_setSupport(PORT_1, JOY_SUPPORT_MENACER);
			break;
		case PORT_TYPE_JUSTIFIER:
			JOY_setSupport(PORT_1, JOY_SUPPORT_JUSTIFIER_BOTH);
			break;
		case PORT_TYPE_MOUSE:
			JOY_setSupport(PORT_1, JOY_SUPPORT_MOUSE);
			break;
		case PORT_TYPE_TEAMPLAYER:
			JOY_setSupport(PORT_1, JOY_SUPPORT_TEAMPLAYER);
			break;
		}
	}
}

//=============================================================================
//! @fn void JoyDataUpdate()
//! @brief JoyのUPDATE
//! @note 適当な実装です
//=============================================================================
void JoyDataUpdate()
{
	u16 i;

	for (i = 0; i < JOY_PORT_NUM /*CAN_USE_JOYPORT_NUM*/; i++)
	{

		if (JoyData[i].porttype == PORT_TYPE_PAD)
		{
			// Support
			switch (JoyData[i].joytype)
			{
			case JOY_TYPE_PAD3:
			case JOY_TYPE_PAD6:
				set_joydata(i, JoyData[i].joytype);
				break;
			default:
				JoyData[i].old = JoyData[i].now = JoyData[i].on = 0x0000;
				break;
			};
		}
		else
		{
			// NO Support
			JoyData[i].old = JoyData[i].now = JoyData[i].on = 0x0000;
		}
	}
}

static void set_joydata(u16 port, u16 joytype)
{
	if (JoyData[port].joytype != joytype)
	{
		JoyData[port].now = JOY_readJoypad(port);
		JoyData[port].old = 0x0000;
		JoyData[port].on = 0x0000;
		return;
	}
	// old
	JoyData[port].old = JoyData[port].now;
	// new
	JoyData[port].now = JOY_readJoypad(port);

	//	KLog_U1x(2, "[JOY]", JoyData[port].now);

	// trg
	JoyData[port].on = JoyData[port].now ^ JoyData[port].old;
	JoyData[port].on &= JoyData[port].now;

	// rep
}
