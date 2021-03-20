# Copyright (c) 2016-2017 Jakob Meng, <jakobmeng@web.de>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

include(FindPackageHandleStandardArgs)

find_library(gperftools_profiler_LIBRARY NAMES profiler)

find_package_handle_standard_args(gperftools_profiler
	FOUND_VAR gperftools_profiler_FOUND
	REQUIRED_VARS gperftools_profiler_LIBRARY
)