// =-=-=-=-=-=-=-
#include "netcdf_port.hpp"
#include <irods/msParam.h>
#include "ncInq.hpp"
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
    * \fn msiNcIntDataTypeToStr (msParam_t *dataTypeParam, msParam_t *outParam,
    ruleExecInfo_t *rei)
    * \brief Covert an integer NETCDF type to string
    * \module core
    *
    * \since 3.2
    *
    * \author  Mike Wan
    * \date    2012
    *
    * \usage See clients/icommands/test/rules3.0/netcdfTest1.r, netcdfTest2.r and netcdfTest3.r.
    * \param[in] dataTypeParam - A INT_MS_T containing the dataType.
    * \param[out] outParam - A STR_MS_T containing the NETCDF type string
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
    msiNcIntDataTypeToStr (msParam_t *dataTypeParam, msParam_t *outParam,
    ruleExecInfo_t *rei)
    {
        int dataType;
        char dataTypeStr[NAME_LEN];
        int status;

        RE_TEST_MACRO ("    Calling msiNcIntDataTypeToStr")

        if (dataTypeParam == NULL || outParam == NULL) return USER__NULL_INPUT_ERR;

        if (strcmp (dataTypeParam->type, INT_MS_T) != 0) {
            rodsLog (LOG_ERROR,
              "msiNcIntDataTypeToStr: Unsupported input dataTypeParam type %s",
              dataTypeParam->type);
            return (USER_PARAM_TYPE_ERR);
        }

        dataType = *((int *) dataTypeParam->inOutStruct);

        if ((status = getNcTypeStr (dataType, dataTypeStr)) < 0) return status;

        fillStrInMsParam (outParam, dataTypeStr);

        return 0;
    }


    irods::ms_table_entry*  plugin_factory( ) {
      ADD_IRODS_MS_TABLE_ENTRY(2, msiNcIntDataTypeToStr );

    }

}; // extern "C"



