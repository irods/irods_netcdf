
message(STATUS "Test if Netcdf found -- " 
  "\n\tNetCDF include_dirs - "  "(${NetCDF_INCLUDE_DIRS})"
  "\n\tNetCDF libraries    - "  "(${NetCDF_LIBRARIES})"
   #"${NetCDF_VERSION}"
)
message(STATUS "Test if hdf5 found -- " 
  "\n\tHDF5 include dirs - " "${HDF5_INCLUDE_DIRS}" 
  "\n\tHDF5 library dirs - " "${HDF5_LIBRARY_DIRS}" 
  "\n\tHDF5 libraries    - " "${HDF5_LIBRARIES}" 
  "\n\tHDF5 HL libraries - " "${HDF5_HL_LIBRARIES}" 
)

message( STATUS "dist = ${IRODS_LINUX_DISTRIBUTION_NAME}")
