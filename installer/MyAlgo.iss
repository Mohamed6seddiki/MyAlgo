#define MyAppName "MyAlgo"
#define MyAppVersion "1.0.0"

[Setup]
AppName=MyAlgo
AppVersion=1.0.0
DefaultDirName={commonpf}\MyAlgo
DefaultGroupName=MyAlgo
OutputDir=Output
OutputBaseFilename=MyAlgoSetup
Compression=lzma
SolidCompression=yes
ChangesEnvironment=yes
ChangesAssociations=yes
PrivilegesRequired=admin
SetupIconFile=..\installer\assets\myalgo.ico

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Files]
Source: "..\build\Release\myalgo.exe"; DestDir: "{app}"; DestName: "myalgo.exe"; Flags: ignoreversion
Source: "..\installer\assets\myalgo.ico"; DestDir: "{app}"; DestName: "icon.ico"; Flags: ignoreversion
Source: "..\README.md"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\examples\*.algo"; DestDir: "{app}\examples"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\docs\*.md"; DestDir: "{app}\docs"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{group}\MyAlgo"; Filename: "{app}\myalgo.exe"; IconFilename: "{app}\icon.ico"
Name: "{commondesktop}\MyAlgo"; Filename: "{app}\myalgo.exe"; IconFilename: "{app}\icon.ico"

;[Registry]
;Root: HKCR; Subkey: ".algo"; ValueType: string; ValueName: ""; ValueData: "MyAlgoFile"; Flags: uninsdeletevalue
;Root: HKCR; Subkey: "MyAlgoFile"; ValueType: string; ValueName: ""; ValueData: "MyAlgo File"; Flags: uninsdeletekey
;Root: HKCR; Subkey: "MyAlgoFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\icon.ico"
;Root: HKCR; Subkey: "MyAlgoFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\myalgo.exe"" ""%1"""

[Run]
Filename: "{app}\myalgo.exe"; Description: "Launch MyAlgo"; Flags: nowait postinstall skipifsilent

[Code]
function IsPathAlreadySet(const AppDir: string): Boolean;
var
  CurrentPath: string;
begin
  if RegQueryStringValue(HKLM, 'SYSTEM\CurrentControlSet\Control\Session Manager\Environment', 'Path', CurrentPath) then
    Result := Pos(';' + Uppercase(AppDir) + ';', ';' + Uppercase(CurrentPath) + ';') > 0
  else
    Result := False;
end;

procedure AddToPath(const AppDir: string);
var
  CurrentPath: string;
begin
  if not RegQueryStringValue(HKLM, 'SYSTEM\CurrentControlSet\Control\Session Manager\Environment', 'Path', CurrentPath) then
    CurrentPath := '';

  if (CurrentPath <> '') and (Copy(CurrentPath, Length(CurrentPath), 1) <> ';') then
    CurrentPath := CurrentPath + ';';

  RegWriteExpandStringValue(
    HKLM,
    'SYSTEM\CurrentControlSet\Control\Session Manager\Environment',
    'Path',
    CurrentPath + AppDir
  );
end;

procedure CurStepChanged(CurStep: TSetupStep);
begin
  if CurStep = ssPostInstall then
  begin
    if not IsPathAlreadySet(ExpandConstant('{app}')) then
      AddToPath(ExpandConstant('{app}'));
  end;
end;