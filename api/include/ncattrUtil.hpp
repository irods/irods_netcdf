/*** Copyright (c), The Regents of the University of California            ***
 *** For more information please refer to files in the COPYRIGHT directory ***/
/* ncattrUtil.h - Header for for ncattrUtil.c */

#ifndef NCATTRUTIL_HPP
#define NCATTRUTIL_HPP

#include <irods/rodsClient.h>
#include <irods/parseCommandLine.h>
#include <irods/rodsPath.h>
#include "ncRegGlobalAttr.hpp"

extern "C" {

#define MAX_ATTR_NAME_PER_LINE		4

    int
    ncattrUtil( rcComm_t *conn, rodsEnv *myRodsEnv, rodsArguments_t *myRodsArgs,
                rodsPathInp_t *rodsPathInp );
    int
    regAttrDataObjUtil( rcComm_t *conn, char *srcPath,
                        rodsEnv *myRodsEnv, rodsArguments_t *rodsArgs,
                        ncRegGlobalAttrInp_t *ncRegGlobalAttrInp );
    int
    initCondForNcattr( rodsEnv *myRodsEnv, rodsArguments_t *rodsArgs,
                       ncRegGlobalAttrInp_t *ncRegGlobalAttrInp );
    int
    regAttrCollUtil( rcComm_t *conn, char *srcColl, rodsEnv *myRodsEnv,
                     rodsArguments_t *rodsArgs, ncRegGlobalAttrInp_t *ncRegGlobalAttrInp );
    int
    rmAttrDataObjUtil( rcComm_t *conn, char *srcPath,
                       rodsEnv *myRodsEnv, rodsArguments_t *rodsArgs,
                       ncRegGlobalAttrInp_t *ncRegGlobalAttrInp );
    int
    rmAttrCollUtil( rcComm_t *conn, char *srcColl, rodsEnv *myRodsEnv,
                    rodsArguments_t *rodsArgs, ncRegGlobalAttrInp_t *ncRegGlobalAttrInp );
    int
    listAttrDataObjUtil( rcComm_t *conn, char *srcPath,
                         rodsEnv *myRodsEnv, rodsArguments_t *rodsArgs );
    int
    listAttrCollUtil( rcComm_t *conn, char *srcColl, rodsEnv *myRodsEnv,
                      rodsArguments_t *rodsArgs );
    int
    queryDataObjForAUV( rcComm_t *conn, char *objPath, rodsArguments_t *rodsArgs,
                        genQueryOut_t **genQueryOut );
    int
    queryAUVForDataObj( rcComm_t *conn, char *collPath, rodsEnv *myRodsEnv, rodsArguments_t *rodsArgs );
}

#endif	/* NCATTRUTIL_H */
