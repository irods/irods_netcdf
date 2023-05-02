/*** Copyright (c), The Regents of the University of California            ***
 *** For more information please refer to files in the COPYRIGHT directory ***/
/* ncGetAggInfo.h
 */

#ifndef NC_GET_AGG_INFO_HPP
#define NC_GET_AGG_INFO_HPP

/* This is a NETCDF API call */

#include <irods/rods.h>
#include <irods/rcMisc.h>
#include <irods/procApiRequest.h>
#include <irods/apiNumber.h>
#include <irods/initServer.hpp>
#include <irods/dataObjInpOut.h>
#include "ncOpen.hpp"
#include "ncGetAggElement.hpp"
#include "ncInq.hpp"
#include "netcdf.h"

/* The file name for the aggregate info */
#define NC_AGG_INFO_FILE_NAME	".aggInfo"

/* data struct for aggregation of netcdf files. Our first attempt assumes
 * the aggregation is based on the time dimension - time series */

typedef struct {
    int numFiles;
    int flags;		/* not used */
    char ncObjectName[MAX_NAME_LEN];
    ncAggElement_t *ncAggElement;	/* pointer to numFiles of
                                         * ncAggElement_t */
} ncAggInfo_t;

#define NcAggInfo_PI       "int numFiles; int flags; str  ncObjectName[MAX_NAME_LEN]; struct *NcAggElement_PI(numFiles);"

struct openedAggInfo_t {
    int aggElementInx;	/* index into the ncAggElement of objNcid */
    int objNcid0;	/* the opened object L1desc for element 0 */
    int objNcid;        /* the opened object L1desc */
    ncAggInfo_t *ncAggInfo;
    ncInqOut_t *ncInqOut0;	/* ncInqOut for objNcid0 */
    ncInqOut_t *ncInqOut;	/* ncInqOut for objNcid */
    openedAggInfo_t& operator =(const openedAggInfo_t& _rhs) {
        aggElementInx = _rhs.aggElementInx;
        objNcid0 = _rhs.objNcid0;
        objNcid = _rhs.objNcid;
        ncAggInfo = _rhs.ncAggInfo;
        ncInqOut0 = _rhs.ncInqOut0;
        ncInqOut = _rhs.ncInqOut;
        return *this;
    }
    openedAggInfo_t(const openedAggInfo_t& _rhs) {
        aggElementInx = _rhs.aggElementInx;
        objNcid0 = _rhs.objNcid0;
        objNcid = _rhs.objNcid;
        ncAggInfo = _rhs.ncAggInfo;
        ncInqOut0 = _rhs.ncInqOut0;
        ncInqOut = _rhs.ncInqOut;
    }
    openedAggInfo_t() :
    aggElementInx(0),
    objNcid0(-1),
    objNcid(-1),
    ncAggInfo(NULL),
    ncInqOut0(NULL),
    ncInqOut(NULL)
    {
    }
};

#if defined(RODS_SERVER)
#define RS_NC_GET_AGG_INFO rsNcGetAggInfo
/* prototype for the server handler */
extern "C" int
rsNcGetAggInfo( rsComm_t *rsComm, ncOpenInp_t *ncOpenInp,
                ncAggInfo_t **ncAggInfo );
#else
#define RS_NC_GET_AGG_INFO NULL
#endif

extern "C" {

    /* rcNcGetAgInfo - get the ncAggInfo of a NETCDF file
     * Input -
     *   rcComm_t *conn - The client connection handle.
     *   ncOpenInp_t *ncOpenInp - generic nc open/create input. Relevant items are:
     *	objPath - the path of the NETCDF aggregate collection.
     *      mode - NC_WRITE - write the ncAggInfo to a file named ncAggInfo.
     *	condInput - condition input (not used).
     * OutPut -
     *   ncAggInfo_t **ncAggInfo - the ncAggInfo of the NETCDF data object.
     */

    /* prototype for the client call */
    int
    rcNcGetAggInfo( rcComm_t *conn, ncOpenInp_t *ncOpenInp,
                    ncAggInfo_t **ncAggInfo );

    int
    addNcAggElement( ncAggElement_t *ncAggElement, ncAggInfo_t *ncAggInfo );
    rodsLong_t
    sumAggElementArraylen( ncAggInfo_t *ncAggInfo, int aggElementInx );
    int
    freeAggInfo( ncAggInfo_t **ncAggInfo );
    int
    getAggBasePath( char *aggCollection, char *basePath );
    int
    getNextAggEleObjPath( ncAggInfo_t *ncAggInfo, char *aggCollection,
                          char *nextObjPath );
}

#endif	/* NC_GET_AGG_INFO_H */
