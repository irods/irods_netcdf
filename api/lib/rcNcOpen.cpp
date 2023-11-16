/**
 * @file  rcNcOpen.c
 *
 */

/*** Copyright (c), The Regents of the University of California            ***
 *** For more information please refer to files in the COPYRIGHT directory ***/
/* This is script-generated code.  */
/* See ncOpen.h for a description of this API call.*/

#include "ncOpen.hpp"
#include "ncClose.hpp"
#include "ncGetAggInfo.hpp"
#include "ncUtil.hpp"
#include "ncApiIndex.hpp"

#include <irods/getRemoteZoneResc.h>
#include <irods/irods_get_l1desc.hpp>
#include <irods/irods_server_api_call.hpp>
#include <irods/miscServerFunct.hpp>

/**
 * \fn rcNcOpen (rcComm_t *conn, ncOpenInp_t *ncOpenInp, int *ncid)
 *
 * \brief open an iRODS data object for netcdf operation (equivalent to nc_open).
 *
 * \user client
 *
 * \category NETCDF operations
 *
 * \since 3.1
 *
 * \author  Mike Wan
 * \date    2012
 *
 * \remark none
 *
 * \note none
 *
 * \usage
 * nc_open a data object /myZone/home/john/myfile.nc for write:
 * \n ncOpenInp_t ncOpenInp;
 * \n int ncid = 0;
 * \n int status;
 * \n bzero (&ncOpenInp, sizeof (ncOpenInp));
 * \n rstrcpy (ncOpenInp.objPath, "/myZone/home/john/myfile.nc", MAX_NAME_LEN);
 * \n ncOpenInp.mode = NC_NOWRITE;
 * \n status = rcNcOpen (conn, &ncOpenInp, &ncid);
 * \n if (status < 0) {
 * \n .... handle the error
 * \n }
 *
 * \param[in] conn - A rcComm_t connection handle to the server.
 * \param[in] ncOpenInp - Elements of ncOpenInp_t used :
 *    \li char \b objPath[MAX_NAME_LEN] - full path of the data object.
 *    \li int \b mode - the mode of the open - valid values are given in netcdf.h - NC_NOWRITE, NC_WRITE
 * \param[out] ncid - the ncid of the opened object.
 *
 * \return integer
 * \retval status of the call. success if greater or equal 0. error if negative.

 * \sideeffect none
 * \pre none
 * \post none
 * \sa none
 * \bug  no known bugs
**/

int
rcNcOpen( rcComm_t *conn, ncOpenInp_t *ncOpenInp, int *ncid ) {
    int status;
    int *myncid = NULL;

    status = procApiRequest( conn, NC_OPEN_AN,  ncOpenInp, NULL,
                             ( void ** )( static_cast< void * >( &myncid ) ), NULL );

    if ( myncid != NULL ) {
        *ncid = *myncid;
        free( myncid );
    }
    return ( status );
}

int
_rcNcOpen( rcComm_t *conn, ncOpenInp_t *ncOpenInp, int **ncid ) {
    int status;
    status = procApiRequest( conn, NC_OPEN_AN,  ncOpenInp, NULL,
                             ( void ** ) ncid, NULL );

    return ( status );
}

#ifdef RODS_SERVER
int
_rsNcOpenDataObj( rsComm_t *rsComm, ncOpenInp_t *ncOpenInp, int **ncid ) {
    int remoteFlag;
    rodsServerHost_t *rodsServerHost;
    int status;
    dataObjInp_t dataObjInp;
    int l1descInx = 0, myncid = 0;

    bzero( &dataObjInp, sizeof( dataObjInp ) );
    rstrcpy( dataObjInp.objPath, ncOpenInp->objPath, MAX_NAME_LEN );
    replKeyVal( &ncOpenInp->condInput, &dataObjInp.condInput );
    remoteFlag = getAndConnRemoteZone( rsComm, &dataObjInp, &rodsServerHost,
                                       REMOTE_OPEN );

    if ( remoteFlag < 0 ) {
        return ( remoteFlag );
    }
    else if ( remoteFlag == LOCAL_HOST ) {
        addKeyVal( &dataObjInp.condInput, NO_OPEN_FLAG_KW, "" );
        if ( getValByKey( &ncOpenInp->condInput, NO_STAGING_KW ) != NULL ) {
            addKeyVal( &dataObjInp.condInput, NO_STAGING_KW, "" );
        }

        l1descInx = irods::server_api_call ( DATA_OBJ_OPEN_AN, rsComm, &dataObjInp );
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
            status = nc_open( my_desc.dataObjInfo->filePath,
                              ncOpenInp->mode, &myncid );
            if ( status != NC_NOERR ) {
                rodsLog( LOG_ERROR,
                         "rsNcOpen: nc_open %s error, status = %d, %s",
                         my_desc.dataObjInfo->filePath, status,
                         nc_strerror( status ) );
                freeL1desc( l1descInx );
                return ( NETCDF_OPEN_ERR + status );
            }
        }
        else {
            /* execute it remotely with dataObjInfo->filePath */
            ncOpenInp_t myNcOpenInp;
            bzero( &myNcOpenInp, sizeof( myNcOpenInp ) );
            myNcOpenInp.mode = ncOpenInp->mode;
            rstrcpy( myNcOpenInp.objPath,
                     my_desc.dataObjInfo->filePath, MAX_NAME_LEN );
            addKeyVal( &myNcOpenInp.condInput, NATIVE_NETCDF_CALL_KW, "" );

            if ( ( status = svrToSvrConnect( rsComm, rodsServerHost ) ) < 0 ) {
                return status;
            }
            status = rcNcOpen( rodsServerHost->conn, &myNcOpenInp, &myncid );
            clearKeyVal( &myNcOpenInp.condInput );
            if ( status < 0 ) {
                rodsLog( LOG_ERROR,
                         "rsNcOpen: rcNcOpen %s error, status = %d",
                         myNcOpenInp.objPath, status );
                freeL1desc( l1descInx );
                return ( status );
            }
        }

        my_desc.l3descInx = myncid;
    }
    else {
        if ( ( status = svrToSvrConnect( rsComm, rodsServerHost ) ) < 0 ) {
            return status;
        }
        status = rcNcOpen( rodsServerHost->conn, ncOpenInp, &myncid );
        if ( status < 0 ) {
            rodsLog( LOG_ERROR,
                     "rsNcOpen: _rcNcOpen %s error, status = %d",
                     ncOpenInp->objPath, status );
            return ( status );
        }
        l1descInx = allocAndSetL1descForZoneOpr( myncid, &dataObjInp,
                    rodsServerHost, NULL );
    }


    l1desc_t& my_desc = irods::get_l1desc( l1descInx );
    if ( ncOpenInp->mode == NC_NOWRITE ) {
        my_desc.oprType = NC_OPEN_FOR_READ;
    }
    else {
        my_desc.oprType = NC_OPEN_FOR_WRITE;
    }
    *ncid = ( int * ) malloc( sizeof( int ) );
    *( *ncid ) = l1descInx;

    return 0;
}

int
_rsNcOpenColl( rsComm_t *rsComm, ncOpenInp_t *ncOpenInp, int **ncid ) {
    int status;
    ncAggInfo_t *ncAggInfo = NULL;
    int l1descInx;
    openedAggInfo_t newOpenedAggInfo;

    status = readAggInfo( rsComm, ncOpenInp->objPath, &ncOpenInp->condInput,
                          &ncAggInfo );
    if ( status < 0 ) {
        return status;
    }

    l1descInx = allocL1desc();
    if ( l1descInx < 0 ) {
        return l1descInx;
    }

    l1desc_t& my_desc = irods::get_l1desc( l1descInx );
    openedAggInfo_t * openedAggInfo = boost::any_cast< openedAggInfo_t >( &my_desc.pluginData );
    if(openedAggInfo == NULL)
    {
        openedAggInfo = &newOpenedAggInfo;
    }
    bzero( openedAggInfo, sizeof( openedAggInfo_t ) );
    openedAggInfo->ncAggInfo = ncAggInfo;
    openedAggInfo->objNcid = -1;    /* not opened */
    openedAggInfo->objNcid0 = -1;   /* not opened */
    my_desc.pluginData = openedAggInfo;
    status = openAggrFile( rsComm, l1descInx, 0 );
    if ( status < 0 ) {
        return status;
    }
    *ncid = ( int * ) malloc( sizeof( int ) );
    *( *ncid ) = l1descInx;

    return 0;
}

int
openAggrFile( rsComm_t *rsComm, int l1descInx, int aggElementInx ) {
    int status, status1;
    ncOpenInp_t ncOpenInp;
    ncCloseInp_t ncCloseInp;
    openedAggInfo_t *openedAggInfo;
    openedAggInfo_t newOpenedAggInfo;
    int *ncid = NULL;

    l1desc_t& my_desc = irods::get_l1desc( l1descInx );
    openedAggInfo = boost::any_cast< openedAggInfo_t >( &my_desc.pluginData );
    if(openedAggInfo == NULL)
    {
        openedAggInfo = &newOpenedAggInfo;
    }
    if ( aggElementInx > 0 && aggElementInx == openedAggInfo->aggElementInx ) {
        return 0;
    }
    bzero( &ncOpenInp, sizeof( ncOpenInp ) );
    rstrcpy( ncOpenInp.objPath,
             openedAggInfo->ncAggInfo->ncAggElement[aggElementInx].objPath,
             MAX_NAME_LEN );
    status = _rsNcOpenDataObj( rsComm, &ncOpenInp, &ncid );
    if ( status >= 0 ) {
        if ( aggElementInx > 0 && openedAggInfo->aggElementInx > 0 ) {
            bzero( &ncCloseInp, sizeof( ncCloseInp ) );
            ncCloseInp.ncid = openedAggInfo->objNcid;
            status1 = irods::server_api_call ( NC_CLOSE_AN, rsComm, &ncCloseInp );
            if ( status1 < 0 ) {
                rodsLogError( LOG_ERROR, status1,
                              "openAndInqAggrFile: rcNcClose error for %s",
                              openedAggInfo->ncAggInfo->ncObjectName );
            }
            if ( openedAggInfo->ncInqOut != NULL ) {
                freeNcInqOut( &openedAggInfo->ncInqOut );
            }
        }
        if ( aggElementInx == 0 ) {
            openedAggInfo->objNcid0 = *ncid;
        }
        else {
            openedAggInfo->objNcid = *ncid;
            openedAggInfo->aggElementInx = aggElementInx;
        }
        my_desc.pluginData = openedAggInfo;
        free( ncid );
    }
    else {
        rodsLogError( LOG_ERROR, status,
                      "openAndInqAggrFile: rsNcOpen error for %s",
                      openedAggInfo->ncAggInfo->ncAggElement[aggElementInx].objPath );
        return status;
    }
    return status;
}
#endif // RODS_SERVER
