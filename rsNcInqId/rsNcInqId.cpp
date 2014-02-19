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
    remoteFlag = resoAndConnHostByDataObjInfo( rsComm,
                 L1desc[l1descInx].dataObjInfo, &rodsServerHost );
    if ( remoteFlag < 0 ) {
        return ( remoteFlag );
    }
    else if ( remoteFlag == LOCAL_HOST ) {
        status = _rsNcInqId( ncInqIdInp->paramType,
                             L1desc[l1descInx].l3descInx, ncInqIdInp->name, outId );
        if ( status < 0 ) {
            return status;
        }
    }
    else {
        /* execute it remotely */
        bzero( &myNcInqIdInp, sizeof( myNcInqIdInp ) );
        myNcInqIdInp.paramType = ncInqIdInp->paramType;
        myNcInqIdInp.ncid = L1desc[l1descInx].l3descInx;
        rstrcpy( myNcInqIdInp.name, ncInqIdInp->name, MAX_NAME_LEN );
        addKeyVal( &myNcInqIdInp.condInput, NATIVE_NETCDF_CALL_KW, "" );
        status = rcNcInqId( rodsServerHost->conn, &myNcInqIdInp, outId );
        clearKeyVal( &myNcInqIdInp.condInput );
        if ( status < 0 ) {
            rodsLog( LOG_ERROR,
                     "rsNcInqIdDataObj: rcNcInqId %d for %s error, status = %d",
                     L1desc[l1descInx].l3descInx,
                     L1desc[l1descInx].dataObjInfo->objPath, status );
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
    /* always use element 0 file aggr collection */
    if ( L1desc[l1descInx].openedAggInfo.objNcid0 == -1 ) {
        return NETCDF_AGG_ELE_FILE_NOT_OPENED;
    }
    myNcInqIdInp = *ncInqIdInp;
    myNcInqIdInp.ncid = L1desc[l1descInx].openedAggInfo.objNcid0;
    bzero( &myNcInqIdInp.condInput, sizeof( keyValPair_t ) );
    status = rsNcInqIdDataObj( rsComm, &myNcInqIdInp, outId );
    if ( status < 0 ) {
        rodsLogError( LOG_ERROR, status,
                      "rsNcInqIdColl: rsNcInqIdDataObj error for l1descInx %d", l1descInx );
    }
    return status;
}

extern "C" {
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
        if ( L1desc[l1descInx].inuseFlag != FD_INUSE ) {
            return BAD_INPUT_DESC_INDEX;
        }
        if ( L1desc[l1descInx].remoteZoneHost != NULL ) {
            bzero( &myNcInqIdInp, sizeof( myNcInqIdInp ) );
            myNcInqIdInp.paramType = ncInqIdInp->paramType;
            myNcInqIdInp.ncid = L1desc[l1descInx].remoteL1descInx;
            rstrcpy( myNcInqIdInp.name, ncInqIdInp->name, MAX_NAME_LEN );

            /* cross zone operation */
            status = rcNcInqId( L1desc[l1descInx].remoteZoneHost->conn,
                                &myNcInqIdInp, outId );
        }
        else {
            if ( L1desc[l1descInx].openedAggInfo.ncAggInfo != NULL ) {
                status = rsNcInqIdColl( rsComm, ncInqIdInp, outId );
            }
            else {
                status = rsNcInqIdDataObj( rsComm, ncInqIdInp, outId );
            }
        }
        return status;
    }

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
        api->fcn_name_ = "rsNcInq";
        
        // =-=-=-=-=-=-=-
        // assign the pack struct key and value
        strncpy( api->in_pack_key, "NcInqIdInp_PI", MAX_NAME_LEN );
        api->inPackInstruct = NcInqIdInp_PI;
        
        //strncpy( api->in_pack_key, "", MAX_NAME_LEN );
        //api->inPackInstruct = ;
        
        // =-=-=-=-=-=-=-
        // and... were done.
        return api;

    } // plugin_factory




}; // extern "C"





