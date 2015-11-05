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

    // =-=-=-=-=-=-=-
    // 2. Write a standard issue microservice

    /**
    * \fn msiNcGetDimNameInInqOut (msParam_t *ncInqOutParam, msParam_t *inxParam,
    msParam_t *varNameParam, msParam_t *outParam, ruleExecInfo_t *rei)
    * \brief Get the name of a dimension in a NcInqOut_MS_T
    * \module core
    *
    * \since 3.2
    *
    * \author  Mike Wan
    * \date    2012
    *
    * \usage See clients/icommands/test/rules3.0/netcdfTest1.r, netcdfTest2.r and netcdfTest3.r.
    * \param[in] ncInqOutParam - A NcInqOut_MS_T which is the output of msiNcInq.
    * \param[in] inxParam - An INT_MS_T containing the index of an element in the variable's dimension array
    * \param[in] varNameParam - A STR_MS_T containing the name of a variable
    * \param[out] outParam - A STR_MS_T containing the name of the dimension
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
    msiNcGetDimNameInInqOut (msParam_t *ncInqOutParam, msParam_t *inxParam,
    msParam_t *varNameParam, msParam_t *outParam, ruleExecInfo_t *rei)
    {
        ncInqOut_t *ncInqOut;
        int inx, i;
        char *name = NULL;

        RE_TEST_MACRO ("    Calling msiNcGetDimNameInInqOut")

        if (ncInqOutParam == NULL || inxParam == NULL || outParam == NULL)
            return USER__NULL_INPUT_ERR;

        if (strcmp (ncInqOutParam->type, NcInqOut_MS_T) != 0) {
            rodsLog (LOG_ERROR,
              "msiNcGetDimNameInInqOut: ncInqOutParam must be NcInqOut_MS_T. %s",
              ncInqOutParam->type);
            return (USER_PARAM_TYPE_ERR);
        } else {
            ncInqOut = (ncInqOut_t *) ncInqOutParam->inOutStruct;
        }
        inx = parseMspForPosInt (inxParam);
        if (inx < UNLIMITED_DIM_INX || inx >= ncInqOut->ndims) {
            rodsLog (LOG_ERROR,
              "msiNcGetDimNameInInqOut: input inx %d is out of range. ndims  = %d",
              inx, ncInqOut->ndims);
            return NETCDF_VAR_COUNT_OUT_OF_RANGE;
        }

        if (inx == UNLIMITED_DIM_INX) {
            /* get the name of unlimdim */
            if (ncInqOut->unlimdimid < 0) return NETCDF_NO_UNLIMITED_DIM;
            for (i = 0; i < ncInqOut->ndims; i++) {
                if (ncInqOut->unlimdimid == ncInqOut->dim[i].id) {
                    name = ncInqOut->dim[i].name;
                    break;
                }
            }
            if (name == NULL) {
                rodsLog (LOG_ERROR,
                  "msiNcGetDimNameInInqOut: no match for unlimdimid %d",
                  ncInqOut->unlimdimid);
                return NETCDF_NO_UNLIMITED_DIM;
            }
        } else {
            char *varName;
            if (varNameParam == NULL) return USER__NULL_INPUT_ERR;
            if (strcmp (varNameParam->type, STR_MS_T) != 0) {
                rodsLog (LOG_ERROR,
                  "msiNcGetDimNameInInqOut: nameParam must be STR_MS_T. %s",
                  varNameParam->type);
                return (USER_PARAM_TYPE_ERR);
            } else {
                varName = (char*) varNameParam->inOutStruct;
            }
            if (strcmp (varName, "null") == 0) {
                /* use the global for inx */
                name = ncInqOut->dim[inx].name;
            } else {
                /* match the varName first */
                for (i = 0; i < ncInqOut->nvars; i++) {
                    int dimId, j;
                    if (strcmp (varName, ncInqOut->var[i].name) == 0) {
                        /* a match in var name */
                        dimId = ncInqOut->var[i].dimId[inx];
                        /* try to match dimId */
                        for (j = 0; j <  ncInqOut->ndims; j++) {
                            if (ncInqOut->dim[j].id == dimId) {
                                name = ncInqOut->dim[j].name;
                                break;
                            }
                        }
                    }
                }
                if (name == NULL) {
                    rodsLog (LOG_ERROR,
                      "msiNcGetDimNameInInqOut: unmatched varName %s and ix %d",
                      varName, inx);
                    return NETCDF_UNMATCHED_NAME_ERR;
                }
            }
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
        msvc->add_operation( "msiNcGetDimNameInInqOut", "msiNcGetDimNameInInqOut" );

        // =-=-=-=-=-=-=-
        // 6. return the newly created microservice plugin
        return msvc;
    }

}; // extern "C"



