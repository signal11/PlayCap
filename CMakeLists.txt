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

project(PlayCapProject)

#### Find Fox-Toolkit ####

IF(WIN32)

   # FOX-Toolkit
   SET(FOX_INCLUDE "../fox" CACHE PATH "Path to FOX Include Path")
   SET(FOX_LIBRARY "fox-1.6.lib" CACHE FILEPATH "FOX Library")

   # PThreads-Win32
   SET(PTHREADS_WIN32_INCLUDE "../pthreads" CACHE PATH "Path to pthread-win32 includes" )
   SET(PTHREADS_WIN32_LIBRARY "pthreadVC2.lib" CACHE FILEPATH "Pthread-Win32 Library")

   # WinPcap
   SET(WINPCAP_INCLUDE "../winpcap" CACHE PATH "Path to WinPcap includes" )
   SET(WINPCAP_LIBRARY "wpcap.lib" CACHE FILEPATH "WinPcap Library")

   # LibPNG
   SET(LIBPNG_INCLUDE "../libpng" CACHE PATH "Path to libPNG includes" )
   SET(LIBPNG_LIBRARY "libpng.lib" CACHE FILEPATH "LibPNG Library")


ELSE(WIN32)

   FIND_PROGRAM(CMAKE_FOX_CONFIG fox-config)
   IF(CMAKE_FOX_CONFIG)
      SET(CMAKE_FOX_CXX_FLAGS "`${CMAKE_FOX_CONFIG} --cflags`")
      SET(FOX_LIBRARIES "`${CMAKE_FOX_CONFIG} --libs`")
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



add_definitions(${CMAKE_FOX_CXX_FLAGS})

include_directories( ${FOX_INCLUDE}
                     ${PTHREADS_WIN32_INCLUDE}
                     ${WINPCAP_INCLUDE} )

add_executable(playcap WIN32
	AboutWindow.cpp
	InterfaceWindow.cpp
	main.cpp
	PlaybackWindow.cpp
)

if (WIN32)
   # Make it start in main() on Windows.
   Set_Target_Properties(playcap  PROPERTIES
      LINK_FLAGS /ENTRY:"mainCRTStartup")
endif (WIN32)


target_link_libraries(playcap ${FOX_LIBRARIES} ${libs})

