//--------------------------------------
// このファイルは mucom88mdcCnvSe で自動生成しています
//--------------------------------------
#ifndef _MU88MD_SE_H_
#define _MU88MD_SE_H_
#include <genesis.h>
#include "mu88md_drv.h"
enum{
	MUB_SEID_N00_OK_D,
	MUB_SEID_N01_CANCEL_D,
	MUB_SEID_N02_CANT_SELECT_D,
	MUB_SEID_N03_CURSOR_MOVE_D,
	MUB_SEID_N04_PAUSE_D,
	MUB_SEID_N05_TALK_D,
	MUB_SEID_N10_COIN_D,
	MUB_SEID_N11_SHOT_D,
	MUB_SEID_N12_BOM_D,
	MUB_SEID_N13_NOISE0000_F,
	MUB_SEID_N14_POWA_E,
	MUB_SEID_NUM
};

//--------------------------------------

//--------------------------------------
#define MUCOM88MD_SE_NUM	(11)
#define MUCOM88MD_SE_SET_NUM	(11)

//--------------------------------------
extern const u16 Mucom88mdSeTbl;
extern const u8 Mucom88md_SeIdTbl[MUCOM88MD_SE_NUM];
extern const SE_REQ_TBL Mucom88md_SeSetTbl[MUCOM88MD_SE_SET_NUM];
#endif /* _MUCOM88MD_SE_H_*/
