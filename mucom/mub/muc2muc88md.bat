rem @echo off
mucom88 -c sampl1.muc -o sampl1.mub
mucom88 -c sampl2.muc -o sampl2.mub

..\mucom88mdcCnv.exe -m mublist.txt -w wavlist.txt

move _mu88md_mub.h ../../src
move _mu88md_mub.c ../../src
move _mu88md_mub*.s ../../src
move _mu88md_pcm*.c ../../src
move _mu88md_pcm*.h ../../src
move _mu88md_pcm*.s ../../src

del *.mub
