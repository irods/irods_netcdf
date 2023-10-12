/*** Copyright (c), The Regents of the University of California            ***
 *** For more information please refer to files in the COPYRIGHT directory ***/
/* This is script-generated code (for the most part).  */
/* See ncArchTimeSeries.h for a description of this API call.*/

#include "netcdf_port.hpp"
#include "ncArchTimeSeries.hpp"
#include "rodsLog.h"
#include "dataObjOpen.h"
#include "rsGlobalExtern.hpp"
#include "rcGlobalExtern.h"
#include "rsApiHandler.hpp"
#include "objMetaOpr.hpp"
#include "physPath.hpp"
#include "specColl.hpp"
#include "getRemoteZoneResc.h"
#include "miscServerFunct.hpp"
#include "dataObjCreate.h"
#include "ncGetAggInfo.hpp"
#include "ncClose.hpp"
#include "ncInq.hpp"
#include "regDataObj.h"
#include "ncUtil.hpp"
#include "irods_get_l1desc.hpp"
#include "irods_resource_backport.hpp"
#include "ncApiIndex.hpp"
#include "irods_server_api_call.hpp"
#include "irods_resource_redirect.hpp"

#ifdef RODS_SERVER

int
_rsNcArchTimeSeries( rsComm_t *rsComm,
                     ncArchTimeSeriesInp_t *ncArchTimeSeriesInp ) {
    int status;
    int dimInx, varInx;
    char *tmpStr;
    dataObjInp_t dataObjInp;
    unsigned int endTime;
    ncOpenInp_t ncOpenInp;
    ncCloseInp_t ncCloseInp;
    int *ncid = NULL;
    ncInqInp_t ncInqInp;
    ncInqOut_t *ncInqOut = NULL;
    ncAggInfo_t *ncAggInfo = NULL;
    rodsLong_t startTimeInx, endTimeInx;
    rodsLong_t fileSizeLimit;

    bzero( &dataObjInp, sizeof( dataObjInp ) );
    rstrcpy( dataObjInp.objPath, ncArchTimeSeriesInp->aggCollection,
             MAX_NAME_LEN );
    replKeyVal( &ncArchTimeSeriesInp->condInput, &dataObjInp.condInput );
    
    int               local = LOCAL_HOST;
    rodsServerHost_t* host  =  0;
    std::string       resc_hier;
    if ( getValByKey( &dataObjInp.condInput, RESC_HIER_STR_KW ) == NULL ) {
        irods::error ret = irods::resource_redirect(
                               irods::OPEN_OPERATION,
                               rsComm,
                               &dataObjInp,
                               resc_hier,
                               host,
                               local );
        if ( !ret.ok() ) {
            std::stringstream msg;
            msg << "failed for [";
            msg << dataObjInp.objPath << "]";
            irods::log( PASSMSG( msg.str(), ret ) );
            return ret.code();
        }

        // =-=-=-=-=-=-=-
        // we resolved the redirect and have a host, set the hier str for subsequent
        // api calls, etc.
        addKeyVal(
            &dataObjInp.condInput,
            RESC_HIER_STR_KW,
            resc_hier.c_str() );

    } // if keyword

    if ( LOCAL_HOST != local ) {
        status = rcNcArchTimeSeries(
                     host->conn,
                     ncArchTimeSeriesInp );
        return status;
    }

    bzero( &ncOpenInp, sizeof( ncOpenInp_t ) );
    rstrcpy( ncOpenInp.objPath, ncArchTimeSeriesInp->objPath, MAX_NAME_LEN );
#ifdef NETCDF4_API
    ncOpenInp.mode = NC_NOWRITE | NC_NETCDF4;
#else
    ncOpenInp.mode = NC_NOWRITE;
#endif
    addKeyVal( &ncOpenInp.condInput, NO_STAGING_KW, "" );

    status = irods::server_api_call ( NC_OPEN_AN, rsComm, &ncOpenInp, &ncid );
    clearKeyVal( &ncOpenInp.condInput );
    if ( status < 0 ) {
        rodsLogError( LOG_ERROR, status,
                      "_rsNcArchTimeSeries: rsNcOpen error for %s", ncOpenInp.objPath );
        return status;
    }
    bzero( &ncInqInp, sizeof( ncInqInp ) );
    ncInqInp.ncid = *ncid;
    bzero( &ncCloseInp, sizeof( ncCloseInp_t ) );
    ncCloseInp.ncid = *ncid;
    free( ncid );
    ncInqInp.paramType = NC_ALL_TYPE;
    ncInqInp.flags = NC_ALL_FLAG;
    status = irods::server_api_call ( NC_INQ_AN, rsComm, &ncInqInp, &ncInqOut );
    if ( status < 0 ) {
        rodsLogError( LOG_ERROR, status,
                      "_rsNcArchTimeSeries: rcNcInq error for %s", ncOpenInp.objPath );
        irods::server_api_call ( NC_CLOSE_AN, rsComm, &ncCloseInp );
        return status;
    }
    for ( dimInx = 0; dimInx < ncInqOut->ndims; dimInx++ ) {
        if ( strcasecmp( ncInqOut->dim[dimInx].name, "time" ) == 0 ) {
            break;
        }
    }
    if ( dimInx >= ncInqOut->ndims ) {
        /* no match */
        rodsLog( LOG_ERROR,
                 "_rsNcArchTimeSeries: 'time' dim does not exist for %s",
                 ncOpenInp.objPath );
        irods::server_api_call ( NC_CLOSE_AN, rsComm, &ncCloseInp );
        freeNcInqOut( &ncInqOut );
        return NETCDF_DIM_MISMATCH_ERR;
    }
    for ( varInx = 0; varInx < ncInqOut->nvars; varInx++ ) {
        if ( strcmp( ncInqOut->dim[dimInx].name, ncInqOut->var[varInx].name )
                == 0 ) {
            break;
        }
    }
    if ( varInx >= ncInqOut->nvars ) {
        /* no match */
        rodsLog( LOG_ERROR,
                 "_rsNcArchTimeSeries: 'time' var does not exist for %s",
                 ncOpenInp.objPath );
        irods::server_api_call ( NC_CLOSE_AN, rsComm, &ncCloseInp );
        freeNcInqOut( &ncInqOut );
        return NETCDF_DIM_MISMATCH_ERR;
    }

    if ( ncInqOut->var[varInx].nvdims != 1 ) {
        rodsLog( LOG_ERROR,
                 "_rsNcArchTimeSeries: 'time' .nvdims = %d is not 1 for %s",
                 ncInqOut->var[varInx].nvdims, ncOpenInp.objPath );
        irods::server_api_call ( NC_CLOSE_AN, rsComm, &ncCloseInp );
        freeNcInqOut( &ncInqOut );
        return NETCDF_DIM_MISMATCH_ERR;
    }

    if ( ( tmpStr = getValByKey( &ncArchTimeSeriesInp->condInput,
                                 NEW_NETCDF_ARCH_KW ) ) != NULL ) {
        /* this is a new archive */
        startTimeInx = strtoll( tmpStr, 0, 0 );
    }
    else {
        status = readAggInfo( rsComm, ncArchTimeSeriesInp->aggCollection,
                              NULL, &ncAggInfo );
        if ( status < 0 ) {
            irods::server_api_call ( NC_CLOSE_AN, rsComm, &ncCloseInp );
            freeNcInqOut( &ncInqOut );
            return status;
        }
        endTime = ncAggInfo->ncAggElement[ncAggInfo->numFiles - 1].endTime;

        status = getTimeInxForArch( rsComm, ncInqInp.ncid, ncInqOut, dimInx,
                                    varInx, endTime, &startTimeInx );
        if ( status < 0 ) {
            irods::server_api_call ( NC_CLOSE_AN, rsComm, &ncCloseInp );
            freeNcInqOut( &ncInqOut );
            freeAggInfo( &ncAggInfo );
            return status;
        }
    }
    endTimeInx = ncInqOut->dim[dimInx].arrayLen - 1;

    if ( ncArchTimeSeriesInp->fileSizeLimit > 0 ) {
        fileSizeLimit = ncArchTimeSeriesInp->fileSizeLimit * ONE_MILLION;
    }
    else {
        fileSizeLimit = ARCH_FILE_SIZE;
    }

    l1desc_t& my_desc = irods::get_l1desc( ncInqInp.ncid );
    status = archPartialTimeSeries(
                 rsComm, ncInqOut, ncAggInfo,
                 my_desc.l3descInx, varInx,
                 ncArchTimeSeriesInp->aggCollection, 
                 resc_hier, startTimeInx,
                 endTimeInx, fileSizeLimit );
    irods::server_api_call ( NC_CLOSE_AN, rsComm, &ncCloseInp );
    freeNcInqOut( &ncInqOut );
    freeAggInfo( &ncAggInfo );

    if ( status >= 0 ) {
        /* update agginfo */
        rstrcpy( ncOpenInp.objPath,  ncArchTimeSeriesInp->aggCollection,
                 MAX_NAME_LEN );
        ncOpenInp.mode = NC_WRITE;
        status = irods::server_api_call ( NC_GET_AGG_INFO_AN, rsComm, &ncOpenInp, &ncAggInfo );
        if ( status < 0 ) {
            rodsLogError( LOG_ERROR, status,
                          "_rsNcArchTimeSeries: rsNcGetAggInfo error for %s",
                          ncOpenInp.objPath );
        }
        else {
            freeAggInfo( &ncAggInfo );
        }
    }
    return status;
}

int archPartialTimeSeries(
    rsComm_t *rsComm,
    ncInqOut_t *ncInqOut,
    ncAggInfo_t *ncAggInfo,
    int srcNcid,
    int timeVarInx,
    char *aggCollection,
    const std::string& resc_hier,
    rodsLong_t startTimeInx,
    rodsLong_t endTimeInx,
    rodsLong_t archFileSize ) {
    dataObjInp_t dataObjInp;
    int status = 0, l1descInx;
    rodsLong_t curTimeInx = startTimeInx;
    ncVarSubset_t ncVarSubset;
    openedDataObjInp_t dataObjCloseInp;
    dataObjInfo_t *myDataObjInfo;
    int nextNumber;
    char basePath[MAX_NAME_LEN];
    int inxInterval;
    rodsLong_t timeStepSize;

    timeStepSize = getTimeStepSize( ncInqOut );
    if ( timeStepSize < 0 ) {
        status = timeStepSize;
        rodsLogError( LOG_ERROR, status,
                      "archPartialTimeSeries: getTimeStepSize error for %s",
                      aggCollection );
        return status;
    }
    inxInterval = archFileSize / timeStepSize + 1;
    bzero( &dataObjInp, sizeof( dataObjInp ) );
    bzero( &ncVarSubset, sizeof( ncVarSubset ) );
    ncVarSubset.numVar = 0;
    ncVarSubset.numSubset = 1;
    rstrcpy( ncVarSubset.ncSubset[0].subsetVarName,
             ncInqOut->var[timeVarInx].name, NAME_LEN );
    ncVarSubset.ncSubset[0].stride = 1;
    addKeyVal( &dataObjInp.condInput, NO_OPEN_FLAG_KW, "" );
    if ( ncAggInfo == NULL ) {
        nextNumber = 0;
        status = getAggBasePath( aggCollection, basePath );
        if ( status < 0 ) {
            return status;
        }
    }
    else {
        nextNumber = getNextAggEleObjPath( ncAggInfo, aggCollection, basePath );
        if ( nextNumber < 0 ) {
            return nextNumber;
        }
    }
    while ( curTimeInx < endTimeInx ) {
        rodsLong_t remainingInx;
        snprintf( dataObjInp.objPath, MAX_NAME_LEN, "%s%-d", basePath,
                  nextNumber );
        nextNumber++;
        int  _rsDataObjCreateWithResc(rsComm_t * , dataObjInp_t* , std::string const &);
        l1descInx = _rsDataObjCreateWithResc(
                        rsComm,
                        &dataObjInp,
                        resc_hier );
        if ( l1descInx < 0 ) {
            return l1descInx;
        }
        l1desc_t& my_desc = irods::get_l1desc( l1descInx );
        myDataObjInfo = my_desc.dataObjInfo;
        rstrcpy( myDataObjInfo->dataType, "netcdf", NAME_LEN );
        memset( &dataObjCloseInp, 0, sizeof( dataObjCloseInp ) );
        dataObjCloseInp.l1descInx = l1descInx;
        ncVarSubset.ncSubset[0].start = curTimeInx;
        /* if it is close enough, just do all of it */
        remainingInx = endTimeInx - curTimeInx + 1;
        if ( ( inxInterval + inxInterval / 2 + 1 ) >= remainingInx ) {
            inxInterval = remainingInx;
        }
        if ( curTimeInx + inxInterval > endTimeInx ) {
            ncVarSubset.ncSubset[0].end = endTimeInx;
        }
        else {
            ncVarSubset.ncSubset[0].end = curTimeInx + inxInterval - 1;
        }
        curTimeInx = ncVarSubset.ncSubset[0].end + 1;

        mkDirForFilePath(
            rsComm, 0,
            myDataObjInfo->filePath,
            myDataObjInfo->rescHier,
            getDefDirMode() );
        status = dumpSubsetToFile( NULL, srcNcid, 0, ncInqOut, &ncVarSubset,
                                   my_desc.dataObjInfo->filePath );
        if ( status >= 0 ) {
            my_desc.bytesWritten = 1;
        }
        else {
            rodsLogError( LOG_ERROR, status,
                          "archPartialTimeSeries: rsRegDataObj for %s failed, status = %d",
                          myDataObjInfo->objPath, status );
            my_desc.oprStatus = status;
            irods::server_api_call ( DATA_OBJ_CLOSE_AN, rsComm, &dataObjCloseInp );
            return ( status );
        }
        int svrRegDataObj(rsComm_t*, DataObjInfo*);
        status = svrRegDataObj( rsComm, myDataObjInfo );
        if ( status < 0 ) {
            rodsLogError( LOG_ERROR, status,
                          "archPartialTimeSeries: rsRegDataObj for %s failed, status = %d",
                          myDataObjInfo->objPath, status );
            my_desc.oprStatus = status;
            irods::server_api_call ( DATA_OBJ_CLOSE_AN, rsComm, &dataObjCloseInp );
            return ( status );
        }
        else {
            myDataObjInfo->replNum = status;
        }
        my_desc.oprStatus = status;
        irods::server_api_call ( DATA_OBJ_CLOSE_AN, rsComm, &dataObjCloseInp );
    }

    return status;
}

int
getTimeInxForArch( rsComm_t *rsComm, int ncid, ncInqOut_t *ncInqOut,
                   int dimInx, int varInx, unsigned int prevEndTime, rodsLong_t *startTimeInx ) {
    rodsLong_t start[1], count[1], stride[1];
    rodsLong_t timeArrayRemain, readCount;
    ncGetVarInp_t ncGetVarInp;
    ncGetVarOut_t *ncGetVarOut = NULL;
    void *bufPtr;
    int i, status;
    unsigned int myTime;


    /* read backward, READ_TIME_SIZE at a time until it is <= prevEndTime */
    timeArrayRemain = ncInqOut->dim[dimInx].arrayLen;
    if ( timeArrayRemain <= READ_TIME_SIZE ) {
        readCount = timeArrayRemain;
    }
    else {
        readCount = READ_TIME_SIZE;
    }
    bzero( &ncGetVarInp, sizeof( ncGetVarInp ) );
    ncGetVarInp.dataType = ncInqOut->var[varInx].dataType;
    ncGetVarInp.ncid = ncid;
    ncGetVarInp.varid =  ncInqOut->var[varInx].id;
    ncGetVarInp.ndim =  ncInqOut->var[varInx].nvdims;
    ncGetVarInp.start = start;
    ncGetVarInp.count = count;
    ncGetVarInp.stride = stride;

    while ( timeArrayRemain > 0 ) {
        int goodInx = -1;
        timeArrayRemain -= readCount;
        start[0] = timeArrayRemain;
        count[0] = readCount;
        stride[0] = 1;

        status = irods::server_api_call ( NC_GET_VARS_BY_TYPE_AN, rsComm, &ncGetVarInp, &ncGetVarOut );
        if ( status < 0 ) {
            rodsLogError( LOG_ERROR, status,
                          "dumpNcInqOut: rcNcGetVarsByType error for %s",
                          ncInqOut->var[varInx].name );
            return status;
        }
        bufPtr = ncGetVarOut->dataArray->buf;

        for ( i = 0; i < ncGetVarOut->dataArray->len; i++ ) {
            myTime = ncValueToInt( ncGetVarOut->dataArray->type, &bufPtr );
            if ( myTime < 0 ) {
                /* XXXX close and clear */
                return myTime;
            }
            if ( myTime > prevEndTime ) {
                break;
            }
            goodInx = i;
        }
        if ( goodInx >= 0 ) {
            *startTimeInx = timeArrayRemain + goodInx + 1;
            return 0;
        }
        if ( timeArrayRemain <= READ_TIME_SIZE ) {
            readCount = timeArrayRemain;
        }
        else {
            readCount = READ_TIME_SIZE;
        }
    }
    *startTimeInx = 0;
    return NETCDF_DIM_MISMATCH_ERR;
}
#endif // RODS_SERVER
extern "C" {
    double get_plugin_interface_version() { return 1.0; }
#ifdef RODS_SERVER
    int
    rsNcArchTimeSeries( rsComm_t *rsComm,
                        ncArchTimeSeriesInp_t *ncArchTimeSeriesInp ) {
        int remoteFlag;
        rodsServerHost_t *rodsServerHost;
        int status;
        dataObjInp_t dataObjInp;
        specCollCache_t *specCollCache = NULL;

        if ( getValByKey( &ncArchTimeSeriesInp->condInput, NATIVE_NETCDF_CALL_KW )
                != NULL ) {
            /* must to be called internally */
            if ( rsComm->proxyUser.authInfo.authFlag < REMOTE_PRIV_USER_AUTH ) {
                return( CAT_INSUFFICIENT_PRIVILEGE_LEVEL );
            }
            status = _rsNcArchTimeSeries( rsComm, ncArchTimeSeriesInp );
        }
        else {
            resolveLinkedPath( rsComm, ncArchTimeSeriesInp->objPath,
                               &specCollCache, &ncArchTimeSeriesInp->condInput );
            bzero( &dataObjInp, sizeof( dataObjInp ) );
            rstrcpy( dataObjInp.objPath, ncArchTimeSeriesInp->objPath,
                     MAX_NAME_LEN );
            replKeyVal( &ncArchTimeSeriesInp->condInput, &dataObjInp.condInput );
            remoteFlag = getAndConnRemoteZone( rsComm, &dataObjInp, &rodsServerHost,
                                               REMOTE_CREATE );
            clearKeyVal( &dataObjInp.condInput );
            if ( remoteFlag < 0 ) {
                return ( remoteFlag );
            }
            else if ( remoteFlag == LOCAL_HOST ) {
                status = _rsNcArchTimeSeries( rsComm, ncArchTimeSeriesInp );
            }
            else {
                if ( ( status = svrToSvrConnect( rsComm, rodsServerHost ) ) < 0 ) {
                    return status;
                }
                status = rcNcArchTimeSeries( rodsServerHost->conn,
                                             ncArchTimeSeriesInp );
            }
        }
        return status;
    }

#endif // RODS_SERVER



    // =-=-=-=-=-=-=-
    // factory function to provide instance of the plugin
    irods::api_entry* plugin_factory(
        const std::string& _inst_name,
        const std::string& _context ) {
        // =-=-=-=-=-=-=-
        // create a api def object
//      irods::apidef_t def = { NC_ARCH_TIME_SERIES_AN,
 //                             RODS_API_VERSION,
  //                            REMOTE_USER_AUTH,
   //                           REMOTE_USER_AUTH,
    //                          "NcArchTimeSeriesInp_PI", 0,
     //                         NULL, 0,
      //                        0, 0
       //                     }; // null fcn ptr, handled in delay_load

        irods::apidef_t def = { NC_ARCH_TIME_SERIES_AN,
                                RODS_API_VERSION,
                                REMOTE_USER_AUTH,
                                REMOTE_USER_AUTH,
                                "NcArchTimeSeriesInp_PI", 0,
                                NULL, 0,
#ifdef RODS_SERVER
                                CPP_14_FUNCTION( rsNcArchTimeSeries ),
#else
                                CPP_14_NOOPFUNC( rsComm_t* , ncArchTimeSeriesInp_t* ),
#endif // RODS_SERVER
                                "api_nc_arch_time_series",
                                [](void*){},
                                irods::clearOutStruct_noop,
                                (funcPtr) RODS_SERVER_ENABLE( (irods::netcdf::api_call_wrapper< ncArchTimeSeriesInp_t* >) )
                              };
        // =-=-=-=-=-=-=-
        // create an api object
        irods::api_entry* api = new irods::api_entry( def );

        // =-=-=-=-=-=-=-
        // assign the pack struct key and value
        api->in_pack_key   = "NcArchTimeSeriesInp_PI";
        api->in_pack_value = NcArchTimeSeriesInp_PI;
        
        // =-=-=-=-=-=-=-
        // and... were done.
        return api;

    } // plugin_factory

}; // extern "C"
