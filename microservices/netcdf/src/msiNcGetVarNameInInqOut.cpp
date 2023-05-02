// =-=-=-=-=-=-=-
#include "netcdf_port.hpp"
#include <irods/msParam.h>
//#include "reGlobalsExtern.hpp"
#include <irods/irods_ms_plugin.hpp>
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
    * \fn msiNcGetVarNameInInqOut (msParam_t *ncInqOutParam, msParam_t *inxParam,
    msParam_t *outParam, ruleExecInfo_t *rei)
    * \brief Get the name of a variable in an array of variables in a NcInqOut_MS_T
    * \module core
    *
    * \since 3.2
    *
    * \author  Mike Wan
    * \date    2012
    *
    * \usage See clients/icommands/test/rules3.0/netcdfTest1.r, netcdfTest2.r and netcdfTest3.r.
    * \param[in] ncInqOutParam - A NcInqOut_MS_T which is the output of msiNcInq.
    * \param[in] inxParam - An INT_MS_T containing the index of an element in this variable array
    * \param[out] outParam - A INT_MS_T containing the name of the variable
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
    msiNcGetVarNameInInqOut (msParam_t *ncInqOutParam, msParam_t *inxParam,
    msParam_t *outParam, ruleExecInfo_t *rei)
    {
        ncInqOut_t *ncInqOut;
        int inx;

        RE_TEST_MACRO ("    Calling msiNcGetVarNameInInqOut")

        if (ncInqOutParam == NULL || inxParam == NULL || outParam == NULL)
            return USER__NULL_INPUT_ERR;

        if (strcmp (ncInqOutParam->type, NcInqOut_MS_T) != 0) {
            rodsLog (LOG_ERROR,
              "msiNcGetVarNameInInqOut: ncInqOutParam must be NcInqOut_MS_T. %s",
              ncInqOutParam->type);
            return (USER_PARAM_TYPE_ERR);
        } else {
            ncInqOut = (ncInqOut_t *) ncInqOutParam->inOutStruct;
        }
        inx = parseMspForPosInt (inxParam);
        if (inx < 0 || inx >= ncInqOut->nvars) {
            rodsLog (LOG_ERROR,
              "msiNcGetVarNameInInqOut: input inx %d is out of range. nvars  = %d",
              inx, ncInqOut->nvars);
            return NETCDF_VAR_COUNT_OUT_OF_RANGE;
        }

        /* global nvars */
        fillStrInMsParam (outParam, ncInqOut->var[inx].name);

        return 0;
    }

    irods::ms_table_entry*  plugin_factory( ) {

        ADD_IRODS_MS_TABLE_ENTRY( 3,msiNcGetVarNameInInqOut );
    }

}; // extern "C"



