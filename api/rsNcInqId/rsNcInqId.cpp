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
#include "irods_get_l1desc.hpp"

#ifdef RODS_SERVER
int
_rsNcInqId( int paramType, int ncid, char *name, int **outId ) {
    int status;
    int myoutId = 0;

    switch ( paramType ) {
    case NC_VAR_T:
        status = nc_inq_varid( ncid, name, &myoutId );
        break;
    case NC_DIM_T:
        status = nc_inq_dimid( ncid, name, &myoutId );
        break;
    default:
        rodsLog( LOG_ERROR,
                 "_rsNcInqId: Unknow paramType %d for %s ", paramType, name );
        return ( NETCDF_INVALID_PARAM_TYPE );
    }

    if ( status == NC_NOERR ) {
        *outId = ( int * ) malloc( sizeof( int ) );
        *( *outId ) = myoutId;
    }
    else {
        rodsLog( LOG_ERROR,
                 "_rsNcInqId: nc_inq error paramType %d for %s. %s ",
                 paramType, name, nc_strerror( status ) );
        status = NETCDF_INQ_ID_ERR + status;
    }
    return status;
}

int
rsNcInqIdDataObj( rsComm_t *rsComm, ncInqIdInp_t *ncInqIdInp, int **outId ) {
    int remoteFlag;
    rodsServerHost_t *rodsServerHost = NULL;
    int l1descInx;
    ncInqIdInp_t myNcInqIdInp;
    int status = 0;

    l1descInx = ncInqIdInp->ncid;
    l1desc_t& my_desc = irods::get_l1desc( l1descInx );
    remoteFlag = resoAndConnHostByDataObjInfo( rsComm,
                 my_desc.dataObjInfo, &rodsServerHost );
    if ( remoteFlag < 0 ) {
        return ( remoteFlag );
    }
    else if ( remoteFlag == LOCAL_HOST ) {
        status = _rsNcInqId( ncInqIdInp->paramType,
                             my_desc.l3descInx, ncInqIdInp->name, outId );
        if ( status < 0 ) {
            return status;
        }
    }
    else {
        /* execute it remotely */
        bzero( &myNcInqIdInp, sizeof( myNcInqIdInp ) );
        myNcInqIdInp.paramType = ncInqIdInp->paramType;
        myNcInqIdInp.ncid = my_desc.l3descInx;
        rstrcpy( myNcInqIdInp.name, ncInqIdInp->name, MAX_NAME_LEN );
        addKeyVal( &myNcInqIdInp.condInput, NATIVE_NETCDF_CALL_KW, "" );
        status = rcNcInqId( rodsServerHost->conn, &myNcInqIdInp, outId );
        clearKeyVal( &myNcInqIdInp.condInput );
        if ( status < 0 ) {
            rodsLog( LOG_ERROR,
                     "rsNcInqIdDataObj: rcNcInqId %d for %s error, status = %d",
                     my_desc.l3descInx,
                     my_desc.dataObjInfo->objPath, status );
            return ( status );
        }
    }
    return status;
}

int
rsNcInqIdColl( rsComm_t *rsComm, ncInqIdInp_t *ncInqIdInp, int **outId ) {
    int status;
    int l1descInx;
    ncInqIdInp_t myNcInqIdInp;

    l1descInx = ncInqIdInp->ncid;
    l1desc_t& my_desc = irods::get_l1desc( l1descInx );
    /* always use element 0 file aggr collection */
    if ( my_desc.openedAggInfo.objNcid0 == -1 ) {
        return NETCDF_AGG_ELE_FILE_NOT_OPENED;
    }
    myNcInqIdInp = *ncInqIdInp;
    myNcInqIdInp.ncid = my_desc.openedAggInfo.objNcid0;
    bzero( &myNcInqIdInp.condInput, sizeof( keyValPair_t ) );
    status = rsNcInqIdDataObj( rsComm, &myNcInqIdInp, outId );
    if ( status < 0 ) {
        rodsLogError( LOG_ERROR, status,
                      "rsNcInqIdColl: rsNcInqIdDataObj error for l1descInx %d", l1descInx );
    }
    return status;
}
#endif // RODS_SERVER

extern "C" {
#ifdef RODS_SERVER
    int
    rsNcInqId( rsComm_t *rsComm, ncInqIdInp_t *ncInqIdInp, int **outId ) {
        int l1descInx;
        ncInqIdInp_t myNcInqIdInp;
        int status = 0;

        if ( getValByKey( &ncInqIdInp->condInput, NATIVE_NETCDF_CALL_KW ) != NULL ) {
            /* just do nc_inq_YYYYid */
            status = _rsNcInqId( ncInqIdInp->paramType, ncInqIdInp->ncid,
                                 ncInqIdInp->name, outId );
            return status;
        }
        l1descInx = ncInqIdInp->ncid;
        if ( l1descInx < 2 || l1descInx >= NUM_L1_DESC ) {
            rodsLog( LOG_ERROR,
                     "rsNcInqId: l1descInx %d out of range",
                     l1descInx );
            return ( SYS_FILE_DESC_OUT_OF_RANGE );
        }
    
        l1desc_t& my_desc = irods::get_l1desc( l1descInx );
        if ( my_desc.inuseFlag != FD_INUSE ) {
            return BAD_INPUT_DESC_INDEX;
        }
        if ( my_desc.remoteZoneHost != NULL ) {
            bzero( &myNcInqIdInp, sizeof( myNcInqIdInp ) );
            myNcInqIdInp.paramType = ncInqIdInp->paramType;
            myNcInqIdInp.ncid = my_desc.remoteL1descInx;
            rstrcpy( myNcInqIdInp.name, ncInqIdInp->name, MAX_NAME_LEN );

            /* cross zone operation */
            status = rcNcInqId( my_desc.remoteZoneHost->conn,
                                &myNcInqIdInp, outId );
        }
        else {
            if ( my_desc.openedAggInfo.ncAggInfo != NULL ) {
                status = rsNcInqIdColl( rsComm, ncInqIdInp, outId );
            }
            else {
                status = rsNcInqIdDataObj( rsComm, ncInqIdInp, outId );
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
        irods::apidef_t def = { NC_INQ_ID_AN,
                                RODS_API_VERSION,
                                REMOTE_USER_AUTH,
                                REMOTE_USER_AUTH,
                                "NcInqIdInp_PI", 0,
                                "INT_PI", 0,
                                0 // null fcn ptr, handled in delay_load
                              }; 
        // =-=-=-=-=-=-=-
        // create an api object
        irods::api_entry* api = new irods::api_entry( def );

        // =-=-=-=-=-=-=-
        // assign the fcn which will handle the api call
#ifdef RODS_SERVER
        api->fcn_name_ = "rsNcInqId";
#endif // RODS_SERVER
                     
        // =-=-=-=-=-=-=-
        // assign the pack struct key and value
        api->in_pack_key   = "NcInqIdInp_PI";
        api->in_pack_value = NcInqIdInp_PI;
        
        // =-=-=-=-=-=-=-
        // and... were done.
        return api;

    } // plugin_factory




}; // extern "C"





