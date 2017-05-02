@echo off

set CONFIGURATION_TYPES=Debug;Release
set SHARED_LIBS=true
set DO_CLEAN=false
set DO_BUILD=false
set DEFINES=
set GENERATOR="Visual Studio 14 2015"
set ERRORLEVEL=0
set BUILD_FOLDER=build_win32

:command_line_start
if "%1" == "" (
  goto command_line_end
)

if "%1" == "-config" (
    echo Config ignored for creating solution
) else if /I "%1" == "-shared" (
    set SHARED_LIBS=%2
) else if /I "%1" == "-clean" (
    set DO_CLEAN=%2
) else if /I "%1" == "-generator" (
    set GENERATOR=%2
) else if /I "%1" == "-build" (
    set DO_BUILD=%2
) else if /I "%1" == "-mem_check" (
    if %2 == true (
      set DEFINES=%DEFINES% -DDBG_USE_MEM_CHECK=1
    )
) else if /I "%1" == "-dbg_use_mem_check" (
    if %2 == true (
      set DEFINES=%DEFINES% -DDBG_USE_MEM_CHECK=1
    )
    if %2 == false (
      set DEFINES=%DEFINES% -DDBG_USE_MEM_CHECK=0
    )
) else if /I "%1" == "-define" (
    set DEFINES=%DEFINES% -D%2=%3
    shift
) else if /I "%1" == "-d" (
    set DEFINES=%DEFINES% -D%2=%3
    shift
) else if /I "%1" == "-build_folder" (
    set BUILD_FOLDER=build_%2
) else (
    echo Wrong command line parameter %1
    goto failed_no_pop
)
shift
shift
goto command_line_start

:command_line_end

echo =================================================================================
echo                Creating Solution for types=%CONFIGURATION_TYPES% shared=%SHARED_LIBS%
echo =================================================================================

mkdir %BUILD_FOLDER% > NUL 2>&1
pushd %BUILD_FOLDER%
if not %ERRORLEVEL% == 0 goto failed_no_pop

if %DO_CLEAN% == true (
  del /F CMakeCache.txt > NUL 2>&1
)

echo -DBUILD_SHARED_LIBS=%SHARED_LIBS% -G%GENERATOR% -DCMAKE_CONFIGURATION_TYPES=%CONFIGURATION_TYPES% %DEFINES% ..
cmake.exe -DBUILD_SHARED_LIBS=%SHARED_LIBS% -G%GENERATOR% -DCMAKE_CONFIGURATION_TYPES=%CONFIGURATION_TYPES% %DEFINES% ..
if not %ERRORLEVEL% == 0 goto failed
popd

:ok
echo =================================================================================
echo                        Creating Solution done.
echo =================================================================================
goto end

:failed
popd
:failed_no_pop
echo =================================================================================
echo           !!!!!!!!!! ------ Failed Creating Solution --------- !!!!!!!!!!
echo =================================================================================
pause

:end
