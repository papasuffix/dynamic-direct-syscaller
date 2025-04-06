@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
ml64.exe stub.asm /c /Fo:stub.obj
cl.exe main.cpp stub.obj /OUT:main.exe /SUBSYSTEM:CONSOLE /std:c++20