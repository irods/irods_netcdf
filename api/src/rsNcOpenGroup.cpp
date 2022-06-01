/*** Copyright (c), The Regents of the University of California            ***
 *** For more information please refer to files in the COPYRIGHT directory ***/
/* This is script-generated code (for the most part).  */
/* See dataObjGet.h for a description of this API call.*/

#include "netcdf_port.hpp"
#include "ncOpenGroup.hpp"
#include <irods/rodsLog.h>
#include <irods/dataObjOpen.h>
#include <irods/rsGlobalExtern.hpp>
#include <irods/rcGlobalExtern.h>
#include <irods/rsApiHandler.hpp>
#include <irods/objMetaOpr.hpp>
#include <irods/physPath.hpp>
#include <irods/specColl.hpp>
#include <irods/getRemoteZoneResc.h>
#include <irods/irods_get_l1desc.hpp>
#include "ncApiIndex.hpp"
#include "ncUtil.hpp"

extern "C" {
    double get_plugin_interface_version() { return 1.0; }
#ifdef RODS_SERVER
    int
    rsNcOpenGroup( rsComm_t *rsComm, ncOpenInp_t *ncOpenGroupInp, int **ncid ) {
        int remoteFlag;
        rodsServerHost_t *rodsServerHost;
        int status;
        dataObjInfo_t *dataObjInfo = NULL;
        int rl1descInx, l1descInx, myncid;
        int rootNcid;

        if ( getValByKey( &ncOpenGroupInp->condInput, NATIVE_NETCDF_CALL_KW ) != NULL ) {
            /* just all nc_open with objPath as file nc file path */
            /* must to be called internally */
            if ( rsComm->proxyUser.authInfo.authFlag < REMOTE_PRIV_USER_AUTH ) {
                return( CAT_INSUFFICIENT_PRIVILEGE_LEVEL );
            }
            status = nc_inq_grp_full_ncid( ncOpenGroupInp->rootNcid,
                                           ncOpenGroupInp->objPath, &myncid );
            if ( status == NC_NOERR ) {
                *ncid = ( int * ) malloc( sizeof( int ) );
                *( *ncid ) = myncid;
                return 0;
            }
            else {
                rodsLog( LOG_ERROR,
                         "rsNcOpenGroup: nc_open %s error, status = %d, %s",
                         ncOpenGroupInp->objPath, status, nc_strerror( status ) );
                return ( NETCDF_OPEN_ERR + status );
            }
        }
        rl1descInx = ncOpenGroupInp->rootNcid;
        l1desc_t& my_desc = irods::get_l1desc( rl1descInx );
        if ( rl1descInx < 2 || rl1descInx >= NUM_L1_DESC ) {
            rodsLog( LOG_ERROR,
                     "rsNcClose: rl1descInx %d out of range",
                     rl1descInx );
            return ( SYS_FILE_DESC_OUT_OF_RANGE );
        }
        if ( my_desc.inuseFlag != FD_INUSE ) {
            return BAD_INPUT_DESC_INDEX;
        }
        if ( my_desc.remoteZoneHost != NULL ) {
            ncOpenInp_t myNcOpenGroupInp;
            bzero( &myNcOpenGroupInp, sizeof( myNcOpenGroupInp ) );
            rstrcpy( myNcOpenGroupInp.objPath,
                     ncOpenGroupInp->objPath, MAX_NAME_LEN );
            myNcOpenGroupInp.rootNcid = my_desc.remoteL1descInx;

            status = rcNcOpenGroup( my_desc.remoteZoneHost->conn,
                                    &myNcOpenGroupInp, &myncid );
            if ( status < 0 ) {
                rodsLog( LOG_ERROR,
                         "rsNcOpenGroup: _rcNcOpenGroup %s error, status = %d",
                         ncOpenGroupInp->objPath, status );
                return ( status );
            }
            l1descInx = allocAndSetL1descForZoneOpr( myncid,
                        my_desc.dataObjInp,
                        my_desc.remoteZoneHost, NULL );
        }
        else {
            /* local zone */
            rootNcid = my_desc.l3descInx;
            remoteFlag = resoAndConnHostByDataObjInfo( rsComm,
                         my_desc.dataObjInfo, &rodsServerHost );
            if ( remoteFlag < 0 ) {
                return ( remoteFlag );
            }
            else if ( remoteFlag == LOCAL_HOST ) {
                status = nc_inq_grp_full_ncid( rootNcid,
                                               ncOpenGroupInp->objPath, &myncid );
                if ( status != NC_NOERR ) {
                    rodsLog( LOG_ERROR,
                             "rsNcOpenGroup: nc_inq_grp_full_ncid %s err, stat = %d, %s",
                             ncOpenGroupInp->objPath, status, nc_strerror( status ) );
                    return ( NETCDF_OPEN_ERR + status );
                }
            }
            else {
                /* execute it remotely */
                ncOpenInp_t myNcOpenGroupInp;
                bzero( &myNcOpenGroupInp, sizeof( myNcOpenGroupInp ) );
                rstrcpy( myNcOpenGroupInp.objPath,
                         ncOpenGroupInp->objPath, MAX_NAME_LEN );
                myNcOpenGroupInp.rootNcid = rootNcid;
                addKeyVal( &myNcOpenGroupInp.condInput, NATIVE_NETCDF_CALL_KW, "" );
                status = rcNcOpenGroup( rodsServerHost->conn, &myNcOpenGroupInp,
                                        &myncid );
                clearKeyVal( &myNcOpenGroupInp.condInput );
                if ( status < 0 ) {
                    rodsLog( LOG_ERROR,
                             "rsNcOpenGroup: rcNcOpenGroup %s error, status = %d",
                             myNcOpenGroupInp.objPath, status );
                    return ( status );
                }
            }
            l1descInx = allocL1desc();
            l1desc_t& new_desc = irods::get_l1desc( l1descInx );
            new_desc.dataObjInfo = dataObjInfo =
                                                ( dataObjInfo_t * ) calloc( 1, sizeof( dataObjInfo_t ) );
            rstrcpy( dataObjInfo->objPath,
                     ncOpenGroupInp->objPath, 
                     MAX_NAME_LEN );
            new_desc.l3descInx = myncid;
        }
        my_desc.oprType = NC_OPEN_GROUP;
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
        irods::apidef_t def = { NC_OPEN_GROUP_AN,
                                RODS_API_VERSION,
                                REMOTE_USER_AUTH,
                                REMOTE_USER_AUTH,
                                "NcOpenInp_PI", 0,
                                "INT_PI", 0,
#ifdef RODS_SERVER
                                CPP_14_FUNCTION( rsNcOpenGroup ),
#else
                                CPP_14_NOOPFUNC( rsComm_t*, ncOpenInp_t*, int**),
#endif // RODS_SERVER
                                "api_nc_open_group",
                                [](void*){},    
                                (funcPtr) RODS_SERVER_ENABLE(( irods::netcdf::api_call_wrapper< ncOpenInp_t* , int** > ))

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
