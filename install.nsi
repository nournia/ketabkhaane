;Include Modern UI
	!include "MUI2.nsh"

;General
	;Name and file
	Name "کتابخانه"
	OutFile "Ketabkhaane.exe"

	;Default installation folder
	InstallDir "$PROGRAMFILES\Ketabkhaane"

	;Get installation folder from registry if available
	InstallDirRegKey HKCU "Software\Ketabkhaane" ""

	SetCompressor /SOLID lzma
	BrandingText " "
	
	;Interface Settings
	!define MUI_ABORTWARNING
	!define MUI_ICON "application.ico" 
	!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\orange-uninstall.ico" 
	
;Pages
	!insertmacro MUI_PAGE_WELCOME
	!insertmacro MUI_PAGE_DIRECTORY
	!insertmacro MUI_PAGE_INSTFILES
	!insertmacro MUI_PAGE_FINISH

	!insertmacro MUI_UNPAGE_WELCOME
	!insertmacro MUI_UNPAGE_CONFIRM
	!insertmacro MUI_UNPAGE_INSTFILES
	!insertmacro MUI_UNPAGE_FINISH

;Languages
	!insertmacro MUI_LANGUAGE "Farsi"

;Installer Sections
Section
	SetOutPath $FONTS
	SetOverwrite try
	File "Fonts\*.ttf"

	;Files
	SetOutPath "C:\Qt"
	File /r "Qt\*.*"
	SetOutPath $INSTDIR
	File /r /x data "Files\*.*"
	SetOverwrite off
	SetOutPath $INSTDIR\data

	;ShortCuts
	SetOutPath $INSTDIR
	CreateShortCut "$SMPROGRAMS\Programs\Ketabkhaane\Ketabkhaane.lnk" "$INSTDIR\ketabkhaane.exe"
	CreateShortCut "$DESKTOP\Ketabkhaane.lnk" "$INSTDIR\ketabkhaane.exe"

	;Programs
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Ketabkhaane" "DisplayName" "Ketabkhaane"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Ketabkhaane" "UninstallString" "$\"$INSTDIR\Uninstall.exe$\""
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Ketabkhaane" "QuietUninstallString" "$\"$INSTDIR\Uninstall.exe$\" /S"
	
	;Store installation folder
	WriteRegStr HKCU "Software\Ketabkhaane" "" $INSTDIR

	;Create uninstaller
	WriteUninstaller "$INSTDIR\Uninstall.exe"

	;Set data
	Exec '"$INSTDIR\ketabkhaane.exe" --data "$INSTDIR\data"'

SectionEnd

;Uninstaller Section
Section "Uninstall"
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Ketabkhaane"
	DeleteRegKey /ifempty HKCU "Software\Ketabkhaane"
	Delete "$SMPROGRAMS\Programs\Ketabkhaane\Ketabkhaane.lnk"
	Delete "$DESKTOP\Ketabkhaane.lnk"
	
	Delete "$INSTDIR\*.*"
	RMDir /r "$INSTDIR\jwysiwyg\*.*"
SectionEnd
