# irods_netcdf

API plugins, microservices, and commands for accessing NetCDF data/metadata contained in iRODS data objects.

## Building the NetCDF Plugin

Dependencies for building and running the plugin are as follows:

   - irods-externals-cmake
   - irods-externals-clang
   - irods-externals-clang-runtime
   - irods-externals-boost

Also make sure the development packages for OpenSSL, NetCDF, and HDF5, as well as iRODS itself, are installed on the build machine.

After cloning the `irods_netcdf` repository locally, perform these steps to build:

```sh
mkdir build ; cd build
cmake /path/to/netcdf_repository
make -j package
```

This will produce the following package files (in .rpm or .deb format):
   1. irods-netcdf-server_modules
   2. irods-netcdf-client_modules
   3. irods-netcdf-icommands

Note:

Dependencies are set such that both the server_modules and icommands packages depend on client_modules.
The remaining prerequisites, ie. the openssl and netcdf libraries, can be installed via yum or apt-get via the
default repositories.

## Installing from the Package Repository

The NetCDF plugin may be installed using the OS distribution's dependency-aware package tool:

  `{apt|yum|dnf} install irods-netcdf-*`
