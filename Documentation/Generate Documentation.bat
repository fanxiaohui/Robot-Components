doxygen .\DoxygenConfig\Doxyfile

set CurrentPath=%~dp0
echo Set oWS = WScript.CreateObject("WScript.Shell") > CreateShortcut.vbs
echo sLinkFile = "Documentation.lnk" >> CreateShortcut.vbs
echo Set oLink = oWS.CreateShortcut(sLinkFile) >> CreateShortcut.vbs
echo oLink.TargetPath = "%CurrentPath%HTML\index.html" >> CreateShortcut.vbs
echo oLink.WorkingDirectory = "%CurrentPath%HTML" >> CreateShortcut.vbs
echo oLink.Save >> CreateShortcut.vbs
cscript CreateShortcut.vbs
del CreateShortcut.vbs