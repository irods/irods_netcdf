// =-=-=-=-=-=-=-
#include "netcdf_port.hpp"
#include <irods/msParam.h>
//#include "reGlobalsExtern.hpp"
#include <irods/irods_ms_plugin.hpp>
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
    * \fn msiNcGetNGrpsInInqOut (msParam_t *ncInqGrpsOutParam, msParam_t *outParam,
    ruleExecInfo_t *rei)
    * \brief Get the number of groups in a NcInqGrpsOut_MS_T
    * \module core
    *
    * \since 3.2
    *
    * \author  Mike Wan
    * \date    2012
    *
    * \usage See clients/icommands/test/rules3.0/netcdfTest1.r, netcdfTest2.r and netcdfTest3.r.
    * \param[in] ncInqGrpsOutParam - A NcInqGrpsOut_MS_T objtained from msiNcInqGrps
    * \param[out] outParam - An INT_MS_T containing the the number of groups
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
    msiNcGetNGrpsInInqOut (msParam_t *ncInqGrpsOutParam, msParam_t *outParam,
    ruleExecInfo_t *rei)
    {
        ncInqGrpsOut_t *ncInqGrpsOut;

        RE_TEST_MACRO ("    Calling msiNcGetNGrpsInInqOut")

        if (ncInqGrpsOutParam == NULL || outParam == NULL)
            return USER__NULL_INPUT_ERR;

        if (strcmp (ncInqGrpsOutParam->type, NcInqGrpsOut_MS_T) != 0) {
            rodsLog (LOG_ERROR,
              "msiNcGetNGrpsInInqOut: ncInqGrpsOutParam must be NcInqGrpsOut_MS_T. %s",
              ncInqGrpsOutParam->type);
            return (USER_PARAM_TYPE_ERR);
        } else {
            ncInqGrpsOut = (ncInqGrpsOut_t *) ncInqGrpsOutParam->inOutStruct;
        }

        /* global nvars */
        fillIntInMsParam (outParam, ncInqGrpsOut->ngrps);

        return 0;
    }


    irods::ms_table_entry*  plugin_factory( ) {

        ADD_IRODS_MS_TABLE_ENTRY( 2, msiNcGetNGrpsInInqOut );

    }

}; // extern "C"



