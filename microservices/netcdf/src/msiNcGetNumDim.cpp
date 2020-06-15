// =-=-=-=-=-=-=-
#include "netcdf_port.hpp"
#include "msParam.h"
//#include "reGlobalsExtern.hpp"
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
    * \fn msiNcGetNumDim (msParam_t *inpParam, msParam_t *outParam, ruleExecInfo_t *rei)
    * \brief Get the number of dimensions of a NcInqWithIdOut_MS_T (output of msiNcInqWithId) or NcGetVarOut_MS_T (output of msiNcGetVarsByType).
    * \module core
    *
    * \since 3.2
    *
    * \author  Mike Wan
    * \date    2012
    *
    * \usage See clients/icommands/test/rules3.0/netcdfTest1.r, netcdfTest2.r and netcdfTest3.r.
    * \param[in] inpParam - A NcInqWithIdOut_MS_T or NcGetVarOut_MS_T which are outputs of the msiNcInqWithId and msiNcGetVarsByType msi, respectively.
    * \param[out] outParam - An INT_MS_T containing the number of dimensions
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
    msiNcGetNumDim (msParam_t *inpParam, msParam_t *outParam,
    ruleExecInfo_t *rei)
    {
        int ndim;

        RE_TEST_MACRO ("    Calling msiNcGetNumDim")

        if (inpParam == NULL || outParam == NULL) return USER__NULL_INPUT_ERR;

        if (strcmp (inpParam->type, NcInqWithIdOut_MS_T) == 0) {
            ncInqWithIdOut_t *ncInqWithIdOut;
            ncInqWithIdOut = (ncInqWithIdOut_t *) inpParam->inOutStruct;
            ndim = ncInqWithIdOut->ndim;
        } else if (strcmp (inpParam->type, NcGetVarInp_MS_T) == 0) {
            ncGetVarInp_t *ncGetVarInp;
            ncGetVarInp = (ncGetVarInp_t *) inpParam->inOutStruct;
            ndim = ncGetVarInp->ndim;
        } else {
            rodsLog (LOG_ERROR,
              "msiNcGetNumDim: Unsupported input Param type %s",
              inpParam->type);
            return (USER_PARAM_TYPE_ERR);
        }
        fillIntInMsParam (outParam, ndim);
        return 0;
    }

    irods::ms_table_entry*  plugin_factory( ) {

        ADD_IRODS_MS_TABLE_ENTRY( 2, msiNcGetNumDim );

    }

}; // extern "C"



