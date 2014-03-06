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
    * \fn msiNcGetNdimsInInqOut (msParam_t *ncInqOutParam, msParam_t *varNameParam,
    msParam_t *outParam, ruleExecInfo_t *rei)
    * \brief Get the number of dimensions of a variable in a NcInqOut_MS_T
    * \module core
    *
    * \since 3.2
    *
    * \author  Mike Wan
    * \date    2012
    *
    * \usage See clients/icommands/test/rules3.0/netcdfTest1.r, netcdfTest2.r and netcdfTest3.r.
    * \param[in] ncInqOutParam - A NcInqOut_MS_T which is the output of msiNcInq.
    * \param[in] varNameParam - A STR_MS_T containing the name of a variable.
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
    msiNcGetNdimsInInqOut (msParam_t *ncInqOutParam, msParam_t *varNameParam,
    msParam_t *outParam, ruleExecInfo_t *rei)
    {
        int ndims = -1;
        ncInqOut_t *ncInqOut;
        char *name;

        RE_TEST_MACRO ("    Calling msiNcGetNdimInInqOut")

        if (ncInqOutParam == NULL || varNameParam == NULL || outParam == NULL)
            return USER__NULL_INPUT_ERR;

        if (strcmp (ncInqOutParam->type, NcInqOut_MS_T) != 0) {
            rodsLog (LOG_ERROR,
              "msiNcGetNdimsInInqOut: ncInqOutParam must be NcInqOut_MS_T. %s",
              ncInqOutParam->type);
            return (USER_PARAM_TYPE_ERR);
        } else {
            ncInqOut = (ncInqOut_t *) ncInqOutParam->inOutStruct;
        }
        if (strcmp (varNameParam->type, STR_MS_T) != 0) {
            rodsLog (LOG_ERROR,
              "msiNcGetNdimsInInqOut: varNameParam must be STR_MS_T. %s",
              varNameParam->type);
            return (USER_PARAM_TYPE_ERR);
        } else {
            name = (char*) varNameParam->inOutStruct;
        }

        if (strcmp (name, "null") == 0) {
            /* global ndims */
            ndims = ncInqOut->ndims;
        } else {
            int i;
            /* variable vndims */
            for (i = 0; i < ncInqOut->nvars; i++) {
                if (strcmp (ncInqOut->var[i].name, name) == 0) {
                    ndims = ncInqOut->var[i].nvdims;
                    break;
                }
            }
            if (ndims < 0) {
                rodsLog (LOG_ERROR,
                  "msiNcGetNdimInInqOut: Unmatch variable name %s.", name);
                return NETCDF_UNMATCHED_NAME_ERR;
            }
        }
        fillIntInMsParam (outParam, ndims);

        return 0;
    }

    // =-=-=-=-=-=-=-
    // 3.  Create the plugin factory function which will return a microservice
    //     table entry
    irods::ms_table_entry*  plugin_factory( ) {
        // =-=-=-=-=-=-=-
        // 4.  allocate a microservice plugin which takes the number of function
        //     params as a parameter to the constructor
        irods::ms_table_entry* msvc = new irods::ms_table_entry( 3 );

        // =-=-=-=-=-=-=-
        // 5. add the microservice function as an operation to the plugin
        //    the first param is the name / key of the operation, the second
        //    is the name of the function which will be the microservice
        msvc->add_operation( "msiNcGetNdimsInInqOut", "msiNcGetNdimsInInqOut" );

        // =-=-=-=-=-=-=-
        // 6. return the newly created microservice plugin
        return msvc;
    }

}; // extern "C"



