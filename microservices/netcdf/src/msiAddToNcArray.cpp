// =-=-=-=-=-=-=-
#include <netcdf_port.hpp>
#include <irods/msParam.h>
//#include <irods/reGlobalsExtern.hpp>
#include <irods/irods_ms_plugin.hpp>
#include "ncGetVarsByType.hpp"
#include "netcdfMS.hpp"

// =-=-=-=-=-=-=-
// STL Includes
#include <iostream>

extern "C" {

    // =-=-=-=-=-=-=-
    // 1. Set the version of the plugin.  1.0 is currently the standard

    // =-=-=-=-=-=-=-
    // 2. Write a standard issue microservice

    /**
    * \fn msiAddToNcArray (msParam_t *elementParam, msParam_t *inxParam,
    msParam_t *ncArrayParam, ruleExecInfo_t *rei)
    * \brief Add a value to a variable value array in a DataArray_PI.
    * \module core
    *
    * \since 3.2
    *
    * \author  Mike Wan
    * \date    2012
    *
    * \usage See clients/icommands/test/rules3.0/netcdfTest1.r, netcdfTest2.r and netcdfTest3.r.
    * \param[in] elementParam - the value to add. Currently, only an INT_MS_T is subported.
    * \param[in] inxParam - An INT_MS_T containing the index of an element in the array to add
    * \param[out] ncArrayParam - A NcGetVarOut_MS_T containing the modified array.
    *
    * \DolVarDependence none
    * \DolVarModified none
    * \iCatAttrDependence none
    * \iCatAttrModified none
    * \sideeffect none
    *
    * \return integer
    * \retval 0 upon success
    * \pre N/A
    * \post N/A
    * \sa N/A
    *
    **/
    int
    msiAddToNcArray (msParam_t *elementParam, msParam_t *inxParam,
    msParam_t *ncArrayParam, ruleExecInfo_t *rei)
    {
        ncGetVarOut_t *ncArray;
        int inx;

        RE_TEST_MACRO ("    Calling msiAddToNcArray")

        if (elementParam == NULL || ncArrayParam == NULL)
            return USER__NULL_INPUT_ERR;

        inx = parseMspForPosInt (inxParam);
        if (inx < 0 || inx >= NC_MAX_DIMS) {
            rodsLog (LOG_ERROR,
              "msiAddToNcArray: input inx %d is out of range. max  = %d",
              inx, NC_MAX_DIMS);
            return NETCDF_VAR_COUNT_OUT_OF_RANGE;
        }

        if (strcmp (elementParam->type, INT_MS_T) == 0) {
            int *intArray;

            if (ncArrayParam->inOutStruct == NULL) {
                /* first time */
                ncArray = (ncGetVarOut_t *) calloc (1, sizeof (ncGetVarOut_t));
                ncArray->dataArray = (dataArray_t *)
                    calloc (1, sizeof (dataArray_t));
                ncArray->dataArray->type = NC_INT;
                rstrcpy (ncArray->dataType_PI, "intDataArray_PI", NAME_LEN);
                ncArray->dataArray->buf  = calloc (1, sizeof (int) * NC_MAX_DIMS);
                fillMsParam (ncArrayParam, NULL, NcGetVarOut_MS_T, ncArray, NULL);
            } else {
                ncArray = (ncGetVarOut_t *) ncArrayParam->inOutStruct;
                if (strcmp (ncArray->dataType_PI, "intDataArray_PI") != 0) {
                    rodsLog (LOG_ERROR,
                      "msiAddToNcArray: wrong dataType_PI for INT_MS_T %s",
                      ncArray->dataType_PI);
                    return (USER_PARAM_TYPE_ERR);
                }
            }
            intArray = (int *) ncArray->dataArray->buf;
            intArray[inx] = *((int *) elementParam->inOutStruct);
            if (ncArray->dataArray->len < inx + 1)
                ncArray->dataArray->len = inx + 1;
        } else {
            /* only do INT_MS_T for now */
            rodsLog (LOG_ERROR,
              "msiAddToNcArray: Unsupported input dataTypeParam type %s",
              elementParam->type);
            return (USER_PARAM_TYPE_ERR);
        }
        return 0;
    }

    irods::ms_table_entry*  plugin_factory( ) 
    {
        ADD_IRODS_MS_TABLE_ENTRY( 3, msiAddToNcArray);
    }

}; // extern "C"



