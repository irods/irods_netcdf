/*** Copyright (c), The Regents of the University of California            ***
 *** For more information please refer to files in the COPYRIGHT directory ***/
/* rsNcRegGlobalAttr.c
 */

#include "ncRegGlobalAttr.hpp"
#include "ncInq.hpp"
#include "ncClose.hpp"
#include "modAVUMetadata.hpp"
#include "icatHighLevelRoutines.hpp"

int
_rsNcRegGlobalAttr( rsComm_t *rsComm, ncRegGlobalAttrInp_t *ncRegGlobalAttrInp ) {
#ifdef RODS_CAT
    ncOpenInp_t ncOpenInp;
    ncCloseInp_t ncCloseInp;
    int *ncidPtr = NULL;
    ncInqInp_t ncInqInp;
    ncInqOut_t *ncInqOut = NULL;
    int i, j, status, status1;
    void *valuePtr;
    modAVUMetadataInp_t modAVUMetadataInp;
    char tempStr[NAME_LEN];

    bzero( &ncOpenInp, sizeof( ncOpenInp_t ) );
    rstrcpy( ncOpenInp.objPath, ncRegGlobalAttrInp->objPath, MAX_NAME_LEN );
#ifdef NETCDF4_API
    ncOpenInp.mode = NC_NOWRITE | NC_NETCDF4;
#else
    ncOpenInp.mode = NC_NOWRITE;
#endif
    addKeyVal( &ncOpenInp.condInput, NO_STAGING_KW, "" );

    status = rsNcOpen( rsComm, &ncOpenInp, &ncidPtr );

    if ( status < 0 ) {
        rodsLogError( LOG_ERROR, status,
                      "_rsNcRegGlobalAttr: rcNcOpen error for %s", ncOpenInp.objPath );
        return status;
    }

    /* do the general inq */
    bzero( &ncInqInp, sizeof( ncInqInp ) );
    ncInqInp.ncid = *ncidPtr;
    free( ncidPtr );
    ncInqInp.paramType = NC_ALL_TYPE;
    ncInqInp.flags = NC_ALL_FLAG;

    status = rsNcInq( rsComm, &ncInqInp, &ncInqOut );
    if ( status < 0 ) {
        rodsLogError( LOG_ERROR, status,
                      "_rsNcRegGlobalAttr: rcNcInq error for %s", ncOpenInp.objPath );
        return status;
    }
    /* register them */
    bzero( &modAVUMetadataInp, sizeof( modAVUMetadataInp ) );
    if ( getValByKey( &ncRegGlobalAttrInp->condInput, ADMIN_KW ) == NULL ) {
        modAVUMetadataInp.arg0 = "add";
    }
    else {
        modAVUMetadataInp.arg0 = "adda";    /* admin mod */
    }
    modAVUMetadataInp.arg1 = "-d";
    modAVUMetadataInp.arg2 =  ncOpenInp.objPath;
    modAVUMetadataInp.arg5 = "";                /* unit */

    /* global attrbutes */
    for ( i = 0; i < ncInqOut->ngatts; i++ ) {
        valuePtr = ncInqOut->gatt[i].value.dataArray->buf;
        modAVUMetadataInp.arg3 = ncInqOut->gatt[i].name;
        valuePtr = ncInqOut->gatt[i].value.dataArray->buf;
        /* see if attri name matches the input value */
        if ( ncRegGlobalAttrInp->numAttrName > 0 &&
                ncRegGlobalAttrInp->attrNameArray != NULL ) {
            int amatch = False;
            for ( j = 0; j < ncRegGlobalAttrInp->numAttrName; j++ ) {
                if ( strcmp( ncRegGlobalAttrInp->attrNameArray[j],
                             ncInqOut->gatt[i].name ) == 0 ) {
                    amatch = True;
                    break;
                }
            }
            if ( amatch == False ) {
                continue;
            }
        }
        if ( ncInqOut->gatt[i].dataType == NC_CHAR ) {
            /* assume it is a string */
            modAVUMetadataInp.arg4 = ( char * ) valuePtr;
        }
        else {
            ncValueToStr( ncInqOut->gatt[i].dataType, &valuePtr,
                          tempStr );
            modAVUMetadataInp.arg4 = tempStr;
        }
        status = rsModAVUMetadata( rsComm, &modAVUMetadataInp );
        if ( status < 0 ) {
            if ( status == CATALOG_ALREADY_HAS_ITEM_BY_THAT_NAME ) {
                status = 0;
            }
            else {
                rodsLogError( LOG_ERROR, status,
                              "_rsNcRegGlobalAttr: rcModAVUMetadata error for %s, attr = %s",
                              ncRegGlobalAttrInp->objPath, modAVUMetadataInp.arg3 );
                break;
            }
        }
    }
    freeNcInqOut( &ncInqOut );
    bzero( &ncCloseInp, sizeof( ncCloseInp_t ) );
    ncCloseInp.ncid = ncInqInp.ncid;
    status1 = rsNcClose( rsComm, &ncCloseInp );
    if ( status1 < 0 ) {
        rodsLogError( LOG_ERROR, status1,
                      "_rsNcRegGlobalAttr: rcNcClose error for %s", ncOpenInp.objPath );
    }
    return ( status );
#else
    return ( SYS_NO_RCAT_SERVER_ERR );
#endif

}

extern "C" {
    int
    rsNcRegGlobalAttr( rsComm_t *rsComm, ncRegGlobalAttrInp_t *ncRegGlobalAttrInp ) {
        int status;
        rodsServerHost_t *rodsServerHost = NULL;

        status = getAndConnRcatHost( rsComm, MASTER_RCAT,
                                     ncRegGlobalAttrInp->objPath, &rodsServerHost );
        if ( status < 0 ) {
            return( status );
        }

        if ( rodsServerHost->localFlag == LOCAL_HOST ) {
#ifdef RODS_CAT
            status = _rsNcRegGlobalAttr( rsComm, ncRegGlobalAttrInp );
#else
            status = SYS_NO_RCAT_SERVER_ERR;
#endif
        }
        else {
            status = rcNcRegGlobalAttr( rodsServerHost->conn, ncRegGlobalAttrInp );
        }
        return ( status );
    }

    // =-=-=-=-=-=-=-
    // factory function to provide instance of the plugin
    irods::api_entry* plugin_factory(
        const std::string& _inst_name,
        const std::string& _context ) {
        // =-=-=-=-=-=-=-
        // create a api def object
        irods::apidef_t def = { NC_REG_GLOBAL_ATTR_AN,
                                RODS_API_VERSION,
                                REMOTE_USER_AUTH,
                                REMOTE_USER_AUTH,
                                "NcRegGlobalAttrInp_PI", 0,
                                NULL, 0,
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
        strncpy( api->in_pack_key, "NcRegGlobalAttrInp_PI", MAX_NAME_LEN );
        api->inPackInstruct = NcRegGlobalAttrInp_PI;
        
        //strncpy( api->in_pack_key, "", MAX_NAME_LEN );
        //api->inPackInstruct = ;
        
        // =-=-=-=-=-=-=-
        // and... were done.
        return api;

    } // plugin_factory




}; // extern "C"



