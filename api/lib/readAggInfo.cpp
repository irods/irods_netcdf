

#ifdef RODS_SERVER



#include "ncUtil.hpp"
#include "dataObjGet.h"
#include "irods_server_api_call.hpp"
#include "ncApiIndex.hpp"
#include "ncApiIndex.hpp"
#include "rodsPackTable.h"

int
readAggInfo( rsComm_t*     rsComm, 
             char*         aggColl, 
             keyValPair_t* condInput,
             ncAggInfo_t** ncAggInfo ) {
    int status;
    dataObjInp_t dataObjInp;
    bytesBuf_t packedBBuf;
    portalOprOut_t *portalOprOut = NULL;

    bzero( &dataObjInp, sizeof( dataObjInp ) );
    bzero( &packedBBuf, sizeof( packedBBuf ) );
    if ( condInput != NULL ) {
        replKeyVal( condInput, &dataObjInp.condInput );
    }
    snprintf( dataObjInp.objPath, MAX_NAME_LEN, "%s/%s",
              aggColl, NC_AGG_INFO_FILE_NAME );
    dataObjInp.oprType = GET_OPR;
    status = irods::server_api_call ( DATA_OBJ_GET_AN, rsComm, &dataObjInp, &portalOprOut, &packedBBuf );
    clearKeyVal( &dataObjInp.condInput );
    if ( portalOprOut != NULL ) {
        free( portalOprOut );
    }
    if ( status < 0 ) {
        if ( status == CAT_NO_ROWS_FOUND ) {
            status = NETCDF_AGG_INFO_FILE_ERR;
        }
        rodsLogError( LOG_ERROR, status,
                      "readAggInfo: rsDataObjGet error for %s", dataObjInp.objPath );
        return status;
    }
    status = unpack_struct( packedBBuf.buf, ( void ** ) ncAggInfo,
                            "NcAggInfo_PI", RodsPackTable, XML_PROT, nullptr );

    if ( status < 0 ) {
        rodsLogError( LOG_ERROR, status,
                      "readAggInfo: unpackStruct error for %s", dataObjInp.objPath );
        return NETCDF_AGG_INFO_FILE_ERR;
    }
    if ( *ncAggInfo == NULL || ( *ncAggInfo )->numFiles == 0 ) {
        rodsLog( LOG_ERROR,
                 "readAggInfo: No ncAggInfo for %s", dataObjInp.objPath );
        return NETCDF_AGG_INFO_FILE_ERR;
    }
    return status;
}

#endif // RODS_SERVER


