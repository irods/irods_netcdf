# Copyright (c) 2016-2018 Jakob Meng, <jakobmeng@web.de>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

# Inspired by: https://github.com/boostorg/hana/blob/master/CMakeLists.txt
include(CheckCCompilerFlag)
macro(maybe_add_c_flag testname flag)
    set(extra_macro_args ${ARGN})
	check_c_compiler_flag(${flag} ${testname})
	if (${testname})
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${flag}")
	endif()
endmacro()

include(CheckCXXCompilerFlag)
macro(maybe_add_cxx_flag testname flag)
    set(extra_macro_args ${ARGN})
	check_cxx_compiler_flag(${flag} ${testname})
	if (${testname})
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}")
	endif()
endmacro()
