// =-=-=-=-=-=-=-
#include "netcdf_port.hpp"
#include "msParam.h"
//#include "reGlobalsExtern.hpp"
#include "irods_ms_plugin.hpp"
#include "ncGetVarsByType.hpp"
#include "netcdfMS.hpp"
#include "netcdfParseUtilities.hpp"
#include "irods_server_api_call.hpp"
#include "ncApiIndex.hpp"

// =-=-=-=-=-=-=-
// STL Includes
#include <iostream>

extern "C" {

    // =-=-=-=-=-=-=-
    // 1. Set the version of the plugin.  1.0 is currently the standard

    // =-=-=-=-=-=-=-
    // 2. Write a standard issue microservice

    /**
    * \fn msiNcGetVarsByType (msParam_t *dataTypeParam, msParam_t *ncidParam,  msParam_t *varidParam, msParam_t *ndimParam, msParam_t *startParam,  msParam_t *countParam, msParam_t *strideParam, msParam_t *outParam, ruleExecInfo_t *rei)
    * \brief General NETCDF variable subsetting microservice (equivalent to nc_get_vars_type API).
    * \module core
    *
    * \since 3.2
    *
    * \author  Mike Wan
    * \date    2012
    *
    * \usage See clients/icommands/test/rules3.0/netcdfTest1.r, netcdfTest2.r and netcdfTest3.r.
    * \param[in] dataTypeParam - A NcGetVarInp_MS_T, STR_MS_T or INT_MS_T. If it is a STR_MS_T or INT_MS_T, it contains dataType of the variable - valid values are defined in ncGetVarsByType.h - 1 (NC_BYTE), 2 (NC_CHAR), 3 (NC_SHORT), 4 (NC_INT), 5 (NC_FLOAT), 6 (NC_DOUBLE), 7 (NC_UBYTE), 8 (NC_USHORT), 9 (NC_UINT), 10 (NC_INT64), 11 (NC_UINT64) and 12 (NC_STRING).
    * \param[in] ncidParam - If dataTypeParam is a  STR_MS_T or INT_MS_T, it is a STR_MS_T or INT_MS_T containing the ncid of the opened object.
    * \param[in] varidParam - If dataTypeParam is a  STR_MS_T or INT_MS_T, it is a STR_MS_T or INT_MS_T containing the variable Id from msiNcInq or msiNcInqId.
    * \param[in] ndimParam - If dataTypeParam is a  STR_MS_T or INT_MS_T, it is a STR_MS_T or INT_MS_T containing the number of dimensions (rank).
    * \param[in] startParam - If dataTypeParam is a  STR_MS_T or INT_MS_T, it is a STR_MS_T or NcGetVarOut_MS_T containing info on vector of rodsLong_t with ndim length specifying the index of an element in the variable where the first of the data values will be read. For STR_MS_T input, the vector is represented by a string containing "start0%start1%start2%...%start[ndim-1]" where the start value in each dimension is separated by a '\%'.
    * \param[in] countParam - same input format as startParam representing a ndim vector of 'count' values - the number of indices selected along each dimension.
    * \param[in] strideParam - same input format as startParam representing a ndim vector of 'stride' values - the interval between selected indices for each dimension.
    * \param[out] outParam - A NcGetVarOut_MS_T containing a ncGetVarOut_t struct. Elements of ncGetVarOut_t:
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
    msiNcGetVarsByType (msParam_t *dataTypeParam, msParam_t *ncidParam,
    msParam_t *varidParam, msParam_t *ndimParam, msParam_t *startParam,
    msParam_t *countParam, msParam_t *strideParam,
    msParam_t *outParam, ruleExecInfo_t *rei)
    {
        rsComm_t *rsComm;
        ncGetVarInp_t ncGetVarInp;
        ncGetVarOut_t *ncGetVarOut = NULL;
        int ndimOut;

        RE_TEST_MACRO ("    Calling msiNcGetVarsByType")

        if (rei == NULL || rei->rsComm == NULL) {
          rodsLog (LOG_ERROR,
            "msiNcGetVarsByType: input rei or rsComm is NULL");
          return (SYS_INTERNAL_NULL_INPUT_ERR);
        }
        rsComm = rei->rsComm;

        if (dataTypeParam == NULL) {
            rodsLog (LOG_ERROR,
              "msiNcGetVarsByType: input dataTypeParam is NULL");
            return (SYS_INTERNAL_NULL_INPUT_ERR);
        }

        /* parse for dataType or ncGetVarInp_t */
        rei->status = parseMspForNcGetVarInp (dataTypeParam, &ncGetVarInp);

        if (rei->status < 0) return rei->status;

        if (ncidParam != NULL) {
            /* parse for ncid */
            ncGetVarInp.ncid = parseMspForPosInt (ncidParam);
            if (ncGetVarInp.ncid < 0) return ncGetVarInp.ncid;
        }

        if (varidParam != NULL) {
            /* parse for varid */
            ncGetVarInp.varid = parseMspForPosInt (varidParam);
            if (ncGetVarInp.varid < 0) return ncGetVarInp.varid;
        }

        if (ndimParam != NULL) {
            /* parse for ndim */
            ncGetVarInp.ndim = parseMspForPosInt (ndimParam);
            if (ncGetVarInp.ndim < 0) return ncGetVarInp.ndim;
        }

        if (startParam != NULL) {
            /* parse for start */
            rei->status = parseStrMspForLongArray (startParam, &ndimOut,
              &ncGetVarInp.start);
            if (rei->status < 0) return rei->status;
            if (ndimOut != ncGetVarInp.ndim) {
                rei->status = NETCDF_DIM_MISMATCH_ERR;
                rodsLogAndErrorMsg (LOG_ERROR, &rsComm->rError, rei->status,
                  "msiNcGetVarsByType: start dim = %d, input ndim = %d",
                  ndimOut, ncGetVarInp.ndim);
                return NETCDF_DIM_MISMATCH_ERR;
            }
        }

        if (countParam != NULL) {
            /* parse for count */
            rei->status = parseStrMspForLongArray (countParam, &ndimOut,
              &ncGetVarInp.count);
            if (rei->status < 0) return rei->status;
            if (ndimOut != ncGetVarInp.ndim) {
                rei->status = NETCDF_DIM_MISMATCH_ERR;
                rodsLogAndErrorMsg (LOG_ERROR, &rsComm->rError, rei->status,
                  "msiNcGetVarsByType: count dim = %d, input ndim = %d",
                  ndimOut, ncGetVarInp.ndim);
                return NETCDF_DIM_MISMATCH_ERR;
            }
        }

        if (strideParam != NULL) {
            /* parse for stride */
            rei->status = parseStrMspForLongArray (strideParam, &ndimOut,
              &ncGetVarInp.stride);
            if (rei->status < 0) return rei->status;
            if (ndimOut != ncGetVarInp.ndim) {
                rei->status = NETCDF_DIM_MISMATCH_ERR;
                rodsLogAndErrorMsg (LOG_ERROR, &rsComm->rError, rei->status,
                  "msiNcGetVarsByType: stride dim = %d, input ndim = %d",
                  ndimOut, ncGetVarInp.ndim);
                return NETCDF_DIM_MISMATCH_ERR;
            }
        }

        rei->status = irods::server_api_call ( NC_GET_VARS_BY_TYPE_AN, rsComm, &ncGetVarInp, &ncGetVarOut);
        clearNcGetVarInp (&ncGetVarInp);
        if (rei->status >= 0) {
            fillMsParam (outParam, NULL, NcGetVarOut_MS_T, ncGetVarOut, NULL);
        } else {
          rodsLogAndErrorMsg (LOG_ERROR, &rsComm->rError, rei->status,
            "msiNcGetVarsByType: api call to ncGetVarsByType failed, status = %d",
            rei->status);
        }

        return (rei->status);
    }

    irods::ms_table_entry*  plugin_factory( ) {
      ADD_IRODS_MS_TABLE_ENTRY(8,       msiNcGetVarsByType );

    }

}; // extern "C"



