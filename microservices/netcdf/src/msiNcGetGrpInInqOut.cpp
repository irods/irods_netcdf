// =-=-=-=-=-=-=-
#include "netcdf_port.hpp"
#include "msParam.h"
//#include "reGlobalsExtern.hpp"
#include "irods_ms_plugin.hpp"
#include "netcdfMS.hpp"
#include "ncInqGrps.hpp"

// =-=-=-=-=-=-=-
// STL Includes
#include <iostream>

extern "C" {

    // =-=-=-=-=-=-=-
    // 1. Set the version of the plugin.  1.0 is currently the standard

    // =-=-=-=-=-=-=-
    // 2. Write a standard issue microservice

    /**
    * \fn msiNcGetGrpInInqOut (msParam_t *ncInqGrpsOutParam,
    msParam_t *inxParam, msParam_t *outParam, ruleExecInfo_t *rei)
    * \brief Get the name of a group in a NcInqGrpsOut_MS_T
    * \module core
    *
    * \since 3.2
    *
    * \author  Mike Wan
    * \date    2012
    *
    * \usage See clients/icommands/test/rules3.0/netcdfTest1.r, netcdfTest2.r and netcdfTest3.r.
    * \param[in] ncInqGrpsOutParam - A NcInqGrpsOut_MS_T objtained from msiNcInqGrps
    * \param[in] inxParam - An INT_MS_T containing the index of an element in the group array
    * \param[out] outParam - A STR_MS_T containing the name of the group
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
    msiNcGetGrpInInqOut (msParam_t *ncInqGrpsOutParam,
    msParam_t *inxParam, msParam_t *outParam, ruleExecInfo_t *rei)
    {
        ncInqGrpsOut_t *ncInqGrpsOut;
        int inx;

        RE_TEST_MACRO ("    Calling msiNcGetGrpInInqOut")

        if (ncInqGrpsOutParam == NULL || inxParam == NULL || outParam == NULL)
            return USER__NULL_INPUT_ERR;

        if (strcmp (ncInqGrpsOutParam->type, NcInqGrpsOut_MS_T) != 0) {
            rodsLog (LOG_ERROR,
              "msiNcGetGrpInInqOut: ncInqGrpsOutParam must be NcInqGrpsOut_MS_T. %s",
              ncInqGrpsOutParam->type);
            return (USER_PARAM_TYPE_ERR);
        } else {
            ncInqGrpsOut = (ncInqGrpsOut_t *) ncInqGrpsOutParam->inOutStruct;
        }
        inx = parseMspForPosInt (inxParam);
        if (inx < 0 || inx >= ncInqGrpsOut->ngrps) {
            rodsLog (LOG_ERROR,
              "msiNcGetGrpInInqOut: input inx %d is out of range. ngrps  = %d",
              inx, ncInqGrpsOut->ngrps);
            return NETCDF_VAR_COUNT_OUT_OF_RANGE;
        }

        /* global nvars */
        fillStrInMsParam (outParam, ncInqGrpsOut->grpName[inx]);

        return 0;
    }

    irods::ms_table_entry*  plugin_factory( ) {
        ADD_IRODS_MS_TABLE_ENTRY( 3, msiNcGetGrpInInqOut );
    }

}; // extern "C"



