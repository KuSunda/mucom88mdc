* このファイルは mucom88mdcCnvSe で自動生成しています
.text

	.align  2

	.global  Mucom88mdSeTbl

Mucom88mdSeTbl:
	dc.w	N00_OK_D-Mucom88mdSeTbl
	dc.w	N01_CANCEL_D-Mucom88mdSeTbl
	dc.w	N02_CANT_SELECT_D-Mucom88mdSeTbl
	dc.w	N03_CURSOR_MOVE_D-Mucom88mdSeTbl
	dc.w	N04_PAUSE_D-Mucom88mdSeTbl
	dc.w	N05_TALK_D-Mucom88mdSeTbl
	dc.w	N10_COIN_D-Mucom88mdSeTbl
	dc.w	N11_SHOT_D-Mucom88mdSeTbl
	dc.w	N12_BOM_D-Mucom88mdSeTbl
	dc.w	N13_NOISE0000_F-Mucom88mdSeTbl
	dc.w	N14_POWA_E-Mucom88mdSeTbl

N00_OK_D:
	dc.w	0xf001,0xf708,0xf10e,0x0344
	dc.w	0x0347,0x0000
N01_CANCEL_D:
	dc.w	0xf007,0xf708,0xf400,0x1001
	dc.w	0x1900,0x04f1,0x0e07,0x3907
	dc.w	0x3300
N02_CANT_SELECT_D:
	dc.w	0xf007,0xf708,0xf400,0x1001
	dc.w	0x1900,0x04f1,0x0e07,0x4907
	dc.w	0x4400
N03_CURSOR_MOVE_D:
	dc.w	0xf008,0xf708,0xf10f,0x0751
	dc.w	0x0000
N04_PAUSE_D:
	dc.w	0xf002,0xf708,0xf10e,0x0754
	dc.w	0x0750,0x0754,0x0e50,0x0000
N05_TALK_D:
	dc.w	0xf008,0xf708,0xf10f,0x0343
	dc.w	0x0140,0x0000
N10_COIN_D:
	dc.w	0xf002,0xf708,0xf10e,0x074b
	dc.w	0x3154,0x0000
N11_SHOT_D:
	dc.w	0xf000,0xf708,0xf400,0x0101
	dc.w	0x7805,0xc8f1,0x0ff5,0x0800
	dc.w	0x0e7b,0xfbff,0xf601,0x0107
	dc.w	0x0000
N12_BOM_D:
	dc.w	0xf002,0xf708,0xf10e,0x0140
	dc.w	0x0141,0x0142,0x0143,0x0144
	dc.w	0x0145,0x0144,0x0142,0x0142
	dc.w	0x0141,0x0140,0x013b,0x013a
	dc.w	0x0139,0x0138,0x0137,0x0136
	dc.w	0x0135,0x0134,0x0133,0x0132
	dc.w	0x0131,0x0130,0x012b,0x012a
	dc.w	0x0129,0x0128,0x0127,0x0126
	dc.w	0x0125,0x0124,0x0123,0x0000
N13_NOISE0000_F:
	dc.w	0xf000,0xf708,0xf10e,0xfaff
	dc.w	0xffff,0xbe01,0x14f7,0x01f8
	dc.w	0x0001,0x50f8,0x0a01,0x50f8
	dc.w	0x0f01,0x50f8,0x1401,0x50f8
	dc.w	0x0001,0x50fd,0xf800,0x0150
	dc.w	0xfdf8,0x0001,0x50fd,0xf800
	dc.w	0x0150,0x0000
N14_POWA_E:
	dc.w	0xf003,0xf708,0xf400,0x1001
	dc.w	0x1900,0x04f1,0x0ff5,0x0b00
	dc.w	0x0754,0xfd07,0x50fb,0xfff6
	dc.w	0x0606,0x0a00,0x0000
