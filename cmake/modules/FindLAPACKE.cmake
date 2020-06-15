# Copyright (c) 2016-2018 Jakob Meng, <jakobmeng@web.de>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

# Ref.: https://github.com/mrirecon/bart/blob/master/cmake/FindLAPACKE.cmake

include(FindPackageHandleStandardArgs)

set(LAPACKE_FOUND false)
set(LAPACKE_LIBRARIES)
set(LAPACKE_LIBRARY_DIRS)
set(LAPACKE_INCLUDE_DIRS)

find_path(LAPACKE_INCLUDE_DIR 
	NAMES lapacke.h
	PATHS ${CMAKE_C_IMPLICIT_INCLUDE_DIRECTORIES})

if(LAPACKE_USE_STATIC_LIBS)
	# References:
	#  https://github.com/Kitware/CMake/blob/master/Modules/FindBoost.cmake
	#  http://cmake.3232098.n2.nabble.com/find-library-for-static-libraries-td6227665.html
	set(_LAPACKE_ORIG_CMAKE_FIND_LIBRARY_SUFFIXES "${CMAKE_FIND_LIBRARY_SUFFIXES}")
	set(CMAKE_FIND_LIBRARY_SUFFIXES "${CMAKE_STATIC_LIBRARY_SUFFIX}")
endif()

find_library(LAPACKE_LIBRARY NAMES lapacke)

if(LAPACKE_USE_STATIC_LIBS)
	set(CMAKE_FIND_LIBRARY_SUFFIXES "${_LAPACKE_ORIG_CMAKE_FIND_LIBRARY_SUFFIXES}")
endif()

if(LAPACKE_LIBRARY AND LAPACKE_INCLUDE_DIR)
	set(LAPACKE_FOUND true)
	get_filename_component(_DIR "${LAPACKE_LIBRARY}" DIRECTORY)
	set(LAPACKE_LIBRARY_DIRS "${_DIR}")
	set(LAPACKE_LIBRARIES "${LAPACKE_LIBRARY}")
	set(LAPACKE_INCLUDE_DIRS "${LAPACKE_INCLUDE_DIR}")
endif()

find_package_handle_standard_args(LAPACKE
	FOUND_VAR LAPACKE_FOUND
	REQUIRED_VARS
		LAPACKE_LIBRARIES
		LAPACKE_LIBRARY_DIRS
		LAPACKE_INCLUDE_DIRS
)

message(STATUS "LAPACKE_FOUND        :${LAPACKE_FOUND}:  - set to true if the library is found")
message(STATUS "LAPACKE_INCLUDE_DIRS :${LAPACKE_INCLUDE_DIRS}: - list of required include directories")
message(STATUS "LAPACKE_LIBRARIES    :${LAPACKE_LIBRARIES}: - list of libraries to be linked")
message(STATUS "LAPACKE_LIBRARY_DIRS :${LAPACKE_LIBRARY_DIRS}: - list of library directories")
