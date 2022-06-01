/*** Copyright (c), The Regents of the University of California            ***
 *** For more information please refer to files in the COPYRIGHT directory ***/
/* ncInqId.h
 */

#ifndef NC_INQ_ID_HPP
#define NC_INQ_ID_HPP

/* This is a NETCDF API call */

#include <irods/rods.h>
#include <irods/rcMisc.h>
#include <irods/procApiRequest.h>
#include <irods/apiNumber.h>
#include <irods/initServer.hpp>
#include <irods/dataObjInpOut.h>
#include "ncOpen.hpp"

/* definition for paramType */
#define NC_VAR_T		0	/* nc variable */
#define NC_DIM_T		1	/* nc dimension */
typedef struct {
    int paramType;
    int ncid;
    int myid;		/* used by some APIs such as rcNcInqWithId */
    int flags;		/* not used */
    char name[MAX_NAME_LEN];
    keyValPair_t condInput;
} ncInqIdInp_t;

#define NcInqIdInp_PI "int paramType; int ncid; int myId; int flags; str name[MAX_NAME_LEN]; struct KeyValPair_PI;"
#if defined(RODS_SERVER)
#define RS_NC_INQ_ID rsNcInqId
/* prototype for the server handler */
extern "C" int
rsNcInqId( rsComm_t *rsComm, ncInqIdInp_t *ncInqIdInp, int **outId );
int
_rsNcInqId( int type, int ncid, char *name, int **outId );
int
_rsNcInqIdDataObj( rsComm_t *rsComm, ncInqIdInp_t *ncInqIdInp, int **outId );
int
_rsNcInqIdColl( rsComm_t *rsComm, ncInqIdInp_t *ncInqIdInp, int **outId );
#else
#define RS_NC_INQ_ID NULL
#endif

extern "C" {

    /* rcNcInqId - general netcdf inq for id (equivalent to nc_inq_dimid,
     *    nc_inq_varid, ....
     * Input -
     *   rcComm_t *conn - The client connection handle.
     *   ncInqIdInp_t struct:
     *     paramType - parameter type - NC_VAR_T, NC_DIM_T, ....
     *     ncid - the the ncid.
     * OutPut -
     *     id - the nc location id. varid for NC_VAR_T, dimid for NC_DIM_T,
     */
    /* prototype for the client call */
    int
    rcNcInqId( rcComm_t *conn, ncInqIdInp_t *ncInqIdInp, int **outId );

}

#endif	/* NC_INQ_ID_H */
