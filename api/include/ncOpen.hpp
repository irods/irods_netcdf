/*** Copyright (c), The Regents of the University of California            ***
 *** For more information please refer to files in the COPYRIGHT directory ***/
/* ncOpen.h
 */

#ifndef NC_OPEN_HPP
#define NC_OPEN_HPP

/* This is a NETCDF API call */

#include <irods/rods.h>
#include <irods/rcMisc.h>
#include <irods/procApiRequest.h>
//#include <irods/apiNumber.hpp>
//#include <irods/initServer.hpp>
#include <irods/dataObjInpOut.h>
#include "netcdf.h"

typedef struct {
    char objPath[MAX_NAME_LEN];	/* for ncOpenGroup, this is the full group
				 * path */
    int mode;
    int rootNcid;		/* used only for ncOpenGroup */
    rodsULong_t	intialsz;	/* used for nc__open, nc__create */
    rodsULong_t bufrsizehint;	/* used for nc__open, nc__create */
    keyValPair_t condInput;	/* not used */
} ncOpenInp_t;

#define NC_OPEN_FOR_READ  1001 // open NETCDF for read
#define NC_OPEN_FOR_WRITE 1000 // open/create NETCDF for write
#define NcOpenInp_PI "str objPath[MAX_NAME_LEN]; int mode; int rootNcid; double intialsz; double bufrsizehint; struct KeyValPair_PI;"

#if defined(RODS_SERVER)
#define RS_NC_OPEN rsNcOpen
/* prototype for the server handler */
extern "C" int
rsNcOpen( rsComm_t *rsComm, ncOpenInp_t *ncOpenInp, int **ncid );
int
_rsNcOpenDataObj( rsComm_t *rsComm, ncOpenInp_t *ncOpenInp, int **ncid );
int
_rsNcOpenColl( rsComm_t *rsComm, ncOpenInp_t *ncOpenInp, int **ncid );
int
openAggrFile( rsComm_t *rsComm, int l1descInx, int aggElementInx );
#else
#define RS_NC_OPEN NULL
#endif

extern "C" {

    /* prototype for the client call */
    /* rcNcOpen - netcdf open an iRODS data object (equivalent to nc_open.
     * Input -
     *   rcComm_t *conn - The client connection handle.
     *   ncOpenInp_t *ncOpenInp - generic nc open/create input. Relevant items are:
     *	objPath - the path of the data object.
     *      mode - the mode of the open - valid values are given in netcdf.h -
     *       NC_NOWRITE (0), NC_WRITE (1), NC_NETCDF4, ...
     *	condInput - condition input (not used).
     * OutPut -
     *   int the ncid of the opened object - an integer descriptor.
     */

    int
    rcNcOpen( rcComm_t *conn, ncOpenInp_t *ncOpenInp, int *ncid );
    int
    _rcNcOpen( rcComm_t *conn, ncOpenInp_t *ncOpenInp, int **ncid );

}

#endif	/* NC_OPEN_H */
