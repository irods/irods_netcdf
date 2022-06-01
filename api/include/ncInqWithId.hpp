/*** Copyright (c), The Regents of the University of California            ***
 *** For more information please refer to files in the COPYRIGHT directory ***/
/* ncInqId.h
 */

#ifndef NC_INQ_WITH_ID_HPP
#define NC_INQ_WITH_ID_HPP

/* This is a NETCDF API call */

#include <irods/rods.h>
#include <irods/rcMisc.h>
#include <irods/procApiRequest.h>
#include <irods/apiNumber.h>
#include <irods/initServer.hpp>
#include <irods/dataObjInpOut.h>
#include "ncInqId.hpp"

typedef struct {
    rodsLong_t mylong;	/* an int output.content depends on paramType.For
			 * NC_DIM_T, this is arrayLen. not used for NC_VAR_T */
    int dataType;		/* data type for NC_VAR_T. */
    int natts;		/* no. of attrs for NC_VAR_T */
    char name[MAX_NAME_LEN];
    int myint;		/* not used */
    int ndim;		/* no. of dimension */
    int *intArray;	/* int array of ndim length */
} ncInqWithIdOut_t;

#define NcInqWithIdOut_PI "double mylong; int dataType; int natts; str name[MAX_NAME_LEN];int myint; int ndim; int *intArray(ndim);"
#if defined(RODS_SERVER)
#define RS_NC_INQ_WITH_ID rsNcInqWithId
/* prototype for the server handler */
extern "C" int
rsNcInqWithId( rsComm_t *rsComm, ncInqIdInp_t *ncInqWithIdInp,
               ncInqWithIdOut_t **ncInqWithIdOut );
int
_rsNcInqWithId( int type, int ncid, int myid, char *name,
                ncInqWithIdOut_t **ncInqWithIdOut );
int
_rsNcInqWithIdColl( rsComm_t *rsComm, ncInqIdInp_t *ncInqWithIdInp,
                   ncInqWithIdOut_t **ncInqWithIdOut );
int
_rsNcInqWithIdDataObj( rsComm_t *rsComm, ncInqIdInp_t *ncInqWithIdInp,
                      ncInqWithIdOut_t **ncInqWithIdOut );
#else
#define RS_NC_INQ_WITH_ID NULL
#endif

extern "C" {

    /* rcNcInqWithId - general netcdf inq with id (equivalent to nc_inq_dim,
     *    ....
     * Input -
     *   rcComm_t *conn - The client connection handle.
     *   ncInqIdInp_t struct:
     *     paramType - parameter type - NC_DIM_T, ....
     *     ncid - int ncid.
     *     myid - depends on paramType.  dimid for NC_DIM_T,
     * OutPut - ncInqWithIdOut_t
     *		mylong- content depends on paramType. length for NC_DIM_T
     *		name - content depends on paramType. name of dim for NC_DIM_T
     */
    /* prototype for the client call */
    int
    rcNcInqWithId( rcComm_t *conn, ncInqIdInp_t *ncInqWithIdInp,
                   ncInqWithIdOut_t **ncInqWithIdOut );

}

#endif	/* NC_INQ_WITH_ID_H */
