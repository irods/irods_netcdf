/*** Copyright (c), The Regents of the University of California            ***
 *** For more information please refer to files in the COPYRIGHT directory ***/
/* This is script-generated code (for the most part).  */
/* See ncOpen.h for a description of this API call.*/

#include "ncOpen.hpp"
#include "ncClose.hpp"
#include "rodsLog.hpp"
#include "dataObjOpen.hpp"
#include "rsGlobalExtern.hpp"
#include "rcGlobalExtern.hpp"
#include "rsApiHandler.hpp"
#include "objMetaOpr.hpp"
#include "physPath.hpp"
#include "specColl.hpp"
#include "getRemoteZoneResc.hpp"
#include "miscServerFunct.hpp"
#include "irods_server_api_call.hpp"
#include "ncApiIndex.hpp"

#include "irods_get_l1desc.hpp"

#include "ncUtil.hpp"

#ifdef RODS_SERVER
int
_rsNcOpenDataObj( rsComm_t *rsComm, ncOpenInp_t *ncOpenInp, int **ncid ) {
    int remoteFlag;
    rodsServerHost_t *rodsServerHost;
    int status;
    dataObjInp_t dataObjInp;
    int l1descInx = 0, myncid = 0;

    bzero( &dataObjInp, sizeof( dataObjInp ) );
    rstrcpy( dataObjInp.objPath, ncOpenInp->objPath, MAX_NAME_LEN );
    replKeyVal( &ncOpenInp->condInput, &dataObjInp.condInput );
    remoteFlag = getAndConnRemoteZone( rsComm, &dataObjInp, &rodsServerHost,
                                       REMOTE_OPEN );

    if ( remoteFlag < 0 ) {
        return ( remoteFlag );
    }
    else if ( remoteFlag == LOCAL_HOST ) {
        addKeyVal( &dataObjInp.condInput, NO_OPEN_FLAG_KW, "" );
        if ( getValByKey( &ncOpenInp->condInput, NO_STAGING_KW ) != NULL ) {
            addKeyVal( &dataObjInp.condInput, NO_STAGING_KW, "" );
        }

        l1descInx = irods::server_api_call ( DATA_OBJ_OPEN_AN, rsComm, &dataObjInp );
        clearKeyVal( &dataObjInp.condInput );
        if ( l1descInx < 0 ) {
            return l1descInx;
        }

        l1desc_t& my_desc = irods::get_l1desc( l1descInx );

        remoteFlag = resoAndConnHostByDataObjInfo( rsComm,
                     my_desc.dataObjInfo, &rodsServerHost );
        if ( remoteFlag < 0 ) {
            return ( remoteFlag );
        }
        else if ( remoteFlag == LOCAL_HOST ) {
            status = nc_open( my_desc.dataObjInfo->filePath,
                              ncOpenInp->mode, &myncid );
            if ( status != NC_NOERR ) {
                rodsLog( LOG_ERROR,
                         "rsNcOpen: nc_open %s error, status = %d, %s",
                         my_desc.dataObjInfo->filePath, status,
                         nc_strerror( status ) );
                freeL1desc( l1descInx );
                return ( NETCDF_OPEN_ERR + status );
            }
        }
        else {
            /* execute it remotely with dataObjInfo->filePath */
            ncOpenInp_t myNcOpenInp;
            bzero( &myNcOpenInp, sizeof( myNcOpenInp ) );
            myNcOpenInp.mode = ncOpenInp->mode;
            rstrcpy( myNcOpenInp.objPath,
                     my_desc.dataObjInfo->filePath, MAX_NAME_LEN );
            addKeyVal( &myNcOpenInp.condInput, NATIVE_NETCDF_CALL_KW, "" );

            if ( ( status = svrToSvrConnect( rsComm, rodsServerHost ) ) < 0 ) {
                return status;
            }
            status = rcNcOpen( rodsServerHost->conn, &myNcOpenInp, &myncid );
            clearKeyVal( &myNcOpenInp.condInput );
            if ( status < 0 ) {
                rodsLog( LOG_ERROR,
                         "rsNcOpen: rcNcOpen %s error, status = %d",
                         myNcOpenInp.objPath, status );
                freeL1desc( l1descInx );
                return ( status );
            }
        }

        my_desc.l3descInx = myncid;
    }
    else {
        if ( ( status = svrToSvrConnect( rsComm, rodsServerHost ) ) < 0 ) {
            return status;
        }
        status = rcNcOpen( rodsServerHost->conn, ncOpenInp, &myncid );
        if ( status < 0 ) {
            rodsLog( LOG_ERROR,
                     "rsNcOpen: _rcNcOpen %s error, status = %d",
                     ncOpenInp->objPath, status );
            return ( status );
        }
        l1descInx = allocAndSetL1descForZoneOpr( myncid, &dataObjInp,
                    rodsServerHost, NULL );
    }


    l1desc_t& my_desc = irods::get_l1desc( l1descInx );
    if ( ncOpenInp->mode == NC_NOWRITE ) {
        my_desc.oprType = NC_OPEN_FOR_READ;
    }
    else {
        my_desc.oprType = NC_OPEN_FOR_WRITE;
    }
    *ncid = ( int * ) malloc( sizeof( int ) );
    *( *ncid ) = l1descInx;

    return 0;
}

int
_rsNcOpenColl( rsComm_t *rsComm, ncOpenInp_t *ncOpenInp, int **ncid ) {
    int status;
    ncAggInfo_t *ncAggInfo = NULL;
    int l1descInx;
    openedAggInfo_t newOpenedAggInfo;

    status = readAggInfo( rsComm, ncOpenInp->objPath, &ncOpenInp->condInput,
                          &ncAggInfo );
    if ( status < 0 ) {
        return status;
    }

    l1descInx = allocL1desc();
    if ( l1descInx < 0 ) {
        return l1descInx;
    }

    l1desc_t& my_desc = irods::get_l1desc( l1descInx );
    openedAggInfo_t * openedAggInfo = boost::any_cast< openedAggInfo_t >( &my_desc.pluginData );
    if(openedAggInfo == NULL)
    {
        openedAggInfo = &newOpenedAggInfo;
    }
    bzero( openedAggInfo, sizeof( openedAggInfo_t ) );
    openedAggInfo->ncAggInfo = ncAggInfo;
    openedAggInfo->objNcid = -1;	/* not opened */
    openedAggInfo->objNcid0 = -1;	/* not opened */
    my_desc.pluginData = openedAggInfo;
    status = openAggrFile( rsComm, l1descInx, 0 );
    if ( status < 0 ) {
        return status;
    }
    *ncid = ( int * ) malloc( sizeof( int ) );
    *( *ncid ) = l1descInx;

    return 0;
}

int
openAggrFile( rsComm_t *rsComm, int l1descInx, int aggElementInx ) {
    int status, status1;
    ncOpenInp_t ncOpenInp;
    ncCloseInp_t ncCloseInp;
    openedAggInfo_t *openedAggInfo;
    openedAggInfo_t newOpenedAggInfo;
    int *ncid = NULL;

    l1desc_t& my_desc = irods::get_l1desc( l1descInx );
    openedAggInfo = boost::any_cast< openedAggInfo_t >( &my_desc.pluginData );
    if(openedAggInfo == NULL)
    {
        openedAggInfo = &newOpenedAggInfo;
    }
    if ( aggElementInx > 0 && aggElementInx == openedAggInfo->aggElementInx ) {
        return 0;
    }
    bzero( &ncOpenInp, sizeof( ncOpenInp ) );
    rstrcpy( ncOpenInp.objPath,
             openedAggInfo->ncAggInfo->ncAggElement[aggElementInx].objPath,
             MAX_NAME_LEN );
    status = _rsNcOpenDataObj( rsComm, &ncOpenInp, &ncid );
    if ( status >= 0 ) {
        if ( aggElementInx > 0 && openedAggInfo->aggElementInx > 0 ) {
            bzero( &ncCloseInp, sizeof( ncCloseInp ) );
            ncCloseInp.ncid = openedAggInfo->objNcid;
            status1 = irods::server_api_call ( NC_CLOSE_AN, rsComm, &ncCloseInp );
            if ( status1 < 0 ) {
                rodsLogError( LOG_ERROR, status1,
                              "openAndInqAggrFile: rcNcClose error for %s",
                              openedAggInfo->ncAggInfo->ncObjectName );
            }
            if ( openedAggInfo->ncInqOut != NULL ) {
                freeNcInqOut( &openedAggInfo->ncInqOut );
            }
        }
        if ( aggElementInx == 0 ) {
            openedAggInfo->objNcid0 = *ncid;
        }
        else {
            openedAggInfo->objNcid = *ncid;
            openedAggInfo->aggElementInx = aggElementInx;
        }
        my_desc.pluginData = openedAggInfo;
        free( ncid );
    }
    else {
        rodsLogError( LOG_ERROR, status,
                      "openAndInqAggrFile: rsNcOpen error for %s",
                      openedAggInfo->ncAggInfo->ncAggElement[aggElementInx].objPath );
        return status;
    }
    return status;
}
#endif // RODS_SERVER

extern "C" {
#ifdef RODS_SERVER
    int
    rsNcOpen( rsComm_t *rsComm, ncOpenInp_t *ncOpenInp, int **ncid ) {
        int status;
        int myncid;
        specCollCache_t *specCollCache = NULL;

        if ( getValByKey( &ncOpenInp->condInput, NATIVE_NETCDF_CALL_KW ) != NULL ) {
            /* just all nc_open with objPath as file nc file path */
            /* must to be called internally */
            if ( rsComm->proxyUser.authInfo.authFlag < REMOTE_PRIV_USER_AUTH ) {
                return( CAT_INSUFFICIENT_PRIVILEGE_LEVEL );
            }
            status = nc_open( ncOpenInp->objPath, ncOpenInp->mode, &myncid );
            if ( status == NC_NOERR ) {
                *ncid = ( int * ) malloc( sizeof( int ) );
                *( *ncid ) = myncid;
                return 0;
            }
            else {
                rodsLog( LOG_ERROR,
                         "rsNcOpen: nc_open %s error, status = %d, %s",
                         ncOpenInp->objPath, status, nc_strerror( status ) );
                return ( NETCDF_OPEN_ERR + status );
            }
        }
        resolveLinkedPath( rsComm, ncOpenInp->objPath, &specCollCache,
                           &ncOpenInp->condInput );

        if ( isColl( rsComm, ncOpenInp->objPath, NULL ) >= 0 ) {
            status = _rsNcOpenColl( rsComm, ncOpenInp, ncid );
        }
        else {
            status = _rsNcOpenDataObj( rsComm, ncOpenInp, ncid );
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
        irods::apidef_t def = { NC_OPEN_AN,
                                RODS_API_VERSION,
                                REMOTE_USER_AUTH,
                                REMOTE_USER_AUTH,
                                "NcOpenInp_PI", 0,
                                "INT_PI", 0,
                                0, 0 // null fcn ptr, handled in delay_load
                              };
        // =-=-=-=-=-=-=-
        // create an api object
        irods::api_entry* api = new irods::api_entry( def );

        // =-=-=-=-=-=-=-
        // assign the fcn which will handle the api call
#ifdef RODS_SERVER
        api->fcn_name_ = "rsNcOpen";
#endif // RODS_SERVER

        // =-=-=-=-=-=-=-
        // assign the pack struct key and value
        api->in_pack_key   = "NcOpenInp_PI";
        api->in_pack_value = NcOpenInp_PI;

        // =-=-=-=-=-=-=-
        // and... were done.
        return api;

    } // plugin_factory

}; // extern "C"
