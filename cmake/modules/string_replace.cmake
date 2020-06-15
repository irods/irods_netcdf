# Copyright (c) 2018 Jakob Meng, <jakobmeng@web.de>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#
# Usage: 
#  ${CMAKE_COMMAND} -D "SOURCE_FILENAME=path/to/input/file" -D "MATCH_STRING=replace this" -D "REPLACE_STRING=insert this" -D "TARGET_FILENAME=path/to/output/file" -P "${CMAKE_CURRENT_SOURCE_DIR}/string_replace.cmake"

#message(STATUS "SOURCE_FILENAME: ${SOURCE_FILENAME}")
#message(STATUS "   MATCH_STRING: ${MATCH_STRING}")
#message(STATUS " REPLACE_STRING: ${REPLACE_STRING}")
#message(STATUS "TARGET_FILENAME: ${TARGET_FILENAME}")

file(READ "${SOURCE_FILENAME}" SOURCE_DATA)
string(REPLACE "${MATCH_STRING}" "${REPLACE_STRING}" TARGET_DATA "${SOURCE_DATA}")
file(WRITE "${TARGET_FILENAME}" "${TARGET_DATA}")
