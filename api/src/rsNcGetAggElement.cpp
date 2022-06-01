/*** Copyright (c), The Regents of the University of California            ***
 *** For more information please refer to files in the COPYRIGHT directory ***/
/* This is script-generated code (for the most part).  */
/* See ncGetAggElement.h for a description of this API call.*/

#include "netcdf_port.hpp"
#include "ncGetAggElement.hpp"
#include "ncInq.hpp"
#include <irods/rodsLog.h>
#include <irods/dataObjOpen.h>
#include <irods/rsGlobalExtern.hpp>
#include <irods/rcGlobalExtern.h>
#include <irods/rsApiHandler.hpp>
#include <irods/rsDataObjOpen.hpp>
#include <irods/objMetaOpr.hpp>
#include <irods/physPath.hpp>
#include <irods/specColl.hpp>
#include <irods/getRemoteZoneResc.h>
#include <irods/irods_get_l1desc.hpp>
#include "ncApiIndex.hpp"

#ifdef RODS_SERVER
static int
_rsNcGetAggElement( rsComm_t *rsComm, dataObjInfo_t *dataObjInfo,
                    ncAggElement_t **ncAggElement ) {
    int status;
    ncInqInp_t ncInqInp;
    ncInqOut_t *ncInqOut = NULL;
    int ncid;
    int i, j;

    if ( dataObjInfo == NULL || ncAggElement == NULL ) {
        return USER__NULL_INPUT_ERR;
    }
    *ncAggElement = NULL;
#ifdef NETCDF4_API
    status = nc_open( dataObjInfo->filePath, NC_NOWRITE | NC_NETCDF4, &ncid );
#else
    status = nc_open( dataObjInfo->filePath, NC_NOWRITE, &ncid );
#endif
    if ( status != NC_NOERR ) {
        rodsLog( LOG_ERROR,
                 "_rsNcGetAggElement: nc_open %s error, status = %d, %s",
                 dataObjInfo->filePath, status, nc_strerror( status ) );
        return NETCDF_OPEN_ERR + status;
    }
    /* do the general inq */
    bzero( &ncInqInp, sizeof( ncInqInp ) );
    ncInqInp.ncid = ncid;
    ncInqInp.paramType = NC_ALL_TYPE;
    ncInqInp.flags = NC_ALL_FLAG;

    status = ncInq( &ncInqInp, &ncInqOut );
    if ( status < 0 ) {
        rodsLogError( LOG_ERROR, status,
                      "_rsNcGetAggElement: ncInq error for %s", dataObjInfo->filePath );
        nc_close( ncid );
        return NETCDF_INQ_ERR + status;
    }
    for ( i = 0; i < ncInqOut->ndims; i++ ) {
        if ( strcasecmp( ncInqOut->dim[i].name, "time" ) == 0 ) {
            break;
        }
    }
    if ( i >= ncInqOut->ndims ) {
        /* no match */
        rodsLog( LOG_ERROR,
                 "_rsNcGetAggElement: 'time' dim does not exist for %s",
                 dataObjInfo->filePath );
        nc_close( ncid );
        return NETCDF_DIM_MISMATCH_ERR;
    }
    for ( j = 0; j < ncInqOut->nvars; j++ ) {
        if ( strcmp( ncInqOut->dim[i].name, ncInqOut->var[j].name ) == 0 ) {
            break;
        }
    }

    if ( j >= ncInqOut->nvars ) {
        /* no match */
        rodsLog( LOG_ERROR,
                 "_rsNcGetAggElement: 'time' var does not exist for %s",
                 dataObjInfo->filePath );
        nc_close( ncid );
        return NETCDF_DIM_MISMATCH_ERR;
    }
    *ncAggElement = ( ncAggElement_t * ) calloc( 1, sizeof( ncAggElement_t ) );
    ( *ncAggElement )->arraylen = ncInqOut->dim[i].arrayLen;

    ( *ncAggElement )->startTime = getNcIntVar( ncid, ncInqOut->var[j].id,
                                   ncInqOut->var[j].dataType, 0 );
    ( *ncAggElement )->endTime = getNcIntVar( ncid, ncInqOut->var[j].id,
                                 ncInqOut->var[j].dataType, ncInqOut->dim[i].arrayLen - 1 );
    timeToAsci( ( *ncAggElement )->startTime, ( *ncAggElement )->astartTime );
    timeToAsci( ( *ncAggElement )->endTime, ( *ncAggElement )->aendTime );
    rstrcpy( ( *ncAggElement )->objPath, dataObjInfo->objPath, MAX_NAME_LEN );

    nc_close( ncid );
    return 0;
}
#endif // RODS_SERVER

extern "C" {
    double get_plugin_interface_version() { return 1.0; }
#ifdef RODS_SERVER
    int
    rsNcGetAggElement( rsComm_t *rsComm, ncOpenInp_t *ncOpenInp,
                       ncAggElement_t **ncAggElement ) {
        int remoteFlag;
        rodsServerHost_t *rodsServerHost;
        specCollCache_t *specCollCache = NULL;
        int status;
        dataObjInp_t dataObjInp;
        int l1descInx;

        bzero( &dataObjInp, sizeof( dataObjInp ) );
        rstrcpy( dataObjInp.objPath, ncOpenInp->objPath, MAX_NAME_LEN );
        replKeyVal( &ncOpenInp->condInput, &dataObjInp.condInput );
        resolveLinkedPath( rsComm, dataObjInp.objPath, &specCollCache,
                           &dataObjInp.condInput );
        remoteFlag = getAndConnRemoteZone( rsComm, &dataObjInp, &rodsServerHost,
                                           REMOTE_OPEN );

        if ( remoteFlag < 0 ) {
            return ( remoteFlag );
        }
        else if ( remoteFlag == LOCAL_HOST ) {
            addKeyVal( &dataObjInp.condInput, NO_OPEN_FLAG_KW, "" );
            addKeyVal( &dataObjInp.condInput, NO_STAGING_KW, "" );

            l1descInx = rsDataObjOpen( rsComm, &dataObjInp );
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
                status = _rsNcGetAggElement( rsComm,
                                             my_desc.dataObjInfo, 
                                             ncAggElement );
                freeL1desc( l1descInx );
            }
            else {
                freeL1desc( l1descInx );
                /* execute it remotely with dataObjInfo->filePath */
                status = rcNcGetAggElement( rodsServerHost->conn, ncOpenInp,
                                            ncAggElement );
            }
        }
        else {
            status = rcNcGetAggElement( rodsServerHost->conn, ncOpenInp,
                                        ncAggElement );
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

        irods::apidef_t def = { NC_GET_AGG_ELEMENT_AN,
                                RODS_API_VERSION,
                                REMOTE_USER_AUTH,
                                REMOTE_USER_AUTH,
                                "NcOpenInp_PI", 0,
                                "NcAggElement_PI", 0,
#ifdef RODS_SERVER
                                CPP_14_FUNCTION( rsNcGetAggElement ),
#else
                                CPP_14_NOOPFUNC( rsComm_t*, ncOpenInp_t *, ncAggElement_t** ),
#endif // RODS_SERVER
                                "api_nc_get_agg_element",
                                [](void*){},
                                (funcPtr) RODS_SERVER_ENABLE(( irods::netcdf::api_call_wrapper<  ncOpenInp_t *, ncAggElement_t** > ))
                              }; 
        // =-=-=-=-=-=-=-
        // create an api object
        irods::api_entry* api = new irods::api_entry( def );

        // =-=-=-=-=-=-=-
        // assign the pack struct key and value
        api->in_pack_key   = "NcOpenInp_PI";
        api->in_pack_value = NcOpenInp_PI;
        
        api->out_pack_key   = "NcAggElement_PI";
        api->out_pack_value = NcAggElement_PI;
        
        // =-=-=-=-=-=-=-
        // and... were done.
        return api;

    } // plugin_factory

}; // extern "C"




