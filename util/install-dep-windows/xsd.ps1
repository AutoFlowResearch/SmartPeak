param(
[string]$dir,
[System.Uri]$src
)

$install_path = $dir+"\"
$xsd_msi = $install_path+'xsd-4.0.msi'
$xsd_dl = $src.AbsoluteUri

Write-Host $xsd_dl
Write-Host $xsd_msi

if((Test-Path $xsd_msi) -eq $false) {
    Write-Host "Downloading xsd package"
    Write-Host $xsd_dl
    Write-Host $xsd_msi
    (new-object System.Net.WebClient).DownloadFile($xsd_dl,$xsd_msi)
}

$xsd_i = $install_path+"xsd"
if((Test-Path $xsd_i) -eq $false) {
  # Install xsd+xerces-c package
  Write-Host "Installing $xsd_msi"
  msiexec /passive BASEDIR=$xsd_i /i $xsd_msi /lp xsd.log
  Write-Host "Done."
}