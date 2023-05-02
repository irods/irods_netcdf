/*** Copyright (c), The Regents of the University of California            ***
 *** For more information please refer to files in the COPYRIGHT directory ***/
/* ncGetAggElement.h
 */

#ifndef NC_GET_AGG_ELEMENT_HPP
#define NC_GET_AGG_ELEMENT_HPP

/* This is a NETCDF API call */

#include <irods/rods.h>
#include <irods/rcMisc.h>
#include <irods/procApiRequest.h>
#include <irods/apiNumber.h>
#include <irods/initServer.hpp>
#include <irods/dataObjInpOut.h>
#include "ncOpen.hpp"
#include "netcdf.h"

/* data struct for aggregation of netcdf files. Our first attempt assumes
 * the aggregation is based on the time dimension - time series */
typedef struct {
    unsigned int startTime;
    unsigned int endTime;
    char astartTime[NAME_LEN];
    char aendTime[NAME_LEN];
    rodsLong_t arraylen;
    char objPath[MAX_NAME_LEN];
} ncAggElement_t;

#define NcAggElement_PI       "int startTime; int endTime; str astartTime[NAME_LEN]; str aendTime[NAME_LEN]; double arraylen; str objPath[MAX_NAME_LEN];"

#if defined(RODS_SERVER)
#define RS_NC_GET_AGG_ELEMENT rsNcGetAggElement
/* prototype for the server handler */
extern "C" int
rsNcGetAggElement( rsComm_t *rsComm, ncOpenInp_t *ncOpenInp,
                   ncAggElement_t **ncAggElement );
#else
#define RS_NC_GET_AGG_ELEMENT NULL
#endif

extern "C" {

    /* rcNcGetAggElement - get the ncAggElement of a NETCDF file
     * Input -
     *   rcComm_t *conn - The client connection handle.
     *   ncOpenInp_t *ncOpenInp - generic nc open/create input. Relevant items are:
     *	objPath - the path of the NETCDF data object.
     *	condInput - condition input (not used).
     * OutPut -
     *   ncAggElement_t **ncAggElement - the ncAggElement of the NETCDF data object.
     */

    /* prototype for the client call */
    int
    rcNcGetAggElement( rcComm_t *conn, ncOpenInp_t *ncOpenInp,
                       ncAggElement_t **ncAggElement );

}

#endif	/* NC_GET_AGG_ELEMENT_H */
