@echo off

set CommonDefines= -DSECHURAN_INTERNAL=1 -DSECHURAN_SLOW=1 -DSECHURAN_WIN32=1
set CommonCompilerFlags= -MTd -nologo -fp:fast -Gm- -GR- -EHa- -Od -Oi -WX -W4 -wd4201 -wd4100 -wd4189 -wd4505 %CommonDefines% -FC -Z7 
set CommonLinkerFlags= -incremental:no -opt:ref user32.lib Gdi32.lib winmm.lib

REM TODO - can we just build both with one exe?

IF NOT EXIST ..\..\build mkdir ..\..\build
pushd ..\..\build

REM 64-bit build
del *.pdb > NUL 2> NUL
REM Optimization switches /O2
echo WAITING FOR PDB > lock.tmp
REM cl %CommonCompilerFlags% ..\sechuran\code\sechuran.cpp -Fmsechuran.map -LD /link -incremental:no -opt:ref -PDB:sechuran_%random%.pdb -EXPORT:GameGetSoundSamples -EXPORT:GameUpdateAndRender
del lock.tmp
cl %CommonCompilerFlags% ..\sechuran\code\win32_sechuran.cpp -Fmwin32_sechuran.map /link %CommonLinkerFlags%
popd