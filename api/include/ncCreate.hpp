/*** Copyright (c), The Regents of the University of California            ***
 *** For more information please refer to files in the COPYRIGHT directory ***/
/* ncCreate.h
 */

#ifndef NC_CREATE_HPP
#define NC_CREATE_HPP

/* This is a NETCDF API call */

#include <irods/rods.h>
#include <irods/rcMisc.h>
#include <irods/procApiRequest.h>
#include <irods/apiNumber.h>
#include <irods/initServer.hpp>
#include <irods/dataObjInpOut.h>
#include "ncOpen.hpp"

#define NC_CREATE 1002 // create NETCDF

#if defined(RODS_SERVER)
#define RS_NC_CREATE rsNcCreate
/* prototype for the server handler */
extern "C" int
rsNcCreate( rsComm_t *rsComm, ncOpenInp_t *ncCreateInp, int **ncid );
#else
#define RS_NC_CREATE NULL
#endif


extern "C" {

    /* prototype for the client call */
    /* rcNcCreate - netcdf create an iRODS data object (equivalent to nc_open.
     * Input -
     *   rcComm_t *conn - The client connection handle.
     *   ncOpenInp_t *ncCreateInp - generic nc open/create input. Relevant items are:
     *	objPath - the path of the data object.
     *      mode - the mode of the create - valid values are given in netcdf.h -
     *       NC_NOCLOBBER, NC_SHARE, NC_64BIT_OFFSET, NC_NETCDF4, NC_CLASSIC_MODEL.
     *	condInput - condition input (not used).
     * OutPut -
     *   int the ncid of the opened object - an integer descriptor.
     */

    int
    rcNcCreate( rcComm_t *conn, ncOpenInp_t *ncCreateInp, int *ncid );

}

#endif	/* NC_CREATE_H */
