// =-=-=-=-=-=-=-
#include "msParam.hpp"
#include "reGlobalsExtern.hpp"
#include "irods_ms_plugin.hpp"
#include "netcdfMS.hpp"

// =-=-=-=-=-=-=-
// STL Includes
#include <iostream>

extern "C" {

    // =-=-=-=-=-=-=-
    // 1. Set the version of the plugin.  1.0 is currently the standard
    double PLUGIN_INTERFACE_VERSION = 1.0;

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
        msvc->add_operation( "msiNcIntDataTypeToStr", "msiNcIntDataTypeToStr" );

        // =-=-=-=-=-=-=-
        // 6. return the newly created microservice plugin
        return msvc;
    }

}; // extern "C"



