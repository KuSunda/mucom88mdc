//*****************************************************************************
/**
 * @file s_main.c
 */
//*****************************************************************************
#include <genesis.h>
#include "mu88md_drv.h"
#include "mu88md_test.h"

#include "_mu88md_mub.h"			// 自動作成

//-----------------------------------------------------------------------------
// Declaration
//-----------------------------------------------------------------------------
int main();
void VBlank_func();

//-----------------------------------------------------------------------------
#define DISP_JOB_INFO	(1)
#define JOB_PEAK_TIME	(20)
#define USE_CUSTOM_SDGK  (0)
//*****************************************************************************
// Program
//*****************************************************************************

//-----------------------------------------------------------------------------
// VBlank　CallBack
//-----------------------------------------------------------------------------
void VBlank_func()
{
	VDP_updateSprites(0, FALSE);
	startTimer(0);
	Mu88_CallBackFunc();
}

//-----------------------------------------------------------------------------
// Main
//-----------------------------------------------------------------------------
extern void Dbg_SetPrintPos(u16 x, u16 y);

// 負荷計測用
#if USE_CUSTOM_SDGK
extern u16 CpuFrameLoad;			// Custom SGDK
#else
u16 CpuFrameLoad;			// Custom SGDK
#endif

u16 CpuFrameLoad_timer;
u16 CpuFrameLoad_max;

#define		MUCOM88_LOAD_MEAN_FRAME_NUM		(1)

int main()
{
	u32 Gametime = 0;
	CpuFrameLoad_timer = 0;
	CpuFrameLoad_max = 0;

	Mu88md_SystemAllInit();				// [Mu88md] Mu88md　初期化
	SYS_setVIntCallback(VBlank_func);	// [Mu88md] CallBack登録

	JoySystemInitilize();
	Mucom88md_SoundTestInit();

	Dbg_SetPrintPos(17,0);
	Dbg_PrintString("YM:", 0);

#if USE_CUSTOM_SDGK
	Dbg_SetPrintPos(24,0);
	Dbg_PrintString("CP:", 0);
#endif
	Dbg_SetPrintPos(31,0);
	Dbg_PrintString("CA:", 0);


	while (1)
	{
		Gametime++;
		Mu88md_FrameReset(); 		// [Mu88md] Mu88mdのリクエスト初期化

		JoyDataUpdate();
		Mucom88md_SoundTestUpdate();


		Mu88md_FrameFinish();		// [Mu88md] Mu88mdのリクエスト完了

#if DISP_JOB_INFO
		{
			u16 cpuload = 0;
#if USE_CUSTOM_SDGK
			cpuload = (CpuFrameLoad * ((u16) 100)) / (u16) (MUCOM88_LOAD_MEAN_FRAME_NUM * 255);

			CpuFrameLoad_timer++;
			if( CpuFrameLoad_timer > JOB_PEAK_TIME ){
				CpuFrameLoad_timer = 0;
				CpuFrameLoad_max = 0;
			}

			if(CpuFrameLoad_max < cpuload){
				CpuFrameLoad_timer = 0;
				CpuFrameLoad_max = cpuload;
			}

			cpuload = CpuFrameLoad_max;
#endif

			char tmp[16];
			// 
			Dbg_SetPrintPos(20, 0);
			intToStr(pSys->ym_access_max,&tmp[0], 3);
			Dbg_PrintString(&tmp[0], 0);

			// 
#if USE_CUSTOM_SDGK
			Dbg_SetPrintPos(27, 0);
			intToStr((s16)cpuload,&tmp[0], 3);
			Dbg_PrintString(&tmp[0], 0);
#endif

			cpuload = SYS_getCPULoad();
			Dbg_SetPrintPos(34, 0);
			intToStr((s16)cpuload,&tmp[0], 3);
			Dbg_PrintString(&tmp[0], 0);


		}
#endif

		SYS_doVBlankProcess();
	}
	return (0);
}
