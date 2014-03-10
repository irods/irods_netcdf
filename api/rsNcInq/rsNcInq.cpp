/*** Copyright (c), The Regents of the University of California            ***
 *** For more information please refer to files in the COPYRIGHT directory ***/
/* This is script-generated code (for the most part).  */
/* See ncInq.h for a description of this API call.*/
#include "ncInq.hpp"
#include "rodsLog.hpp"
#include "rsGlobalExtern.hpp"
#include "rcGlobalExtern.hpp"
#include "rsApiHandler.hpp"
#include "objMetaOpr.hpp"
#include "physPath.hpp"
#include "specColl.hpp"
#include "getRemoteZoneResc.hpp"
#include "irods_get_l1desc.hpp"
/*** Copyright (c), The Regents of the University of California            ***
 *** For more information please refer to files in the COPYRIGHT directory ***/
/* This is script-generated code (for the most part).  */
/* See dataObjGet.h for a description of this API call.*/

#include "ncInqId.hpp"
#include "rodsLog.hpp"
#include "rsGlobalExtern.hpp"
#include "rcGlobalExtern.hpp"
#include "rsApiHandler.hpp"
#include "objMetaOpr.hpp"
#include "physPath.hpp"
#include "specColl.hpp"
#include "getRemoteZoneResc.hpp"


#ifdef RODS_SERVER
int
rsNcInqColl( rsComm_t *rsComm, ncInqInp_t *ncInqInp, ncInqOut_t **ncInqOut ) {
    int status;
    int l1descInx;
    ncOpenInp_t ncOpenInp;
    ncInqInp_t myNcInqInp;
    int i;

    l1descInx = ncInqInp->ncid;
        
    l1desc_t& my_desc = irods::get_l1desc( l1descInx );
    /* always use element 0 file aggr collection */
    if ( my_desc.openedAggInfo.objNcid0 == -1 ) {
        return NETCDF_AGG_ELE_FILE_NOT_OPENED;
    }
    myNcInqInp = *ncInqInp;
    myNcInqInp.ncid = my_desc.openedAggInfo.objNcid0;
    bzero( &myNcInqInp.condInput, sizeof( keyValPair_t ) );
    status = rsNcInqDataObj( rsComm, &myNcInqInp, ncInqOut );
    if ( status < 0 ) {
        rodsLogError( LOG_ERROR, status,
                      "rsNcInqColl: rsNcInqDataObj error for %s", ncOpenInp.objPath );
        return status;
    }
    /* make correction to 'time' dimension */
    for ( i = 0; i < ( *ncInqOut )->ndims; i++ ) {
        if ( strcasecmp( ( *ncInqOut )->dim[i].name, "time" ) == 0 ) {
            ( *ncInqOut )->dim[i].arrayLen = sumAggElementArraylen(
                                                 my_desc.openedAggInfo.ncAggInfo,
                                                 my_desc.openedAggInfo.ncAggInfo->numFiles );
            if ( ( *ncInqOut )->dim[i].arrayLen < 0 ) {
                status = ( *ncInqOut )->dim[i].arrayLen;
                freeNcInqOut( ncInqOut );
            }
            break;
        }
    }
    return status;
}

int
rsNcInqDataObj( rsComm_t *rsComm, ncInqInp_t *ncInqInp, ncInqOut_t **ncInqOut ) {
    int remoteFlag;
    rodsServerHost_t *rodsServerHost = NULL;
    int l1descInx;
    ncInqInp_t myNcInqInp;
    int status = 0;

    l1descInx = ncInqInp->ncid;

    l1desc_t& my_desc = irods::get_l1desc( l1descInx );
    if ( my_desc.remoteZoneHost != NULL ) {
        bzero( &myNcInqInp, sizeof( myNcInqInp ) );
        myNcInqInp.ncid = my_desc.remoteL1descInx;

        /* cross zone operation */
        status = rcNcInq( my_desc.remoteZoneHost->conn,
                          &myNcInqInp, ncInqOut );
    }
    else {
        remoteFlag = resoAndConnHostByDataObjInfo( rsComm,
                     my_desc.dataObjInfo, &rodsServerHost );
        if ( remoteFlag < 0 ) {
            return ( remoteFlag );
        }
        else if ( remoteFlag == LOCAL_HOST ) {
            myNcInqInp = *ncInqInp;
            myNcInqInp.ncid = my_desc.l3descInx; /// <=================
            bzero( &myNcInqInp.condInput, sizeof( myNcInqInp.condInput ) );
            status = _rsNcInq( rsComm, &myNcInqInp, ncInqOut );
            if ( status < 0 ) {
                return status;
            }
        }
        else {
            /* execute it remotely */
            myNcInqInp = *ncInqInp;
            myNcInqInp.ncid = my_desc.l3descInx;
            bzero( &myNcInqInp.condInput, sizeof( myNcInqInp.condInput ) );
            addKeyVal( &myNcInqInp.condInput, NATIVE_NETCDF_CALL_KW, "" );
            status = rcNcInq( rodsServerHost->conn, &myNcInqInp,
                              ncInqOut );
            clearKeyVal( &myNcInqInp.condInput );
            if ( status < 0 ) {
                rodsLog( LOG_ERROR,
                         "rsNcInq: rcsNcInq %d for %s error, status = %d",
                         my_desc.l3descInx,
                         my_desc.dataObjInfo->objPath, status );
                return ( status );
            }
        }
    }
    return status;
}

int
_rsNcInq( rsComm_t *rsComm, ncInqInp_t *ncInqInp, ncInqOut_t **ncInqOut ) {
    int status = ncInq( ncInqInp, ncInqOut );
    return status;
}
#endif // RODS_SERVER

extern "C" {
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
        if ( my_desc.openedAggInfo.ncAggInfo != NULL ) {
            status = rsNcInqColl( rsComm, ncInqInp, ncInqOut );
        }
        else {
            status = rsNcInqDataObj( rsComm, ncInqInp, ncInqOut );
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
                                0, 0 // null fcn ptr, handled in delay_load
                              };
        // =-=-=-=-=-=-=-
        // create an api object
        irods::api_entry* api = new irods::api_entry( def );

        // =-=-=-=-=-=-=-
        // assign the fcn which will handle the api call
#ifdef RODS_SERVER
        api->fcn_name_ = "rsNcInq";
#endif // RODS_SERVER

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
