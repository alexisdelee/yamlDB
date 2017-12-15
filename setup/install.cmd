@echo off

if not exist "%SystemDrive%\yamlSGBD" mkdir "%SystemDrive%\yamlSGBD"
if not exist "%SystemDrive%\yamlSGBD\assets" mkdir "%SystemDrive%\yamlSGBD\assets"
if not exist "%HOMEPATH%\.yaml" mkdir "%HOMEPATH%\.yaml"
if not exist "%HOMEPATH%\.yaml\78de" mkdir "%HOMEPATH%\.yaml\78de"

xcopy /s "bin" "%SystemDrive%\yamlSGBD\"
copy "assets\sql.ico" "%SystemDrive%\yamlSGBD\assets"
copy "assets\yaml.conf" "%SystemDrive%\yamlSGBD"
copy "assets\ui.css" "%SystemDrive%\yamlSGBD"
xcopy /s "assets\78de" "%HOMEPATH%\.yaml\78de"

set oName="%TEMP%\%RANDOM%-%RANDOM%-%RANDOM%-%RANDOM%.vbs"
echo Set oWS = WScript.CreateObject("WScript.Shell") > %oName%
echo sLinkFile = "%USERPROFILE%\Desktop\Laucher SGBD.lnk" >> %oName%
echo Set oLink = oWS.CreateShortcut(sLinkFile) >> %oName%
echo oLink.TargetPath = "%SystemDrive%\yamlSGBD\sgbd.exe" >> %oName%
echo oLink.Description = "Launcher SGBD" >> %oName%
echo oLink.IconLocation = "%SystemDrive%\yamlSGBD\assets\sql.ico" >> %oName%
echo oLink.Save >> %oName%
cscript %oName%
del %oName%

pause