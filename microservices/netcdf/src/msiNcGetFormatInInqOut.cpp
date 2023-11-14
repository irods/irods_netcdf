// =-=-=-=-=-=-=-
#include <netcdf_port.hpp>
#include <irods/msParam.h>
//#include <irods/reGlobalsExtern.hpp>
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
    * \fn msiNcGetFormatInInqOut (msParam_t *ncInqOutParam, msParam_t *outParam,
    ruleExecInfo_t *rei)
    * \brief Get the format of the NETCDF file in  a NcInqOut_MS_T
    * \module core
    *
    * \since 3.2
    *
    * \author  Mike Wan
    * \date    2012
    *
    * \usage See clients/icommands/test/rules3.0/netcdfTest1.r, netcdfTest2.r and netcdfTest3.r.
    * \param[in] ncInqOutParam - A NcInqOut_MS_T which is the output of msiNcInq.
    * \param[out] outParam - An INT_MS_T containing the format value
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
    msiNcGetFormatInInqOut (msParam_t *ncInqOutParam, msParam_t *outParam,
    ruleExecInfo_t *rei)
    {
        ncInqOut_t *ncInqOut;

        RE_TEST_MACRO ("    Calling msiNcGetFormatInInqOut")

        if (ncInqOutParam == NULL || outParam == NULL)
            return USER__NULL_INPUT_ERR;

        if (strcmp (ncInqOutParam->type, NcInqOut_MS_T) != 0) {
            rodsLog (LOG_ERROR,
              "msiNcGetFormatInInqOut: ncInqOutParam must be NcInqOut_MS_T. %s",
              ncInqOutParam->type);
            return (USER_PARAM_TYPE_ERR);
        } else {
            ncInqOut = (ncInqOut_t *) ncInqOutParam->inOutStruct;
        }

        /* global nvars */
        fillIntInMsParam (outParam, ncInqOut->format);

        return 0;
    }


    irods::ms_table_entry*  plugin_factory( ) {

        ADD_IRODS_MS_TABLE_ENTRY (2,msiNcGetFormatInInqOut );

    }

}; // extern "C"



