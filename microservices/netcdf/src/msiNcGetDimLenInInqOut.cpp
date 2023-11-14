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
    * \fn msiNcGetDimLenInInqOut (msParam_t *ncInqOutParam, msParam_t *inxParam,
    msParam_t *varNameParam, msParam_t *outParam, ruleExecInfo_t *rei)
    * \brief Get the length of a dimension of a variable in a NcInqOut_MS_T
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
    * \param[out] outParam - An INT_MS_T containing  the length of the dimension.
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
    msiNcGetDimLenInInqOut (msParam_t *ncInqOutParam, msParam_t *inxParam,
    msParam_t *varNameParam, msParam_t *outParam, ruleExecInfo_t *rei)
    {
        ncInqOut_t *ncInqOut;
        int inx, i;
        int arrayLen = -1;

        RE_TEST_MACRO ("    Calling msiNcGetDimLenInInqOut")

        if (ncInqOutParam == NULL || inxParam == NULL || outParam == NULL)
            return USER__NULL_INPUT_ERR;

        if (strcmp (ncInqOutParam->type, NcInqOut_MS_T) != 0) {
            rodsLog (LOG_ERROR,
              "msiNcGetDimLenInInqOut: ncInqOutParam must be NcInqOut_MS_T. %s",
              ncInqOutParam->type);
            return (USER_PARAM_TYPE_ERR);
        } else {
            ncInqOut = (ncInqOut_t *) ncInqOutParam->inOutStruct;
        }
        inx = parseMspForPosInt (inxParam);
        if (inx < UNLIMITED_DIM_INX || inx >= ncInqOut->nvars) {
            rodsLog (LOG_ERROR,
              "msiNcGetDimLenInInqOut: input inx %d is out of range. nvars  = %d",
              inx, ncInqOut->nvars);
            return NETCDF_VAR_COUNT_OUT_OF_RANGE;
        }

        if (inx == UNLIMITED_DIM_INX) {
            /* get the name of unlimdim */
            if (ncInqOut->unlimdimid < 0) return NETCDF_NO_UNLIMITED_DIM;
            for (i = 0; i < ncInqOut->ndims; i++) {
                if (ncInqOut->unlimdimid == ncInqOut->dim[i].id) {
                    arrayLen = ncInqOut->dim[i].arrayLen;
                    break;
                }
            }
            if (arrayLen == -1) {
                rodsLog (LOG_ERROR,
                  "msiNcGetDimLenInInqOut: no match for unlimdimid %d",
                  ncInqOut->unlimdimid);
                return NETCDF_NO_UNLIMITED_DIM;
            }
        } else {
            char *varName;
            if (varNameParam == NULL) return USER__NULL_INPUT_ERR;
            if (strcmp (varNameParam->type, STR_MS_T) != 0) {
                rodsLog (LOG_ERROR,
                  "msiNcGetDimLenInInqOut: nameParam must be STR_MS_T. %s",
                  varNameParam->type);
                return (USER_PARAM_TYPE_ERR);
            } else {
                varName = (char*) varNameParam->inOutStruct;
            }
            if (strcmp (varName, "null") == 0) {
                /* use the global for inx */
                arrayLen = ncInqOut->dim[inx].arrayLen;
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
                                arrayLen = ncInqOut->dim[j].arrayLen;
                                break;
                            }
                        }
                    }
                }
                if (arrayLen == -1) {
                    rodsLog (LOG_ERROR,
                      "msiNcGetDimLenInInqOut: unmatched varName %s and ix %d",
                      varName, inx);
                    return NETCDF_UNMATCHED_NAME_ERR;
                }
            }
        }
        fillIntInMsParam (outParam, arrayLen);

        return 0;
    }

    irods::ms_table_entry*  plugin_factory( ) 
    {
	ADD_IRODS_MS_TABLE_ENTRY (4,msiNcGetDimLenInInqOut );
    }

}; // extern "C"



