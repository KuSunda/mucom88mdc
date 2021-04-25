//*****************************************************************************
//*****************************************************************************
#ifndef _MU88MD_DRV_H_
#define _MU88MD_DRV_H_
#include <genesis.h>

//*****************************************************************************
// Defein
//*****************************************************************************

//*****************************************************************************
// Struct
//*****************************************************************************

#define DEBUG_LOG_USE		(0)		// KModへログを出力する

//=============================================================================
// Channel
//=============================================================================
#define FM_CHANNNEL_NUM		(6)
#define DCSG_CHANNNEL_NUM	(3)
#define PCM_CHANNNEL_NUM	(1)		// FM 6ch差し替え

enum{
	FM_0 = 0,
	FM_1,
	FM_2,
	DCSG_0,
	DCSG_1,
	DCSG_2,
	PCM_0,
	FM_3,
	FM_4,
	FM_5,

	OPN2_CH_NUM,
};

// ---------------------------------------------------------------------------
// チャンネルの情報 Channelワーク

enum{
	CH_FM1 = 0,
	CH_FM2,
	CH_FM3,
	CH_SSG1,
	CH_SSG2,
	CH_SSG3,
	CH_RYTM,
	CH_FM4,
	CH_FM5,
	CH_FM6,
	CH_PCM,

	CH_SE_SSG1,
	CH_SE_SSG2,
	CH_SE_SSG3,
	
	CHANNEL_WK_NUM
};

//=============================================================================
// Mucom88定義
//=============================================================================
#define MAXCH		(11)		// MusicChannel MAX
#define	ALLMAXCH	(14)		// include SE Chennnel Max

#define CH_TYPE_FM		(0)
#define CH_TYPE_DCSG	(1)
#define CH_TYPE_PCM		(2)
#define CH_TYPE_RHYTHM	(3)

#define BIT_0			( 0x01 )
#define BIT_1			( 0x01<<1 )
#define BIT_2			( 0x01<<2 )
#define BIT_3			( 0x01<<3 )
#define BIT_4			( 0x01<<4 )
#define BIT_5			( 0x01<<5 )
#define BIT_6			( 0x01<<6 )
#define BIT_7			( 0x01<<7 )

//=============================================================================
// FM Define
//=============================================================================
#define FM_REG_0x22	(0x22)		// 
#define FM_REG_0x27	(0x27)		// FM:Mode / Timer
#define FM_REG_0x28	(0x28)		// Slot/Channnel
#define FM_REG_0x40	(0x40)		// Total Level
#define FM_REG_0xa4	(0xa4)		// 
#define FM_REG_0xaa	(0xaa)		// 
#define FM_REG_0xad	(0xad)		// 
#define FM_REG_0xb4	(0xb4)		// 

// FM音源 アクセス IO
#define	FM_IO13_ADDR	((u8*)0xa04000)	// 	; FM 1-3 
#define	FM_IO13_ADDR2	((u8*)0xa04001)	//	; FM 1-3 
#define	FM_IO46_ADDR	((u8*)0xa04002)	//	; FM 4-6 
#define	FM_IO46_ADDR2	((u8*)0xa04003)	//	; FM 4-6 

//=============================================================================
// DCSG Define
//=============================================================================
// ; DCSG アクセス
//#define DCSG_IO_ADRS	((u8*)0xa07f11)	//	; PSG アクセスI/Oアドレス
#define DCSG_IO_ADRS	((u8*)PSG_PORT)	//	; PSG アクセスI/Oアドレス

#define DCSG_TONE1_80H		(0x80)
#define DCSG_ATT1_90H		(0x90)
#define DCSG_TONE2_A0H		(0xa0)
#define DCSG_ATT2_B0H		(0xb0)
#define DCSG_TONE3_C0H		(0xc0)
#define DCSG_ATT3_D0H		(0xd0)
#define DCSG_NOISE4_E0H		(0xe0)
#define DCSG_NATT4_F0H		(0xf0)

//=============================================================================
// MML DATA COMMAND
//=============================================================================
#define MML_CMD_TIE_SLUR	(0xfd)
#define MML_CMD_REST_FLAG	(0x80)

//=============================================================================
// Z80 Access
//=============================================================================
#define Z80_WORK_RAM_ADDR (0x1e00)
#define Z80_RAM_TOP_ADDR (Z80_RAM_START | Z80_WORK_RAM_ADDR)	//WORK（64Kと通信できるようにアドレスを管理）

#define COMM_CH0 (0x1FFE)			// FF = done, < 80 = SFX ID to play
#define COMM_CH1 (0x1FFF)			// FF = done, < 80 = SFX ID to play
#define FM_ACCESS (0x1FFD)			// indicate that Z80 is accessing DAC register (00 = ok, 2A = DAC access)

// LEN CH0
#define COMM_CH0_BANK (0x1FD0)      // 0xD0 L 0xD1 M 0xD2 H
// LEN CH0
#define COMM_CH0_LEN (0x1FD3)      // 0xD0 L 0xD1 M 0xD2 H

// LEN CH1
#define COMM_CH1_BANK (0x1FE0)      // 0xD8 L 0xD9 M 0xDA H
// LEN CH1
#define COMM_CH1_LEN (0x1FE3)      // 0xD8 L 0xD9 M 0xDA H


#define COMM_CH0L_LEN (COMM_CH0_LEN)      // 0xD0 L 0xD1 M 0xD2 H
#define COMM_CH0H_LEN (COMM_CH0_LEN+1)      // 0xD0 L 0xD1 M 0xD2 H

#define COMM_CH1L_LEN (COMM_CH1_LEN)      // 0xD8 L 0xD9 M 0xDA H
#define COMM_CH1H_LEN (COMM_CH1_LEN+1)      // 0xD8 L 0xD9 M 0xDA H

// LEN 
//#define COMM_CH0_BANK (0x1FE3)      // 0xD0 L 0xD1 M 0xD2 H
//#define COMM_CH1_BANK (0x1FD8)      // 0xD8 L 0xD9 M 0xDA H

#define COMM_CH0L_BANK (COMM_CH0_BANK)      	// 0xD0 L 0xD1 M 0xD2 H
#define COMM_CH1L_BANK (COMM_CH1_BANK)      	// 0xD8 L 0xD9 M 0xDA H
#define COMM_CH0M_BANK (COMM_CH0_BANK+1)      // 0xD0 L 0xD1 M 0xD2 H
#define COMM_CH1M_BANK (COMM_CH1_BANK+1)      // 0xD8 L 0xD9 M 0xDA H
#define COMM_CH0H_BANK (COMM_CH0_BANK+2)      // 0xD0 L 0xD1 M 0xD2 H
#define COMM_CH1H_BANK (COMM_CH1_BANK+2)      // 0xD8 L 0xD9 M 0xDA H


#define Z80_COMM_CH0_ADDR ((u8*)(Z80_RAM_START | COMM_CH0))		
#define Z80_COMM_CH1_ADDR ((u8*)(Z80_RAM_START | COMM_CH1))		
#define Z80_NOW_REG_ADDR ((u8*)(Z80_RAM_START | FM_ACCESS))		 

#define Z80_COMM_CH0_BANK_ADDR ((u8*)(Z80_RAM_START | COMM_CH0_BANK))		
#define Z80_COMM_CH1_BANK_ADDR ((u8*)(Z80_RAM_START | COMM_CH1_BANK))		

#define Z80_COMM_CH0L_BANK_ADDR ((u8*)(Z80_RAM_START | COMM_CH0L_BANK))		
#define Z80_COMM_CH1L_BANK_ADDR ((u8*)(Z80_RAM_START | COMM_CH1L_BANK))		
#define Z80_COMM_CH0M_BANK_ADDR ((u8*)(Z80_RAM_START | COMM_CH0M_BANK))		
#define Z80_COMM_CH1M_BANK_ADDR ((u8*)(Z80_RAM_START | COMM_CH1M_BANK))		
#define Z80_COMM_CH0H_BANK_ADDR ((u8*)(Z80_RAM_START | COMM_CH0H_BANK))		
#define Z80_COMM_CH1H_BANK_ADDR ((u8*)(Z80_RAM_START | COMM_CH1H_BANK))		

#define Z80_COMM_CH0L_LEN_ADDR ((u8*)(Z80_RAM_START | COMM_CH0L_LEN))		
#define Z80_COMM_CH0H_LEN_ADDR ((u8*)(Z80_RAM_START | COMM_CH0H_LEN))		
#define Z80_COMM_CH1L_LEN_ADDR ((u8*)(Z80_RAM_START | COMM_CH1L_LEN))		
#define Z80_COMM_CH1H_LEN_ADDR ((u8*)(Z80_RAM_START | COMM_CH1H_LEN))		

#define ADRESS_TO_H(adr)	((((u32)adr)&0x00ff0000)>>16)
#define ADRESS_TO_M(adr)	((((u32)adr)&0x0000ff00)>>8)
#define ADRESS_TO_L(adr)	((((u32)adr)&0x000000ff))

//=============================================================================
//=============================================================================


//=============================================================================
// Musicリクエスト
//=============================================================================
#define MUB_REQ_NONE			(0x00)		//; NONE
#define MUB_REQ_START			(0x01)		//; START
#define MUB_REQ_STOP			(0x02)		//; STOP
#define MUB_REQ_FADEOUT			(0x03)		//; FADEOUT

// 演奏ステータス
#define MUB_STATE_STOP			(0x00)		// ; Music 停止中
#define MUB_STATE_PLAY			(0x01)		// ; Music 演奏中
#define MUB_STATE_LOOP			(0x02)		// ; 曲が1回ループした。(非ループ曲なら演奏終了となる)

//=============================================================================
// Struct
//=============================================================================
typedef struct _PCM_REQ_Q{
	u8 req_no;
	u8 prio;
}PCM_REQ_Q;

#define PCM_REQ_CHANNNEL_NUM		(2)

//=============================================================================
// SystemWork
//=============================================================================
typedef struct _MUCOM88_SYS_WORK{
	//-------------------------
	// 68kとz80のヘッダー情報(アドレスの管理に注意)
	// データ実体のヘッダ部
	u8 *Mub_Top_68k;	// mubのアドレス(Z80版に入力されるアドレスと同じ)
	u16 OtoDatOffset_68k;		// FM音色までのオフセット
	u16 mub_size;		// バイナリ全体サイズ
	u8 *Mu_Top_68k;		// mubのアドレス(Z80版に入力されるアドレスと同じ)
	u8 *TB_TOP_68k;

	// SEのBase
	u8 *Base68k_se_adrs[3];	// Base68k_adrs

	// 現在のBase
	u8 *Base68k_adrs;	// Base68k_adrs


	// Z80向け
	u8	MusicNum;	// ; 1byte c200h 含まれている曲数
	u16	Mu_Top;		// ; used 2

	//-------------------------
	// リクエスト受け取り
	u8	req_command;
	u8	req_param;

	u8	MUSICNUM;	// リクエスト曲のパック内番号 (一旦0限定)          ; 1byte c200h
	u8	SND_STATUS;	// 演奏中フラグ

	u8  FMPORT;

	u8	PCMFLG;
	u8	DRMF1;
	u8	PCMLR;
	u8	SSGF1;
	u8	PCMNUM;

	// 
	u8	PLSET1;
	u8	PLSET2;

	u8	CHIDX;

	u8	CHNUM;	// DB	0
	u8	C2NUM;	// DB	0
	u16	TB_TOP;	// DW	0
	u8	TIMER_B;	// DB	100

	u8	PVMODE;		//  DB	0	;PCMvolMODE

	// Volume関係
	u8	TOTALV;		// DB	0
	u8	FDCO_WAIT;	// FADEOUTのスピード(PC88は0x0f)
	u8	FDCO_LV;	// FADE Volume Level

	u8	FLGADR;		// ; F9  サブコマンドで使用(曲からフラグを立てる)

	u8	LOOP_MUB;	// != 0 なら、Loopの曲
	u8	MUB_LOOPED;	// 曲がループしたか？

	u8	DCSG_OCT_UNDER;	// DCSG演奏不可能低音のリクエスト回数
	u8	DCSG_NOISEP;	// MIXMODE変更回数
	u8	DCSG_NOISEW;	// ノイズパラメータ設定回数

	u16 NEWFNM;

	u16 FNUM;

	u16 FMSUB8;
	u8	FPORT;

	u16 sHL;		// サブコマンドを呼ぶときにhlを保存する

	u8 SOFEV7_ret;		/* SOFEV7で設定されたボリューム */

	u8 NOW_CUE_REQ;

	PCM_REQ_Q now_pcm_req_wk[2];
	PCM_REQ_Q pcm_req_wk[2];
	
	// Se State
	u8 SeState;
	u8 SEMODE;

	// PCM STATE
	u8 pcm_state_z80[2];

	// debug
	s16	ym_access_num;			// 1FrameのYMアクセス数
	s16	ym_access_max;			// 1FrameのYMアクセス数
	s16	ym_access_timer;			// 1FrameのYMアクセス数

}MUCOM88_SYS_WORK;

#define SE_PLAY_BIT1		(0x01)
#define SE_PLAY_BIT2		(0x02)
#define SE_PLAY_BIT3		(0x04)

//=============================================================================
// SE System Work
//=============================================================================
typedef struct _MUCOM88_SE_SYS_WORK{
	u8 PlayState[3];	
	// Se State
	u8 SeState;
}MUCOM88_SE_SYS_WORK;

#define SE_PLAY_STATE_OFF		(0)
#define SE_PLAY_STATE_ON		(1)

//=============================================================================
// FM管理ワーク
//=============================================================================
#define LOOP_NEST_NUM		(6)

typedef struct _FM_CH_WORK{
	u8	counter_0;				// DB	1	; LENGTH カウンター		IX+ 0
	u8	tone_no_1;				// DB	24	; 音色ナンバー		1
	
	u8	data_adress_2;			// DW	0	; DATA ADDRES WORK	2,3 L
	u8	data_adress_3;			// DW	0	; DATA ADDRES WORK	2,3 H
	u8	data_top_adress_4;		// DW	0	; DATA TOP ADDRES	4,5
	u8	data_top_adress_5;		// DW	0	; DATA TOP ADDRES	4,5
	u8	volume_6;				// DB	10	; VOLUME DATA		6
	u8	algorithm_7;			// DB	0	; アルゴリズム No.		7
	u8	channel_no_8;			// DB	0	; チャンネルナンバー	8
	u8	detune_9;				// DW	0	; デチューン DATA		9,10
	u8	detune_10;				// DW	0	; デチューン DATA		9,10
	u8	fm_no_11;				// DB	0	;			11
	u8	reverb_12;				// DB	0	; for リバーブﾞ		12
//	u8	soft_evn[5];			// DS	5	; SOFT ENVE DUMMY	13-17
	u8	dcsg_senv_rr_17;		// DS	5	; SOFT ENVE DUMMY	13-17
	u8	q_on_18;				// DB	0	; qオンタイズ		18

	u8	lfo_delay_keep_19;		// DB	0	; LFO DELAY		19
	u8	lfo_delay_wk_20;		// DB	0	; WORK			20 lfo
	u8	lfo_counter_keep_21;	// DB	0	; LFO COUNTER		21
	u8	lfo_counter_wk_22;		// DB	0	; WORK			22 lfo
	u8	lfo_range_l_keep_23;	// DW	0	; LFO 変化量 2BYTE	23,24
	u8	lfo_range_h_keep_24;	// DW	0	; LFO 変化量 2BYTE	23,24
	u8	lfo_range_l_wk_25;		// DW	0	; WORK			25,26
	u8	lfo_range_h_wk_26;		// DW	0	; WORK			25,26

	u8	lfo_peak_level_keep_27;	// DB	0	; LFO PEAK LEVEL	27
	u8	lfo_peak_level_wk_28;	// DB	0	; WORK			28
	u8	fnum1_data_29;			// DB	0	; FNUM1 DATA		29
	u8	fnum2_data_30;			// DB	0	; B/FNUM2 DATA		30
	u8	flag_31;				// DB	00000001B
								// FM
								// bit 7=LFO FLAG	31
								// bit	6=KEYOFF FLAG
								// bit	5=LFO CONTINUE FLAG
								// bit	4=TIE FLAG
								// bit	3=MUTE FLAG
								// bit	2=LFO 1SHOT FLAG
								// 
								// bit	0=1LOOPEND FLAG

								// DCSG
								//  00000100B	; 
								// bit 7=LFO FLAG	31
								// bit	6=KEYOFF FLAG
								// bit	5=LFO CONTINUE FLAG
								// bit	4=TIE FLAG
								// 		3=MUTE FLAG
								// 		0=1LOOPEND FG

	u8	befor_code_32;			// DB 	0	; BEFORE CODE		32
	u8	flag_33;				// DB	0	; bit	6 =		33
								//					5 = REVERVE FLAG
								// 					4 = REVERVE MODE
	u16	return_adress_34;		// DW	0	;	リターンアドレス	34,35

	// ; MegaDrive拡張
	u8	rep_ptr_36;				// DB	0	; RepPtr	36
	u8	rep_counter_37[LOOP_NEST_NUM];	// DB	0,0,0,0; RepNum (4ネスト) 37 38 39 40
	u8	se_int_41;				// DB	0	; 41 none DCSG SE Cutinフラグ
	u8	noise_mode_42;			// DB	0	; 42 none DCSG NoiseModeFlag　0:P1 / 1:P2
	u8	noise_freq_43;			// DB	0	; 43 none DCSG ノイズ周波数(5Bit)

	// DCSG用ワーク
								//	u8	dcsg_volume_6;	
								// DB	8		; CURENT VOLUME(bit0-3)	6
								// bit 4 = attack flag
								// bit 5 = decay flag
								// bit 6 = sustain flag
								// bit 7 = soft envelope flag

	u8	dcsgvol_reg_7;	// DB	8   ; VOL.REG.No.			7		 DCSG_ATT?_REG に差し替え

//	u8	dcsg_soft_env_counter_11_;	// DB	0   ; SOFT ENVE COUNTER		11
	u8	dcsg_senv_vol_11;	// DB	0   ; SOFT ENVE COUNTER		11
	u8	dcsg_senv_al_12;	// DS	6   ; SOFT ENVE				12-17
	u8	dcsg_senv_ar_13;	// DS	6   ; SOFT ENVE				12-17
	u8	dcsg_senv_dr_14;	// DS	6   ; SOFT ENVE				12-17
	u8	dcsg_senv_sr_15;	// DS	6   ; SOFT ENVE				12-17
	u8	dcsg_senv_sl_16;	// DS	6   ; SOFT ENVE				12-17
//	u8	dcsg_sont_env_17;	// DS	6   ; SOFT ENVE				12-17

	u8	dcsg_hard_env_33;	// DB      0       
							//  bit 7 = HARD ENVE FLAG	33
							//  bit 0-3 = HARD ENVE TYPE
	u8	volume_pcm_6;		// DB	10	; VOLUME DATA		6

	u8	fm_port_no;			// DB	10	; VOLUME DATA		6
}FM_CH_WORK;

// flag31 FLAG
#define FM_31_BIT7_LFO			(BIT_7)
#define FM_31_BIT6_KEYOFF		(BIT_6)
#define FM_31_BIT5_LFO_CONTINUE	(BIT_5)
#define FM_31_BIT4_TIE			(BIT_4)
#define FM_31_BIT3_MUTE			(BIT_3)
#define FM_31_BIT2_LFO_1SHOT	(BIT_2)
#define FM_31_BIT1_				(BIT_1)
#define FM_31_BIT0_LOOPEND		(BIT_0)

// flag33
#define FM_33_BIT6_TTL_TREMOLO_FLAG	(BIT_6)	/*!< 未実装のトレモロ・ワウワウ効果 */
#define FM_33_BIT5_REVERVE_FLAG	(BIT_5)
#define FM_33_BIT4_REVERVE_MODE	(BIT_4)

// DCSG Volume 6
#define DCSG_6_BITS_VOL		(BIT_0|BIT_1|BIT_2|BIT_3)		//	u8	dcsg_volume_6;	// DB	8		; CURENT VOLUME(bit0-3)	6
#define DCSG_6_BIT4_ATTACK		(BIT_4)		// bit 4 = attack flag
#define DCSG_6_BIT5_DECAY		(BIT_5)		// bit 5 = decay flag
#define DCSG_6_BIT6_SUSTAIN		(BIT_6)		// bit 6 = sustain flag
#define DCSG_6_BIT7_SOFT_ENV		(BIT_7)		// bit 7 = soft envelope flag

//#define DCSG_41_BIT7_SE_INT_ON		(BIT_0)		// bit 0 = 割り込み中
#define DCSG_41_BIT7_SE_DCSG_OFF	(BIT_0)		// bit 1 = DCSG OUT停止

#define DCSG_33_BIT7_HARDENV	(BIT_7)

#define DCSG_42_NOISE	(0x01)

// ---------------------------------------------------------------------------
#define MUCOM_FMVOICE_MAX (32)

// ---------------------------------------------------------------------------
#define FADE_SPEED_WAIT		(0x04)

// ---------------------------------------------------------------------------
// .mubファイルのヘッダー
// @note mubはリトルエンディアンのデータなので、ワードデータの扱いに注意
// ---------------------------------------------------------------------------
typedef struct _MUBHED
{
	//	Memory Data structure
	u8 magic[4];	//  = new byte[4]; // magic number
	u32 dataoffset; // DATA offset
	u32 datasize;   // DATA size
	u32 tagdata;	// TAG data file offset (0=none)
	u32 tagsize;	// TAG data size
	u32 pcmdata;	// PCM data offset (0=none)
	u32 pcmsize;	// PCM data size
	u32 jumpcount;  // Jump count (for skip)
	u32 jumpline;   // Jump line number (for skip)

	//	Extend data (2.0 Header)
	//
	u16 ext_flags;		 // Option flags ( MUCOM_FLAG_* )
	u8 ext_system;		 // build system ( MUCOM_SYSTEM_* )
	u8 ext_target;		 // playback target ( MUCOM_TARGET_* )
	u16 ext_channel_num; // Max channel table num
	u16 ext_fmvoice_num; // internal FM voice table num

	u32 ext_player;					   // external player option
	u32 pad1;						   // not use (reserved)
	u8 ext_fmvoice[MUCOM_FMVOICE_MAX]; // FM voice no.(orginal) table
} MUBHED;


//*****************************************************************************
// _subから移植
//*****************************************************************************
// ---------------------------------------------------------------------------
// SE関係
enum{
	SE_REQ_COMMAND = 0,
	SE_REQ_NO,

	SE_REQ_ARRAY_NUM,
};

enum{
	SE_CHANNEL_ID1 = 0,
	SE_CHANNEL_ID2,
	SE_CHANNEL_ID3,

	SE_CHANNEL_NUM
};

#define MUB_SEID_NONE    (0xff)
#define MUB_SEID_STOP    (0xff)


typedef struct _SE_REQ_TBL{
	u8	req_no[SE_CHANNEL_NUM];	// 0xff none
}SE_REQ_TBL;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Musicリクエスト
#define MUB_REQ_NONE	(0x00)
#define MUB_REQ_START	(0x01)
#define MUB_REQ_STOP	(0x02)
#define MUB_REQ_FADEOUT	(0x03)

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// SE リクエスト
#define SE_REQ_NONE		(0x00)
#define SE_REQ_NML		(0x01)
#define SE_REQ_TYPE1	(0x02)		// 未使用
#define SE_REQ_TYPE2	(0x03)		// 未使用

// ---------------------------------------------------------------------------
// リクエスト管理
// Z80に転送するリクエストデータを1つの構造体で管理して纏めて転送する
// ---------------------------------------------------------------------------
typedef struct _MUCOM88MDZ_REQ_WORK{
	// 
	u8	req_finish;

	// mub
	u8	mub_req_flag;
	u8	mub_req_no;
	u32	mub_bank;
	u8	mub_req_command;
	u8	mub_req_param;

	// SE
	u32	se_bank;
	u8	se_req_flag[SE_CHANNEL_NUM];
	u8	Mucom88mdz_SeReqArray[SE_REQ_ARRAY_NUM][SE_CHANNEL_NUM];

	// pcm
	u8	pcm_req_flag[2];
	u8	pcm_req_no[2];
	u8	pcm_req_prio[2];

	u32	pcm_bank;
	u8	pcm_req_command;			// Play/Stop
	u8	pcm_req_param;
	u16	start_adrs;
	u16	pcm_size;

	// Debug
	u8	cue;

}MUCOM88MD_REQ_WORK;

//*****************************************************************************
/**
 * Extern
*/
extern void Mucom88mdz_Init();
extern void Mu88md_FrameReset();
extern void Mu88md_SeReq(u8 se_no, u8 channel);
extern void Mu88md_SePackReq(u8 set_no);
extern void Mu88md_FrameFinish();

extern void Mu88md_SystemAllInit();
extern void Mu88_CallBackFunc();
extern void SwapMubHeaderEndian(MUBHED *src, MUBHED *dst);

extern FM_CH_WORK fm_ch_work[6];
extern FM_CH_WORK dcsg_ch_work[3];
extern FM_CH_WORK pcm_ch_work[1];

extern MUCOM88_SYS_WORK *pSys;
extern MUCOM88MD_REQ_WORK Mu88md_ReqWork;

// mu88md_drv01.c
extern void KLog_U1h(char *str, u32 num);
extern void KLog_U2h(char *str, u32 num, u32 num2);

#endif
