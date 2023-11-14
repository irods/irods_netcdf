// =-=-=-=-=-=-=-
#include <netcdf_port.hpp>
#include <irods/msParam.h>
//#include <irods/reGlobalsExtern.hpp>
#include <irods/irods_ms_plugin.hpp>
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
    * \fn msiNcGetArrayLen (msParam_t *inpParam, msParam_t *outParam, ruleExecInfo_t *rei)
    * \brief Get the array length of a NcInqWithIdOut_MS_T (output of msiNcInqWithId) or NcGetVarOut_MS_T (output of msiNcGetVarsByType).
    * \module core
    *
    * \since 3.2
    *
    * \author  Mike Wan
    * \date    2012
    *
    * \usage See clients/icommands/test/rules3.0/netcdfTest1.r, netcdfTest2.r and netcdfTest3.r.
    * \param[in] inpParam - A NcInqWithIdOut_MS_T or NcGetVarOut_MS_T which are outputs of the msiNcInqWithId and msiNcGetVarsByType msi, respectively.
    * \param[out] outParam - An INT_MS_T containing the array length.
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
    msiNcGetArrayLen (msParam_t *inpParam, msParam_t *outParam,
    ruleExecInfo_t *rei)
    {
        // should use rodsLong_t, but some problem with rule engine */
        // rodsLong_t arrayLen;
        int arrayLen;

        RE_TEST_MACRO ("    Calling msiNcGetArrayLen")

        if (inpParam == NULL || outParam == NULL) return USER__NULL_INPUT_ERR;

        if (strcmp (inpParam->type, NcInqWithIdOut_MS_T) == 0) {
            ncInqWithIdOut_t *ncInqWithIdOut;
            ncInqWithIdOut = (ncInqWithIdOut_t *) inpParam->inOutStruct;
            arrayLen = ncInqWithIdOut->mylong;
        } else if (strcmp (inpParam->type, NcGetVarOut_MS_T) == 0) {
            ncGetVarOut_t *ncGetVarOut;
            ncGetVarOut = (ncGetVarOut_t *) inpParam->inOutStruct;
            if (ncGetVarOut == NULL || ncGetVarOut->dataArray == NULL)
                return USER__NULL_INPUT_ERR;
            arrayLen = ncGetVarOut->dataArray->len;
        } else {
            rodsLog (LOG_ERROR,
              "msiNcGetArrayLen: Unsupported input Param type %s",
              inpParam->type);
            return (USER_PARAM_TYPE_ERR);
        }
        fillIntInMsParam (outParam, arrayLen);
        return 0;
    }


    irods::ms_table_entry*  plugin_factory( ) 
    {
        ADD_IRODS_MS_TABLE_ENTRY( 2, msiNcGetArrayLen );
    }

};



