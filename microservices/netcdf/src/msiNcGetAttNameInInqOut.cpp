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
    * \fn msiNcGetAttNameInInqOut (msParam_t *ncInqOutParam, msParam_t *inxParam,
    msParam_t *varNameParam, msParam_t *outParam, ruleExecInfo_t *rei)
    * \brief Get the name of an attribute of a varible in a NcInqOut_MS_T
    * \module core
    *
    * \since 3.2
    *
    * \author  Mike Wan
    * \date    2012
    *
    * \usage See clients/icommands/test/rules3.0/netcdfTest1.r, netcdfTest2.r and netcdfTest3.r.
    * \param[in] ncInqOutParam - A NcInqOut_MS_T which is the output of msiNcInq.
    * \param[in] inxParam - An INT_MS_T containing the index of an element in the variable's attribute array
    * \param[in] varNameParam - A STR_MS_T containing the name of a variable
    * \param[out] outParam - A STR_MS_T containing the name of the attribute
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
    msiNcGetAttNameInInqOut (msParam_t *ncInqOutParam, msParam_t *inxParam,
    msParam_t *varNameParam, msParam_t *outParam, ruleExecInfo_t *rei)
    {
        ncInqOut_t *ncInqOut;
        int inx, i;
        char *varName;
        char *name = NULL;

        RE_TEST_MACRO ("    Calling msiNcGetAttNameInInqOut")

        if (ncInqOutParam == NULL || inxParam == NULL || outParam == NULL)
            return USER__NULL_INPUT_ERR;

        if (strcmp (ncInqOutParam->type, NcInqOut_MS_T) != 0) {
            rodsLog (LOG_ERROR,
              "msiNcGetAttNameInInqOut: ncInqOutParam must be NcInqOut_MS_T. %s",
              ncInqOutParam->type);
            return (USER_PARAM_TYPE_ERR);
        } else {
            ncInqOut = (ncInqOut_t *) ncInqOutParam->inOutStruct;
        }
        inx = parseMspForPosInt (inxParam);

        if (varNameParam == NULL) return USER__NULL_INPUT_ERR;

        if (strcmp (varNameParam->type, STR_MS_T) != 0) {
            rodsLog (LOG_ERROR,
              "msiNcGetAttNameInInqOut: nameParam must be STR_MS_T. %s",
              varNameParam->type);
            return (USER_PARAM_TYPE_ERR);
        } else {
            varName = (char*) varNameParam->inOutStruct;
        }

        if (strcmp (varName, "null") == 0) {
            /* use the global att */
            if (inx < 0 || inx >= ncInqOut->ngatts) {
                rodsLog (LOG_ERROR,
                  "msiNcGetAttNameInInqOut: input inx %d out of range. ngatts = %d",
                  inx, ncInqOut->ngatts);
                return NETCDF_VAR_COUNT_OUT_OF_RANGE;
            }
            name = ncInqOut->gatt[inx].name;
        } else {
            /* match the varName first */
            for (i = 0; i < ncInqOut->nvars; i++) {
                if (strcmp (varName, ncInqOut->var[i].name) == 0) {
                    /* a match in var name */
                    break;
                }
            }
            if (i >= ncInqOut->nvars) {
                rodsLog (LOG_ERROR,
                  "msiNcGetAttNameInInqOut: unmatched varName %s", varName);
                return NETCDF_UNMATCHED_NAME_ERR;
            }
            if (inx < 0 || inx >= ncInqOut->var[i].natts) {
                rodsLog (LOG_ERROR,
                  "msiNcGetAttNameInInqOut: input inx %d out of range. natts = %d",
                  inx, ncInqOut->var[i].natts);
                return NETCDF_VAR_COUNT_OUT_OF_RANGE;
            }
            name = ncInqOut->var[i].att[inx].name;
        }
        fillStrInMsParam (outParam, name);

        return 0;
    }

    // =-=-=-=-=-=-=-
    // 3.  Create the plugin factory function which will return a microservice
    //     table entry
    irods::ms_table_entry*  plugin_factory( ) {
        // =-=-=-=-=-=-=-
        // 4.  allocate a microservice plugin which takes the number of function
        //     params as a parameter to the constructor
        irods::ms_table_entry* msvc = new irods::ms_table_entry( 4 );

        // =-=-=-=-=-=-=-
        // 5. add the microservice function as an operation to the plugin
        //    the first param is the name / key of the operation, the second
        //    is the name of the function which will be the microservice
        msvc->add_operation( "msiNcGetAttNameInInqOut", "msiNcGetAttNameInInqOut" );

        // =-=-=-=-=-=-=-
        // 6. return the newly created microservice plugin
        return msvc;
    }

}; // extern "C"



