/*** Copyright (c), The Regents of the University of California            ***
 *** For more information please refer to files in the COPYRIGHT directory ***/
/* This is script-generated code (for the most part).  */
/* See dataObjGet.h for a description of this API call.*/

#include "netcdf_port.hpp"
#include "ncInqGrps.hpp"
#include "rodsLog.h"
#include "dataObjOpen.h"
#include "rsGlobalExtern.hpp"
#include "rcGlobalExtern.h"
#include "rsApiHandler.hpp"
#include "objMetaOpr.hpp"
#include "physPath.hpp"
#include "specColl.hpp"
#include "getRemoteZoneResc.h"
#include "irods_get_l1desc.hpp"
#include "ncApiIndex.hpp"
#include "ncUtil.hpp"


#ifdef RODS_SERVER
int
_rsNcInqGrps( int ncid, ncInqGrpsOut_t **ncInqGrpsOut ) {
    int numgrps = 0;
    ncInqGrpsOut_t *myNInqGrpsOut = NULL;
    int *grpNcid;
    int i, status;

    status = nc_inq_grps( ncid, &numgrps, NULL );
    if ( status != NC_NOERR ) {
        rodsLog( LOG_ERROR,
                 "_rsNcInqGrps: nc_inq_grps error ncid = %d.  %s ",
                 ncid, nc_strerror( status ) );
        status = NETCDF_INQ_ERR + status;
        return status;
    }
    myNInqGrpsOut = *ncInqGrpsOut = ( ncInqGrpsOut_t * )
                                    calloc( 1, sizeof( ncInqGrpsOut_t ) );

    if ( numgrps <= 0 ) {
        return 0;
    }

    grpNcid = ( int * ) calloc( 1, numgrps * sizeof( int ) );

    status = nc_inq_grps( ncid, &numgrps, grpNcid );
    if ( status != NC_NOERR ) {
        rodsLog( LOG_ERROR,
                 "_rsNcInqGrps: nc_inq_grps error.  %s ", nc_strerror( status ) );
        free( grpNcid );
        status = NETCDF_INQ_ERR + status;
        return status;
    }
    myNInqGrpsOut->grpName = ( char ** ) calloc( 1, numgrps * sizeof( char * ) );
    for ( i = 0; i < numgrps; i++ ) {
        size_t len;
        status = nc_inq_grpname_len( grpNcid[i], &len );
        if ( status != NC_NOERR ) {
            rodsLog( LOG_ERROR,
                     "_rsNcInqGrps: c_inq_grpname_len error.  %s ",
                     nc_strerror( status ) );
            freeNcInqGrpsOut( ncInqGrpsOut );
            free( grpNcid );
            status = NETCDF_INQ_ERR + status;
            return status;
        }
        myNInqGrpsOut->grpName[i] = ( char * ) malloc( len + 1 );
        myNInqGrpsOut->ngrps++;
        status = nc_inq_grpname_full( grpNcid[i], &len,
                                      myNInqGrpsOut->grpName[i] );
        if ( status != NC_NOERR ) {
            rodsLog( LOG_ERROR,
                     "_rsNcInqGrps: nc_inq_grpname_full error.  %s ",
                     nc_strerror( status ) );
            freeNcInqGrpsOut( ncInqGrpsOut );
            free( grpNcid );
            status = NETCDF_INQ_ERR + status;
            return status;
        }
    }
    free( grpNcid );
    return 0;
}
#endif // RODS_SERVER

extern "C" {
    double get_plugin_interface_version() { return 1.0; }
#ifdef RODS_SERVER
    int
    rsNcInqGrps( rsComm_t *rsComm, ncInqGrpsInp_t *ncInqGrpsInp,
                     ncInqGrpsOut_t **ncInqGrpsOut ) {
        int remoteFlag;
        rodsServerHost_t *rodsServerHost;
        int status;
        int l1descInx;
        int ncid;

        if ( getValByKey( &ncInqGrpsInp->condInput, NATIVE_NETCDF_CALL_KW ) != NULL ) {
            /* just all nc_open with objPath as file nc file path */
            /* must to be called internally */
            if ( rsComm->proxyUser.authInfo.authFlag < REMOTE_PRIV_USER_AUTH ) {
                return( CAT_INSUFFICIENT_PRIVILEGE_LEVEL );
            }
            status = _rsNcInqGrps( ncInqGrpsInp->ncid, ncInqGrpsOut );
            return status;
        }
        l1descInx = ncInqGrpsInp->ncid;
        if ( l1descInx < 2 || l1descInx >= NUM_L1_DESC ) {
            rodsLog( LOG_ERROR,
                     "rsNcClose: l1descInx %d out of range",
                     l1descInx );
            return ( SYS_FILE_DESC_OUT_OF_RANGE );
        }
        
        l1desc_t& my_desc = irods::get_l1desc( l1descInx );
        if ( my_desc.inuseFlag != FD_INUSE ) {
            return BAD_INPUT_DESC_INDEX;
        }
        if ( my_desc.remoteZoneHost != NULL ) {
            ncInqGrpsInp_t myNcInqGrpsInp;
            bzero( &myNcInqGrpsInp, sizeof( myNcInqGrpsInp ) );
            myNcInqGrpsInp.ncid = my_desc.remoteL1descInx;

            status = rcNcInqGrps( my_desc.remoteZoneHost->conn,
                                  &myNcInqGrpsInp, ncInqGrpsOut );
            if ( status < 0 ) {
                rodsLog( LOG_ERROR,
                         "rsNcInqGrps: rcNcInqGrps, status = %d", status );
            }
        }
        else {
            /* local zone */
            ncid = my_desc.l3descInx;
            remoteFlag = resoAndConnHostByDataObjInfo( rsComm,
                         my_desc.dataObjInfo, &rodsServerHost );
            if ( remoteFlag < 0 ) {
                return ( remoteFlag );
            }
            else if ( remoteFlag == LOCAL_HOST ) {
                status = _rsNcInqGrps( ncid, ncInqGrpsOut );
            }
            else {
                /* execute it remotely */
                ncInqGrpsInp_t myNcInqGrpsInp;
                bzero( &myNcInqGrpsInp, sizeof( myNcInqGrpsInp ) );
                myNcInqGrpsInp.ncid = ncid;
                addKeyVal( &myNcInqGrpsInp.condInput, NATIVE_NETCDF_CALL_KW, "" );
                status = rcNcInqGrps( rodsServerHost->conn, &myNcInqGrpsInp,
                                      ncInqGrpsOut );
                clearKeyVal( &myNcInqGrpsInp.condInput );
                if ( status < 0 ) {
                    rodsLog( LOG_ERROR,
                             "rsNcInqGrps: rcNcInqGrps, status = %d", status );
                }
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

        irods::apidef_t def = { NC_INQ_GRPS_AN,
                                RODS_API_VERSION,
                                REMOTE_USER_AUTH,
                                REMOTE_USER_AUTH,
                                "NcInqGrpsInp_PI", 0,
                                "NcInqGrpsOut_PI", 0,
#ifdef RODS_SERVER
                                CPP_14_FUNCTION( rsNcInqGrps ),
#else
                                CPP_14_NOOPFUNC( rsComm_t*, ncInqGrpsInp_t *, ncInqGrpsOut_t **),
#endif // RODS_SERVER
                                "api_nc_inq_grps",
                                [](void*){},
                                (funcPtr) RODS_SERVER_ENABLE(( irods::netcdf::api_call_wrapper< ncInqGrpsInp_t *, ncInqGrpsOut_t ** > ))
                              };
        // =-=-=-=-=-=-=-
        // create an api object
        irods::api_entry* api = new irods::api_entry( def );

        // =-=-=-=-=-=-=-
        // assign the pack struct key and value
        api->in_pack_key   = "NcInqGrpsInp_PI";
        api->in_pack_value = NcInqGrpsInp_PI;

        api->out_pack_key   = "NcInqGrpsOut_PI";
        api->out_pack_value = NcInqGrpsOut_PI;

        // =-=-=-=-=-=-=-
        // and... were done.
        return api;

    } // plugin_factory

}; // extern "C"
