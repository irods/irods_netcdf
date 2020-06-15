# Copyright (c) 2016-2017 Jakob Meng, <jakobmeng@web.de>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

include(FindPackageHandleStandardArgs)

find_program(GZIP NAMES gzip)

find_package_handle_standard_args(GZIP
	FOUND_VAR GZIP_FOUND
	REQUIRED_VARS GZIP
)
