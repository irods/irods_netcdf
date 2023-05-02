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
    * \fn msiNcGetAttValStrInInqOut (msParam_t *ncInqOutParam, msParam_t *whichAttParam,
    msParam_t *varNameParam, msParam_t *outParam, ruleExecInfo_t *rei)
    * \brief Get the value of an atrribute of a variable in a NcInqOut_MS_T
    * \module core
    *
    * \since 3.2
    *
    * \author  Mike Wan
    * \date    2012
    *
    * \usage See clients/icommands/test/rules3.0/netcdfTest1.r, netcdfTest2.r and netcdfTest3.r.
    * \param[in] ncInqOutParam - A NcInqOut_MS_T which is the output of msiNcInq.
    * \param[in] whichAttParam - If it is a STR_MS_T, it contains the name of the attribute. If it is an INT_MS_T, it contains the index of an element in the attribute array.
    * \param[in] varNameParam - A STR_MS_T conataining the name of a variable
    * \param[out] outParam - An INT_MS_T containing the value of the atrribute
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
    msiNcGetAttValStrInInqOut (msParam_t *ncInqOutParam, msParam_t *whichAttParam,
    msParam_t *varNameParam, msParam_t *outParam, ruleExecInfo_t *rei)
    {
        int status;
        ncGetVarOut_t *value = NULL;
        char tempStr[NAME_LEN];
        void *bufPtr;

        RE_TEST_MACRO ("    Calling msiNcGetAttValStrInInqOut")
        status = _msiNcGetAttValInInqOut (ncInqOutParam, whichAttParam,
          varNameParam, &value);

        if (status < 0) return status;

        bufPtr = value->dataArray->buf;
        if (value->dataArray->type == NC_CHAR && value->dataArray->len > 0) {
            /* must be a string */
            status = ncValueToStr (NC_STRING, &bufPtr, tempStr);
        } else {
            status = ncValueToStr (value->dataArray->type, &bufPtr, tempStr);
        }
        if (status < 0) return status;

        fillStrInMsParam (outParam, tempStr);

        return status;
    }

    int
    _msiNcGetAttValInInqOut (msParam_t *ncInqOutParam, msParam_t *whichAttParam,
    msParam_t *varNameParam, ncGetVarOut_t **ncGetVarOut)
    {
        ncInqOut_t *ncInqOut;
        int i;
        int inx;
        char *varName, *attName;
        ncGetVarOut_t *value = NULL;

        if (ncInqOutParam == NULL || whichAttParam == NULL || ncGetVarOut == NULL)
            return USER__NULL_INPUT_ERR;

        *ncGetVarOut = NULL;

        if (strcmp (ncInqOutParam->type, NcInqOut_MS_T) != 0) {
            rodsLog (LOG_ERROR,
              "_msiNcGetAttValInInqOut: ncInqOutParam must be NcInqOut_MS_T. %s",
              ncInqOutParam->type);
            return (USER_PARAM_TYPE_ERR);
        } else {
            ncInqOut = (ncInqOut_t *) ncInqOutParam->inOutStruct;
        }
        /* whichAttParam can be inx or attName */
        if (strcmp (whichAttParam->type, STR_MS_T) == 0) {
            attName = (char *)whichAttParam->inOutStruct;
            inx = -1;
        } else if (strcmp (whichAttParam->type, INT_MS_T) == 0) {
            inx = *((int *) whichAttParam->inOutStruct);
            attName = NULL;
        } else {
            rodsLog (LOG_ERROR,
              "_msiNcGetAttValInInqOut:whichAttParam must be INT_MS_T/STR_MS_T. %s",
              whichAttParam->type);
            return (USER_PARAM_TYPE_ERR);
        }
        if (varNameParam == NULL) return USER__NULL_INPUT_ERR;

        if (strcmp (varNameParam->type, STR_MS_T) != 0) {
            rodsLog (LOG_ERROR,
              "_msiNcGetAttValInInqOut: varNameParam must be STR_MS_T. %s",
              varNameParam->type);
            return (USER_PARAM_TYPE_ERR);
        } else {
            varName = (char*) varNameParam->inOutStruct;
        }

        if (strcmp (varName, "null") == 0) {
            /* use the global att */
            if (attName == NULL) {
                if (inx < 0 || inx >= ncInqOut->ngatts) {
                    rodsLog (LOG_ERROR,
                      "_msiNcGetAttValInInqOut:inp inx %d out of range. ngatts=%d",
                      inx, ncInqOut->ngatts);
                    return NETCDF_VAR_COUNT_OUT_OF_RANGE;
                }
                value = &ncInqOut->gatt[inx].value;
            } else {
                /* input is a att name */
                for (i = 0; i < ncInqOut->ngatts; i++) {
                    if (strcmp (attName, ncInqOut->gatt[i].name) == 0) {
                        value = &ncInqOut->gatt[i].value;
                        break;
                    }
                }
                if (value == NULL) {
                    rodsLog (LOG_ERROR,
                      "_msiNcGetAttValInInqOut: unmatched attName %s", attName);
                    return NETCDF_UNMATCHED_NAME_ERR;
                }
            }
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
                  "_msiNcGetAttValInInqOut: unmatched varName %s", varName);
                return NETCDF_UNMATCHED_NAME_ERR;
            }
            if (attName == NULL) {
                if (inx < 0 || inx >= ncInqOut->var[i].natts) {
                    rodsLog (LOG_ERROR,
                      "_msiNcGetAttNameInInqOut:inp inx %d out of range. natts=%d",
                      inx, ncInqOut->var[i].natts);
                    return NETCDF_VAR_COUNT_OUT_OF_RANGE;
                }
                value = &ncInqOut->var[i].att[inx].value;
            } else {
                /* input is a att name */
                int j;
                for (j = 0; j < ncInqOut->ngatts; j++) {
                    if (strcmp (attName, ncInqOut->var[i].att[j].name) == 0) {
                        value = &ncInqOut->var[i].att[j].value;
                        break;
                    }
                }
                if (value == NULL) {
                    rodsLog (LOG_ERROR,
                      "_msiNcGetAttValInInqOut: unmatched attName %s", attName);
                    return NETCDF_UNMATCHED_NAME_ERR;
                }
            }
        }
        *ncGetVarOut = value;

        return 0;
    }


    irods::ms_table_entry*  plugin_factory( ) 
    {
      ADD_IRODS_MS_TABLE_ENTRY (4, msiNcGetAttValStrInInqOut );
    }

}; // extern "C"



