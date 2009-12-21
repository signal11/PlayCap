This is PlayCap, the tool for playing back Wireshark, tcpdump, and libpcap
captures.  In its early versions, only straight playback (full speed) is
supported.  In future versions, variable speed playback may be added if it
is desired.  PlayCap has a very simple user interface, designed to be
completely intuitive to anyone using it.

Platforms Supported
--------------------
PlayCap has currently been tested on Windows XP, Ubuntu 9.10, and Mac OS X
10.5, with more testing planned in the near future.

Installation / Building from Source
=====================================

Linux
------
On Linux, PlayCap currently must be built from source. CMake (www.cmake.org) is
used instead of autotools to configure the build.

To install the 3rd party dependencies on an Ubuntu system, GCC must first be
installed. If it has not already been installed, run:
  sudo apt-get install build-essential
Then run the following:
  sudo apt-get install libfox-dev libpcap-dev cmake

To build the software, run the following commands from inside the source
distribution:
  cmake .
  make
  sudo make install

Run the software by running the following from the command line:
  playcap

Mac OS X
---------
On Mac OSX, PlayCap is built from the command line using cmake. To
prepare a computer for build, first install XCode and Ports, then get the
dependencies from ports by typing the following:
  sudo port install fox libpcap cmake

To build the software, run the following commands from inside the source
distribution:
  cmake .
  make
  sudo make install

Run the software by running the following from the command line:
  playcap

Windows
--------
Installation: 
On Windows, the easiest method of installation is to run the installer
located on the download page on the github.com website.  The installer will
prompt to run the WinPcap installer.  WinPcap is required for PlayCap to run
on Windows.  WinPcap is also installed by Wireshark (and other programs), so
it is likely to already be installed on your computer if you use one of
these tools.

Setting up a Windows computer for build of PlayCap:
Windows builds are currently only supported using Visual Studio 2008. Other
compilers should work, but will need a re-build of the fox-toolkit library. 
Building of this library is described later in this document.  For VS 2008,
the build should work out of the box using CMake.

1. Install CMake (downloadable from www.cmake.org)
2. Download PlayCap-Externals.zip from the main PlayCap download site and
   extract it just outside of PlayCap, so that PlayCap-Externals and PlayCap
   are on the same level, as shown:

     Parent_Folder
       |
       +PlayCap
       +PlayCap-Externals

3. Run CMake, and specify PlayCap's directory in the top of the CMake GUI.
4. In CMake, press "Configure" then "Generate". This will generate a Visual
   Studio Project file.
5. Open the newly created Visual Studio project (.SLN) file in Visual Studio,
   select the build configuration (DEBUG or RELEASE) and press the Build
   button.
6. Copy the DLLs from the PlayCap-Externals directory to an appropriate folder.
7. Install WinPcap_4_1_1.exe from the Playcap-Externals folder (if
   necessary).
8. Run the newly-built playcap.exe


To Re-build FOX-Toolkit on Windows (for example on another compiler):
----------------------------------------------------------------------
1. Open the FOX Project from windows\vcpp\win32.dsw
2. In the FOX workspace, add HAVE_PNG_H=1 to the preprocessor
   in the DEBUG and RELEASE for projects fox and foxdll (4 times total).
3. In the FOX workspace, add libpng.lib and zlib.lib to the list of libraries
   linked to in the DEBUG and RELEASE configurations for the foxdll project.

4. In the Visual Studio Options window, select (from the tree
   on the left) "Projects and Solutions" then "VC++ Directories."
   Then select "Include files" from the combo box on the top right.
   Add the paths for libpng\include and zlib\include from the place
   you extracted PlayCap-Externals.zip
5. Select "Library Files" from the combo box at the top right of
   the Options window. Add the paths for libpng\lib and zlib\lib from
   the place you exracted PlayCap-Externals.zip
6. Build projects fox and foxdll depending on whether you want to use
   the static lib or the DLL.
7. Build projects fox and foxdll.
8. Get the LIB and DLL files from the lib\ directory of the fox distribution.
9. When done, take the added folders out of the global include and library
   paths if desired.
