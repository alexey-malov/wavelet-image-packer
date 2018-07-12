# Microsoft Developer Studio Project File - Name="bwlpacker_static" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=bwlpacker_static - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "bwlpacker_static.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "bwlpacker_static.mak" CFG="bwlpacker_static - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "bwlpacker_static - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "bwlpacker_static - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "bwlpacker_static - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../brainwavelets/include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"lib\bwlpacker_static.lib"

!ELSEIF  "$(CFG)" == "bwlpacker_static - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../brainwavelets/include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"lib\bwlpacker_static_debug.lib"

!ENDIF 

# Begin Target

# Name "bwlpacker_static - Win32 Release"
# Name "bwlpacker_static - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\src\ArbitraryWaveletPacker.cpp
# End Source File
# Begin Source File

SOURCE=.\src\BWLPicturePacker.cpp
# End Source File
# Begin Source File

SOURCE=.\src\BWLWaveletPacker.cpp
# End Source File
# Begin Source File

SOURCE=.\src\IArbitraryWaveletPacker.cpp
# End Source File
# Begin Source File

SOURCE=.\src\IPicturePacker.cpp
# End Source File
# Begin Source File

SOURCE=.\src\IRectangularWaveletPacker.cpp
# End Source File
# Begin Source File

SOURCE=.\src\IWaveletPacker.cpp
# End Source File
# Begin Source File

SOURCE=.\src\PackBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RectangularWaveletPacker.cpp
# End Source File
# Begin Source File

SOURCE=.\src\utils.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\src\ArbitraryWaveletPacker.h
# End Source File
# Begin Source File

SOURCE=.\src\bwl_includes.h
# End Source File
# Begin Source File

SOURCE=.\include\bwlpacker.h
# End Source File
# Begin Source File

SOURCE=.\src\BWLPicturePacker.h
# End Source File
# Begin Source File

SOURCE=.\src\BWLWaveletPacker.h
# End Source File
# Begin Source File

SOURCE=.\include\IArbitraryWaveletPacker.h
# End Source File
# Begin Source File

SOURCE=.\include\IPicturePacker.h
# End Source File
# Begin Source File

SOURCE=.\include\IRectangularWaveletPacker.h
# End Source File
# Begin Source File

SOURCE=.\include\IWaveletPacker.h
# End Source File
# Begin Source File

SOURCE=.\src\PackBuffer.h
# End Source File
# Begin Source File

SOURCE=.\src\RectangularWaveletPacker.h
# End Source File
# Begin Source File

SOURCE=.\src\utils.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Readme.txt
# End Source File
# End Target
# End Project
