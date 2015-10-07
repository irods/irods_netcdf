/*** Copyright (c), The Regents of the University of California            ***
 *** For more information please refer to files in the COPYRIGHT directory ***/
/* This is script-generated code (for the most part).  */
/* See ncGetAggInfo.h for a description of this API call.*/

#include "ncGetAggInfo.hpp"
#include "ncInq.hpp"
#include "rodsLog.hpp"
#include "dataObjOpen.hpp"
#include "rsGlobalExtern.hpp"
#include "rcGlobalExtern.hpp"
#include "rsApiHandler.hpp"
#include "objMetaOpr.hpp"
#include "physPath.hpp"
#include "specColl.hpp"
#include "openCollection.hpp"
#include "readCollection.hpp"
#include "closeCollection.hpp"
#include "closeCollection.hpp"
#include "dataObjPut.hpp"
#include "dataObjGet.hpp"
#include "irods_server_api_call.hpp"
#include "ncApiIndex.hpp"

extern "C" {
    double get_plugin_interface_version() { return 1.0; }
#ifdef RODS_SERVER
    int
    rsNcGetAggInfo( rsComm_t *rsComm, ncOpenInp_t *ncOpenInp,
                    ncAggInfo_t **ncAggInfo ) {
        specCollCache_t *specCollCache = NULL;
        collInp_t collInp;
        int handleInx;
        collEnt_t *collEnt;
        int status = 0;
        int status2 = 0;
        int savedStatus = 0;
        ncOpenInp_t childNcOpenInp;
        ncAggElement_t *ncAggElement = NULL;
        bytesBuf_t *packedBBuf = NULL;

        bzero( &collInp, sizeof( collInp ) );
        rstrcpy( collInp.collName, ncOpenInp->objPath, MAX_NAME_LEN );
        resolveLinkedPath( rsComm, collInp.collName, &specCollCache,
                           &ncOpenInp->condInput );
        collInp.flags = VERY_LONG_METADATA_FG;
        handleInx = irods::server_api_call ( OPEN_COLLECTION_AN, rsComm, &collInp );
        if ( handleInx < 0 ) {
            rodsLog( LOG_ERROR,
                     "rsNcGetAggInfo: rsOpenCollection of %s error. status = %d",
                     collInp.collName, handleInx );
            return ( handleInx );
        }
        bzero( &childNcOpenInp, sizeof( childNcOpenInp ) );
        *ncAggInfo = ( ncAggInfo_t * ) calloc( 1, sizeof( ncAggInfo_t ) );
        rstrcpy( ( *ncAggInfo )->ncObjectName, ncOpenInp->objPath, MAX_NAME_LEN );
        while ( ( status2 = irods::server_api_call ( READ_COLLECTION_AN, rsComm, &handleInx, &collEnt ) ) >= 0 ) {
            if ( collEnt->objType != DATA_OBJ_T ) {
                free( collEnt );
                continue;
            }
            else if ( strcmp( collEnt->dataType, "netcdf" ) != 0 ) {
                if ( strcmp( collEnt->dataName, NC_AGG_INFO_FILE_NAME ) != 0 ) {
                    rodsLog( LOG_NOTICE,
                             "rsNcGetAggInfo: dataType of %s in %s is not 'netcdf' type",
                             collEnt->dataName, collInp.collName );
                    savedStatus = NETCDF_INVALID_DATA_TYPE;
                }
                free( collEnt );
                continue;
            }
            snprintf( childNcOpenInp.objPath, MAX_NAME_LEN, "%s/%s",
                      collInp.collName, collEnt->dataName );
            status = irods::server_api_call ( NC_GET_AGG_ELEMENT_AN, rsComm, &childNcOpenInp, &ncAggElement );
            if ( status < 0 ) {
                rodsLogError( LOG_ERROR, status,
                              "rsNcGetAggInfo: rsNcGetAggElement of %s error.",
                              childNcOpenInp.objPath );
                free( collEnt );
                break;
            }
            else {
                status = addNcAggElement( ncAggElement, *ncAggInfo );
                free( ncAggElement );
                if ( status < 0 ) {
                    free( collEnt );
                    break;
                }
            }
            free( collEnt );
        }
        irods::server_api_call ( CLOSE_COLLECTION_AN, rsComm, &handleInx );
        if ( status2 < 0 && status2 != CAT_NO_ROWS_FOUND && status >= 0 ) {
            status = status2;
        }
        if ( status >= 0 && ( ncOpenInp->mode & NC_WRITE ) != 0 ) {
            dataObjInp_t dataObjInp;
            portalOprOut_t *portalOprOut = NULL;
            status = packStruct( ( void * ) * ncAggInfo, &packedBBuf, "NcAggInfo_PI",
                                 RodsPackTable, 0, XML_PROT );
            if ( status < 0 ) {
                rodsLogError( LOG_ERROR, status,
                              "rsNcGetAggInfo: packStruct error for %s",
                              childNcOpenInp.objPath );
                return status;
            }
            /* write it */
            bzero( &dataObjInp, sizeof( dataObjInp ) );
            replKeyVal( &ncOpenInp->condInput, &dataObjInp.condInput );
            snprintf( dataObjInp.objPath, MAX_NAME_LEN, "%s/%s",
                      collInp.collName, NC_AGG_INFO_FILE_NAME );
            dataObjInp.dataSize = packedBBuf->len;
            dataObjInp.oprType = PUT_OPR;
            addKeyVal( &dataObjInp.condInput, DATA_INCLUDED_KW, "" );
            addKeyVal( &dataObjInp.condInput, FORCE_FLAG_KW, "" );
            status = irods::server_api_call ( DATA_OBJ_PUT_AN, rsComm, &dataObjInp, packedBBuf, &portalOprOut );
            clearBBuf( packedBBuf );
            clearKeyVal( &dataObjInp.condInput );
            if ( portalOprOut != NULL ) {
                free( portalOprOut );
            }
            if ( status < 0 ) {
                rodsLogError( LOG_ERROR, status,
                              "rsNcGetAggInfo: rsDataObjPut error for %s", dataObjInp.objPath );
            }
        }
        if ( status < 0 ) {
            return status;
        }
        else {
            return savedStatus;
        }
    }
#endif // RODS_SERVER
    // =-=-=-=-=-=-=-
    // factory function to provide instance of the plugin
    irods::api_entry* plugin_factory(
        const std::string& _inst_name,
        const std::string& _context ) {
        // =-=-=-=-=-=-=-
        // create a api def object
        irods::apidef_t def = { NC_GET_AGG_INFO_AN,
                                RODS_API_VERSION,
                                REMOTE_USER_AUTH,
                                REMOTE_USER_AUTH,
                                "NcOpenInp_PI", 0,
                                "NcAggInfo_PI", 0,
                                0, 0 // null fcn ptr, handled in delay_load
                              }; 
        // =-=-=-=-=-=-=-
        // create an api object
        irods::api_entry* api = new irods::api_entry( def );

        // =-=-=-=-=-=-=-
        // assign the fcn which will handle the api call
#ifdef RODS_SERVER
        api->fcn_name_ = "rsNcGetAggInfo";
#endif // RODS_SERVER
         
        // =-=-=-=-=-=-=-
        // assign the pack struct key and value
        api->in_pack_key   = "NcOpenInp_PI";
        api->in_pack_value = NcOpenInp_PI;
        
        api->out_pack_key   = "NcAggInfo_PI";
        api->out_pack_value = NcAggInfo_PI;
        
        // =-=-=-=-=-=-=-
        // and... were done.
        return api;

    } // plugin_factory

}; // extern "C" 

