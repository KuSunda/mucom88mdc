//*****************************************************************************
/**
 * @file mu88md_asub.h
 * @brief アセンブラサブルーチンのヘッダー
 */
//*****************************************************************************
#ifndef _MUCOM88MD_YMSUB_H_
#define _MUCOM88MD_YMSUB_H_

#include <genesis.h>

extern void WriteFmPortBat0(u8 wreg,u8 param);
extern void WriteFmPortBat1(u8 wreg,u8 param);
extern void WriteFmPortCore0(u8 wreg,u8 param);
extern void WriteFmPortCore1(u8 wreg,u8 param);

#endif		// _MUCOM88MD_YMSUB_H_