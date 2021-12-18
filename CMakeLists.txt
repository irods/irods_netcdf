
cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

find_package(IRODS 4.2.10 EXACT REQUIRED)

set(IRODS_PLUGIN_REVISION "0")
set(IRODS_PLUGIN_VERSION "${IRODS_VERSION}.${IRODS_PLUGIN_REVISION}")

set(CMAKE_C_COMPILER ${IRODS_EXTERNALS_FULLPATH_CLANG}/bin/clang)
set(CMAKE_CXX_COMPILER ${IRODS_EXTERNALS_FULLPATH_CLANG}/bin/clang++)
set(CMAKE_EXE_LINKER_FLAGS_INIT "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")

project(irods_netcdf_plugin 
VERSION "${IRODS_PLUGIN_VERSION}"
LANGUAGES C CXX)

set(CMAKE_CXX_FLAGS "-std=c++17 -stdlib=libc++ -Wno-write-strings ${CMAKE_CXX_FLAGS}")

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH "/opt/irods-externals/clang-runtime6.0-0/lib:/opt/irods-externals/boost1.67.0-0/lib")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

find_package(NetCDF REQUIRED)
find_package(HDF5 COMPONENTS HL REQUIRED)
find_package(OpenSSL REQUIRED)

add_subdirectory(use-libs)  # demo for printing HDF5 and NETCDF output variables

#include(FeatureSummary)
#set(CMAKE_INCLUDE_DIRECTORIES_PROJECT_BEFORE ON)

set_property(GLOBAL PROPERTY CMAKE_POSITION_INDEPENDENT_CODE ON)

file(GLOB static_lib_src "${CMAKE_SOURCE_DIR}/api/lib/*.cpp")

#----------- build API cli/srv foundational static libs

set(ALL_INCLUDES
    ${IRODS_INCLUDE_DIRS}
    ${IRODS_EXTERNALS_FULLPATH_BOOST}/include
    ${IRODS_EXTERNALS_FULLPATH_JSON}/include
    ${CMAKE_SOURCE_DIR}/include
    ${HDF5_INCLUDE_DIRS}
    ${NetCDF_INCLUDE_DIRS}
)


add_library(client_netcdf_lib STATIC ${static_lib_src})
set_property(TARGET client_netcdf_lib PROPERTY POSITION_INDEPENDENT_CODE ON)
target_include_directories(client_netcdf_lib 
    PRIVATE
    "${CMAKE_SOURCE_DIR}/api/include"
    ${ALL_INCLUDES}
)

add_library(server_netcdf_lib STATIC ${static_lib_src})
set_property(TARGET server_netcdf_lib PROPERTY POSITION_INDEPENDENT_CODE ON)
target_include_directories(server_netcdf_lib 
    PRIVATE
    "${CMAKE_SOURCE_DIR}/api/include"
    ${ALL_INCLUDES}
)
target_compile_definitions(server_netcdf_lib PRIVATE RODS_SERVER)

#----------- initialize lists of install targets

set(API_LIBRARY_TARGETS_server)
set(API_LIBRARY_TARGETS_client)
set(MSVC_LIBRARY_TARGETS)
set(RUNTIME_TARGETS)

#----------- definitions

function(STRIP_LEADING_PATH in out)
  string(REGEX REPLACE "^.*/" "" output "${in}" )
  set("${out}" "${output}" PARENT_SCOPE)
endfunction(STRIP_LEADING_PATH)

function(STRIP_CPP in out )
  string(REGEX REPLACE "\.cpp$" "" output "${in}")
  set("${out}" "${output}" PARENT_SCOPE)
endfunction(STRIP_CPP)


#----------- build API cli/srv modules

file(GLOB api_modules_src "${CMAKE_SOURCE_DIR}/api/src/*.cpp")

foreach (src ${api_modules_src})
    strip_leading_path("${src}" stub_cpp)
    strip_cpp("${stub_cpp}" stub)
    foreach(target client server)
        set(module_name "${stub}_${target}")
        list(APPEND API_LIBRARY_TARGETS_${target} "${module_name}")
        add_library("${module_name}" MODULE ${src})
        target_link_libraries("${module_name}" PRIVATE 
                irods_client
                # irods_server # - note server binary provides this dependency
                ${target}_netcdf_lib
                NetCDF::NetCDF
        )
        target_include_directories("${module_name}"
            PRIVATE
            ${CMAKE_SOURCE_DIR}/api/include
            ${ALL_INCLUDES}
        )
        if("${target}" STREQUAL "server")
            target_compile_definitions("${module_name}" PRIVATE RODS_SERVER)
        endif()
    endforeach()
endforeach()


#----------- build microservice modules

# --> NetCDF

file(GLOB msvc_netcdf_modules_src "${CMAKE_SOURCE_DIR}/microservices/netcdf/src/*.cpp")

foreach (src ${msvc_netcdf_modules_src})
    strip_leading_path("${src}" stub_cpp)
    strip_cpp("${stub_cpp}" stub)
    add_library("${stub}" MODULE ${src})
    list(APPEND MSVC_LIBRARY_TARGETS "${stub}")
    target_link_libraries("${stub}" 
        PRIVATE
        server_netcdf_lib
        NetCDF::NetCDF)
    target_compile_definitions("${stub}" PRIVATE RODS_SERVER)
    target_include_directories("${stub}"
        PRIVATE
        ${CMAKE_SOURCE_DIR}/api/include
        ${CMAKE_SOURCE_DIR}/microservices/netcdf/include
        ${ALL_INCLUDES}
    )
endforeach()

# --> HDF5

file(GLOB msvc_hdf5_modules_src "${CMAKE_SOURCE_DIR}/microservices/hdf5/src/msi*.cpp")

foreach (src ${msvc_hdf5_modules_src})
    strip_leading_path("${src}" stub_cpp)
    strip_cpp("${stub_cpp}" stub)
    add_library("${stub}" MODULE ${src})
    list(APPEND MSVC_LIBRARY_TARGETS "${stub}")
    target_compile_definitions("${stub}" PRIVATE RODS_SERVER)
    target_link_libraries("${stub}" PRIVATE 
        NetCDF::NetCDF
        #${HDF5_LIBRARIES}     # These were not linked in for iRODS 4.1 functionality,
        #${HDF5_HL_LIBRARIES}  #    therefore we'll leave them out for now.
    )
    target_include_directories("${stub}"
        PRIVATE
        ${CMAKE_SOURCE_DIR}/api/include
        ${CMAKE_SOURCE_DIR}/microservices/hdf5/include
        ${ALL_INCLUDES}
    )
endforeach()


# --> icommands

file(GLOB icommands_src "${CMAKE_SOURCE_DIR}/icommands/src/*.cpp")


foreach (src ${icommands_src})
    strip_leading_path("${src}" stub_cpp)
    strip_cpp("${stub_cpp}" stub)
    add_executable(${stub} "${src}")
    target_include_directories(${stub}
        PRIVATE ${ALL_INCLUDES}
        "${CMAKE_SOURCE_DIR}/api/include"
    )
    list(APPEND RUNTIME_TARGETS ${stub})
    target_link_libraries(
        ${stub}
        PRIVATE
        irods_client
        irods_plugin_dependencies
        irods_common
        client_netcdf_lib
        ${IRODS_EXTERNALS_FULLPATH_BOOST}/lib/libboost_program_options.so
        ${IRODS_EXTERNALS_FULLPATH_BOOST}/lib/libboost_thread.so
        ${IRODS_EXTERNALS_FULLPATH_BOOST}/lib/libboost_regex.so
        ${IRODS_EXTERNALS_FULLPATH_BOOST}/lib/libboost_system.so
        NetCDF::NetCDF
        ${HDF5_LIBRARIES}
        ${HDF5_HL_LIBRARIES}
    )
endforeach()

if (NOT CPACK_PACKAGING_INSTALL_PREFIX)
  set(CPACK_PACKAGING_INSTALL_PREFIX "/" CACHE STRING "Package root path. \"/\" is correct for normal package builds.." FORCE)
  message(STATUS "Setting unspecified CPACK_PACKAGING_INSTALL_PREFIX to '${CPACK_PACKAGING_INSTALL_PREFIX}'. This is the correct setting for normal builds.")
endif()

list(APPEND CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION "${CPACK_PACKAGING_INSTALL_PREFIX}var")
list(APPEND CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION "${CPACK_PACKAGING_INSTALL_PREFIX}var/lib")
list(APPEND CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION "${CPACK_PACKAGING_INSTALL_PREFIX}var/lib/irods")
list(APPEND CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION "${CPACK_PACKAGING_INSTALL_PREFIX}usr")
list(APPEND CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION "${CPACK_PACKAGING_INSTALL_PREFIX}usr/lib")
list(APPEND CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION "${CPACK_PACKAGING_INSTALL_PREFIX}usr/lib/irods")
list(APPEND CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION "${CPACK_PACKAGING_INSTALL_PREFIX}usr/lib/irods/plugins")
list(APPEND CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION "${CPACK_PACKAGING_INSTALL_PREFIX}usr/lib/irods/plugins/api")
list(APPEND CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION "${CPACK_PACKAGING_INSTALL_PREFIX}usr/lib/irods/plugins/microservices")

# --- CMake install targets

install(
      TARGETS ${RUNTIME_TARGETS}
      RUNTIME
      COMPONENT icommands
      DESTINATION /usr/bin
)

install(
      TARGETS ${API_LIBRARY_TARGETS_server}
      LIBRARY
      COMPONENT server_modules
      DESTINATION /usr/lib/irods/plugins/api
)

install(
      TARGETS ${API_LIBRARY_TARGETS_client}
      LIBRARY
      COMPONENT client_modules
      DESTINATION /usr/lib/irods/plugins/api
)

install(
      TARGETS ${MSVC_LIBRARY_TARGETS}
      LIBRARY
      COMPONENT server_modules
      DESTINATION /usr/lib/irods/plugins/microservices
)

install(
      DIRECTORY ${CMAKE_SOURCE_DIR}/microservices/rules/
      COMPONENT server_modules
      DESTINATION /var/lib/irods/netcdf-plugin-tests
)
# --- CPack configuration ---

if (IRODS_LINUX_DISTRIBUTION_NAME STREQUAL "ubuntu" OR IRODS_LINUX_DISTRIBUTION_NAME STREQUAL "debian")
  if (NOT IRODS_PLUGIN_LINUX_DISTRIBUTION_VERSION_CODENAME)
    execute_process(
      COMMAND "lsb_release" "-s" "-c"
      RESULT_VARIABLE IRODS_PLUGIN_EXECUTE_PROCESS_RESULT_LINUX_DISTRIBUTION_VERSION_CODENAME
      OUTPUT_VARIABLE IRODS_PLUGIN_LINUX_DISTRIBUTION_VERSION_CODENAME
      )
    string(STRIP ${IRODS_PLUGIN_LINUX_DISTRIBUTION_VERSION_CODENAME} IRODS_PLUGIN_LINUX_DISTRIBUTION_VERSION_CODENAME)
    if (NOT ${IRODS_PLUGIN_EXECUTE_PROCESS_RESULT_LINUX_DISTRIBUTION_VERSION_CODENAME} STREQUAL "0")
      message(FATAL_ERROR "Linux lsb_release shortname detection failed\n${IRODS_PLUGIN_EXECUTE_PROCESS_RESULT_LINUX_DISTRIBUTION_VERSION_CODENAME}")
    endif()
    set(IRODS_PLUGIN_LINUX_DISTRIBUTION_VERSION_CODENAME ${IRODS_PLUGIN_LINUX_DISTRIBUTION_VERSION_CODENAME} CACHE STRING "Linux distribution version codename, e.g. {precise, wheezy, trusty, jessie, ...}." FORCE)
    message(STATUS "Setting unspecified IRODS_PLUGIN_LINUX_DISTRIBUTION_VERSION_CODENAME to '${IRODS_PLUGIN_LINUX_DISTRIBUTION_VERSION_CODENAME}'")
  endif()
endif()

if (NOT CPACK_GENERATOR)
  set(CPACK_GENERATOR ${IRODS_CPACK_GENERATOR} CACHE STRING "CPack generator to use, e.g. {DEB, RPM, TGZ}." FORCE)
  message(STATUS "Setting unspecified CPACK_GENERATOR to ${CPACK_GENERATOR}. This is the correct setting for normal builds.")
endif()

if (CPACK_GENERATOR STREQUAL "RPM")
  set(IRODS_PLUGIN_PACKAGE_FILE_NAME_SUFFIX "-${IRODS_PLUGIN_VERSION}-1.${CMAKE_SYSTEM_PROCESSOR}")
elseif(CPACK_GENERATOR STREQUAL "DEB")
  if (CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
    set(CMAKE_SYSTEM_PROCESSOR "amd64")
  endif()
  set(IRODS_PLUGIN_PACKAGE_FILE_NAME_SUFFIX "_${IRODS_PLUGIN_VERSION}~${IRODS_PLUGIN_LINUX_DISTRIBUTION_VERSION_CODENAME}_${CMAKE_SYSTEM_PROCESSOR}")
else()
  set(IRODS_PLUGIN_PACKAGE_FILE_NAME_SUFFIX "-${IRODS_PLUGIN_VERSION}-${IRODS_LINUX_DISTRIBUTION_NAME}${IRODS_LINUX_DISTRIBUTION_VERSION_MAJOR}-${CMAKE_SYSTEM_PROCESSOR}")
endif()


set(CPACK_PACKAGE_FILE_NAME "irods-netcdf${IRODS_PLUGIN_PACKAGE_FILE_NAME_SUFFIX}")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
set(CPACK_COMPONENT_INCLUDE_TOPLEVEL_DIRECTORY OFF)


set(CPACK_PACKAGE_VERSION ${IRODS_PLUGIN_VERSION})
set(CPACK_PACKAGE_CONTACT "Renaissance Computing Institute <info@irods.org>")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "The integrated Rule-Oriented Data System")
set(CPACK_PACKAGE_VENDOR "Renaissance Computing Institute <info@irods.org>")


set(CPACK_DEB_COMPONENT_INSTALL ON)
set(CPACK_DEBIAN_PACKAGE_SECTION "contrib/science")
set(CPACK_DEBIAN_COMPRESSION_TYPE "gzip")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "extra")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://irods.org")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS OFF)
set(CPACK_DEBIAN_PACKAGE_CONTROL_STRICT_PERMISSION ON)
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_SOURCE_DIR}/packaging/postinst;")


set(CPACK_RPM_COMPONENT_INSTALL ON )
set(CPACK_RPM_PACKAGE_RELEASE "1")
set(CPACK_RPM_PACKAGE_LICENSE "BSD-3-Clause")
set(CPACK_RPM_PACKAGE_VENDOR "iRODS Consortium")
set(CPACK_RPM_PACKAGE_URL "https://irods.org")
set(CPACK_RPM_PACKAGE_AUTOREQ 0)
set(CPACK_RPM_PACKAGE_AUTOPROV 0)
set(CPACK_RPM_POST_INSTALL_SCRIPT_FILE "${CMAKE_SOURCE_DIR}/packaging/postinst")


set(CPACK_ARCHIVE_COMPONENT_INSTALL OFF)


set(CPACK_DEBIAN_PACKAGE_NAME "irods-netcdf")


set(CPACK_DEBIAN_PACKAGE_DEPENDS "${IRODS_PACKAGE_DEPENDENCIES_STRING},
    irods-runtime (= ${IRODS_CPACK_DEBIAN_PACKAGE_VERSION}), libc6")

if (IRODS_LINUX_DISTRIBUTION_NAME STREQUAL "ubuntu")
    if (IRODS_LINUX_DISTRIBUTION_VERSION_MAJOR GREATER_EQUAL "18")
         set(CPACK_DEBIAN_PACKAGE_DEPENDS "${CPACK_DEBIAN_PACKAGE_DEPENDS}, libnetcdf13, libhdf5-100")
    else()
         set(CPACK_DEBIAN_PACKAGE_DEPENDS "${CPACK_DEBIAN_PACKAGE_DEPENDS}, libnetcdf11, libhdf5-10")
    endif()
endif()

set(CPACK_RPM_PACKAGE_NAME "irods-netcdf")
if (IRODS_LINUX_DISTRIBUTION_NAME STREQUAL "centos" OR IRODS_LINUX_DISTRIBUTION_NAME STREQUAL "centos linux")
  set(CPACK_RPM_PACKAGE_REQUIRES "${IRODS_PACKAGE_DEPENDENCIES_STRING}, irods-runtime = ${IRODS_VERSION}, hdf5, netcdf")
elseif (IRODS_LINUX_DISTRIBUTION_NAME STREQUAL "opensuse")
  set(CPACK_RPM_PACKAGE_REQUIRES "${IRODS_PACKAGE_DEPENDENCIES_STRING}, irods-runtime = ${IRODS_VERSION}, libhdf5-103, libnetcdf15")
endif()


set (CPACK_RPM_SERVER_MODULES_PACKAGE_REQUIRES "irods-netcdf-client_modules")
set (CPACK_RPM_ICOMMANDS_PACKAGE_REQUIRES "irods-netcdf-client_modules")

set (CPACK_DEBIAN_SERVER_MODULES_PACKAGE_DEPENDS "irods-netcdf-client_modules")
set (CPACK_DEBIAN_ICOMMANDS_PACKAGE_DEPENDS  "irods-netcdf-client_modules")


foreach (component_name icommands server_modules client_modules)

    set(PACKAGE_AND_COMPONENT "${CPACK_RPM_PACKAGE_NAME}-${component_name}")
    set(VERSION_ETC "${IRODS_PLUGIN_VERSION}-${IRODS_LINUX_DISTRIBUTION_NAME}-${IRODS_LINUX_DISTRIBUTION_VERSION_MAJOR}-${CMAKE_SYSTEM_PROCESSOR}")

    string(TOUPPER "${component_name}" CPACK_RPM_PACKAGE_NAME_UPPERCASE)
    set(CPACK_RPM_${CPACK_RPM_PACKAGE_NAME_UPPERCASE}_FILE_NAME "${PACKAGE_AND_COMPONENT}-${VERSION_ETC}.rpm")
    set(CPACK_DEBIAN_${CPACK_RPM_PACKAGE_NAME_UPPERCASE}_FILE_NAME "${PACKAGE_AND_COMPONENT}-${VERSION_ETC}.deb")

    unset(PACKAGE_AND_COMPONENT)
    unset(VERSION_ETC)
    unset(CPACK_RPM_PACKAGE_NAME_UPPERCASE)

endforeach()


include(CPack)


cpack_add_component(
	icommands
)

cpack_add_component(
	server_modules
)

cpack_add_component(
	client_modules
)

