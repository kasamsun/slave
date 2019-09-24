# Microsoft Developer Studio Project File - Name="Slave" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Slave - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Slave.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Slave.mak" CFG="Slave - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Slave - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Slave - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Slave - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "STRICT" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x41e /d "NDEBUG"
# ADD RSC /l 0x41e /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib winmm.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "Slave - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "STRICT" /YX /FD /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x41e /d "_DEBUG"
# ADD RSC /l 0x41e /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib winmm.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Slave - Win32 Release"
# Name "Slave - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AboutDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\BmpPreviewDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\kswafx\src\Button.cpp
# End Source File
# Begin Source File

SOURCE=.\CardWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\ChatEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\kswafx\src\Control.cpp
# End Source File
# Begin Source File

SOURCE=.\kswafx\src\CoolButton.cpp
# End Source File
# Begin Source File

SOURCE=.\kswafx\src\CoolDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\kswafx\src\CoolStatic.cpp
# End Source File
# Begin Source File

SOURCE=.\kswafx\src\DCEx.cpp
# End Source File
# Begin Source File

SOURCE=.\kswafx\src\Dialog.cpp
# End Source File
# Begin Source File

SOURCE=.\kswafx\src\Edit.cpp
# End Source File
# Begin Source File

SOURCE=.\FaceSelectDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\kswafx\src\GenericClickWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\kswafx\src\GenericWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\HelpDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\kswafx\src\IcmpPing.cpp
# End Source File
# Begin Source File

SOURCE=.\JoinDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\kswafx\src\MainAfx.cpp
# End Source File
# Begin Source File

SOURCE=.\MsgPopupWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\kswafx\src\MyThread.cpp
# End Source File
# Begin Source File

SOURCE=.\kswafx\src\MyWinsock.cpp
# End Source File
# Begin Source File

SOURCE=.\NetStatDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PickCardWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\Player.cpp
# End Source File
# Begin Source File

SOURCE=.\PlayerDetailWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\PlayerWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\kswafx\src\RegistryKey.cpp
# End Source File
# Begin Source File

SOURCE=.\ResultDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SettingDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Slave.cpp
# End Source File
# Begin Source File

SOURCE=.\Slave.rc
# End Source File
# Begin Source File

SOURCE=.\SlaveAI.cpp
# End Source File
# Begin Source File

SOURCE=.\SlaveApp.cpp
# End Source File
# Begin Source File

SOURCE=.\SlaveComm.cpp
# End Source File
# Begin Source File

SOURCE=.\SlaveWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\SlvPing.cpp
# End Source File
# Begin Source File

SOURCE=.\StartGameWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\kswafx\src\Static.cpp
# End Source File
# Begin Source File

SOURCE=.\kswafx\src\Tooltip.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AboutDlg.h
# End Source File
# Begin Source File

SOURCE=.\BmpPreviewDlg.h
# End Source File
# Begin Source File

SOURCE=.\kswafx\include\Button.h
# End Source File
# Begin Source File

SOURCE=.\CardWnd.h
# End Source File
# Begin Source File

SOURCE=.\ChatEdit.h
# End Source File
# Begin Source File

SOURCE=.\kswafx\include\CoolButton.h
# End Source File
# Begin Source File

SOURCE=.\kswafx\include\CoolDlg.h
# End Source File
# Begin Source File

SOURCE=.\kswafx\include\CoolStatic.h
# End Source File
# Begin Source File

SOURCE=.\kswafx\include\dcex.h
# End Source File
# Begin Source File

SOURCE=.\kswafx\include\Dialog.h
# End Source File
# Begin Source File

SOURCE=.\kswafx\include\Edit.h
# End Source File
# Begin Source File

SOURCE=.\FaceSelectDlg.h
# End Source File
# Begin Source File

SOURCE=.\kswafx\include\GenericClickWnd.h
# End Source File
# Begin Source File

SOURCE=.\kswafx\include\GenericWnd.h
# End Source File
# Begin Source File

SOURCE=.\HelpDlg.h
# End Source File
# Begin Source File

SOURCE=.\kswafx\include\IcmpPing.h
# End Source File
# Begin Source File

SOURCE=.\JoinDlg.h
# End Source File
# Begin Source File

SOURCE=.\kswafx\include\MainAfx.h
# End Source File
# Begin Source File

SOURCE=.\MsgPopupWnd.h
# End Source File
# Begin Source File

SOURCE=.\kswafx\include\MyThread.h
# End Source File
# Begin Source File

SOURCE=.\kswafx\include\MyWinsock.h
# End Source File
# Begin Source File

SOURCE=.\NetStatDlg.h
# End Source File
# Begin Source File

SOURCE=.\PickCardWnd.h
# End Source File
# Begin Source File

SOURCE=.\Player.h
# End Source File
# Begin Source File

SOURCE=.\PlayerDetailWnd.h
# End Source File
# Begin Source File

SOURCE=.\PlayerWnd.h
# End Source File
# Begin Source File

SOURCE=.\kswafx\include\RegistryKey.h
# End Source File
# Begin Source File

SOURCE=.\ResultDlg.h
# End Source File
# Begin Source File

SOURCE=.\SettingDlg.h
# End Source File
# Begin Source File

SOURCE=.\Slave.h
# End Source File
# Begin Source File

SOURCE=.\SlaveAI.h
# End Source File
# Begin Source File

SOURCE=.\SlaveApp.h
# End Source File
# Begin Source File

SOURCE=.\SlaveCard.h
# End Source File
# Begin Source File

SOURCE=.\SlaveComm.h
# End Source File
# Begin Source File

SOURCE=.\SlaveWnd.h
# End Source File
# Begin Source File

SOURCE=.\SlvPing.h
# End Source File
# Begin Source File

SOURCE=.\StartGameWnd.h
# End Source File
# Begin Source File

SOURCE=.\kswafx\include\Static.h
# End Source File
# Begin Source File

SOURCE=.\kswafx\include\Tooltip.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\16_card01.bmp
# End Source File
# Begin Source File

SOURCE=.\res\about.ico
# End Source File
# Begin Source File

SOURCE=.\res\Actmask.ICO
# End Source File
# Begin Source File

SOURCE=".\res\Aesop's Son.ICO"
# End Source File
# Begin Source File

SOURCE=".\res\Arab Sam.ico"
# End Source File
# Begin Source File

SOURCE=.\res\Argo.ico
# End Source File
# Begin Source File

SOURCE=.\res\arngao.ICO
# End Source File
# Begin Source File

SOURCE=.\res\arrowb.ICO
# End Source File
# Begin Source File

SOURCE=.\res\Arthur.ico
# End Source File
# Begin Source File

SOURCE=".\res\Babs Bunny.ico"
# End Source File
# Begin Source File

SOURCE=".\res\Baby Bart.ico"
# End Source File
# Begin Source File

SOURCE=.\res\Barney.ICO
# End Source File
# Begin Source File

SOURCE=".\res\Bart Picasso.ico"
# End Source File
# Begin Source File

SOURCE=".\res\Bart Polo.ico"
# End Source File
# Begin Source File

SOURCE=".\res\Bart the Mime.ICO"
# End Source File
# Begin Source File

SOURCE=".\res\Bart The Raven.ICO"
# End Source File
# Begin Source File

SOURCE=.\res\Bartina.ico
# End Source File
# Begin Source File

SOURCE=.\res\Bartman.ico
# End Source File
# Begin Source File

SOURCE=.\res\Batman1.ico
# End Source File
# Begin Source File

SOURCE=.\res\Bert.ICO
# End Source File
# Begin Source File

SOURCE=.\res\Bettle.ico
# End Source File
# Begin Source File

SOURCE=".\res\Big Bird.ICO"
# End Source File
# Begin Source File

SOURCE=".\res\Bill The Cat1.ico"
# End Source File
# Begin Source File

SOURCE=.\res\Binkley.ico
# End Source File
# Begin Source File

SOURCE=".\res\Bird's Any.ico"
# End Source File
# Begin Source File

SOURCE=.\res\bitmap2.bmp
# End Source File
# Begin Source File

SOURCE=".\res\Black Bart.ico"
# End Source File
# Begin Source File

SOURCE=.\res\bmp1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp10.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp11.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp12.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp13.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp14.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp15.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp16.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp17.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp18.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp19.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp20.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp21.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp22.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp23.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp24.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp25.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp26.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp27.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp28.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp29.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp3.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp30.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp31.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp32.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp33.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp34.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp35.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp36.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp37.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp38.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp39.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp4.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp40.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp41.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp42.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp43.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp44.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp45.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp46.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp47.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp48.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp49.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp5.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp50.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp51.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp52.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp6.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp7.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp8.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp9.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bochan.ICO
# End Source File
# Begin Source File

SOURCE=".\res\Boo Boo.ICO"
# End Source File
# Begin Source File

SOURCE=.\res\Brain.ico
# End Source File
# Begin Source File

SOURCE=.\res\Brainy.ICO
# End Source File
# Begin Source File

SOURCE=.\res\Brutus.ICO
# End Source File
# Begin Source File

SOURCE=.\res\Bullwinkle.ICO
# End Source File
# Begin Source File

SOURCE=.\res\Butch.ico
# End Source File
# Begin Source File

SOURCE=.\res\Calvin.ICO
# End Source File
# Begin Source File

SOURCE=".\res\CB Dirty Snowman.ico"
# End Source File
# Begin Source File

SOURCE=".\res\CB Freda.ico"
# End Source File
# Begin Source File

SOURCE=".\res\CB Pig Pen.ico"
# End Source File
# Begin Source File

SOURCE=".\res\CB Violet.ico"
# End Source File
# Begin Source File

SOURCE=".\res\CB WWII Ace.ico"
# End Source File
# Begin Source File

SOURCE=".\res\Charlie Brown.ICO"
# End Source File
# Begin Source File

SOURCE=.\res\Chicken.ico
# End Source File
# Begin Source File

SOURCE=.\res\Concord.ico
# End Source File
# Begin Source File

SOURCE=".\res\Cookie Monster.ICO"
# End Source File
# Begin Source File

SOURCE=".\res\Cool Sly.ico"
# End Source File
# Begin Source File

SOURCE=".\res\Corporal Punishment.ico"
# End Source File
# Begin Source File

SOURCE=".\res\Count Bartula.ICO"
# End Source File
# Begin Source File

SOURCE=.\res\cross.ico
# End Source File
# Begin Source File

SOURCE=.\res\curclick.cur
# End Source File
# Begin Source File

SOURCE=.\res\curgrasp.cur
# End Source File
# Begin Source File

SOURCE=.\res\curpoint.cur
# End Source File
# Begin Source File

SOURCE=.\res\Cyberdog.ico
# End Source File
# Begin Source File

SOURCE=.\res\Cyote.ico
# End Source File
# Begin Source File

SOURCE=".\res\Dacin' Homer.ICO"
# End Source File
# Begin Source File

SOURCE=".\res\Daffy Duck.ICO"
# End Source File
# Begin Source File

SOURCE=".\res\Dilbert Boss.ICO"
# End Source File
# Begin Source File

SOURCE=.\res\Dilbert.ICO
# End Source File
# Begin Source File

SOURCE=.\res\Dino.ICO
# End Source File
# Begin Source File

SOURCE=.\res\dirnorm.ico
# End Source File
# Begin Source File

SOURCE=.\res\dirrev.ICO
# End Source File
# Begin Source File

SOURCE=".\res\DM Colonel K.ico"
# End Source File
# Begin Source File

SOURCE=".\res\DM Crumbs.ico"
# End Source File
# Begin Source File

SOURCE=".\res\DM Custard.ico"
# End Source File
# Begin Source File

SOURCE=".\res\DM Danger Mouse.ico"
# End Source File
# Begin Source File

SOURCE=".\res\DM Nero.ico"
# End Source File
# Begin Source File

SOURCE=".\res\DM Penfold.ico"
# End Source File
# Begin Source File

SOURCE=".\res\Donald 1.ICO"
# End Source File
# Begin Source File

SOURCE=".\res\Doogie Bart.ico"
# End Source File
# Begin Source File

SOURCE=".\res\Dot 2.ICO"
# End Source File
# Begin Source File

SOURCE=".\res\Dr. Scratchy.ico"
# End Source File
# Begin Source File

SOURCE=.\res\Dragon.ico
# End Source File
# Begin Source File

SOURCE=".\res\Drowned Cat.ico"
# End Source File
# Begin Source File

SOURCE=.\res\dsdisc.ico
# End Source File
# Begin Source File

SOURCE=.\res\dsdisc2.ico
# End Source File
# Begin Source File

SOURCE=.\res\dsdisc3.ico
# End Source File
# Begin Source File

SOURCE=.\res\dslast.ico
# End Source File
# Begin Source File

SOURCE=.\res\dspass.ico
# End Source File
# Begin Source File

SOURCE=.\res\dswait.ico
# End Source File
# Begin Source File

SOURCE=".\res\Duck Dodgers.ico"
# End Source File
# Begin Source File

SOURCE=.\res\dukdik.ICO
# End Source File
# Begin Source File

SOURCE=.\res\Eeyore.ICO
# End Source File
# Begin Source File

SOURCE=".\res\Elmer Fudd.ico"
# End Source File
# Begin Source File

SOURCE=.\res\Elmyra.ico
# End Source File
# Begin Source File

SOURCE=.\res\Ernie.ICO
# End Source File
# Begin Source File

SOURCE=".\res\Evil Calvin.ico"
# End Source File
# Begin Source File

SOURCE=.\res\exit.ico
# End Source File
# Begin Source File

SOURCE=".\res\Fallout Boy.ico"
# End Source File
# Begin Source File

SOURCE=.\res\Felix.ICO
# End Source File
# Begin Source File

SOURCE=.\res\Fifi.ico
# End Source File
# Begin Source File

SOURCE=.\res\Foghorn.ICO
# End Source File
# Begin Source File

SOURCE=.\res\Fred.ICO
# End Source File
# Begin Source File

SOURCE=.\res\Freda.ICO
# End Source File
# Begin Source File

SOURCE=".\res\FS Duck.ICO"
# End Source File
# Begin Source File

SOURCE=".\res\FS Eugene.ICO"
# End Source File
# Begin Source File

SOURCE=".\res\FS2 Clown.ICO"
# End Source File
# Begin Source File

SOURCE=".\res\FS2 Cow.ICO"
# End Source File
# Begin Source File

SOURCE=".\res\FS2 God.ICO"
# End Source File
# Begin Source File

SOURCE=".\res\FS2 Goofy Pooch.ICO"
# End Source File
# Begin Source File

SOURCE=".\res\FS2 Penguin2.ICO"
# End Source File
# Begin Source File

SOURCE=".\res\FS2 Snake.ICO"
# End Source File
# Begin Source File

SOURCE=".\res\Garfield 2.ICO"
# End Source File
# Begin Source File

SOURCE=.\res\Gargamel.ICO
# End Source File
# Begin Source File

SOURCE=.\res\Geek.ico
# End Source File
# Begin Source File

SOURCE=.\res\General.ico
# End Source File
# Begin Source File

SOURCE=.\res\Gidney.ico
# End Source File
# Begin Source File

SOURCE=".\res\Goofy 1.ICO"
# End Source File
# Begin Source File

SOURCE=.\res\Gromit2.ICO
# End Source File
# Begin Source File

SOURCE=.\res\gundam.ICO
# End Source File
# Begin Source File

SOURCE=".\res\Hagar the Horrible.ICO"
# End Source File
# Begin Source File

SOURCE=.\res\Hampton.ICO
# End Source File
# Begin Source File

SOURCE=".\res\Happy Bart.ICO"
# End Source File
# Begin Source File

SOURCE=".\res\Happy Little Elf.ico"
# End Source File
# Begin Source File

SOURCE=.\res\headcom1.ICO
# End Source File
# Begin Source File

SOURCE=.\res\headcom2.ICO
# End Source File
# Begin Source File

SOURCE=.\res\headcom3.ICO
# End Source File
# Begin Source File

SOURCE=.\res\headcom4.ICO
# End Source File
# Begin Source File

SOURCE=.\res\headcom5.ICO
# End Source File
# Begin Source File

SOURCE=.\res\headcom6.ICO
# End Source File
# Begin Source File

SOURCE=.\res\hellokitty.ICO
# End Source File
# Begin Source File

SOURCE=.\res\help.ico
# End Source File
# Begin Source File

SOURCE=.\res\Hiroshi.ICO
# End Source File
# Begin Source File

SOURCE=.\res\Hobbes.ICO
# End Source File
# Begin Source File

SOURCE=.\res\Hodgepodge.ico
# End Source File
# Begin Source File

SOURCE=.\res\Huckleberry.ICO
# End Source File
# Begin Source File

SOURCE=.\res\ico00001.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00002.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00003.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00004.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00005.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00006.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00007.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon2.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon3.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon4.ico
# End Source File
# Begin Source File

SOURCE=.\res\isking.ico
# End Source File
# Begin Source File

SOURCE=.\res\ispeople.ico
# End Source File
# Begin Source File

SOURCE=.\res\isqueen.ico
# End Source File
# Begin Source File

SOURCE=.\res\isslave.ico
# End Source File
# Begin Source File

SOURCE=.\res\isvslave.ico
# End Source File
# Begin Source File

SOURCE=.\res\iwamari.ico
# End Source File
# Begin Source File

SOURCE=.\res\Jerry.ICO
# End Source File
# Begin Source File

SOURCE=.\res\Jimmy.ico
# End Source File
# Begin Source File

SOURCE=.\res\join.ico
# End Source File
# Begin Source File

SOURCE=.\res\Jon.ICO
# End Source File
# Begin Source File

SOURCE=".\res\Judge Bart.ICO"
# End Source File
# Begin Source File

SOURCE=".\res\Junior Camper Bart.ico"
# End Source File
# Begin Source File

SOURCE=.\res\Junior.ico
# End Source File
# Begin Source File

SOURCE=.\res\Kazama.ICO
# End Source File
# Begin Source File

SOURCE=.\res\Kermit.ICO
# End Source File
# Begin Source File

SOURCE=".\res\KR Kaman Rider 1 (old).ICO"
# End Source File
# Begin Source File

SOURCE=".\res\KR Kaman Rider 1.ICO"
# End Source File
# Begin Source File

SOURCE=".\res\KR Kaman Rider 2.ICO"
# End Source File
# Begin Source File

SOURCE=".\res\KR Riderman 4.ICO"
# End Source File
# Begin Source File

SOURCE=".\res\KR-Amazon 6.ICO"
# End Source File
# Begin Source File

SOURCE=".\res\KR-Black 11.ICO"
# End Source File
# Begin Source File

SOURCE=".\res\KR-Black RX 12.ICO"
# End Source File
# Begin Source File

SOURCE=".\res\Kr-j 15.ico"
# End Source File
# Begin Source File

SOURCE=".\res\KR-Shinn 13.ICO"
# End Source File
# Begin Source File

SOURCE=".\res\KR-Sky 8.ICO"
# End Source File
# Begin Source File

SOURCE=".\res\KR-Strong 7.ICO"
# End Source File
# Begin Source File

SOURCE=".\res\KR-Super 9.ICO"
# End Source File
# Begin Source File

SOURCE=".\res\Kr-v3.ico"
# End Source File
# Begin Source File

SOURCE=".\res\Kr-x 5.ico"
# End Source File
# Begin Source File

SOURCE=".\res\Kr-zo 14.ico"
# End Source File
# Begin Source File

SOURCE=".\res\Kr-zx 10.ico"
# End Source File
# Begin Source File

SOURCE=.\res\Linus.ICO
# End Source File
# Begin Source File

SOURCE=.\res\Lisa.ICO
# End Source File
# Begin Source File

SOURCE=.\res\logo.bmp
# End Source File
# Begin Source File

SOURCE=".\res\Lucy 1.ICO"
# End Source File
# Begin Source File

SOURCE=.\res\Marvin.ICO
# End Source File
# Begin Source File

SOURCE=.\res\masao.ico
# End Source File
# Begin Source File

SOURCE=.\res\matzusaka.ICO
# End Source File
# Begin Source File

SOURCE=.\res\McBain.ico
# End Source File
# Begin Source File

SOURCE=".\res\Michael Jackson Bart.ico"
# End Source File
# Begin Source File

SOURCE=.\res\Milo.ico
# End Source File
# Begin Source File

SOURCE=.\res\misae.ICO
# End Source File
# Begin Source File

SOURCE=".\res\Mr Peabody.ICO"
# End Source File
# Begin Source File

SOURCE=".\res\Mr. Skullhead.ico"
# End Source File
# Begin Source File

SOURCE=".\res\MS-06F.ico"
# End Source File
# Begin Source File

SOURCE=".\res\MS-06S.ico"
# End Source File
# Begin Source File

SOURCE=".\res\MS-14F.ico"
# End Source File
# Begin Source File

SOURCE=".\res\MS-14FS.ico"
# End Source File
# Begin Source File

SOURCE=".\res\MS-21C.ico"
# End Source File
# Begin Source File

SOURCE=.\res\msgactio.ico
# End Source File
# Begin Source File

SOURCE=.\res\msgsyste.ico
# End Source File
# Begin Source File

SOURCE=.\res\nanako.ICO
# End Source File
# Begin Source File

SOURCE=.\res\nenechan.ico
# End Source File
# Begin Source File

SOURCE=".\res\Ninja Bart.ico"
# End Source File
# Begin Source File

SOURCE=.\res\nobody.ico
# End Source File
# Begin Source File

SOURCE=.\res\noofbkng.ico
# End Source File
# Begin Source File

SOURCE=.\res\noofbqun.ico
# End Source File
# Begin Source File

SOURCE=.\res\noofking.ico
# End Source File
# Begin Source File

SOURCE=.\res\noofslav.ico
# End Source File
# Begin Source File

SOURCE=.\res\Odie.ICO
# End Source File
# Begin Source File

SOURCE=".\res\Olive Oyl.ICO"
# End Source File
# Begin Source File

SOURCE=.\res\Oliver.ico
# End Source File
# Begin Source File

SOURCE=".\res\Opus II.ICO"
# End Source File
# Begin Source File

SOURCE=.\res\Paige.ICO
# End Source File
# Begin Source File

SOURCE=".\res\Papa Smurf.ICO"
# End Source File
# Begin Source File

SOURCE=.\res\Parasite.ico
# End Source File
# Begin Source File

SOURCE=.\res\pd_wood.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Pebbles.ICO
# End Source File
# Begin Source File

SOURCE=".\res\Pepe le Pu.ICO"
# End Source File
# Begin Source File

SOURCE=.\res\Piglet.ICO
# End Source File
# Begin Source File

SOURCE=".\res\Pink Panther.ICO"
# End Source File
# Begin Source File

SOURCE=.\res\Pinky.ICO
# End Source File
# Begin Source File

SOURCE=".\res\Plucky Duck.ico"
# End Source File
# Begin Source File

SOURCE=.\res\Pooh.ICO
# End Source File
# Begin Source File

SOURCE=".\res\Popeye Jeep.ico"
# End Source File
# Begin Source File

SOURCE=.\res\Popeye.ICO
# End Source File
# Begin Source File

SOURCE=.\res\Portnoy.ico
# End Source File
# Begin Source File

SOURCE=.\res\pos1.ICO
# End Source File
# Begin Source File

SOURCE=.\res\pos2.ICO
# End Source File
# Begin Source File

SOURCE=.\res\pos3.ICO
# End Source File
# Begin Source File

SOURCE=.\res\pos4.ICO
# End Source File
# Begin Source File

SOURCE=.\res\pos5.ICO
# End Source File
# Begin Source File

SOURCE=.\res\pos6.ICO
# End Source File
# Begin Source File

SOURCE=.\res\Preston.ico
# End Source File
# Begin Source File

SOURCE=".\res\Private Buggs.ICO"
# End Source File
# Begin Source File

SOURCE=".\res\Psychiatrist Bart.ico"
# End Source File
# Begin Source File

SOURCE=.\res\quit.ico
# End Source File
# Begin Source File

SOURCE=.\res\rabbit.ICO
# End Source File
# Begin Source File

SOURCE=".\res\Radioactive Man.ico"
# End Source File
# Begin Source File

SOURCE=".\res\RGM-79C.ico"
# End Source File
# Begin Source File

SOURCE=.\res\RoadRunner.ICO
# End Source File
# Begin Source File

SOURCE=".\res\Rocko 2.ICO"
# End Source File
# Begin Source File

SOURCE=.\res\Roger.ICO
# End Source File
# Begin Source File

SOURCE=".\res\RX-78-2.ico"
# End Source File
# Begin Source File

SOURCE=".\res\RX-78GP-01.ico"
# End Source File
# Begin Source File

SOURCE=".\res\RX-78GP-02A.ico"
# End Source File
# Begin Source File

SOURCE=".\res\RX-78GP-03S.ico"
# End Source File
# Begin Source File

SOURCE=.\res\Sam.ico
# End Source File
# Begin Source File

SOURCE=".\res\Santa's Little Helper.ICO"
# End Source File
# Begin Source File

SOURCE=.\res\Schnauzi.ico
# End Source File
# Begin Source File

SOURCE=.\res\Schroder.ICO
# End Source File
# Begin Source File

SOURCE=.\res\Scooby.ICO
# End Source File
# Begin Source File

SOURCE=.\res\setting.ico
# End Source File
# Begin Source File

SOURCE=".\res\Sgt Snorkle.ico"
# End Source File
# Begin Source File

SOURCE=.\res\Shaggy.ICO
# End Source File
# Begin Source File

SOURCE=.\res\Sheep.ico
# End Source File
# Begin Source File

SOURCE=.\res\Sherman.ICO
# End Source File
# Begin Source File

SOURCE=.\res\Shermie.ICO
# End Source File
# Begin Source File

SOURCE=.\res\Shinchan.ICO
# End Source File
# Begin Source File

SOURCE=.\res\shinobu.ICO
# End Source File
# Begin Source File

SOURCE=".\res\Shoe Elf.ico"
# End Source File
# Begin Source File

SOURCE=".\res\SHR Bone Head.ico"
# End Source File
# Begin Source File

SOURCE=".\res\SHR Darn.ico"
# End Source File
# Begin Source File

SOURCE=".\res\SHR Dem Bones.ico"
# End Source File
# Begin Source File

SOURCE=".\res\SHR Four Times One.ico"
# End Source File
# Begin Source File

SOURCE=".\res\SHR Snowman.ico"
# End Source File
# Begin Source File

SOURCE=".\res\SHR Three Girl.ico"
# End Source File
# Begin Source File

SOURCE=".\res\SHR unpack.ico"
# End Source File
# Begin Source File

SOURCE=".\res\SHR Wow.ico"
# End Source File
# Begin Source File

SOURCE=".\res\Sideshow Rahim.ico"
# End Source File
# Begin Source File

SOURCE=.\res\slave.ico
# End Source File
# Begin Source File

SOURCE=.\res\smcard.ico
# End Source File
# Begin Source File

SOURCE=.\res\Smurfette.ICO
# End Source File
# Begin Source File

SOURCE=.\res\Snoopy.ICO
# End Source File
# Begin Source File

SOURCE=".\res\Space Foreman.ico"
# End Source File
# Begin Source File

SOURCE=".\res\Speedy G.ico"
# End Source File
# Begin Source File

SOURCE=.\res\start.ico
# End Source File
# Begin Source File

SOURCE=.\res\Steve.ico
# End Source File
# Begin Source File

SOURCE=.\res\Stimpy.ICO
# End Source File
# Begin Source File

SOURCE=.\res\Stinkbomb.ico
# End Source File
# Begin Source File

SOURCE=.\res\Stork.ico
# End Source File
# Begin Source File

SOURCE=.\res\svblogo.bmp
# End Source File
# Begin Source File

SOURCE=.\res\svcard.bmp
# End Source File
# Begin Source File

SOURCE=.\res\svconne1.ico
# End Source File
# Begin Source File

SOURCE=.\res\svconnec.ico
# End Source File
# Begin Source File

SOURCE=.\res\svofflin.ico
# End Source File
# Begin Source File

SOURCE=.\res\svonline.ico
# End Source File
# Begin Source File

SOURCE=".\res\Swee' Pea.ICO"
# End Source File
# Begin Source File

SOURCE=.\res\Sylvester.ICO
# End Source File
# Begin Source File

SOURCE=".\res\Terminator Bart.ico"
# End Source File
# Begin Source File

SOURCE=".\res\The Count.ICO"
# End Source File
# Begin Source File

SOURCE=".\res\The Malcontent.ico"
# End Source File
# Begin Source File

SOURCE=".\res\The Snitch.ico"
# End Source File
# Begin Source File

SOURCE=.\res\Tigger.ICO
# End Source File
# Begin Source File

SOURCE=.\res\Tom.ICO
# End Source File
# Begin Source File

SOURCE=.\res\Tweety.ICO
# End Source File
# Begin Source File

SOURCE=".\res\Ultraman Ace.ICO"
# End Source File
# Begin Source File

SOURCE=".\res\Ultraman Astra.ICO"
# End Source File
# Begin Source File

SOURCE=".\res\Ultraman Father.ICO"
# End Source File
# Begin Source File

SOURCE=".\res\Ultraman Great01.ICO"
# End Source File
# Begin Source File

SOURCE=".\res\Ultraman Jack.ICO"
# End Source File
# Begin Source File

SOURCE=".\res\Ultraman King.ICO"
# End Source File
# Begin Source File

SOURCE=".\res\Ultraman Leo.ICO"
# End Source File
# Begin Source File

SOURCE=".\res\Ultraman mother.ICO"
# End Source File
# Begin Source File

SOURCE=".\res\Ultraman Neos.ICO"
# End Source File
# Begin Source File

SOURCE=".\res\Ultraman Power.ICO"
# End Source File
# Begin Source File

SOURCE=".\res\Ultraman Taro.ICO"
# End Source File
# Begin Source File

SOURCE=".\res\Ultraman Tiga.ICO"
# End Source File
# Begin Source File

SOURCE=".\res\Ultraman Ultraman80.ICO"
# End Source File
# Begin Source File

SOURCE=".\res\Ultraman Ultraseven.ICO"
# End Source File
# Begin Source File

SOURCE=".\res\Ultraman Ultraseven21.ICO"
# End Source File
# Begin Source File

SOURCE=".\res\Ultraman Unknown.ICO"
# End Source File
# Begin Source File

SOURCE=".\res\Ultraman Zearth.ICO"
# End Source File
# Begin Source File

SOURCE=".\res\Ultraman Zoffy.ICO"
# End Source File
# Begin Source File

SOURCE=.\res\Violet.ICO
# End Source File
# Begin Source File

SOURCE=.\res\Wallace.ico
# End Source File
# Begin Source File

SOURCE=.\res\Wendolene.ico
# End Source File
# Begin Source File

SOURCE=.\res\white.ICO
# End Source File
# Begin Source File

SOURCE=.\res\Wile.ICO
# End Source File
# Begin Source File

SOURCE=.\res\Wilma.ICO
# End Source File
# Begin Source File

SOURCE=.\res\Wimpy.ICO
# End Source File
# Begin Source File

SOURCE=".\res\Winterized Bart.ico"
# End Source File
# Begin Source File

SOURCE=.\res\Woodstock.ICO
# End Source File
# Begin Source File

SOURCE=.\res\Woody.ICO
# End Source File
# Begin Source File

SOURCE=.\res\yagusa.ICO
# End Source File
# Begin Source File

SOURCE=".\res\Yakko 1.ICO"
# End Source File
# Begin Source File

SOURCE=.\res\Yogi.ICO
# End Source File
# Begin Source File

SOURCE=.\res\yoshie.ICO
# End Source File
# Begin Source File

SOURCE=.\res\yoshinga.ICO
# End Source File
# Begin Source File

SOURCE=.\res\Zero.ico
# End Source File
# Begin Source File

SOURCE=.\res\Ziggy.ICO
# End Source File
# End Group
# Begin Source File

SOURCE=.\res\boing.wav
# End Source File
# Begin Source File

SOURCE=.\res\Clapping.wav
# End Source File
# Begin Source File

SOURCE=.\res\discard1.wav
# End Source File
# Begin Source File

SOURCE=.\res\discard2.wav
# End Source File
# Begin Source File

SOURCE=.\res\ohho.wav
# End Source File
# Begin Source File

SOURCE=.\res\shuffle.wav
# End Source File
# Begin Source File

SOURCE=.\res\Swooz.wav
# End Source File
# End Target
# End Project
