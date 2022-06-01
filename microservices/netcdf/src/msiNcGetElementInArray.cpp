// =-=-=-=-=-=-=-
#include "netcdf_port.hpp"
#include <irods/msParam.h>
//#include "reGlobalsExtern.hpp"
#include <irods/irods_ms_plugin.hpp>
#include "ncGetVarsByType.hpp"
#include "netcdfMS.hpp"
#include "ncInqWithId.hpp"

// =-=-=-=-=-=-=-
// STL Includes
#include <iostream>

extern "C" {

    // =-=-=-=-=-=-=-
    // 1. Set the version of the plugin.  1.0 is currently the standard

    // =-=-=-=-=-=-=-
    // 2. Write a standard issue microservice

    /**
    * \fn msiNcGetElementInArray (msParam_t *arrayStructParam, msParam_t *indexParam, msParam_t *outParam, ruleExecInfo_t *rei)
    * \brief Get the value of an element in an array. The position of the element in the array is given in the indexParam input.
    * \module core
    *
    * \since 3.2
    *
    * \author  Mike Wan
    * \date    2012
    *
    * \usage See clients/icommands/test/rules3.0/netcdfTest1.r, netcdfTest2.r and netcdfTest3.r.
    * \param[in] indexParam - An INT_MS_T containing the index of an element in the array
    * \param[out] outParam - An INT_MS_T, CHAR_MS_T, STR_MS_T, or DOUBLE_MS_T, etc (depending on the dataType of the array) containing the value of the element).
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
    msiNcGetElementInArray (msParam_t *arrayStructParam, msParam_t *indexParam,
    msParam_t *outParam, ruleExecInfo_t *rei)
    {
        int myindex;
        void *myarray;
        int dataType;
        int arrayLen;
        char *charArray;
        int *intArray;
        float *floatArray;
        rodsLong_t *longArray;
        char **strArray;

        RE_TEST_MACRO ("    Calling msiNcGetElementInArray")

        if (arrayStructParam == NULL || indexParam == NULL ||
          outParam == NULL) return USER__NULL_INPUT_ERR;

        if (strcmp (arrayStructParam->type, NcInqWithIdOut_MS_T) == 0) {
            /* intArray for the id array */
            ncInqWithIdOut_t *ncInqWithIdOut;
            ncInqWithIdOut = (ncInqWithIdOut_t *) arrayStructParam->inOutStruct;
            dataType = NC_INT;
            myarray = (void *) ncInqWithIdOut->intArray;
            arrayLen = ncInqWithIdOut->ndim;
        } else if (strcmp (arrayStructParam->type, NcGetVarOut_MS_T) == 0) {
            ncGetVarOut_t *ncGetVarOut;
            ncGetVarOut = (ncGetVarOut_t *) arrayStructParam->inOutStruct;
            if (ncGetVarOut == NULL || ncGetVarOut->dataArray == NULL)
                return USER__NULL_INPUT_ERR;
            dataType = ncGetVarOut->dataArray->type;
            myarray = ncGetVarOut->dataArray->buf;
            arrayLen = ncGetVarOut->dataArray->len;
        } else {
            rodsLog (LOG_ERROR,
              "msiNcGetNumDim: Unsupported input Param type %s",
              arrayStructParam->type);
            return (USER_PARAM_TYPE_ERR);
        }
        myindex = parseMspForPosInt (indexParam);
        if (myindex < 0 || myindex >= arrayLen) {
            rodsLog (LOG_ERROR,
              "msiNcGetElementInArray: input index %d out of range. arrayLen = %d",
              myindex, arrayLen);
            return (NETCDF_DIM_MISMATCH_ERR);
        }

        switch (dataType) {
          case NC_CHAR:
          case NC_BYTE:
          case NC_UBYTE:
            charArray = (char *) myarray;
            fillCharInMsParam (outParam, charArray[myindex]);
            break;
          case NC_STRING:
            strArray = (char **) myarray;
            fillStrInMsParam (outParam, strArray[myindex]);
            break;
          case NC_INT:
          case NC_UINT:
            intArray = (int *) myarray;
            fillIntInMsParam (outParam, intArray[myindex]);
            break;
          case NC_FLOAT:
            floatArray = (float *) myarray;
            fillFloatInMsParam (outParam, floatArray[myindex]);
            break;
          case NC_INT64:
          case NC_UINT64:
          case NC_DOUBLE:
            longArray = (rodsLong_t *) myarray;
            fillDoubleInMsParam (outParam, longArray[myindex]);
            break;
          default:
            rodsLog (LOG_ERROR,
              "msiNcGetElementInArray: Unknow dataType %d", dataType);
            return (NETCDF_INVALID_DATA_TYPE);
        }
        return 0;
    }



    irods::ms_table_entry*  plugin_factory( ) {

      ADD_IRODS_MS_TABLE_ENTRY(3,msiNcGetElementInArray );

    }

}; // extern "C"



