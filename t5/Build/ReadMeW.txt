ReadMe for Fireworkz Build (Microsoft Windows)
----------------------------------------------

** This Source Code Form is subject to the terms of the Mozilla Public
** License, v. 2.0. If a copy of the MPL was not distributed with this
** file, You can obtain one at https://mozilla.org/MPL/2.0/.

** Copyright � 2013-2022 Stuart Swales

Prerequisites
-------------

A Windows SVN client such as TortoiseSVN (minimal install, with command line tools, no en_US dictionary, no additional iconsets will suffice).

GNU Win32 Patch to generate the patched BTTNCUR(P) source files.

Microsoft Visual Studio 2019 (C compiler, headers, libraries, linker). The free Community Edition is suitable (you don't need Blend, MFC, SQL or Web development options). You need to add 'C++ Windows XP Support for VS 2017 (v141) tools [deprecated]'.

I recommend setting:

    Tools - Options - Text editor - C++:
        Tabs - Insert Spaces
        Formatting - Spacing - Spacing for control blocks - Insert space between keyword ... (Off)

InnoSetup to create the distributable setup executable.

GNU Win32 Zip to generate the final distributable file.

NB. Fireworkz Pro can not be built for Windows.


First-time configuration and build
----------------------------------

You first need to acquire some files that are needed to compile Fireworkz but can't be redistributed by this repository.

See

external/Microsoft/InsideOLE2/README.TXT

for the 'Inside OLE 2' BTTNCUR sample.

Then run

external/setup-windows.cmd

to copy and patch those files as needed.

Double-click on Build\windows\firewrkz-vs2019.sln to load the solution and project files into Visual Studio.

Choose the variant to build (e.g. Debug XP ANSI or Release XP ANSI).

Select Build -> Build Solution.


Subsequent builds
-----------------

To clean up a build, select Build -> Build Solution.

In order to build again, select Build -> Build Solution or Build -> Rebuild Solution.


Creating a package for distribution
-----------------------------------

First build a Release copy.

These scripts assume that N: is mapped to the root of your development tree i.e. the dir containing \fireworkz

You can easily do this with a batch file dropped in your Startup folder containing

SUBST N: %USERPROFILE%\cs-dev

or some appropriate equivalent for your own dev environment.

At a command prompt:

cd /p N:\fireworkz\trunk\t5

pushd firewrkz\windows\OUTx86

BLD32CD.BAT

This copies the contents of DISC1-32.SRC, the release executable, and other supporting components (e.g. Draw file DLLs) to the directory CD.

firewrkz.iss

brings up InnoSetup with this script loaded.

From the InnoSetup menu, select Build -> Compile.

This creates a setup file for distribution in InnoSetup, named as per the inbuilt release number, fireworkz-x_yy_zz_0-windows-x86.exe

Then use BLD32CD2.BAT to create the zip for distribution, tidying intermediate temporary directories.
