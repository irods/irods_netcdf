
#include "nc_util.hpp"

/* _rsNcGetVarsByType - moved from server to client because it can be
 * called by client too
 */
int
_rsNcGetVarsByType( int ncid, ncGetVarInp_t *ncGetVarInp,
                    ncGetVarOut_t **ncGetVarOut ) {
    int status;
    size_t start[NC_MAX_DIMS], count[NC_MAX_DIMS];
    ptrdiff_t stride[NC_MAX_DIMS];
    int i;
    int len = 1;
    int hasStride = 0;

    if ( ncGetVarInp == NULL || ncGetVarOut == NULL ) {
        return USER__NULL_INPUT_ERR;
    }

    bzero( start, sizeof( start ) );
    bzero( count, sizeof( count ) );
    bzero( stride, sizeof( stride ) );
    for ( i = 0; i < ncGetVarInp->ndim; i++ ) {
        start[i] = ncGetVarInp->start[i];
        count[i] = ncGetVarInp->count[i];
        stride[i] = ncGetVarInp->stride[i];
    }
    len = getSizeForGetVars( ncGetVarInp );
    if ( len <= 0 ) {
        return len;
    }
    *ncGetVarOut = ( ncGetVarOut_t * ) calloc( 1, sizeof( ncGetVarOut_t ) );
    ( *ncGetVarOut )->dataArray = ( dataArray_t * ) calloc( 1, sizeof( dataArray_t ) );
    ( *ncGetVarOut )->dataArray->len = len;
    ( *ncGetVarOut )->dataArray->type = ncGetVarInp->dataType;

    switch ( ncGetVarInp->dataType ) {
    case NC_CHAR:
        ( *ncGetVarOut )->dataArray->buf = calloc( 1, sizeof( char ) * len );
        rstrcpy( ( *ncGetVarOut )->dataType_PI, "charDataArray_PI", NAME_LEN );
        if ( hasStride != 0 ) {
            status = nc_get_vars_text( ncid, ncGetVarInp->varid, start, count,
                                       stride, ( char * )( *ncGetVarOut )->dataArray->buf );
        }
        else {
            status = nc_get_vara_text( ncid, ncGetVarInp->varid, start, count,
                                       ( char * )( *ncGetVarOut )->dataArray->buf );
        }
        break;
    case NC_BYTE:
    case NC_UBYTE:
        ( *ncGetVarOut )->dataArray->buf = calloc( 1, sizeof( char ) * len );
        rstrcpy( ( *ncGetVarOut )->dataType_PI, "charDataArray_PI", NAME_LEN );
        if ( hasStride != 0 ) {
            status = nc_get_vars_uchar( ncid, ncGetVarInp->varid, start, count,
                                        stride, ( unsigned char * )( *ncGetVarOut )->dataArray->buf );
        }
        else {
            status = nc_get_vara_uchar( ncid, ncGetVarInp->varid, start, count,
                                        ( unsigned char * )( *ncGetVarOut )->dataArray->buf );
        }
        break;
    case NC_STRING:
        ( *ncGetVarOut )->dataArray->buf = calloc( 1, sizeof( char * ) * len );
        rstrcpy( ( *ncGetVarOut )->dataType_PI, "strDataArray_PI", NAME_LEN );
        if ( hasStride != 0 ) {
            status = nc_get_vars_string( ncid, ncGetVarInp->varid, start, count,
                                         stride, ( char ** )( *ncGetVarOut )->dataArray->buf );
        }
        else {
            status = nc_get_vara_string( ncid, ncGetVarInp->varid, start, count,
                                         ( char ** )( *ncGetVarOut )->dataArray->buf );
        }
        break;
    case NC_INT:
        ( *ncGetVarOut )->dataArray->buf = calloc( 1, sizeof( int ) * len );
        rstrcpy( ( *ncGetVarOut )->dataType_PI, "intDataArray_PI", NAME_LEN );
        if ( hasStride != 0 ) {
            status = nc_get_vars_int( ncid, ncGetVarInp->varid, start, count,
                                      stride, ( int * )( *ncGetVarOut )->dataArray->buf );
        }
        else {
            status = nc_get_vara_int( ncid, ncGetVarInp->varid, start, count,
                                      ( int * )( *ncGetVarOut )->dataArray->buf );
        }
        break;
    case NC_UINT:
        ( *ncGetVarOut )->dataArray->buf = calloc( 1, sizeof( unsigned int ) * len );
        rstrcpy( ( *ncGetVarOut )->dataType_PI, "intDataArray_PI", NAME_LEN );
        if ( hasStride != 0 ) {
            status = nc_get_vars_uint( ncid, ncGetVarInp->varid, start, count,
                                       stride, ( unsigned int * )( *ncGetVarOut )->dataArray->buf );
        }
        else {
            status = nc_get_vara_uint( ncid, ncGetVarInp->varid, start, count,
                                       ( unsigned int * )( *ncGetVarOut )->dataArray->buf );
        }
        break;
    case NC_SHORT:
        ( *ncGetVarOut )->dataArray->buf = calloc( 1, sizeof( short ) * len );
        rstrcpy( ( *ncGetVarOut )->dataType_PI, "int16DataArray_PI", NAME_LEN );
        if ( hasStride != 0 ) {
            status = nc_get_vars_short( ncid, ncGetVarInp->varid, start, count,
                                        stride, ( short * )( *ncGetVarOut )->dataArray->buf );
        }
        else {
            status = nc_get_vara_short( ncid, ncGetVarInp->varid, start, count,
                                        ( short * )( *ncGetVarOut )->dataArray->buf );
        }
        break;
    case NC_USHORT:
        ( *ncGetVarOut )->dataArray->buf = calloc( 1, sizeof( short ) * len );
        rstrcpy( ( *ncGetVarOut )->dataType_PI, "int16DataArray_PI", NAME_LEN );
        if ( hasStride != 0 ) {
            status = nc_get_vars_ushort( ncid, ncGetVarInp->varid, start, count,
                                         stride, ( unsigned short* )( *ncGetVarOut )->dataArray->buf );
        }
        else {
            status = nc_get_vara_ushort( ncid, ncGetVarInp->varid, start, count,
                                         ( unsigned short* )( *ncGetVarOut )->dataArray->buf );
        }
        break;
    case NC_INT64:
        ( *ncGetVarOut )->dataArray->buf = calloc( 1, sizeof( long long ) * len );
        rstrcpy( ( *ncGetVarOut )->dataType_PI, "int64DataArray_PI", NAME_LEN );
        if ( hasStride != 0 ) {
            status = nc_get_vars_longlong( ncid, ncGetVarInp->varid, start,
                                           count, stride, ( long long * )( *ncGetVarOut )->dataArray->buf );
        }
        else {
            status = nc_get_vara_longlong( ncid, ncGetVarInp->varid, start,
                                           count, ( long long * )( *ncGetVarOut )->dataArray->buf );
        }
        break;
    case NC_UINT64:
        ( *ncGetVarOut )->dataArray->buf = calloc( 1, sizeof( unsigned long long ) * len );
        rstrcpy( ( *ncGetVarOut )->dataType_PI, "int64DataArray_PI", NAME_LEN );
        if ( hasStride != 0 ) {
            status = nc_get_vars_ulonglong( ncid, ncGetVarInp->varid,
                                            start, count, stride,
                                            ( unsigned long long * )( *ncGetVarOut )->dataArray->buf );
        }
        else {
            status = nc_get_vara_ulonglong( ncid, ncGetVarInp->varid, start,
                                            count, ( unsigned long long * )( *ncGetVarOut )->dataArray->buf );
        }
        break;
    case NC_FLOAT:
        ( *ncGetVarOut )->dataArray->buf = calloc( 1, sizeof( float ) * len );
        rstrcpy( ( *ncGetVarOut )->dataType_PI, "intDataArray_PI", NAME_LEN );
        if ( hasStride != 0 ) {
            status = nc_get_vars_float( ncid, ncGetVarInp->varid, start, count,
                                        stride, ( float * )( *ncGetVarOut )->dataArray->buf );
        }
        else {
            status = nc_get_vara_float( ncid, ncGetVarInp->varid, start, count,
                                        ( float * )( *ncGetVarOut )->dataArray->buf );
        }
        break;
    case NC_DOUBLE:
        ( *ncGetVarOut )->dataArray->buf = calloc( 1, sizeof( double ) * len );
        rstrcpy( ( *ncGetVarOut )->dataType_PI, "int64DataArray_PI", NAME_LEN );
        if ( hasStride != 0 ) {
            status = nc_get_vars_double( ncid, ncGetVarInp->varid, start, count,
                                         stride, ( double * )( *ncGetVarOut )->dataArray->buf );
        }
        else {
            status = nc_get_vara_double( ncid, ncGetVarInp->varid, start, count,
                                         ( double * )( *ncGetVarOut )->dataArray->buf );
        }
        break;
    default:
        rodsLog( LOG_ERROR,
                 "_rsNcGetVarsByType: Unknow dataType %d", ncGetVarInp->dataType );
        return ( NETCDF_INVALID_DATA_TYPE );
    }

    if ( status != NC_NOERR ) {
        freeNcGetVarOut( ncGetVarOut );
        rodsLog( LOG_ERROR,
                 "_rsNcGetVarsByType:  nc_get_vars err varid %d dataType %d. %s ",
                 ncGetVarInp->varid, ncGetVarInp->dataType, nc_strerror( status ) );
        status = NETCDF_GET_VARS_ERR - status;
    }
    return status;
}


int
readAggInfo( rsComm_t*     rsComm, 
             char*         aggColl, 
             keyValPair_t* condInput,
             ncAggInfo_t** ncAggInfo ) {
    int status;
    dataObjInp_t dataObjInp;
    bytesBuf_t packedBBuf;
    portalOprOut_t *portalOprOut = NULL;

    bzero( &dataObjInp, sizeof( dataObjInp ) );
    bzero( &packedBBuf, sizeof( packedBBuf ) );
    if ( condInput != NULL ) {
        replKeyVal( condInput, &dataObjInp.condInput );
    }
    snprintf( dataObjInp.objPath, MAX_NAME_LEN, "%s/%s",
              aggColl, NC_AGG_INFO_FILE_NAME );
    dataObjInp.oprType = GET_OPR;
    status = rsDataObjGet( rsComm, &dataObjInp, &portalOprOut, &packedBBuf );
    clearKeyVal( &dataObjInp.condInput );
    if ( portalOprOut != NULL ) {
        free( portalOprOut );
    }
    if ( status < 0 ) {
        if ( status == CAT_NO_ROWS_FOUND ) {
            status = NETCDF_AGG_INFO_FILE_ERR;
        }
        rodsLogError( LOG_ERROR, status,
                      "readAggInfo: rsDataObjGet error for %s", dataObjInp.objPath );
        return status;
    }
    status = unpackStruct( packedBBuf.buf, ( void ** ) ncAggInfo,
                           "NcAggInfo_PI", RodsPackTable, XML_PROT );

    if ( status < 0 ) {
        rodsLogError( LOG_ERROR, status,
                      "readAggInfo: unpackStruct error for %s", dataObjInp.objPath );
        return NETCDF_AGG_INFO_FILE_ERR;
    }
    if ( *ncAggInfo == NULL || ( *ncAggInfo )->numFiles == 0 ) {
        rodsLog( LOG_ERROR,
                 "readAggInfo: No ncAggInfo for %s", dataObjInp.objPath );
        return NETCDF_AGG_INFO_FILE_ERR;
    }
    return status;
}


