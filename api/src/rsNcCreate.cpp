/*** Copyright (c), The Regents of the University of California            ***
 *** For more information please refer to files in the COPYRIGHT directory ***/
/* This is script-generated code (for the most part).  */
/* See dataObjGet.h for a description of this API call.*/

#include <netcdf_port.hpp>
#include "ncCreate.hpp"
#include "ncClose.hpp"
#include "ncApiIndex.hpp"
#include "ncUtil.hpp"
#include <irods/rodsLog.h>
#include <irods/dataObjCreate.h>
#include <irods/rsGlobalExtern.hpp>
#include <irods/rcGlobalExtern.h>
#include <irods/rsApiHandler.hpp>
#include <irods/objMetaOpr.hpp>
#include <irods/dataObjUnlink.h>
#include <irods/physPath.hpp>
#include <irods/specColl.hpp>
#include <irods/regDataObj.h>
#include <irods/getRemoteZoneResc.h>
#include <irods/irods_get_l1desc.hpp>
#include <irods/irods_server_api_call.hpp>

#ifdef RODS_SERVER
#include <irods/rsDataObjUnlink.hpp>
#include <irods/rsRegDataObj.hpp>
#endif

extern "C" {
    double get_plugin_interface_version() { return 1.0; }
#ifdef RODS_SERVER
    int
    rsNcCreate( rsComm_t *rsComm, ncOpenInp_t *ncCreateInp, int **ncid ) {
        int remoteFlag;
        rodsServerHost_t *rodsServerHost;
        specCollCache_t *specCollCache = NULL;
        int status;
        dataObjInp_t dataObjInp;
        int l1descInx, myncid;

        if ( getValByKey( &ncCreateInp->condInput, NATIVE_NETCDF_CALL_KW ) != NULL ) {
            /* just do nc_open with objPath as file nc file path */
            /* must to be called internally */
            if ( rsComm->proxyUser.authInfo.authFlag < REMOTE_PRIV_USER_AUTH ) {
                return( CAT_INSUFFICIENT_PRIVILEGE_LEVEL );
            }
            status = nc_create( ncCreateInp->objPath, ncCreateInp->mode, &myncid );
            if ( status == NC_NOERR ) {
                *ncid = ( int * ) malloc( sizeof( int ) );
                *( *ncid ) = myncid;
                return 0;
            }
            else {
                rodsLog( LOG_ERROR,
                         "rsNccreate: nc_create %s error, status = %d, %s",
                         ncCreateInp->objPath, status, nc_strerror( status ) );
                return ( NETCDF_OPEN_ERR + status );
            }
        }
        bzero( &dataObjInp, sizeof( dataObjInp ) );
        rstrcpy( dataObjInp.objPath, ncCreateInp->objPath, MAX_NAME_LEN );
        replKeyVal( &ncCreateInp->condInput, &dataObjInp.condInput );
        resolveLinkedPath( rsComm, dataObjInp.objPath, &specCollCache,
                           &dataObjInp.condInput );
        remoteFlag = getAndConnRemoteZone( rsComm, &dataObjInp, &rodsServerHost,
                                           REMOTE_OPEN );

        if ( remoteFlag < 0 ) {
            return ( remoteFlag );
        }
        else if ( remoteFlag == LOCAL_HOST ) {
            addKeyVal( &dataObjInp.condInput, NO_OPEN_FLAG_KW, "" );
            int _rsDataObjCreate(rsComm_t*, DataObjInp*);
            l1descInx = _rsDataObjCreate( rsComm, &dataObjInp );
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
                status = nc_create( my_desc.dataObjInfo->filePath,
                                    ncCreateInp->mode, &myncid );
                if ( status != NC_NOERR ) {
                    rodsLog( LOG_ERROR,
                             "rsNcCreate: nc_open %s error, status = %d, %s",
                             ncCreateInp->objPath, status, nc_strerror( status ) );
                    freeL1desc( l1descInx );
                    return ( NETCDF_CREATE_ERR + status );
                }
            }
            else {
                /* execute it remotely with dataObjInfo->filePath */
                ncOpenInp_t myNcCreateInp;
                bzero( &myNcCreateInp, sizeof( myNcCreateInp ) );
                rstrcpy( myNcCreateInp.objPath,
                         my_desc.dataObjInfo->filePath, MAX_NAME_LEN );
                addKeyVal( &myNcCreateInp.condInput, NATIVE_NETCDF_CALL_KW, "" );
                status = rcNcCreate( rodsServerHost->conn, &myNcCreateInp, &myncid );
                clearKeyVal( &myNcCreateInp.condInput );
                if ( status < 0 ) {
                    rodsLog( LOG_ERROR,
                             "rsNcCreate: _rcNcCreate %s error, status = %d",
                             myNcCreateInp.objPath, status );
                    freeL1desc( l1descInx );
                    return ( status );
                }
            }
            my_desc.l3descInx = myncid;
            /* need to reg here since NO_OPEN_FLAG_KW does not do it */
            if ( my_desc.dataObjInfo->specColl == NULL ) {
                status = svrRegDataObj( rsComm, my_desc.dataObjInfo );
                if ( status < 0 ) {
                    ncCloseInp_t myNcCloseInp;
                    bzero( &myNcCloseInp, sizeof( myNcCloseInp ) );
                    myNcCloseInp.ncid = l1descInx;
                    irods::server_api_call ( NC_CLOSE_AN, rsComm, &myNcCloseInp );
                    l3Unlink( rsComm, my_desc.dataObjInfo );
                    rodsLog( LOG_ERROR,
                             "rsNcCreate: svrRegDataObj for %s failed, status = %d",
                             my_desc.dataObjInfo->objPath, status );
                    freeL1desc( l1descInx );
                    return ( NETCDF_CREATE_ERR + status );
                }
            }
        }
        else {
            addKeyVal( &dataObjInp.condInput, CROSS_ZONE_CREATE_KW, "" );
            status = rcNcCreate( rodsServerHost->conn, ncCreateInp, &myncid );
            /* rm it to avoid confusion */
            rmKeyVal( &dataObjInp.condInput, CROSS_ZONE_CREATE_KW );
            if ( status < 0 ) {
                rodsLog( LOG_ERROR,
                         "rsNcCreate: _rcNcCreate %s error, status = %d",
                         ncCreateInp->objPath, status );
                return ( status );
            }
            l1descInx = allocAndSetL1descForZoneOpr( myncid, &dataObjInp,
                        rodsServerHost, NULL );
        }
        
        l1desc_t& my_desc = irods::get_l1desc( l1descInx );
        my_desc.oprType = NC_CREATE;
        *ncid = ( int * ) malloc( sizeof( int ) );
        *( *ncid ) = l1descInx;

        return 0;
    }
#endif // RODS_SERVER

    // =-=-=-=-=-=-=-
    // factory function to provide instance of the plugin
    irods::api_entry* plugin_factory(
        const std::string& _inst_name,
        const std::string& _context ) {
        // =-=-=-=-=-=-=-
        // create a api def object
//      irods::apidef_t def = { NC_CREATE_AN,
 //                             RODS_API_VERSION,
  //                            REMOTE_USER_AUTH,
   //                           REMOTE_USER_AUTH,
    //                          "NcOpenInp_PI", 0,
     //                         NULL, 0,
      //                        0, 0
       //                     }; // null fcn ptr, handled in delay_load

        irods::apidef_t def = { NC_CREATE_AN,
                                RODS_API_VERSION,
                                REMOTE_USER_AUTH,
                                REMOTE_USER_AUTH,
                                "NcOpenInp_PI", 0,
                                NULL, 0,
#ifdef RODS_SERVER
                                CPP_14_FUNCTION(rsNcCreate),
#else
                                CPP_14_NOOPFUNC( rsComm_t*, ncOpenInp_t * , int ** ),
#endif // RODS_SERVER
                                "api_nc_create",
                                [](void*){},
                                irods::clearOutStruct_noop,
                                (funcPtr) RODS_SERVER_ENABLE(( irods::netcdf::api_call_wrapper< ncOpenInp_t *, int ** > ))
                              };

        // =-=-=-=-=-=-=-
        // create an api object
        irods::api_entry* api = new irods::api_entry( def );

        // =-=-=-=-=-=-=-
        // assign the pack struct key and value
        api->in_pack_key   = "NcOpenInp_PI";
        api->in_pack_value = NcOpenInp_PI;
        
        // =-=-=-=-=-=-=-
        // and... were done.
        return api;

    } // plugin_factory




}; // extern "C"

