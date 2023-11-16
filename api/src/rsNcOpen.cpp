/*** Copyright (c), The Regents of the University of California            ***
 *** For more information please refer to files in the COPYRIGHT directory ***/
/* This is script-generated code (for the most part).  */
/* See ncOpen.h for a description of this API call.*/

#include <netcdf_port.hpp>
#include "ncOpen.hpp"
#include "ncApiIndex.hpp"
#include <irods/rodsLog.h>
#include <irods/dataObjOpen.h>
#include <irods/rsGlobalExtern.hpp>
#include <irods/rcGlobalExtern.h>
#include <irods/rsApiHandler.hpp>
#include <irods/objMetaOpr.hpp>
#include <irods/physPath.hpp>
#include <irods/specColl.hpp>

#include "ncUtil.hpp"

extern "C" {
    double get_plugin_interface_version() { return 1.0; }
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
#ifdef RODS_SERVER
                                CPP_14_FUNCTION(rsNcOpen ),
#else
                                CPP_14_NOOPFUNC( rsComm_t*, ncOpenInp_t *, int **),
#endif // RODS_SERVER
                                "api_nc_open",
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

        api->out_pack_key   = "INT_PI";
        api->out_pack_value = "int myInt;"; // INT_PI , stolen from rodsPackInStruct.h

        // =-=-=-=-=-=-=-
        // and... were done.
        return api;

    } // plugin_factory

}; // extern "C"
