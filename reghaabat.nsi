;Include Modern UI
	!include "MUI2.nsh"

;General
	;Name and file
	Name "رقابت"
	OutFile "Reghaabat-0.9.1.exe"

	;Default installation folder
	InstallDir "$PROGRAMFILES\Reghaabat"

	;Get installation folder from registry if available
	InstallDirRegKey HKCU "Software\Reghaabat" ""

	SetCompressor /SOLID lzma
	BrandingText " "
	
	;Interface Settings
	!define MUI_ABORTWARNING
	!define MUI_ICON "Reghaabat.ico" 
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
	File /r "Files\data\*.*"

	;ShortCuts
	SetOutPath $INSTDIR
	CreateShortCut "$SMPROGRAMS\Programs\Reghaabat\Reghaabat.lnk" "$INSTDIR\reghaabat.exe"
	CreateShortCut "$DESKTOP\Reghaabat.lnk" "$INSTDIR\reghaabat.exe"

	;Programs
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Reghaabat" "DisplayName" "Reghaabat"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Reghaabat" "UninstallString" "$\"$INSTDIR\Uninstall.exe$\""
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Reghaabat" "QuietUninstallString" "$\"$INSTDIR\Uninstall.exe$\" /S"
	
	;Store installation folder
	WriteRegStr HKCU "Software\Reghaabat" "" $INSTDIR

	;Create uninstaller
	WriteUninstaller "$INSTDIR\Uninstall.exe"
SectionEnd

;Uninstaller Section
Section "Uninstall"
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Reghaabat"
	DeleteRegKey /ifempty HKCU "Software\Reghaabat"
	Delete "$SMPROGRAMS\Programs\Reghaabat\Reghaabat.lnk"
	Delete "$DESKTOP\Reghaabat.lnk"
	
	Delete "$INSTDIR\*.*"
	RMDir /r "$INSTDIR\jwysiwyg\*.*"
SectionEnd
