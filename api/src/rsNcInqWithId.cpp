/*** Copyright (c), The Regents of the University of California            ***
 *** For more information please refer to files in the COPYRIGHT directory ***/
/* This is script-generated code (for the most part).  */
/* See dataObjGet.h for a description of this API call.*/



#include "netcdf_port.hpp"
#include "ncInqWithId.hpp"
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

#ifdef RODS_SERVER
int
_rsNcInqWithId( int paramType, int ncid, int myid, char *name,
                ncInqWithIdOut_t **ncInqWithIdOut ) {
    int status;
    char myname[MAX_NAME_LEN];
    size_t mylong = 0;
    int mytype = 0;
    int mynatts = 0;
    int myndim = 0;
    int intArray[NC_MAX_VAR_DIMS];

    myname[0] = '\0';
    if ( name == NULL || ncInqWithIdOut == NULL ) {
        return USER__NULL_INPUT_ERR;
    }

    switch ( paramType ) {
    case NC_DIM_T:
        status = nc_inq_dim( ncid, myid, myname, &mylong );
        break;
    case NC_VAR_T:
        status = nc_inq_var( ncid, myid, myname, &mytype, &myndim, intArray,
                             &mynatts );
        break;
    default:
        rodsLog( LOG_ERROR,
                 "_rsNcInqWithId: Unknow paramType %d for %s ", paramType, myname );
        return ( NETCDF_INVALID_PARAM_TYPE );
    }

    if ( status == NC_NOERR ) {
        *ncInqWithIdOut = ( ncInqWithIdOut_t * ) calloc( 1, sizeof
                          ( ncInqWithIdOut_t ) );
        ( *ncInqWithIdOut )->mylong = mylong;
        ( *ncInqWithIdOut )->dataType = mytype;
        ( *ncInqWithIdOut )->natts = mynatts;
        if ( myndim > 0 ) {
            int len = sizeof( int ) * myndim;
            ( *ncInqWithIdOut )->ndim = myndim;
            ( *ncInqWithIdOut )->intArray = ( int * ) calloc( 1, len );
            memcpy( ( *ncInqWithIdOut )->intArray, intArray, len );
        }
        rstrcpy( ( *ncInqWithIdOut )->name, myname, MAX_NAME_LEN );
    }
    else {
        rodsLog( LOG_ERROR,
                 "_rsNcInqWithId: nc_inq error paramType %d for %s. %s ",
                 paramType, name, nc_strerror( status ) );
        *ncInqWithIdOut = NULL;
        status = NETCDF_INQ_ID_ERR + status;
    }
    return status;
}

int
_rsNcInqWithIdDataObj( rsComm_t *rsComm, ncInqIdInp_t *ncInqWithIdInp,
                      ncInqWithIdOut_t **ncInqWithIdOut ) {
    int remoteFlag;
    rodsServerHost_t *rodsServerHost = NULL;
    int l1descInx;
    ncInqIdInp_t myNcInqWithIdInp;
    int status = 0;

    l1descInx = ncInqWithIdInp->ncid;
    l1desc_t& my_desc = irods::get_l1desc( l1descInx );
    remoteFlag = resoAndConnHostByDataObjInfo( rsComm,
                 my_desc.dataObjInfo, &rodsServerHost );
    if ( remoteFlag < 0 ) {
        return ( remoteFlag );
    }
    else if ( remoteFlag == LOCAL_HOST ) {
        status = _rsNcInqWithId( ncInqWithIdInp->paramType,
                                 my_desc.l3descInx,  ncInqWithIdInp->myid,
                                 ncInqWithIdInp->name, ncInqWithIdOut );
        if ( status < 0 ) {
            return status;
        }
    }
    else {
        /* execute it remotely */
        bzero( &myNcInqWithIdInp, sizeof( myNcInqWithIdInp ) );
        myNcInqWithIdInp.paramType = ncInqWithIdInp->paramType;
        myNcInqWithIdInp.ncid = my_desc.l3descInx;
        myNcInqWithIdInp.myid = ncInqWithIdInp->myid;
        rstrcpy( myNcInqWithIdInp.name, ncInqWithIdInp->name, MAX_NAME_LEN );
        addKeyVal( &myNcInqWithIdInp.condInput, NATIVE_NETCDF_CALL_KW, "" );
        status = rcNcInqWithId( rodsServerHost->conn, &myNcInqWithIdInp,
                                ncInqWithIdOut );
        clearKeyVal( &myNcInqWithIdInp.condInput );
        if ( status < 0 ) {
            rodsLog( LOG_ERROR,
                     "rsNcInqWithIdDataObj: rcNcInqWithId %d for %s error, status=%d",
                     my_desc.l3descInx,
                     my_desc.dataObjInfo->objPath, status );
            return ( status );
        }
    }
    return status;
}

int
_rsNcInqWithIdColl( rsComm_t *rsComm, ncInqIdInp_t *ncInqWithIdInp,
                   ncInqWithIdOut_t **ncInqWithIdOut ) {
    int status;
    int l1descInx;
    ncInqIdInp_t myNcInqWithIdInp;

    l1descInx = ncInqWithIdInp->ncid;
    l1desc_t& my_desc = irods::get_l1desc( l1descInx );
    /* always use element 0 file aggr collection */
    openedAggInfo_t * openedAggInfo = boost::any_cast< openedAggInfo_t >( &my_desc.pluginData );
    if ( openedAggInfo == NULL || openedAggInfo->objNcid0 == -1 ) {
        return NETCDF_AGG_ELE_FILE_NOT_OPENED;
    }
    myNcInqWithIdInp = *ncInqWithIdInp;
    myNcInqWithIdInp.ncid = openedAggInfo->objNcid0;
    bzero( &myNcInqWithIdInp.condInput, sizeof( keyValPair_t ) );
    status = _rsNcInqWithIdDataObj( rsComm, &myNcInqWithIdInp, ncInqWithIdOut );
    if ( status < 0 ) {
        rodsLogError( LOG_ERROR, status,
                      "rsNcInqWithIdColl: rsNcInqWithIdDataObj error for l1descInx %d",
                      l1descInx );
    }
    return status;
}
#endif // RODS_SERVER

extern "C" {
    double get_plugin_interface_version() { return 1.0; }
#ifdef RODS_SERVER
    int
    rsNcInqWithId( rsComm_t *rsComm, ncInqIdInp_t *ncInqWithIdInp,
                   ncInqWithIdOut_t **ncInqWithIdOut ) {
        int l1descInx;
        ncInqIdInp_t myNcInqWithIdInp;
        int status = 0;

        if ( getValByKey( &ncInqWithIdInp->condInput, NATIVE_NETCDF_CALL_KW ) !=
                NULL ) {
            /* just do nc_inq_YYYY */
            status = _rsNcInqWithId( ncInqWithIdInp->paramType,
                                     ncInqWithIdInp->ncid, ncInqWithIdInp->myid, ncInqWithIdInp->name,
                                     ncInqWithIdOut );
            return status;
        }
        l1descInx = ncInqWithIdInp->ncid;
        l1desc_t& my_desc = irods::get_l1desc( l1descInx );
        if ( l1descInx < 2 || l1descInx >= NUM_L1_DESC ) {
            rodsLog( LOG_ERROR,
                     "rsNcInqWithId: l1descInx %d out of range",
                     l1descInx );
            return ( SYS_FILE_DESC_OUT_OF_RANGE );
        }
        if ( my_desc.inuseFlag != FD_INUSE ) {
            return BAD_INPUT_DESC_INDEX;
        }
        if ( my_desc.remoteZoneHost != NULL ) {
            bzero( &myNcInqWithIdInp, sizeof( myNcInqWithIdInp ) );
            myNcInqWithIdInp.paramType = ncInqWithIdInp->paramType;
            myNcInqWithIdInp.myid = ncInqWithIdInp->myid;
            myNcInqWithIdInp.ncid = my_desc.remoteL1descInx;
            rstrcpy( myNcInqWithIdInp.name, ncInqWithIdInp->name, MAX_NAME_LEN );

            /* cross zone operation */
            status = rcNcInqWithId( my_desc.remoteZoneHost->conn,
                                    &myNcInqWithIdInp, ncInqWithIdOut );
        }
        else {
            openedAggInfo_t * openedAggInfo = boost::any_cast< openedAggInfo_t >( &my_desc.pluginData );
            if ( openedAggInfo != NULL && openedAggInfo->ncAggInfo != NULL ) {
                status = _rsNcInqWithIdColl( rsComm, ncInqWithIdInp, ncInqWithIdOut );
            }
            else {
                status = _rsNcInqWithIdDataObj( rsComm, ncInqWithIdInp,
                                               ncInqWithIdOut );
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

        irods::apidef_t def = { NC_INQ_WITH_ID_AN,
                                RODS_API_VERSION,
                                REMOTE_USER_AUTH,
                                REMOTE_USER_AUTH,
                                "NcInqIdInp_PI", 0,
                                "NcInqWithIdOut_PI", 0,
#ifdef RODS_SERVER
                                CPP_14_FUNCTION( rsNcInqWithId ),
#else
                                CPP_14_NOOPFUNC( rsComm_t*,  ncInqIdInp_t *, ncInqWithIdOut_t **), 
#endif // RODS_SERVER
                                "api_nc_inq_with_id",
                                [](void*){},
                                irods::clearOutStruct_noop,
                                (funcPtr) RODS_SERVER_ENABLE(( irods::netcdf::api_call_wrapper< ncInqIdInp_t *, ncInqWithIdOut_t ** > ))
                              };
        // =-=-=-=-=-=-=-
        // create an api object
        irods::api_entry* api = new irods::api_entry( def );

        // =-=-=-=-=-=-=-
        // assign the pack struct key and value
        api->in_pack_key   = "NcInqIdInp_PI";
        api->in_pack_value = NcInqIdInp_PI;

        api->out_pack_key   = "NcInqWithIdOut_PI";
        api->out_pack_value = NcInqWithIdOut_PI;

        // =-=-=-=-=-=-=-
        // and... were done.
        return api;

    } // plugin_factory

}; // extern "C" 
