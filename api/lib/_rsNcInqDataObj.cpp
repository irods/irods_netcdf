#include "ncInq.hpp"
#include "irods_get_l1desc.hpp"

#ifdef RODS_SERVER
int
_rsNcInqDataObj( rsComm_t *rsComm, ncInqInp_t *ncInqInp, ncInqOut_t **ncInqOut ) {
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
_rsNcInqColl( rsComm_t *rsComm, ncInqInp_t *ncInqInp, ncInqOut_t **ncInqOut ) {
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
    status = _rsNcInqDataObj( rsComm, &myNcInqInp, ncInqOut );
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
_rsNcInq( rsComm_t *rsComm, ncInqInp_t *ncInqInp, ncInqOut_t **ncInqOut ) {
    int status = ncInq( ncInqInp, ncInqOut );
    return status;
}



#endif
