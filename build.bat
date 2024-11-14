::
: \file build.bat
: \author Isaiah Lateer
: 
: Used to build the project.
: 

@echo off

setlocal enabledelayedexpansion

pushd "%programfiles%"

for /r %%a in (*vcvarsall.bat) do (
    set devcmd="%%a"
)

popd

if not defined devcmd (
    pushd "%programfiles(x86)%"

    for /r %%a in (*vcvarsall.bat) do (
        set devcmd="%%a"
    )

    popd
)

if not defined devcmd (
    echo [ERROR] Failed to find developer command prompt.
    exit
)

pushd "%~dp0"

if not exist obj mkdir obj
if not exist obj\%target% mkdir obj\%target%

if not exist bin mkdir bin
if not exist bin\%target% mkdir bin\%target%

for /r %%a in (*.c) do (
    if /i "%%~xa"==".c" (
        set source_files=!source_files! %%a
    )
)

call set "source_files=%%source_files:%~dp0=%%"
set source_files=!source_files:~1!

set flags=/nologo /EHsc /fp:precise /sdl /GS /W3 /WX /MD
set flags=%flags% /DNDEBUG /D_UNICODE /DUNICODE
set flags=%flags% /Isrc /Foobj\ /Febin\noise_generator.exe

call %devcmd% x64

cl %source_files% %flags%

popd
