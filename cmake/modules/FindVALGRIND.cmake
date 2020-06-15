# Copyright (c) 2016-2017 Jakob Meng, <jakobmeng@web.de>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

include(FindPackageHandleStandardArgs)

find_program(VALGRIND NAMES valgrind)

find_package_handle_standard_args(VALGRIND
	FOUND_VAR VALGRIND_FOUND
	REQUIRED_VARS VALGRIND
)
