// =-=-=-=-=-=-=-
#include "msParam.hpp"
#include "reGlobalsExtern.hpp"
#include "irods_ms_plugin.hpp"
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
    * \fn msiNcGetDataType (msParam_t *inpParam, msParam_t *outParam, ruleExecInfo_t *rei)
    * \brief Get the dataType of a NcInqWithIdOut_MS_T (output of msiNcInqWithId), NcGetVarInp_MS_T (input of msiNcGetVarsByType) or NcGetVarOut_MS_T (output of msiNcGetVarsByType).
    * \module core
    *
    * \since 3.2
    *
    * \author  Mike Wan
    * \date    2012
    *
    * \usage See clients/icommands/test/rules3.0/netcdfTest1.r, netcdfTest2.r and netcdfTest3.r.
    * \param[in] inpParam - A NcInqWithIdOut_MS_T, NcGetVarInp_MS_T or NcGetVarOut_MS_T.
    * \param[out] outParam - An INT_MS_T containing the dataType.
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
    msiNcGetDataType (msParam_t *inpParam, msParam_t *outParam,
    ruleExecInfo_t *rei)
    {
        int dataType;

        RE_TEST_MACRO ("    Calling msiNcGetDataType")

        if (inpParam == NULL || outParam == NULL) return USER__NULL_INPUT_ERR;

        if (strcmp (inpParam->type, NcInqWithIdOut_MS_T) == 0) {
            ncInqWithIdOut_t *ncInqWithIdOut;
            ncInqWithIdOut = (ncInqWithIdOut_t *) inpParam->inOutStruct;
            dataType = ncInqWithIdOut->dataType;
        } else if (strcmp (inpParam->type, NcGetVarInp_MS_T) == 0) {
            ncGetVarInp_t *ncGetVarInp;
            ncGetVarInp = (ncGetVarInp_t *) inpParam->inOutStruct;
            dataType = ncGetVarInp->dataType;
        } else if (strcmp (inpParam->type, NcGetVarOut_MS_T) == 0) {
            ncGetVarOut_t *ncGetVarOut;
            ncGetVarOut = (ncGetVarOut_t *) inpParam->inOutStruct;
            if (ncGetVarOut == NULL || ncGetVarOut->dataArray == NULL)
                return USER__NULL_INPUT_ERR;
            dataType = ncGetVarOut->dataArray->type;
        } else {
            rodsLog (LOG_ERROR,
              "msiNcGetNumDim: Unsupported input Param type %s",
              inpParam->type);
            return (USER_PARAM_TYPE_ERR);
        }
        fillIntInMsParam (outParam, dataType);
        return 0;
    }


    // =-=-=-=-=-=-=-
    // 3.  Create the plugin factory function which will return a microservice
    //     table entry
    irods::ms_table_entry*  plugin_factory( ) {
        // =-=-=-=-=-=-=-
        // 4.  allocate a microservice plugin which takes the number of function
        //     params as a parameter to the constructor
        irods::ms_table_entry* msvc = new irods::ms_table_entry( 2 );

        // =-=-=-=-=-=-=-
        // 5. add the microservice function as an operation to the plugin
        //    the first param is the name / key of the operation, the second
        //    is the name of the function which will be the microservice
        msvc->add_operation( "msiNcGetDataType", "msiNcGetDataType" );

        // =-=-=-=-=-=-=-
        // 6. return the newly created microservice plugin
        return msvc;
    }

}; // extern "C"



