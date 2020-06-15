# Copyright (c) 2016-2019 Jakob Meng, <jakobmeng@web.de>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

include(FindPackageHandleStandardArgs)

if(hdf5_FIND_QUIETLY)
    set(maybe_quiet QUIET)
else()
    set(maybe_quiet)
endif()

if(NOT hdf5_FOUND)
	# hdf5-*.pc for pkg-config is available on Debian, but not e.g. on Gentoo
	find_package(PkgConfig QUIET)
	if(PKG_CONFIG_FOUND)
		foreach(lib hdf5-mpich hdf5-openmpi hdf5-serial)
			pkg_check_modules(hdf5 ${maybe_quiet} ${lib})
			if(hdf5_FOUND)
				# make absolute library paths
				set(rlibs)
				foreach(rlib ${hdf5_LIBRARIES})
					find_library(${rlib}_path ${rlib} PATHS ${hdf5_LIBRARY_DIRS})
					if(${rlib}_path)
						list(APPEND rlibs ${${rlib}_path})
					endif()
				endforeach()
				set(hdf5_LIBRARIES ${rlibs})
				break()
			endif()
		endforeach()
	endif()
endif()

if(NOT hdf5_FOUND)
	find_path(hdf5_INCLUDE_DIR
		NAMES hdf5.h
		PATHS ${CMAKE_C_IMPLICIT_INCLUDE_DIRECTORIES})
	
	if(hdf5_USE_STATIC_LIBS)
		# References:
		#  https://github.com/Kitware/CMake/blob/master/Modules/FindBoost.cmake
		#  http://cmake.3232098.n2.nabble.com/find-library-for-static-libraries-td6227665.html
		set(_hdf5_ORIG_CMAKE_FIND_LIBRARY_SUFFIXES "${CMAKE_FIND_LIBRARY_SUFFIXES}")
		set(CMAKE_FIND_LIBRARY_SUFFIXES "${CMAKE_STATIC_LIBRARY_SUFFIX}")
	endif()

	find_library(hdf5_LIBRARY NAMES hdf5)

	if(hdf5_USE_STATIC_LIBS)
		set(CMAKE_FIND_LIBRARY_SUFFIXES "${_hdf5_ORIG_CMAKE_FIND_LIBRARY_SUFFIXES}")
	endif()

	if(hdf5_LIBRARY AND hdf5_INCLUDE_DIR)
		set(hdf5_FOUND TRUE)
		get_filename_component(_DIR "${hdf5_LIBRARY}" DIRECTORY)
		set(hdf5_LIBRARY_DIRS "${_DIR}")
		set(hdf5_LIBRARIES "${hdf5_LIBRARY}")
		set(hdf5_INCLUDE_DIRS "${hdf5_INCLUDE_DIR}")
	endif()
endif()

find_package_handle_standard_args(hdf5
	FOUND_VAR hdf5_FOUND
	REQUIRED_VARS hdf5_LIBRARIES hdf5_LIBRARY_DIRS hdf5_INCLUDE_DIRS
)

