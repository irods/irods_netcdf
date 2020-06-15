# Copyright (c) 2016-2018 Jakob Meng, <jakobmeng@web.de>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

find_package(OBJDUMP QUIET)
if(NOT OBJDUMP_FOUND)
    message(WARNING "objdump not found. Macro maybe_disassemble is a noop.")
endif()

find_package(GZIP QUIET)
if(NOT GZIP_FOUND)
    message(WARNING "gzip not found. Macro maybe_disassemble is a noop.")
endif()

find_package(PERF QUIET)
if(NOT PERF_FOUND)
    message(WARNING "perf not found. Macro maybe_perf is a noop.")
endif()

find_package(gperftools_profiler QUIET)
if(NOT gperftools_profiler_FOUND)
    message(WARNING "gperftools_profiler not found. Macro maybe_gperftools_profile is a noop.")
endif()

find_package(VALGRIND QUIET)
if(NOT VALGRIND_FOUND)
    message(WARNING "Valgrind not found. Macro maybe_callgrind is a noop.")
endif()



macro(has_disassemble _bool)
    if (OBJDUMP AND GZIP)
        set(${_bool} TRUE)
    else()
        set(${_bool} FALSE)
    endif()
endmacro()

macro(maybe_disassemble _dst_target_NAME _src_target_NAME)
    if (OBJDUMP AND GZIP)
        add_custom_command(
            TARGET ${_dst_target_NAME}
            POST_BUILD
            #COMMAND ${OBJDUMP} -Mintel --demangle --source --disassemble-all -t $<TARGET_FILE:${_src_target_NAME}> | ${GZIP} > $<TARGET_FILE:${_src_target_NAME}>.alst.gz
            COMMAND ${OBJDUMP} -Mintel --demangle --source --disassemble     -t $<TARGET_FILE:${_src_target_NAME}>           > $<TARGET_FILE:${_src_target_NAME}>.lst
            COMMAND ${OBJDUMP} -Mintel --demangle          --disassemble     -t $<TARGET_FILE:${_src_target_NAME}> | ${GZIP} > $<TARGET_FILE:${_src_target_NAME}>.s.gz
            VERBATIM
        )
    endif()
endmacro()

macro(has_callgrind _bool)
    if (VALGRIND_FOUND)
        set(${_bool} TRUE)
    else()
        set(${_bool} FALSE)
    endif()
endmacro()

macro(maybe_callgrind _dst_target_NAME _src_target_NAME)
    if(VALGRIND_FOUND)
        add_custom_command(
            TARGET ${_dst_target_NAME}
            POST_BUILD
            COMMAND ${VALGRIND} --tool=callgrind --dump-instr=yes --callgrind-out-file=$<TARGET_FILE:${_src_target_NAME}>.cg $<TARGET_FILE:${_src_target_NAME}>
            VERBATIM
        )
    endif()
endmacro()

macro(has_gperftools_profile _bool)
    if (gperftools_profiler_FOUND)
        set(${_bool} TRUE)
    else()
        set(${_bool} FALSE)
    endif()
endmacro()

macro(maybe_gperftools_profile _dst_target_NAME _src_target_NAME)
    if(gperftools_profiler_FOUND)
        add_custom_command(
            TARGET ${_dst_target_NAME}
            POST_BUILD
            COMMAND env CPUPROFILE=$<TARGET_FILE:${_src_target_NAME}>.prof CPUPROFILE_FREQUENCY=10000 LD_PRELOAD=${gperftools_profiler_LIBRARY} $<TARGET_FILE:${_src_target_NAME}>
            VERBATIM
        )
    endif()
endmacro()

macro(has_perf _bool)
    if (PERF_FOUND)
        set(${_bool} TRUE)
    else()
        set(${_bool} FALSE)
    endif()
endmacro()

macro(maybe_perf _dst_target_NAME _src_target_NAME)
    if(PERF_FOUND)
        add_custom_command(
            TARGET ${_dst_target_NAME}
            POST_BUILD
            COMMAND ${PERF} stat -r 10 -d $<TARGET_FILE:${_src_target_NAME}> > $<TARGET_FILE:${_src_target_NAME}>.perf.out 2> $<TARGET_FILE:${_src_target_NAME}>.perf || echo "Running 'perf' tool for '$<TARGET_FILE:${_src_target_NAME}>' failed with:"
            COMMAND cat $<TARGET_FILE:${_src_target_NAME}>.perf
            VERBATIM
        )
    endif()
endmacro()

# # Ref.: https://stackoverflow.com/a/7788165/6490710
# macro(maybe_list_dirs result_var parent_dir)
#     file(GLOB children RELATIVE ${parent_dir} ${parent_dir}/*)
#     set(dirs "")
#     foreach(child ${children})
#         if(IS_DIRECTORY ${parent_dir}/${child})
#         list(APPEND dirs ${child})
#     endif()
#     endforeach()
#     set(${result_var} ${dirs})
# endmacro()
