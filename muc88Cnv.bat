rem Convert BGM and SE and Rebuild

call clean_mu88md.bat

pushd .\mucom\mub
call muc2muc88md.bat
popd
pushd .\mucom\se
call se2muc88md.bat
popd

make -f makefile.gen clean
make -f makefile.gen release
