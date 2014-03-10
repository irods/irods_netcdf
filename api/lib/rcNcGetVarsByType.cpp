/**
 * @file  rcNcGetVarsByType.c
 *
 */

/* This is script-generated code.  */
/* See ncGetVarsByType.h for a description of this API call.*/

#include "ncGetVarsByType.hpp"
#include "ncUtil.hpp"
/**
 * \fn rcNcGetVarsByType (rcComm_t *conn,  ncGetVarInp_t *ncGetVarInp,
ncGetVarOut_t **ncGetVarOut)
 *
 * \brief General NETCDF variable subsetting function (equivalent to nc_get_vars_type API).
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
 * Example for this API can be rather involved. See the getSingleNcVarData function in rcNcInq.c on the usage of this API.
 *
 * \param[in] conn - A rcComm_t connection handle to the server.
 * \param[in] ncGetVarInp - Elements of ncGetVarInp_t used :
 *    \li int \b dataType - valid values are defined in ncGetVarsByType.h - NC_BYTE, NC_CHAR, NC_SHORT, NC_INT, NC_FLOAT, NC_DOUBLE, NC_UBYTE, NC_USHORT, NC_UINT, NC_INT64, NC_UINT64 or NC_STRING.
 *    \li int \b ncid - the ncid from ncNcOpen.
 *    \li int \b varid - the variable Id from rcNcInq or rcNcInqId.
 *    \li int \b ndim - number of dimensions (rank).
 *    \li rodsLong_t \b *start - A vector of rodsLong_t with ndim length specifying the index in the variable where the first of the data values will be read.
 *    \li rodsLong_t \b *count - A vector of rodsLong_t with ndim length specifying the number of indices selected along each dimension.
 *    \li rodsLong_t \b *stride - A vector of rodsLong_t with ndim length specifying for each dimension, the interval between selected indices.
 * \param[out] ncGetVarOut - a ncGetVarOut_t. Elements of ncGetVarOut_t:
 *    \li char \b dataType_PI[NAME_LEN] - Packing instruction of the dataType.
 *    \li dataArray_t \b *dataArray - returned values of the variable. dataArray->type gives the var type; dataArray->len gives the var length; dataArray->buf contains the var values.
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
rcNcGetVarsByType( rcComm_t *conn,  ncGetVarInp_t *ncGetVarInp,
                   ncGetVarOut_t **ncGetVarOut ) {
    int status;
    status = procApiRequest( conn, NC_GET_VARS_BY_TYPE_AN, ncGetVarInp, NULL,
                             ( void ** ) ncGetVarOut, NULL );

    return ( status );
}

int
freeNcGetVarOut( ncGetVarOut_t **ncGetVarOut ) {
    if ( ncGetVarOut == NULL || *ncGetVarOut == NULL ) return
            USER__NULL_INPUT_ERR;

    clearNcGetVarOut( *ncGetVarOut );
    free( *ncGetVarOut );
    *ncGetVarOut = NULL;
    return 0;
}

int
clearNcGetVarOut( ncGetVarOut_t *ncGetVarOut ) {
    if ( ncGetVarOut == NULL ) {
        return USER__NULL_INPUT_ERR;
    }

    if ( ( ncGetVarOut )->dataArray != NULL ) {
        if ( ( ncGetVarOut )->dataArray->buf != NULL ) {
            free( ( ncGetVarOut )->dataArray->buf );
        }
        free( ( ncGetVarOut )->dataArray );
    }
    return 0;
}

void
clearNcGetVarInp( void * voidInp ) {
    ncGetVarInp_t * ncGetVarInp = ( ncGetVarInp_t * ) voidInp;
    if ( ncGetVarInp == NULL ) {
        irods::log( ERROR( USER__NULL_INPUT_ERR, 
              "Null input error in clearNcGetVarInp." ) );
        return;
    }
    if ( ncGetVarInp->start != NULL ) {
        free( ncGetVarInp->start );
        ncGetVarInp->start = NULL;
    }
    if ( ncGetVarInp->count != NULL ) {
        free( ncGetVarInp->count );
        ncGetVarInp->count = NULL;
    }
    if ( ncGetVarInp->stride != NULL ) {
        free( ncGetVarInp->stride );
        ncGetVarInp->stride = NULL;
    }
    clearKeyVal( &ncGetVarInp->condInput );
    return;
}

int
getSizeForGetVars( ncGetVarInp_t *ncGetVarInp ) {
    int i;
    int len = 1;

    for ( i = 0; i < ncGetVarInp->ndim; i++ ) {
        if ( ncGetVarInp->count[i] <= 0 ) {
            return NETCDF_VAR_COUNT_OUT_OF_RANGE;
        }
        /* cal dataArray->len */
        if ( ncGetVarInp->stride[i] <= 0 ) {
            ncGetVarInp->stride[i] = 1;
        }
        len = len * ( ( ncGetVarInp->count[i] - 1 ) / ncGetVarInp->stride[i] + 1 );
    }
    return len;
}

int
getDataTypeSize( int dataType ) {
    int size = 0;

    switch ( dataType ) {
    case NC_CHAR:
    case NC_BYTE:
    case NC_UBYTE:
        size = sizeof( char );
        break;
    case NC_STRING:
        size = sizeof( char * );
        break;
    case NC_INT:
    case NC_UINT:
        size = sizeof( int );
        break;
    case NC_SHORT:
    case NC_USHORT:
        size = sizeof( short );
        break;
    case NC_INT64:
    case NC_UINT64:
        size = sizeof( rodsLong_t );
        break;
    case NC_FLOAT:
        size = sizeof( float );
        break;
    case NC_DOUBLE:
        size = sizeof( double );
        break;
        rodsLog( LOG_ERROR,
                 "getDataTypeSize: Unknow dataType %d", dataType );
        return ( NETCDF_INVALID_DATA_TYPE );
    }
    return size;
}






