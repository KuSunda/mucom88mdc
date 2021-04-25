mucom88 -c N00_OK.muc -o N00_OK.mub
mucom88 -c N01_CANCEL.muc -o N01_CANCEL.mub
mucom88 -c N02_CANT_SELECT.muc -o N02_CANT_SELECT.mub
mucom88 -c N03_CURSOR_MOVE.muc -o N03_CURSOR_MOVE.mub
mucom88 -c N04_PAUSE.muc -o N04_PAUSE.mub
mucom88 -c N05_TALK.muc -o N05_TALK.mub
mucom88 -c N10_COIN.muc -o N10_COIN.mub
mucom88 -c N11_SHOT.muc -o N11_SHOT.mub
mucom88 -c N12_BOM.muc -o N12_BOM.mub
mucom88 -c N13_NOISE0000.muc -o N13_NOISE0000.mub
mucom88 -c N14_POWA.muc -o N14_POWA.mub

..\mucom88mdcCnvSe.exe se_list.txt 

move _mu88md*.c ..\..\src
move _mu88md*.h ..\..\src
move _mu88md*.s ..\..\src
del *.mub
