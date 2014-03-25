/*** Copyright (c), The Regents of the University of California            ***
 *** For more information please refer to files in the COPYRIGHT directory ***/
/* ncUtil.h - Header for for ncUtil.c */

#ifndef NCUTIL_HPP
#define NCUTIL_HPP

#include "rodsClient.hpp"
#include "parseCommandLine.hpp"
#include "rodsPath.hpp"
#include "ncInq.hpp"
#include "ncGetVarsByType.hpp"
#include "ncClose.hpp"
#include "ncOpen.hpp"
#include "ncGetAggInfo.hpp"
#include "netcdf.h"

extern "C" {

    int readAggInfo( rsComm_t *rsComm, char *aggColl, keyValPair_t *condInput, ncAggInfo_t **ncAggInfo );

    int
    ncUtil( rcComm_t *conn, rodsEnv *myRodsEnv, rodsArguments_t *myRodsArgs,
            rodsPathInp_t *rodsPathInp );
    int
    ncOperDataObjUtil( rcComm_t *conn, char *srcPath,
                       rodsEnv *myRodsEnv, rodsArguments_t *rodsArgs,
                       ncOpenInp_t *ncOpenInp, ncVarSubset_t *ncVarSubset );
    int
    initCondForNcOper( rodsEnv *myRodsEnv, rodsArguments_t *rodsArgs,
                       ncOpenInp_t *ncOpenInp, ncVarSubset_t *ncVarSubset );
    int
    ncOperCollUtil( rcComm_t *conn, char *srcColl, rodsEnv *myRodsEnv,
                    rodsArguments_t *rodsArgs, ncOpenInp_t *ncOpenInp, ncVarSubset_t *ncVarSubset );
    int
    catAggInfo( rcComm_t *conn, char *srcColl );
    int
    setAggInfo( rcComm_t *conn, char *srcColl, rodsEnv *myRodsEnv,
                rodsArguments_t *rodsArgs, ncOpenInp_t *ncOpenInp );
}

#endif	/* NCUTIL_H */
