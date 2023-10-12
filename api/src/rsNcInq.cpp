/*** Copyright (c), The Regents of the University of California            ***
 *** For more information please refer to files in the COPYRIGHT directory ***/
/* This is script-generated code (for the most part).  */
/* See ncInq.hpp for a description of this API call.*/

#include "netcdf_port.hpp"
#include "ncInq.hpp"
#include "rodsLog.h"
#include "rsGlobalExtern.hpp"
#include "rcGlobalExtern.h"
#include "rsApiHandler.hpp"
#include "objMetaOpr.hpp"
#include "physPath.hpp"
#include "specColl.hpp"
#include "getRemoteZoneResc.h"
#include "irods_get_l1desc.hpp"
#include "ncApiIndex.hpp"
#include "ncGetAggInfo.hpp"
#include "ncUtil.hpp"

extern "C" {
    double get_plugin_interface_version() { return 1.0; }
#ifdef RODS_SERVER
    int
    rsNcInq( rsComm_t *rsComm, ncInqInp_t *ncInqInp, ncInqOut_t **ncInqOut ) {
        int l1descInx;
        int status = 0;
        if ( getValByKey( &ncInqInp->condInput, NATIVE_NETCDF_CALL_KW ) !=
                NULL ) {
            /* just do nc_inq */
            status = _rsNcInq( rsComm, ncInqInp, ncInqOut );
            return status;
        }
        l1descInx = ncInqInp->ncid;
        l1desc_t& my_desc = irods::get_l1desc( l1descInx );
        if ( l1descInx < 2 || l1descInx >= NUM_L1_DESC ) {
            rodsLog( LOG_ERROR,
                     "rsNcInq: l1descInx %d out of range",
                     l1descInx );
            return ( SYS_FILE_DESC_OUT_OF_RANGE );
        }
        if ( my_desc.inuseFlag != FD_INUSE ) {
            return BAD_INPUT_DESC_INDEX;
        }
        openedAggInfo_t * openedAggInfo = boost::any_cast< openedAggInfo_t >( &my_desc.pluginData );
        if ( openedAggInfo != NULL && openedAggInfo->ncAggInfo != NULL ) {
            status = _rsNcInqColl( rsComm, ncInqInp, ncInqOut );
        }
        else {
            status = _rsNcInqDataObj( rsComm, ncInqInp, ncInqOut );
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

        irods::apidef_t def = { NC_INQ_AN,
                                RODS_API_VERSION,
                                REMOTE_USER_AUTH,
                                REMOTE_USER_AUTH,
                                "NcInqInp_PI", 0,
                                "NcInqOut_PI", 0,
#ifdef RODS_SERVER
                                CPP_14_FUNCTION( rsNcInq ),
#else
                                CPP_14_NOOPFUNC( rsComm_t*, ncInqInp_t *, ncInqOut_t ** ),
#endif // RODS_SERVER
                                "api_nc_inq",
                                [](void*){},
                                irods::clearOutStruct_noop,
                                (funcPtr) RODS_SERVER_ENABLE(( irods::netcdf::api_call_wrapper< ncInqInp_t *, ncInqOut_t ** > ))
                              };

        // =-=-=-=-=-=-=-
        // create an api object
        irods::api_entry* api = new irods::api_entry( def );

        // =-=-=-=-=-=-=-
        // assign the pack struct key and value
        api->in_pack_key   = "NcInqInp_PI";
        api->in_pack_value = NcInqInp_PI;

        api->out_pack_key   = "NcInqOut_PI";
        api->out_pack_value = NcInqOut_PI;

        // =-=-=-=-=-=-=-
        // add all the supporting packinstructions
        api->extra_pack_struct[ "NcGenVarOut_PI" ] = NcGenVarOut_PI;
        api->extra_pack_struct[ "NcGenAttOut_PI" ] = NcGenAttOut_PI;
        api->extra_pack_struct[ "NcGenDimOut_PI" ] = NcGenDimOut_PI;

        // =-=-=-=-=-=-=-
        // and... were done.
        return api;

    } // plugin_factory

}; // extern "C"
