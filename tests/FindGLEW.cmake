#
# Try to find GLEW library and include path.
# Once done this will define
#
# GLEW_FOUND
# GLEW_INCLUDE_PATH
# GLEW_LIBRARY
# 

IF (WIN32)
	FIND_PATH( GLEW_INCLUDE_PATH GL/glew.h
		$ENV{PROGRAMFILES}/GLEW/include
		${PROJECT_SOURCE_DIR}/src/nvgl/glew/include
		DOC "The directory where GL/glew.h resides")
	FIND_LIBRARY( GLEW_LIBRARY
		NAMES glew GLEW glew32 glew32s
		PATHS
		$ENV{PROGRAMFILES}/GLEW/lib
		${PROJECT_SOURCE_DIR}/src/nvgl/glew/bin
		${PROJECT_SOURCE_DIR}/src/nvgl/glew/lib
		DOC "The GLEW library")
ELSE (WIN32)
        message("GLEW_DIR: " ${GLEW_DIR})
	FIND_PATH( GLEW_INCLUDE_PATH GL/glew.h
                PATHS ${GLEW_DIR}/include /usr/include /usr/local/include
                NO_DEFAULT_PATH
		DOC "The directory where GL/glew.h resides")
	FIND_LIBRARY( GLEW_LIBRARY
		NAMES GLEW glew
		PATHS ${GLEW_DIR}/lib /usr/lib /usr/local/lib
                NO_DEFAULT_PATH
		DOC "The GLEW library")
ENDIF (WIN32)

IF (GLEW_INCLUDE_PATH)
	SET( GLEW_FOUND 1 CACHE STRING "Set to 1 if GLEW is found, 0 otherwise")
        MESSAGE(STATUS "GLEW found in " ${GLEW_INCLUDE_PATH} " " ${GLEW_LIBRARY})
ELSE (GLEW_INCLUDE_PATH)
	SET( GLEW_FOUND 0 CACHE STRING "Set to 1 if GLEW is found, 0 otherwise")
ENDIF (GLEW_INCLUDE_PATH)
