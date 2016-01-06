@echo off

call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x64
cd bin 2>NUL || echo Creating bin folder && mkdir bin && cd bin

set IgnoreWarn= -wd4100 -wd4101 -wd4189 -wd4706 -wd4701 -wd4505
set CLFlags= -MT -nologo -Od -W4 -WX -Zi %IgnoreWarn% -D_CRT_SECURE_NO_WARNINGS
set LDFlags= -opt:ref user32.lib shell32.lib

cl %CLFlags% ../src/pack_sprites.cpp -link %LDFlags% -out:pack_sprites.exe
cl %CLFlags% ../src/pack_binary.cpp -link %LDFlags% -out:pack_binary.exe

copy ..\src\convert_mp3.py convert_mp3.py >NUL

cd ..

