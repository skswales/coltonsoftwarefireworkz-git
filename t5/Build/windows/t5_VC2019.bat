@rem This Source Code Form is subject to the terms of the Mozilla Public
@rem License, v. 2.0. If a copy of the MPL was not distributed with this
@rem file, You can obtain one at https://mozilla.org/MPL/2.0/.

@rem Copyright � 2020-2022 Stuart Swales

chcp 1252

if exist local_env.bat call local_env

if exist "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat"  call "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x86
if exist      "%ProgramFiles%\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat"  call      "%ProgramFiles%\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x86

call t5_tbt

set FIREWORKZ_ROOT=N:\fireworkz\%FIREWORKZ_TBT%

@rem display environment set for development
set

cd /D %FIREWORKZ_ROOT%

title %FIREWORKZ_TBT% - Fireworkz
