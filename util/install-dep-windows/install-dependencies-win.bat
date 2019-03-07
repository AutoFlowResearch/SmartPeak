
@echo on

set install="C:\projects\deps"

REM xsd, xerces-c by download

echo %install%

REM Installing xsd + xerces-c
PowerShell -NoProfile -ExecutionPolicy Bypass -Command "& '.\xsd.ps1' -dir %install% -src 'http://www.codesynthesis.com/download/xsd/4.0/windows/i686/xsd-4.0.msi' -Verbose"
PowerShell -NoProfile -ExecutionPolicy Bypass -Command "& '.\seqan.ps1' -Verbose"


