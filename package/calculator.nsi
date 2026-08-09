!include "MUI2.nsh"
!include "FileFunc.nsh"

Name "Calculator"
OutFile "Calculator-1.01-win64-installer.exe"
InstallDir "$PROGRAMFILES64\Calculator"
RequestExecutionLevel admin

!define PRODUCT_NAME "Calculator"
!define PRODUCT_VERSION "1.01"
!define PRODUCT_PUBLISHER "Primo"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_LANGUAGE "English"

Section "Install" SecMain
    SetOutPath "$INSTDIR"

    File "..\build-release\Calculator.exe"
    File "C:\msys64\ucrt64\bin\Qt6Core.dll"
    File "C:\msys64\ucrt64\bin\Qt6Gui.dll"
    File "C:\msys64\ucrt64\bin\Qt6Widgets.dll"
    File "C:\msys64\ucrt64\bin\libgcc_s_seh-1.dll"
    File "C:\msys64\ucrt64\bin\libstdc++-6.dll"
    File "C:\msys64\ucrt64\bin\libwinpthread-1.dll"
    File "C:\msys64\ucrt64\bin\libfreetype-6.dll"
    File "C:\msys64\ucrt64\bin\libharfbuzz-0.dll"
    File "C:\msys64\ucrt64\bin\libmd4c.dll"
    File "C:\msys64\ucrt64\bin\libpng16-16.dll"
    File "C:\msys64\ucrt64\bin\zlib1.dll"

    SetOutPath "$INSTDIR\platforms"
    File "C:\msys64\ucrt64\share\qt6\plugins\platforms\qwindows.dll"

    SetOutPath "$INSTDIR\imageformats"
    File "C:\msys64\ucrt64\share\qt6\plugins\imageformats\qjpeg.dll"
    File "C:\msys64\ucrt64\share\qt6\plugins\imageformats\qgif.dll"
    File "C:\msys64\ucrt64\share\qt6\plugins\imageformats\qico.dll"

    SetOutPath "$INSTDIR\styles"
    File "C:\msys64\ucrt64\share\qt6\plugins\styles\qmodernwindowsstyle.dll"

    SetOutPath "$INSTDIR"

    CreateDirectory "$SMPROGRAMS\Calculator"
    CreateShortCut "$SMPROGRAMS\Calculator\Calculator.lnk" "$INSTDIR\Calculator.exe"
    CreateShortCut "$DESKTOP\Calculator.lnk" "$INSTDIR\Calculator.exe"

    WriteUninstaller "$INSTDIR\Uninstall.exe"

    WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "DisplayName" "${PRODUCT_NAME}"
    WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
    WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
    WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\Uninstall.exe"
    WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "InstallLocation" "$INSTDIR"
    WriteRegDWORD HKLM "${PRODUCT_UNINST_KEY}" "NoModify" 1
    WriteRegDWORD HKLM "${PRODUCT_UNINST_KEY}" "NoRepair" 1
    ${GetSize} "$INSTDIR" "/S=0K" $0 $1 $2
    WriteRegDWORD HKLM "${PRODUCT_UNINST_KEY}" "EstimatedSize" $0
SectionEnd

Section "Uninstall"
    Delete "$INSTDIR\Calculator.exe"
    Delete "$INSTDIR\Qt6Core.dll"
    Delete "$INSTDIR\Qt6Gui.dll"
    Delete "$INSTDIR\Qt6Widgets.dll"
    Delete "$INSTDIR\libgcc_s_seh-1.dll"
    Delete "$INSTDIR\libstdc++-6.dll"
    Delete "$INSTDIR\libwinpthread-1.dll"
    Delete "$INSTDIR\libfreetype-6.dll"
    Delete "$INSTDIR\libharfbuzz-0.dll"
    Delete "$INSTDIR\libmd4c.dll"
    Delete "$INSTDIR\libpng16-16.dll"
    Delete "$INSTDIR\zlib1.dll"
    Delete "$INSTDIR\platforms\qwindows.dll"
    Delete "$INSTDIR\imageformats\qjpeg.dll"
    Delete "$INSTDIR\imageformats\qgif.dll"
    Delete "$INSTDIR\imageformats\qico.dll"
    Delete "$INSTDIR\styles\qmodernwindowsstyle.dll"
    Delete "$INSTDIR\Uninstall.exe"

    RMDir "$INSTDIR\platforms"
    RMDir "$INSTDIR\imageformats"
    RMDir "$INSTDIR\styles"
    RMDir "$INSTDIR"

    Delete "$SMPROGRAMS\Calculator\Calculator.lnk"
    RMDir "$SMPROGRAMS\Calculator"
    Delete "$DESKTOP\Calculator.lnk"

    DeleteRegKey HKLM "${PRODUCT_UNINST_KEY}"
SectionEnd
