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
    * \fn msiNcGetVarIdInInqOut (msParam_t *ncInqOutParam, msParam_t *whichVarParam,
    msParam_t *outParam, ruleExecInfo_t *rei)
    * \brief Get the NETCDF variable ID of a variable in a NcInqOut_MS_T
    * \module core
    *
    * \since 3.2
    *
    * \author  Mike Wan
    * \date    2012
    *
    * \usage See clients/icommands/test/rules3.0/netcdfTest1.r, netcdfTest2.r and netcdfTest3.r.
    * \param[in] ncInqOutParam - A NcInqOut_MS_T which is the output of msiNcInq.
    * \param[in] whichVarParam - If it is a STR_MS_T, it contains the name of the variable. If it is an INT_MS_T, it contains the index of an element in the variable array.
    * \param[out] outParam - An INT_MS_T containing the variable ID.
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
    msiNcGetVarIdInInqOut (msParam_t *ncInqOutParam, msParam_t *whichVarParam,
    msParam_t *outParam, ruleExecInfo_t *rei)
    {
        ncInqOut_t *ncInqOut;
        int inx;
        char *varName;
        int id = -1;

        RE_TEST_MACRO ("    msiNcGetVarIdInInqOut msiNcGetVarNameInInqOut")

        if (ncInqOutParam == NULL || whichVarParam == NULL || outParam == NULL)
            return USER__NULL_INPUT_ERR;

        if (strcmp (ncInqOutParam->type, NcInqOut_MS_T) != 0) {
            rodsLog (LOG_ERROR,
              "msiNcGetVarIdInInqOut: ncInqOutParam must be NcInqOut_MS_T. %s",
              ncInqOutParam->type);
            return (USER_PARAM_TYPE_ERR);
        } else {
            ncInqOut = (ncInqOut_t *) ncInqOutParam->inOutStruct;
        }
      /* whichVarParam can be inx or attName */
        if (strcmp (whichVarParam->type, STR_MS_T) == 0) {
            varName = (char *)whichVarParam->inOutStruct;
            inx = -1;
        } else if (strcmp (whichVarParam->type, INT_MS_T) == 0) {
            inx = *((int *) whichVarParam->inOutStruct);
            varName = NULL;
        } else {
            rodsLog (LOG_ERROR,
              "msiNcGetVarIdInInqOut:whichVarParam must be INT_MS_T/STR_MS_T. %s",
              whichVarParam->type);
            return (USER_PARAM_TYPE_ERR);
        }

        if (varName == NULL) {
            if (inx < 0 || inx >= ncInqOut->nvars) {
                rodsLog (LOG_ERROR,
                  "msiNcGetVarIdInInqOut:inp inx %d out of range. nvars=%d",
                  inx, ncInqOut->nvars);
                return NETCDF_VAR_COUNT_OUT_OF_RANGE;
            }
            id = ncInqOut->var[inx].id;
        } else {
            /* input is a var name */
            int i;
            for (i = 0; i < ncInqOut->nvars; i++) {
                if (strcmp (varName, ncInqOut->var[i].name) == 0) {
                    id = ncInqOut->var[i].id;
                    break;
                }
            }
            if (id < 0) {
                rodsLog (LOG_ERROR,
                  "msiNcGetVarIdInInqOut: unmatched varName %s", varName);
                return NETCDF_UNMATCHED_NAME_ERR;
            }
        }
        fillIntInMsParam (outParam, id);

        return 0;
    }

    // =-=-=-=-=-=-=-
    // 3.  Create the plugin factory function which will return a microservice
    //     table entry
    irods::ms_table_entry*  plugin_factory( ) {
        // =-=-=-=-=-=-=-
        // 4.  allocate a microservice plugin which takes the number of function
        //     params as a parameter to the constructor
      ADD_IRODS_MS_TABLE_ENTRY( 3, msiNcGetVarIdInInqOut );


    }

}; // extern "C"



