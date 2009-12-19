#############################################################################
#
#    Copyright 2009 Alan Ott, Signal 11 Software
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#############################################################################

cmake_minimum_required(VERSION 2.4)
if(COMMAND cmake_policy)
   cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

project(PlayCapProject)

#### Find Fox-Toolkit ####

IF(WIN32)

   # FOX-Toolkit
   SET(FOX_INCLUDE "${CMAKE_SOURCE_DIR}/../PlayCap-Externals/fox/include" CACHE PATH "Path to FOX Include Path")
   SET(FOX_LIBRARY "${CMAKE_SOURCE_DIR}/../PlayCap-Externals/fox/lib/fox-1.6.lib" CACHE FILEPATH "FOX Library")

   # PThreads-Win32
   SET(PTHREADS_WIN32_INCLUDE "${CMAKE_SOURCE_DIR}/../PlayCap-Externals/pthreads-win32/include" CACHE PATH "Path to pthread-win32 includes" )
   SET(PTHREADS_WIN32_LIBRARY "${CMAKE_SOURCE_DIR}/../PlayCap-Externals/pthreads-win32/lib/pthreadVC2.lib" CACHE FILEPATH "Pthread-Win32 Library")

   # WinPcap
   SET(WINPCAP_INCLUDE "${CMAKE_SOURCE_DIR}/../PlayCap-Externals/WpdPack/Include" CACHE PATH "Path to WinPcap includes" )
   SET(WINPCAP_LIBRARY "${CMAKE_SOURCE_DIR}/../PlayCap-Externals/WpdPack/Lib/wpcap.lib" CACHE FILEPATH "WinPcap Library")

   # LibPNG
   SET(LIBPNG_INCLUDE "${CMAKE_SOURCE_DIR}/../PlayCap-Externals/libpng/include" CACHE PATH "Path to libPNG includes" )
   SET(LIBPNG_LIBRARY "${CMAKE_SOURCE_DIR}/../PlayCap-Externals/libpng/lib/libpng.lib" CACHE FILEPATH "LibPNG Library")

   # DLLs (For Installer)
   SET(DLL_PATH "${CMAKE_SOURCE_DIR}/../PlayCap-Externals" CACHE PATH "Path to DLLs to Install" )


ELSE(WIN32)

   FIND_PROGRAM(CMAKE_FOX_CONFIG fox-config)
   IF(CMAKE_FOX_CONFIG)
      # Set CMAKE_FOX_CXX_FLAGS to those required to link to FOX
      exec_program(
         ${CMAKE_FOX_CONFIG}
         ARGS --cflags
         OUTPUT_VARIABLE CMAKE_FOX_CXX_FLAGS)
      # Set FOX_LIBRARIES to those required to link to FOX
      exec_program(
         ${CMAKE_FOX_CONFIG}
         ARGS --libs
         OUTPUT_VARIABLE FOX_LIBRARIES)

      #SET(CMAKE_FOX_CXX_FLAGS "`${CMAKE_FOX_CONFIG} --cflags`")
      #SET(FOX_LIBRARIES "`${CMAKE_FOX_CONFIG} --libs`")
   ENDIF(CMAKE_FOX_CONFIG)

ENDIF(WIN32)

if(FOX_LIBRARIES)
   if(CMAKE_FOX_CXX_FLAGS)
      set(FOX_FOUND 1)
   endif(CMAKE_FOX_CXX_FLAGS)
endif(FOX_LIBRARIES)
#### End find Fox-Toolkit ####

if (WIN32)
   list(APPEND libs ${FOX_LIBRARY}
                    ${PTHREADS_WIN32_LIBRARY}
                    ${WINPCAP_LIBRARY}
                    ${LIBPNG_LIBRARY})
   list(APPEND libs winmm ws2_32)

else (WIN32)
   list(APPEND libs pcap pthread)
endif (WIN32)


set(SOURCES
    AboutWindow.cpp
    InterfaceWindow.cpp
    main.cpp
    PlaybackWindow.cpp)

if (WIN32)
   list(APPEND SOURCES PlayCap.rc)
endif(WIN32)


add_definitions(${CMAKE_FOX_CXX_FLAGS})

include_directories( ${FOX_INCLUDE}
                     ${PTHREADS_WIN32_INCLUDE}
                     ${WINPCAP_INCLUDE} )

add_executable(playcap WIN32
               ${SOURCES}	
)

if (WIN32)
   # Make it start in main() on Windows.
   Set_Target_Properties(playcap  PROPERTIES
      LINK_FLAGS /ENTRY:"mainCRTStartup")
endif (WIN32)


target_link_libraries(playcap ${FOX_LIBRARIES} ${libs})

install (
   TARGETS playcap
   RUNTIME DESTINATION bin
   )

if (WIN32) 
   install (
      FILES
              ${DLL_PATH}/libpng13.dll
              ${DLL_PATH}/zlib1.dll
              ${DLL_PATH}/pthreadVC2.dll
              DESTINATION bin
   )
   install (
      FILES
              ${DLL_PATH}/WinPcap_4_0_1.exe
              DESTINATION .
   )
endif(WIN32)


#### CPack ####
set(CPACK_PACKAGE_NAME "PlayCap")
set(CPACK_PACKAGE_VENDOR "Signal 11 Software")
set(CPACK_PACKAGE_VERSION "0.1.0")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.3.1-6), libFOX-1.6-0")
set(CPACK_PACKAGE_CONTACT "Signal 11 Software")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Alan Ott, Signal 11 Software, alan@signal11.us")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Replays tcpdump, Wireshark, and libpcap capture files")
set(CPACK_DEBIAN_PACKAGE_SECTION "")
set(CPACK_DEBIAN_PACKAGE_RECOMMENDS "wireshark")
set(DEBIAN_PACKAGE_BUILDS_DEPENDS "libFOX-1.6-dev, libpcap-dev, libpng-dev")
SET(CPACK_PACKAGE_EXECUTABLES "PlayCap" "PlayCap")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/COPYING.txt")
if (WIN32 AND NOT UNIX)
  #SET(CPACK_PACKAGE_ICON "${CMake_SOURCE_DIR}/Utilities/Release\\\\InstallIcon.bmp")
  SET(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\playcap.exe")
  SET(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY} PlayCap")
  SET(CPACK_NSIS_HELP_LINK "http:\\\\\\\\www.signal11.us")
  SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\www.signal11.us")
  SET(CPACK_NSIS_CONTACT "alan@signal11.us")
  SET(CPACK_NSIS_MODIFY_PATH ON)
  SET(CPACK_NSIS_EXTRA_INSTALL_COMMANDS
        "ExecWait \\\"$INSTDIR\\\\WinPcap_4_0_1.exe\\\"
         Delete   \\\"$INSTDIR\\\\WinPcap_4_0_1.exe\\\" ")

else(WIN32 AND NOT UNIX)
  SET(CPACK_STRIP_FILES "bin/playcap")
  SET(CPACK_SOURCE_STRIP_FILES "")
endif(WIN32 AND NOT UNIX)
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Alan Ott")

INCLUDE(CPack)


