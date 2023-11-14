#ifndef NETCDF_CORE_DEFS_H__
#define NETCDF_CORE_DEFS_H__

#include <irods/rods.h>
#include <irods/objInfo.h>
#include <irods/dataObjInpOut.h>

extern "C" int catDataObj( rcComm_t *conn, char *objPath );


#endif   // NETCDF_CORE_DEFS_H__
