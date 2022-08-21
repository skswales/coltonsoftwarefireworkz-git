; firewrkz.iss

; This Source Code Form is subject to the terms of the Mozilla Public
; License, v. 2.0. If a copy of the MPL was not distributed with this
; file, You can obtain one at http://mozilla.org/MPL/2.0/.

; Copyright (C) 2006-2016 Stuart Swales

; Script generated by the Inno Setup Script Wizard.

; NB Some files are taken from absolute path - change as needed, others relative to this file

#define MainBinaryName  "firewrkz.exe"
#define SetupBaseName   "setup_firewrkz_"
#define FilesPath       AddBackSlash(SourcePath) + "CD"
#define AppVersion      GetFileVersion(AddBackslash(FilesPath) + MainBinaryName)
#define AVF1            Copy(AppVersion, 1, Pos(".", AppVersion) - 1) + "_" + Copy(AppVersion, Pos(".", AppVersion) + 1)
#define AVF2            Copy(AVF1,       1, Pos(".", AVF1      ) - 1) + "_" + Copy(AVF1      , Pos(".", AVF1      ) + 1)
#define AppVersionFile  Copy(AVF2,       1, Pos(".", AVF2      ) - 1) + "_" + Copy(AVF2      , Pos(".", AVF2      ) + 1)

[Setup]
; AppVersion=2.xx.yy
; AppVerName=Colton Software Fireworkz 2.xx.yy
AppVersion=2.00.05
AppVerName=Colton Software Fireworkz 2.00.05
AppCopyright=Copyright (C) 1992-2016 Colton Software
AppId=Colton Fireworkz for Windows (32-bit)
AppName=Colton Software Fireworkz for Windows (32-bit)
AppPublisher=Colton Software
AppPublisherURL=http://groups.google.com/group/colton-software-fireworkz
AppSupportURL=http://groups.google.com/group/colton-software-fireworkz
AppUpdatesURL=http://groups.google.com/group/colton-software-fireworkz
DefaultDirName={pf}\Colton Software\Fireworkz
DefaultGroupName=Colton Software
LicenseFile=.\CD\LICENCE.TXT
OutputDir=InnoSetup
OutputBaseFilename={#SetupBaseName + AppVersionFile}
PrivilegesRequired=poweruser
Compression=lzma
SolidCompression=yes
SetupIconFile=..\..\..\firewrkz\resource\windows\prog.ico
AllowNoIcons=yes
ChangesAssociations=yes
UserInfoPage=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "CD\RelNotes.htm"; DestDir: "{app}"; Flags: ignoreversion isreadme
Source: "CD\firewrkz.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "CD\DefaultUser\*"; DestDir: "{app}\DefaultUser\"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "CD\System\*"; DestDir: "{app}\System\"; Flags: ignoreversion recursesubdirs createallsubdirs
;
; INI file used by Dial Solutions DLLs
Source: "CD\Windows\RO.ini"; DestDir: "{win}"; Flags: onlyifdoesntexist

[Icons]
;
; Start Menu Icon
;
Name: "{group}\Fireworkz"; Filename: "{app}\firewrkz.exe"
;
; Optional Desktop Icon
;
Name: "{userdesktop}\Fireworkz"; Filename: "{app}\firewrkz.exe"; Tasks: desktopicon

[Registry]
;
; Main keys (don't override existing installation's preferences)
;
Root: HKLM; Subkey: "SOFTWARE\Colton Software"; Flags: uninsdeletekeyifempty
Root: HKLM; Subkey: "SOFTWARE\Colton Software\Fireworkz"; Flags: uninsdeletekeyifempty
Root: HKLM; Subkey: "SOFTWARE\Colton Software\Fireworkz"; ValueType: string; ValueName: "Directory"; ValueData: "{app}"; Flags: uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\Colton Software\Fireworkz"; ValueType: string; ValueName: "SystemPath"; ValueData: "{app}\System\UK"; Flags: uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\Colton Software\Fireworkz"; ValueType: string; ValueName: "NetworkPath"; ValueData: ""; Flags: uninsdeletekey createvalueifdoesntexist
Root: HKLM; Subkey: "SOFTWARE\Colton Software\Fireworkz"; ValueType: string; ValueName: "Usr1"; ValueData: "{userinfoname}"; Flags: uninsdeletekey createvalueifdoesntexist
Root: HKLM; Subkey: "SOFTWARE\Colton Software\Fireworkz"; ValueType: string; ValueName: "Usr2"; ValueData: "{userinfoorg}"; Flags: uninsdeletekey createvalueifdoesntexist
Root: HKLM; Subkey: "SOFTWARE\Colton Software\Fireworkz"; ValueType: string; ValueName: "RegistrationNumber"; ValueData: ""; Flags: uninsdeletekey createvalueifdoesntexist
Root: HKLM; Subkey: "SOFTWARE\Colton Software\Fireworkz"; ValueType: string; ValueName: "ButtonStyle"; ValueData: "97"; Flags: uninsdeletekey createvalueifdoesntexist
Root: HKLM; Subkey: "SOFTWARE\Colton Software\Fireworkz"; ValueType: string; ValueName: "ReportEnable"; ValueData: "0"; Flags: uninsdeletekey createvalueifdoesntexist
; StandardTemplates set / DefaultTemplate may be forced to be updated for each user when program is run - change each file's version (ISO date)
Root: HKLM; Subkey: "SOFTWARE\Colton Software\Fireworkz"; ValueType: string; ValueName: "StandardTemplates"; ValueData: "20150915:Letter.fwt;20150915:Sheet.fwt"; Flags: uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\Colton Software\Fireworkz"; ValueType: string; ValueName: "DefaultTemplate"; ValueData: "20150915:firewrkz.fwt"; Flags: uninsdeletekey
;
Root: HKLM; Subkey: "SOFTWARE\Colton Software\Fireworkz"; ValueType: none;   ValueName: "DialogStyle"; ValueData: "xx"; Flags: deletevalue
;
; Various keys are now cloned to each HKCU by Fireworkz on first-run (UserPath non present or empty)
;
Root: HKCU; Subkey: "SOFTWARE\Colton Software\Fireworkz"; ValueType: none;   ValueName: "DialogStyle"; ValueData: "xx"; Flags: deletevalue
;
; Our file types
;
Root: HKLM; SubKey: "SOFTWARE\Classes\.fwk"; ValueType: string; ValueName: ; ValueData: "ColtonSoftware.Fireworkz.Document"; Flags: uninsdeletekey
Root: HKLM; SubKey: "SOFTWARE\Classes\.fwk\ColtonSoftware.Fireworkz.Document\ShellNew"; ValueType: string; ValueName: "Filename"; ValueData: "firewrkz.fwt"; Flags: uninsdeletekey
;
Root: HKLM; SubKey: "SOFTWARE\Classes\.fwt"; ValueType: string; ValueName: ; ValueData: "ColtonSoftware.Fireworkz.Document"; Flags: uninsdeletekey
;
Root: HKLM; SubKey: "SOFTWARE\Classes\ColtonSoftware.Fireworkz.Document"; ValueType: string; ValueName: ; ValueData: "Colton Fireworkz Document"; Flags: uninsdeletekey
Root: HKLM; SubKey: "SOFTWARE\Classes\ColtonSoftware.Fireworkz.Document\DefaultIcon"; ValueType: string; ValueName: ; ValueData: "{app}\firewrkz.exe,1"; Flags: uninsdeletekey
Root: HKLM; SubKey: "SOFTWARE\Classes\ColtonSoftware.Fireworkz.Document\shell\New\command"; ValueType: string; ValueName: ; ValueData: """{app}\firewrkz.exe"" /n"; Flags: uninsdeletekey
Root: HKLM; SubKey: "SOFTWARE\Classes\ColtonSoftware.Fireworkz.Document\shell\New\ddeexec"; ValueType: string; ValueName: ; ValueData: "[FileNew(""%1"")]"; Flags: uninsdeletekey
Root: HKLM; SubKey: "SOFTWARE\Classes\ColtonSoftware.Fireworkz.Document\shell\New\ddeexec\application"; ValueType: string; ValueName: ; ValueData: "Fireworkz"; Flags: uninsdeletekey
Root: HKLM; SubKey: "SOFTWARE\Classes\ColtonSoftware.Fireworkz.Document\shell\New\ddeexec\topic"; ValueType: string; ValueName: ; ValueData: "System"; Flags: uninsdeletekey
Root: HKLM; SubKey: "SOFTWARE\Classes\ColtonSoftware.Fireworkz.Document\shell\Open\command"; ValueType: string; ValueName: ; ValueData: """{app}\firewrkz.exe"" /n"; Flags: uninsdeletekey
Root: HKLM; SubKey: "SOFTWARE\Classes\ColtonSoftware.Fireworkz.Document\shell\Open\ddeexec"; ValueType: string; ValueName: ; ValueData: "[FileOpen(""%1"")]"; Flags: uninsdeletekey
Root: HKLM; SubKey: "SOFTWARE\Classes\ColtonSoftware.Fireworkz.Document\shell\Open\ddeexec\application"; ValueType: string; ValueName: ; ValueData: "Fireworkz"; Flags: uninsdeletekey
Root: HKLM; SubKey: "SOFTWARE\Classes\ColtonSoftware.Fireworkz.Document\shell\Open\ddeexec\topic"; ValueType: string; ValueName: ; ValueData: "System"; Flags: uninsdeletekey
Root: HKLM; SubKey: "SOFTWARE\Classes\ColtonSoftware.Fireworkz.Document\shell\Print\command"; ValueType: string; ValueName: ; ValueData: """{app}\firewrkz.exe"" /n"; Flags: uninsdeletekey
Root: HKLM; SubKey: "SOFTWARE\Classes\ColtonSoftware.Fireworkz.Document\shell\Print\ddeexec"; ValueType: string; ValueName: ; ValueData: "[FilePrint(""%1"")]"; Flags: uninsdeletekey
Root: HKLM; SubKey: "SOFTWARE\Classes\ColtonSoftware.Fireworkz.Document\shell\Print\ddeexec\application"; ValueType: string; ValueName: ; ValueData: "Fireworkz"; Flags: uninsdeletekey
Root: HKLM; SubKey: "SOFTWARE\Classes\ColtonSoftware.Fireworkz.Document\shell\Print\ddeexec\topic"; ValueType: string; ValueName: ; ValueData: "System"; Flags: uninsdeletekey
Root: HKLM; SubKey: "SOFTWARE\Classes\ColtonSoftware.Fireworkz.Document\shell\PrintTo\command"; ValueType: string; ValueName: ; ValueData: """{app}\firewrkz.exe"" /n"; Flags: uninsdeletekey
Root: HKLM; SubKey: "SOFTWARE\Classes\ColtonSoftware.Fireworkz.Document\shell\PrintTo\ddeexec"; ValueType: string; ValueName: ; ValueData: "[FilePrintTo(""%1"",""%2"")]"; Flags: uninsdeletekey
Root: HKLM; SubKey: "SOFTWARE\Classes\ColtonSoftware.Fireworkz.Document\shell\PrintTo\ddeexec\application"; ValueType: string; ValueName: ; ValueData: "Fireworkz"; Flags: uninsdeletekey
Root: HKLM; SubKey: "SOFTWARE\Classes\ColtonSoftware.Fireworkz.Document\shell\PrintTo\ddeexec\topic"; ValueType: string; ValueName: ; ValueData: "System"; Flags: uninsdeletekey
;
; So users can start application from Explorer's Run dialog by entering just the EXE's filename and no path
;
Root: HKLM; Subkey: "SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\firewrkz.exe"; ValueType: string; ValueName: ""; ValueData: "{app}\firewrkz.exe"; Flags: uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\firewrkz.exe"; ValueType: string; ValueName: "Path"; ValueData: "{app}"; Flags: uninsdeletekey
;
; Font mapping entries used by Dial Solutions DLLs
; Only write these once so that we don't override any user font mapping
Root: HKLM; Subkey: "SOFTWARE\Dial Solutions Limited"; Flags: uninsdeletekeyifempty
Root: HKLM; Subkey: "SOFTWARE\Dial Solutions Limited\DsFntMap"; Flags: uninsdeletekeyifempty
; NB Allow Dial Solutions DLLs to create the Fontmap subkey for correct initialisation
; We also allow standard users ability to change font mapping
Root: HKLM; Subkey: "SOFTWARE\Dial Solutions Limited\DsFntMap\1.0"; Permissions: users-modify

[Run]
;
; Offer to launch app on Setup complete
;
Filename: "{app}\firewrkz.exe"; Description: "{cm:LaunchProgram,Fireworkz}"; Flags: nowait postinstall skipifsilent

; end of .iss file
