# Copyright (c) 2018 Jakob Meng, <jakobmeng@web.de>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

include(FindPackageHandleStandardArgs)

set(FLAME_FOUND false)
set(FLAME_LIBRARIES)
set(FLAME_LIBRARY_DIRS)
set(FLAME_INCLUDE_DIRS)

find_path(FLAME_INCLUDE_DIR
    NAMES FLAME.h
    PATHS ${FLAME_PATH}/include /opt/libflame/include)
find_library(FLAME_LIBRARY
    NAMES flame
    PATHS ${FLAME_PATH}/lib /opt/libflame/lib)

if(FLAME_LIBRARY AND FLAME_INCLUDE_DIR)
	set(FLAME_FOUND true)
	get_filename_component(_DIR "${FLAME_LIBRARY}" DIRECTORY)
	set(FLAME_LIBRARY_DIRS "${_DIR}")
	set(FLAME_LIBRARIES "${FLAME_LIBRARY}")
	set(FLAME_INCLUDE_DIRS "${FLAME_INCLUDE_DIR}")
endif()

find_package_handle_standard_args(FLAME
	FOUND_VAR FLAME_FOUND
	REQUIRED_VARS
		FLAME_LIBRARIES
		FLAME_LIBRARY_DIRS
		FLAME_INCLUDE_DIRS
)

message(STATUS "FLAME_FOUND        :${FLAME_FOUND}:  - set to true if the library is found")
message(STATUS "FLAME_INCLUDE_DIRS :${FLAME_INCLUDE_DIRS}: - list of required include directories")
message(STATUS "FLAME_LIBRARIES    :${FLAME_LIBRARIES}: - list of libraries to be linked")
message(STATUS "FLAME_LIBRARY_DIRS :${FLAME_LIBRARY_DIRS}: - list of library directories")
