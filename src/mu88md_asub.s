|======================================================================
| For YM Acsess Sub
|======================================================================

|======================================================================
| Macro
|----------------------------------------------------------------------
.macro func _name, _align=2
    .section .text.asm.\_name
    .globl  \_name
    .type   \_name, @function
    .align \_align
  \_name:
.endm

|----------------------------------------------------------------------
| BusReq / Wait / Ym Acsess / Z80:RegSet / Release
| PSGOUTの代用ファンクション
|----------------------------------------------------------------------
| extern void WriteFmPortBat0(u8 wreg,u8 param);
func WriteFmPortBat0

	movem.l	%a6/%a3,-(%sp)
	movem.l	%d0/%d1,-(%sp)

    move.l  20(%sp),%d0
    move.l  24(%sp),%d1

	bsr.s	fm_z80_wait_sub

	move.b	%d0,0000(%a6)
	nop
	nop

	move.b	%d1,0001(%a6)

	nop
	nop
	nop
	nop
	nop
	nop							| 48(over 47)

	nop
	nop
	nop
	nop
	nop							| 88 (over 83)


	move.b	#0x2a,0000(%a6)		| for Z80

	move.w	#0000,(%a3)			| Z80 Start

	movem.l	(%sp)+,%d0/%d1
	movem.l	(%sp)+,%a6/%a3
	rts

|- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
fm_z80_wait_sub:
	lea		0xa11100, %a3
	lea		0xa04000, %a6

	move.w	#0x0100,(%a3)

wait_z80:
	btst.b	#0,(%a3)
	bne.s	wait_z80
	rts

|----------------------------------------------------------------------
| BusReq / Wait / Ym Acsess / Z80:RegSet / Release
|----------------------------------------------------------------------
| extern void WriteFmPortBat1(u8 wreg,u8 param);
func WriteFmPortBat1

	movem.l	%a6/%a3,-(%sp)
	movem.l	%d0/%d1,-(%sp)

	move.l  20(%sp),%d0
	move.l  24(%sp),%d1

	bsr.s	fm_z80_wait_sub

	move.b	%d0,0002(%a6)
	nop
	nop
	
	move.b	%d1,0003(%a6)

	nop
	nop
	nop
	nop
	nop
	nop

	nop
	nop
	nop
	nop
	nop

	move.b	#0x2a,0000(%a6)		| for Z80

	move.w	#0000,(%a3)			| Z80 Start

	movem.l	(%sp)+,%d0/%d1
	movem.l	(%sp)+,%a6/%a3
	rts


|----------------------------------------------------------------------
| 音色変更用の連続呼び出し用（BusReq / jsr / Z80:YM Set / ReqRelease）
| パラメータセット＋呼び出しを連続しても耐えられるWait
|----------------------------------------------------------------------
| extern void WriteFmPortCore0(u8 wreg,u8 param);
func WriteFmPortCore0

	movem.l	%a6/%a3,-(%sp)
	movem.l	%d0/%d1,-(%sp)

    move.l  20(%sp),%d0
    move.l  24(%sp),%d1

	lea		0xa04000, %a6

	move.b	%d0,0000(%a6)
	nop
	nop

	move.b	%d1,0001(%a6)

	nop
	nop
	nop
	nop
	nop
	nop

	nop

	movem.l	(%sp)+,%d0/%d1
	movem.l	(%sp)+,%a6/%a3
	rts

|----------------------------------------------------------------------
| 音色変更用の連続呼び出し用（BusReq / jsr / Z80:YM Set / ReqRelease）
| パラメータセット＋呼び出しを連続しても耐えられるWait
|----------------------------------------------------------------------
| extern void WriteFmPortCore1(u8 wreg,u8 param);
func WriteFmPortCore1
	movem.l	%a6/%a3,-(%sp)
	movem.l	%d0/%d1,-(%sp)
	move.l  20(%sp),%d0
	move.l  24(%sp),%d1

	lea		0xa04000, %a6

	move.b	%d0,0002(%a6)
	nop
	nop
	
	move.b	%d1,0003(%a6)

	nop
	nop
	nop
	nop
	nop
	nop

	nop

	movem.l	(%sp)+,%d0/%d1
	movem.l	(%sp)+,%a6/%a3
	rts
