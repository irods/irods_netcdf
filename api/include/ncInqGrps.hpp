/*** Copyright (c), The Regents of the University of California            ***
 *** For more information please refer to files in the COPYRIGHT directory ***/
/* ncInqGrp.h
 */

#ifndef NC_INQ_GRPS_HPP
#define NC_INQ_GRPS_HPP

/* This is a NETCDF API call */

#include <irods/rods.h>
#include <irods/rcMisc.h>
#include <irods/procApiRequest.h>
#include <irods/apiNumber.h>
#include <irods/initServer.hpp>
#include <irods/dataObjInpOut.h>
#include "ncOpen.hpp"

typedef struct {
    int ncid;
    int flags;
    char name[MAX_NAME_LEN];  /* for NC_ALL_FLAG == 0, the name */
    keyValPair_t condInput;
} ncInqGrpsInp_t;

#define NcInqGrpsInp_PI "int ncid; int flags; str name[MAX_NAME_LEN]; struct KeyValPair_PI;"

typedef struct {
    int ngrps;
    int flags;		/* not used */
    char **grpName;	/* array of grpNcid */
} ncInqGrpsOut_t;

#define NcInqGrpsOut_PI "int ngrps; int flags; str *grpName[ngrps];"

#if defined(RODS_SERVER)
#define RS_NC_INQ_GRPS rsNcInqGrps
/* prototype for the server handler */
extern "C" int
rsNcInqGrps( rsComm_t *rsComm, ncInqGrpsInp_t *ncInqGrpsInp,
             ncInqGrpsOut_t **ncInqGrpsOut );
int
_rsNcInqGrps( int ncid, ncInqGrpsOut_t **ncInqGrpsOut );
#else
#define RS_NC_INQ_GRPS NULL
#endif

extern "C" {

    /* rcNcInqGrps - Given a location id or the root id and the absolute group
     * path, return the number of groups it contains, and an array of their ncids.

     * Input -
     *   rcComm_t *conn - The client connection handle.
     *   ncInqGrpInp_t struct:
     *     ncid - the the ncid.
     * OutPut - ncInqGrpOut_t.
     */
    /* prototype for the client call */
    int
    rcNcInqGrps( rcComm_t *conn, ncInqGrpsInp_t *ncInqGrpsInp,
                 ncInqGrpsOut_t **ncInqGrpsOut );
    int
    freeNcInqGrpsOut( ncInqGrpsOut_t **ncInqGrpsOut );
}

#endif	/* NC_INQ_GRPS_H */
