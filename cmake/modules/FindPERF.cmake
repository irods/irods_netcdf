# Copyright (c) 2016-2017 Jakob Meng, <jakobmeng@web.de>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

include(FindPackageHandleStandardArgs)

find_program(PERF NAMES perf)
if(PERF_FOUND)
	execute_process(COMMAND perf stat /bin/true 
		RESULT_VARIABLE PERF_TEST 
		OUTPUT_VARIABLE PERF_OUTPUT 
		ERROR_VARIABLE PERF_ERROR)
	if(NOT PERF_TEST EQUAL 0)
		message(WARNING "Test of '${PERF}' tool failed with message:\n${PERF_ERROR}")
		set(PERF_FOUND false)
	endif()
endif()

find_package_handle_standard_args(PERF
	FOUND_VAR PERF_FOUND
	REQUIRED_VARS PERF
)
