/*** Copyright (c), The Regents of the University of California            ***
 *** For more information please refer to files in the COPYRIGHT directory ***/
/* This is script-generated code (for the most part).  */
/* See dataObjGet.h for a description of this API call.*/

#include "netcdf_port.hpp"
#include "ncClose.hpp"
#include "rodsLog.h"
#include "dataObjClose.h"
#include "rsGlobalExtern.hpp"
#include "rcGlobalExtern.h"
#include "rsApiHandler.hpp"
#include "objMetaOpr.hpp"
#include "physPath.hpp"
#include "specColl.hpp"
#include "getRemoteZoneResc.h"
#include "irods_get_l1desc.hpp"
#include "ncApiIndex.hpp"
#include "irods_server_api_call.hpp"
#include "ncGetAggInfo.hpp"

#ifdef RODS_SERVER
static int ncCloseColl( rsComm_t *rsComm, int l1descInx );
static int ncCloseDataObj( rsComm_t *rsComm, int l1descInx );

static int closeAggrFiles( rsComm_t *rsComm, int l1descInx ) {
    int status;
    openedAggInfo_t *openedAggInfo;
    int savedStatus = 0;

    l1desc_t& my_desc = irods::get_l1desc( l1descInx );
    openedAggInfo = boost::any_cast< openedAggInfo_t >( &my_desc.pluginData );
    if(openedAggInfo == NULL)
    {
        status = -1;
        rodsLogError( LOG_ERROR, status,
                      "closeAggrFiles: rcNcClose error - no associated openedAggInfo" );
        savedStatus = status;
    }
    if ( openedAggInfo->aggElementInx > 0 && openedAggInfo->objNcid >= 0 ) {
        status = ncCloseDataObj( rsComm, openedAggInfo->objNcid );
        if ( status < 0 ) {
            rodsLogError( LOG_ERROR, status,
                          "closeAggrFiles: rcNcClose error for objNcid %d",
                          openedAggInfo->objNcid );
            savedStatus = status;
        }
    }
    if ( openedAggInfo->objNcid0 >= 0 ) {
        status = ncCloseDataObj( rsComm,  openedAggInfo->objNcid0 );
        if ( status < 0 ) {
            rodsLogError( LOG_ERROR, status,
                          "closeAggrFiles: rcNcClose error for objNcid0 %d",
                          openedAggInfo->objNcid0 );
            savedStatus = status;
        }
    }
    openedAggInfo->aggElementInx = openedAggInfo->objNcid =
                                      openedAggInfo->objNcid0 = -1;
    return savedStatus;
}

static int ncCloseColl( rsComm_t *rsComm, int l1descInx ) {
    openedDataObjInp_t dataObjCloseInp;
    int status = 0;

    status = closeAggrFiles( rsComm, l1descInx );
    if ( status < 0 ) {
        rodsLogError( LOG_ERROR, status,
                      "ncCloseColl: closeAggrFiles error" );
    }
    l1desc_t& my_desc = irods::get_l1desc( l1descInx );
    openedAggInfo_t * openedAggInfo = boost::any_cast< openedAggInfo_t >( &my_desc.pluginData );
    if(openedAggInfo != NULL)
    {
        freeAggInfo( &openedAggInfo->ncAggInfo );
        freeNcInqOut( &openedAggInfo->ncInqOut0 );
        freeNcInqOut( &openedAggInfo->ncInqOut );
    }

    my_desc.l3descInx = 0;
    bzero( &dataObjCloseInp, sizeof( dataObjCloseInp ) );
    dataObjCloseInp.l1descInx = l1descInx;
    status = irods::server_api_call ( DATA_OBJ_CLOSE_AN, rsComm, &dataObjCloseInp );
    if ( status < 0 ) {
        rodsLog( LOG_ERROR,
                 "ncCloseColl: rsDataObjClose %d error, status = %d",
                 l1descInx, status );
    }
    return status;
}

static int ncCloseDataObj( rsComm_t *rsComm, int l1descInx ) {
    int remoteFlag;
    rodsServerHost_t *rodsServerHost = NULL;
    ncCloseInp_t myNcCloseInp;
    openedDataObjInp_t dataObjCloseInp;
    int status = 0;

    l1desc_t& my_desc = irods::get_l1desc( l1descInx );
    remoteFlag = resoAndConnHostByDataObjInfo( rsComm,
                 my_desc.dataObjInfo, &rodsServerHost );
    if ( remoteFlag < 0 ) {
        return ( remoteFlag );
    }
    else if ( remoteFlag == LOCAL_HOST ) {
        status = nc_close( my_desc.l3descInx );
        if ( status != NC_NOERR ) {
            rodsLog( LOG_ERROR,
                     "ncCloseDataObj: nc_close %d for %s error, status = %d, %s",
                     my_desc.l3descInx,
                     my_desc.dataObjInfo->objPath, status,
                     nc_strerror( status ) );
            freeL1desc( l1descInx );
            return ( NETCDF_CLOSE_ERR + status );
        }
        my_desc.l3descInx = 0;
    }
    else {
        /* execute it remotely */
        bzero( &myNcCloseInp, sizeof( myNcCloseInp ) );
        myNcCloseInp.ncid = my_desc.l3descInx;
        addKeyVal( &myNcCloseInp.condInput, NATIVE_NETCDF_CALL_KW, "" );
        status = rcNcClose( rodsServerHost->conn, &myNcCloseInp );
        clearKeyVal( &myNcCloseInp.condInput );
        if ( status < 0 ) {
            rodsLog( LOG_ERROR,
                     "ncCloseDataObj: rcNcClose %d for %s error, status = %d",
                     my_desc.l3descInx,
                     my_desc.dataObjInfo->objPath, status );
            freeL1desc( l1descInx );
            return ( status );
        }
    }
    
    my_desc.l3descInx = 0;
    bzero( &dataObjCloseInp, sizeof( dataObjCloseInp ) );
    dataObjCloseInp.l1descInx = l1descInx;
    status = irods::server_api_call ( DATA_OBJ_CLOSE_AN, rsComm, &dataObjCloseInp );
    if ( status < 0 ) {
        rodsLog( LOG_ERROR,
                 "ncCloseDataObj: rcNcClose %d error, status = %d",
                 l1descInx, status );
    }
    return status;
}

#endif // RODS_SERVER

extern "C" {
    double get_plugin_interface_version() { return 1.0; }
#ifdef RODS_SERVER
    int
    rsNcClose( rsComm_t *rsComm, ncCloseInp_t *ncCloseInp ) {
        int l1descInx;
        ncCloseInp_t myNcCloseInp;
        int status = 0;

        if ( getValByKey( &ncCloseInp->condInput, NATIVE_NETCDF_CALL_KW ) != NULL ) {
            /* just do nc_close */
            status = nc_close( ncCloseInp->ncid );
            if ( status != NC_NOERR ) {
                rodsLog( LOG_ERROR,
                         "rsNcClose: nc_close %d error, status = %d, %s",
                         ncCloseInp->ncid, status, nc_strerror( status ) );
                status = NETCDF_CLOSE_ERR + status;
            }
            return status;
        }
        l1descInx = ncCloseInp->ncid;

        if ( l1descInx < 2 || l1descInx >= NUM_L1_DESC ) {
            rodsLog( LOG_ERROR,
                     "rsNcClose: l1descInx %d out of range",
                     l1descInx );
            return ( SYS_FILE_DESC_OUT_OF_RANGE );
        }
    
        l1desc_t& my_desc = irods::get_l1desc( l1descInx );
        if ( my_desc.inuseFlag != FD_INUSE ) {
            return BAD_INPUT_DESC_INDEX;
        }
        if ( my_desc.remoteZoneHost != NULL ) {
            bzero( &myNcCloseInp, sizeof( myNcCloseInp ) );
            myNcCloseInp.ncid = my_desc.remoteL1descInx;
            /* cross zone operation */
            status = rcNcClose( my_desc.remoteZoneHost->conn,
                                &myNcCloseInp );
            /* the remote zone resc will do the registration */
            freeL1desc( l1descInx );
        }
        else {
            if ( my_desc.oprType == NC_OPEN_GROUP ) {
                /* group open. Just free the L1desc */
                freeL1desc( l1descInx );
                return 0;
            }

            openedAggInfo_t * openedAggInfo = boost::any_cast< openedAggInfo_t >( &my_desc.pluginData );
            if ( openedAggInfo != NULL && openedAggInfo->ncAggInfo != NULL ) {
                status = ncCloseColl( rsComm, l1descInx );
            }
            else {
                status = ncCloseDataObj( rsComm, l1descInx );
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

        irods::apidef_t def = { NC_CLOSE_AN,
                                RODS_API_VERSION,
                                REMOTE_USER_AUTH,
                                REMOTE_USER_AUTH,
                                "NcCloseInp_PI", 0,
                                NULL, 0,
#ifdef RODS_SERVER
                                CPP_14_FUNCTION(rsNcClose ),
#else
                                CPP_14_NOOPFUNC( rsComm_t*, ncCloseInp_t* ),
#endif // RODS_SERVER
                                "api_nc_close",
                                [](void*){},
                                (funcPtr) RODS_SERVER_ENABLE(( irods::netcdf::api_call_wrapper<  ncCloseInp_t* > ))

                              }; // null fcn ptr, handled in delay_load
        // =-=-=-=-=-=-=-
        // create an api object
        irods::api_entry* api = new irods::api_entry( def );

        // =-=-=-=-=-=-=-
        // assign the pack struct key and value
        api->in_pack_key   = "NcCloseInp_PI";
        api->in_pack_value = NcCloseInp_PI;
        
        // =-=-=-=-=-=-=-
        // and... were done.
        return api;

    } // plugin_factory

}; // extern "C"
