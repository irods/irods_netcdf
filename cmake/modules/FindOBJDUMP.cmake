# Copyright (c) 2016-2017 Jakob Meng, <jakobmeng@web.de>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

include(FindPackageHandleStandardArgs)

find_program(OBJDUMP NAMES objdump)

find_package_handle_standard_args(OBJDUMP
	FOUND_VAR OBJDUMP_FOUND
	REQUIRED_VARS OBJDUMP
)
