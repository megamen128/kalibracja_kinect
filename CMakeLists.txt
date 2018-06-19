cmake_minimum_required(VERSION 2.8) 
PROJECT (kalibracja_kinect)
set( OpenCV_FOUND 1 )
find_package(OpenCV REQUIRED )
find_package(OpenNI REQUIRED )

set( NAME_SRC
    src/main.cpp    
)

set( NAME_HEADERS       
     include/cv.h
	 include/highgui.h
 	include/OpenNI.h
)



OPTION (ENABLE_OPENNI2_NITE2 ON)

IF( ENABLE_OPENNI2_NITE2 )
set(OPENNI2_DEFINITIONS ${PC_OPENNI_CFLAGS_OTHER})
FIND_LIBRARY( OPENNI2_LIBRARY
             NAMES OpenNI2
             HINTS ${PC_OPENNI2_LIBDIR} ${PC_OPENNI2_LIBRARY_DIRS} /usr/lib
             PATHS "$ENV{PROGRAMFILES}/OpenNI2/Lib${OPENNI2_SUFFIX}" "$ENV{PROGRAMW6432}/OpenNI2/Lib${OPENNI2_SUFFIX}" "$ENV{PROGRAMW6432}/OpenNI2"
             PATH_SUFFIXES lib lib64
)
FIND_PATH( OPENNI2_INCLUDE_DIR OpenNI.h
          HINTS ${PC_OPENNI2_INCLUDEDIR} ${PC_OPENNI2_INCLUDE_DIRS} 
                  /usr/include/openni2 /usr/include/ni2
                  PATHS "$ENV{PROGRAMFILES}/OpenNI2/include" "$ENV{PROGRAMW6432}/OpenNI2/include"
          PATH_SUFFIXES openni2 ni2)

FIND_LIBRARY( NITE2_LIBRARY
             NAMES NiTE2
             HINTS ${PC_OPENNI2_LIBDIR} ${PC_OPENNI2_LIBRARY_DIRS} /usr/lib
             PATHS "$ENV{PROGRAMFILES}/PrimeSense/NiTE2/lib${OPENNI2_SUFFIX}" "$ENV{PROGRAMW6432}/PrimeSense/NiTE2/lib${OPENNI2_SUFFIX}"
             PATH_SUFFIXES lib
)
FIND_PATH( NITE2_INCLUDE_DIR NiTE.h
          HINTS ${PC_OPENNI2_INCLUDEDIR} ${PC_OPENNI2_INCLUDE_DIRS} /usr/include/openni2 /usr/include/nite2
          PATHS "$ENV{PROGRAMFILES}/PrimeSense/NiTE2/include" "$ENV{PROGRAMW6432}/PrimeSense/NiTE2/include"
          PATH_SUFFIXES openni2         
)
ENDIF( ENABLE_OPENNI2_NITE2 )

link_directories( ${OPENNI2_LIBRARY} ${CMAKE_BINARY_DIR}/bin  )
INCLUDE_DIRECTORIES( ${OPENNI2_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/include  )

add_executable( kalib ${NAME_SRC} ${NAME_HEADERS} )
target_link_libraries( kalib ${OPENNI2_LIBRARY} ${OpenCV_LIBS} )
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)


