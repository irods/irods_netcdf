cmake_minimum_required(VERSION 3.12.0 FATAL_ERROR)
# target_link_libraries for object targets

set(IRODS_MINIMUM_VERSION "4.90.0")
find_package(IRODS "${IRODS_MINIMUM_VERSION}" REQUIRED)

set(IRODS_PLUGIN_REVISION "0")
set(IRODS_PLUGIN_VERSION "${IRODS_MINIMUM_VERSION}.${IRODS_PLUGIN_REVISION}")

include(IrodsCXXCompiler)
set(CMAKE_CXX_STANDARD ${IRODS_CXX_STANDARD})
set(CMAKE_EXE_LINKER_FLAGS_INIT "-Wl,--export-dynamic -Wl,--enable-new-dtags -Wl,--as-needed -Wl,-z,defs")
set(CMAKE_MODULE_LINKER_FLAGS_INIT "-Wl,--enable-new-dtags -Wl,--as-needed -Wl,-z,defs")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE_INIT "-Wl,--gc-sections -Wl,-z,combreloc")
set(CMAKE_MODULE_LINKER_FLAGS_RELEASE_INIT "-Wl,--gc-sections -Wl,-z,combreloc")
include(IrodsRunpathDefaults)

project(irods_netcdf_plugin
  VERSION "${IRODS_PLUGIN_VERSION}"
  LANGUAGES C CXX)

include("${IRODS_TARGETS_PATH}")

include(GNUInstallDirs)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED COMPONENTS Crypto SSL)
find_package(nlohmann_json "3.6.1" REQUIRED)
find_package(NetCDF REQUIRED)
find_package(HDF5 "1.6" REQUIRED COMPONENTS C)

set_property(GLOBAL PROPERTY CMAKE_POSITION_INDEPENDENT_CODE ON)

add_compile_options(-Wno-write-strings)

#----------- common code object libraries

include(ObjectTargetHelpers)

# --> api (server/client variants)

foreach (variant IN ITEMS client server)
  add_library(
    irods_netcdf_obj_${variant}
    OBJECT
    "${CMAKE_CURRENT_SOURCE_DIR}/api/lib/_rsNcGetVarsByType.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/api/lib/_rsNcInqDataObj.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/api/lib/ncUtil.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/api/lib/ncarchUtil.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/api/lib/ncattrUtil.cpp"
    # see issue #42
    #"${CMAKE_CURRENT_SOURCE_DIR}/api/lib/ooiCi.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/api/lib/rcNcArchTimeSeries.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/api/lib/rcNcClose.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/api/lib/rcNcCreate.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/api/lib/rcNcGetAggElement.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/api/lib/rcNcGetAggInfo.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/api/lib/rcNcGetVarsByType.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/api/lib/rcNcInq.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/api/lib/rcNcInqGrps.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/api/lib/rcNcInqId.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/api/lib/rcNcInqWithId.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/api/lib/rcNcOpen.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/api/lib/rcNcOpenGroup.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/api/lib/rcNcRegGlobalAttr.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/api/lib/rcOoiGenServReq.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/api/lib/readAggInfo.cpp"
  )
  target_link_libraries(
    irods_netcdf_obj_${variant}
    PUBLIC
    irods_${variant}
    nlohmann_json::nlohmann_json
    NetCDF::NetCDF
  )
  target_include_directories(
    irods_netcdf_obj_${variant}
    PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/api/include>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    # Normally any IRODS_EXTERNALS stuff would be private,
    # but since we distribute no dev packages, it's easier to put them in PUBLIC
    # and wrap them in BUILD_INTERFACE
    "$<BUILD_INTERFACE:${IRODS_EXTERNALS_FULLPATH_BOOST}/include>"
  )

  set_target_properties(irods_netcdf_obj_${variant} PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
  set_target_properties(irods_netcdf_obj_${variant} PROPERTIES INTERFACE_POSITION_INDEPENDENT_CODE TRUE)
  set_target_properties(irods_netcdf_obj_${variant} PROPERTIES EXCLUDE_FROM_ALL TRUE)
endforeach()
target_compile_definitions(irods_netcdf_obj_server PRIVATE RODS_SERVER)

# --> HDF5

add_library(
  irods_hdf5_obj
  OBJECT
  "${CMAKE_CURRENT_SOURCE_DIR}/microservices/hdf5/lib/clH5Dataset.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/microservices/hdf5/lib/clH5File.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/microservices/hdf5/lib/clH5Group.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/microservices/hdf5/lib/h5Attribute.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/microservices/hdf5/lib/h5Ctor.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/microservices/hdf5/lib/h5Dataset.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/microservices/hdf5/lib/h5Dtor.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/microservices/hdf5/lib/h5File.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/microservices/hdf5/lib/h5Group.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/microservices/hdf5/lib/h5Object.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/microservices/hdf5/lib/h5String.cpp"
)
target_link_libraries(
  irods_hdf5_obj
  PUBLIC
  irods_server
  hdf5::hdf5
)
target_include_directories(
  irods_hdf5_obj
  PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/microservices/hdf5/include>"
)
target_compile_definitions(
  irods_hdf5_obj
  PUBLIC
  H5_USE_16_API
)
set_target_properties(irods_hdf5_obj PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
set_target_properties(irods_hdf5_obj PROPERTIES INTERFACE_POSITION_INDEPENDENT_CODE TRUE)
set_target_properties(irods_hdf5_obj PROPERTIES EXCLUDE_FROM_ALL TRUE)

#----------- build API cli/srv modules

set(
  IRODS_NETCDF_API_MODULES
  # see issue #39
  #rsNcArchTimeSeries
  rsNcClose
  # see issue #40
  #rsNcCreate
  rsNcGetAggElement
  rsNcGetAggInfo
  rsNcGetVarsByType
  rsNcInq
  rsNcInqGrps
  rsNcInqId
  rsNcInqWithId
  rsNcOpen
  rsNcOpenGroup
  rsNcRegGlobalAttr
  # see issues #42 and #44
  #rsOoiGenServReq
)

foreach (module ${IRODS_NETCDF_API_MODULES})
  set(module_src "${CMAKE_CURRENT_SOURCE_DIR}/api/src/${module}.cpp")
  foreach (variant IN ITEMS client server)
    add_library(${module}_${variant} MODULE "${module_src}")
    target_link_objects(
      ${module}_${variant}
      PRIVATE
      irods_netcdf_obj_${variant}
    )

    install(
      TARGETS ${module}_${variant}
      LIBRARY
      COMPONENT ${variant}_modules
      DESTINATION "${IRODS_PLUGINS_DIRECTORY}/api"
    )
  endforeach()
  target_compile_definitions(${module}_server PRIVATE RODS_SERVER)
endforeach()

#----------- build microservice modules

# --> NetCDF

set(
  IRODS_NETCDF_MSI_MODULES
  msiAddToNcArray
  msiFloatToString
  msiFreeNcStruct
  msiNcClose
  msiNcCreate
  msiNcGetArrayLen
  msiNcGetAttNameInInqOut
  msiNcGetAttValStrInInqOut
  msiNcGetDataType
  msiNcGetDimLenInInqOut
  msiNcGetDimNameInInqOut
  msiNcGetElementInArray
  msiNcGetFormatInInqOut
  msiNcGetGrpInInqOut
  msiNcGetNGrpsInInqOut
  msiNcGetNattsInInqOut
  msiNcGetNdimsInInqOut
  msiNcGetNumDim
  msiNcGetNvarsInInqOut
  msiNcGetVarIdInInqOut
  msiNcGetVarNameInInqOut
  msiNcGetVarTypeInInqOut
  msiNcGetVarsByType
  msiNcInq
  msiNcInqGrps
  msiNcInqId
  msiNcInqWithId
  msiNcIntDataTypeToStr
  msiNcOpen
  msiNcOpenGroup
  msiNcRegGlobalAttr
  msiNcSubsetVar
  msiNcVarStat
)

foreach (module ${IRODS_NETCDF_MSI_MODULES})
  set(module_src "${CMAKE_CURRENT_SOURCE_DIR}/microservices/netcdf/src/${module}.cpp")
  add_library(${module} MODULE "${module_src}")
  target_link_objects(
    ${module}
    PRIVATE
    irods_netcdf_obj_server
  )
  target_include_directories(
    ${module}
    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/microservices/netcdf/include"
  )
  target_compile_definitions(${module} PRIVATE RODS_SERVER)

  install(
    TARGETS ${module}
    LIBRARY
    COMPONENT server_modules
    DESTINATION "${IRODS_PLUGINS_DIRECTORY}/microservices"
  )
endforeach()

# --> HDF5
# See issue #48

set(
  IRODS_HDF5_MSI_MODULES
  #msiH5Dataset_read
  #msiH5Dataset_read_attribute
  #msiH5File_close
  #msiH5File_open
  #msiH5Group_read_attribute
)

foreach (module ${IRODS_HDF5_MSI_MODULES})
  set(module_src "${CMAKE_CURRENT_SOURCE_DIR}/microservices/hdf5/src/${module}.cpp")
  add_library(${module} MODULE "${module_src}")
  target_link_libraries(
    ${module}
    PRIVATE
    irods_hdf5_obj
  )
  target_link_objects(
    ${module}
    PRIVATE
    irods_netcdf_obj_server
  )
  target_include_directories(
    ${module}
    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/microservices/hdf5/include"
  )
  target_compile_definitions(${module} PRIVATE RODS_SERVER)

  install(
    TARGETS ${module}
    LIBRARY
    COMPONENT server_modules
    DESTINATION "${IRODS_PLUGINS_DIRECTORY}/microservices"
  )
endforeach()

install(
  DIRECTORY ${CMAKE_SOURCE_DIR}/microservices/rules/
  COMPONENT server_modules
  DESTINATION "${IRODS_HOME_DIRECTORY}/netcdf-plugin-tests"
)

# --> icommands

set(
  IRODS_NETCDF_MSI_ICOMMANDS
  inc
  incarch
  incattr
)

foreach (icommand ${IRODS_NETCDF_MSI_ICOMMANDS})
  set(icommand_src "${CMAKE_CURRENT_SOURCE_DIR}/icommands/src/${icommand}.cpp")
  add_executable(${icommand} "${icommand_src}")
  target_link_objects(
    ${icommand}
    PRIVATE
    irods_netcdf_obj_client
  )

  install(
    TARGETS ${icommand}
    RUNTIME
    COMPONENT icommands
    DESTINATION "${CMAKE_INSTALL_BINDIR}"
  )
endforeach()

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

include(IrodsCPackCommon)

list(APPEND CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION "${CPACK_PACKAGING_INSTALL_PREFIX}var")
list(APPEND CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION "${CPACK_PACKAGING_INSTALL_PREFIX}${IRODS_HOME_DIRECTORY}")
list(APPEND CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION "${CPACK_PACKAGING_INSTALL_PREFIX}${IRODS_PLUGINS_DIRECTORY}")
list(APPEND CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION "${CPACK_PACKAGING_INSTALL_PREFIX}${IRODS_PLUGINS_DIRECTORY}/api")
list(APPEND CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION "${CPACK_PACKAGING_INSTALL_PREFIX}${IRODS_PLUGINS_DIRECTORY}/microservices")

set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
set(CPACK_COMPONENT_INCLUDE_TOPLEVEL_DIRECTORY OFF)

set(CPACK_PACKAGE_VERSION ${IRODS_PLUGIN_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "The integrated Rule-Oriented Data System")


set(CPACK_DEB_COMPONENT_INSTALL ON)
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS OFF)
set(CPACK_DEBIAN_PACKAGE_CONTROL_STRICT_PERMISSION ON)
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_SOURCE_DIR}/packaging/postinst;")
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)

set(CPACK_RPM_COMPONENT_INSTALL ON )
set(CPACK_RPM_PACKAGE_LICENSE "BSD-3-Clause")
set(CPACK_RPM_PACKAGE_AUTOREQ 0)
set(CPACK_RPM_PACKAGE_AUTOPROV 0)
set(CPACK_RPM_POST_INSTALL_SCRIPT_FILE "${CMAKE_SOURCE_DIR}/packaging/postinst")
set(CPACK_RPM_FILE_NAME RPM-DEFAULT)

set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)

set(CPACK_DEBIAN_PACKAGE_NAME "irods-netcdf")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "${IRODS_PACKAGE_DEPENDENCIES_STRING}, irods-runtime (= ${IRODS_VERSION}), libc6")

if (IRODS_LINUX_DISTRIBUTION_NAME STREQUAL "ubuntu")
  if (IRODS_LINUX_DISTRIBUTION_VERSION_MAJOR STREQUAL "22")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "${CPACK_DEBIAN_PACKAGE_DEPENDS}, libnetcdf19, libhdf5-103")
  elseif (IRODS_LINUX_DISTRIBUTION_VERSION_MAJOR STREQUAL "20")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "${CPACK_DEBIAN_PACKAGE_DEPENDS}, libnetcdf15, libhdf5-103")
  elseif (IRODS_LINUX_DISTRIBUTION_VERSION_MAJOR STREQUAL "18")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "${CPACK_DEBIAN_PACKAGE_DEPENDS}, libnetcdf13, libhdf5-100")
  else()
    message(FATAL_ERROR "Building on Ubuntu is supported for these versions only: 18, 20, 22.")
  endif()
endif()

if (IRODS_LINUX_DISTRIBUTION_NAME STREQUAL "debian")
  if (IRODS_LINUX_DISTRIBUTION_VERSION_MAJOR STREQUAL "12")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "${CPACK_DEBIAN_PACKAGE_DEPENDS}, libnetcdf19, libhdf5-103")
  elseif (IRODS_LINUX_DISTRIBUTION_VERSION_MAJOR STREQUAL "11")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "${CPACK_DEBIAN_PACKAGE_DEPENDS}, libnetcdf18, libhdf5-103")
  else()
    message(FATAL_ERROR "Building on Debian is supported for these versions only: 11, 12.")
  endif()
endif()

set(CPACK_RPM_PACKAGE_NAME "irods-netcdf")
set(CPACK_RPM_PACKAGE_REQUIRES "${IRODS_PACKAGE_DEPENDENCIES_STRING}, irods-runtime = ${IRODS_VERSION}, hdf5, netcdf")


set(CPACK_RPM_SERVER_MODULES_PACKAGE_REQUIRES "irods-netcdf-client_modules")
set(CPACK_RPM_ICOMMANDS_PACKAGE_REQUIRES "irods-netcdf-client_modules")

set(CPACK_DEBIAN_SERVER_MODULES_PACKAGE_DEPENDS "irods-netcdf-client_modules")
set(CPACK_DEBIAN_ICOMMANDS_PACKAGE_DEPENDS  "irods-netcdf-client_modules")

include(CPack)

cpack_add_component(icommands)
cpack_add_component(server_modules)
cpack_add_component(client_modules)
