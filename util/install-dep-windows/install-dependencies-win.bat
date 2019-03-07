
@echo on

set install="C:\projects\OpenMS-build"

REM xsd, xerces-c by download

echo %install%

REM Installing xsd + xerces-c
PowerShell -NoProfile -ExecutionPolicy Bypass -Command "& '.\xsd.ps1' -dir 'C:\projects\OpenMS-build\xsd-4.0.msi' -src 'http://www.codesynthesis.com/download/xsd/4.0/windows/i686/xsd-4.0.msi' -Verbose"


