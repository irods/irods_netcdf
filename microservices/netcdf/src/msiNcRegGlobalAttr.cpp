// =-=-=-=-=-=-=-
#include "msParam.hpp"
#include "reGlobalsExtern.hpp"
#include "irods_ms_plugin.hpp"
#include "netcdfMS.hpp"
#include "ncRegGlobalAttr.hpp"

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
    * \fn msiNcRegGlobalAttr (msParam_t *objPathParam, msParam_t *adminParam,
    msParam_t *outParam, ruleExecInfo_t *rei)
    * \brief Extract the NETCDF global variables in an iRODS data object and register them as AUV.
    * \module core
    *
    * \since 3.2
    *
    * \author  Mike Wan
    * \date    2012
    *
    * \usage See clients/icommands/test/rules3.0/netcdfTest1.r, netcdfTest2.r and netcdfTest3.r.
    * \param[in] objPathParam - A STR_MS_T containing the object path of a NETCDF file
    * \param[in] adminParam - An INT_MS_T or STR_MS_T containing the admin flag (admin operation on behalf of the user)
    * \param[out] outParam - An INT_MS_T containing the status of the operation
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
    msiNcRegGlobalAttr (msParam_t *objPathParam, msParam_t *adminParam,
    msParam_t *outParam, ruleExecInfo_t *rei)
    {
        rsComm_t *rsComm;
        ncRegGlobalAttrInp_t ncRegGlobalAttrInp;

        RE_TEST_MACRO ("    Calling msiNcRegGlobalAttr")

        if (rei == NULL || rei->rsComm == NULL) {
          rodsLog (LOG_ERROR,
            "msiNcRegGlobalAttr: input rei or rsComm is NULL");
          return (SYS_INTERNAL_NULL_INPUT_ERR);
        }

        rsComm = rei->rsComm;
        if (objPathParam == NULL) {
            rodsLog (LOG_ERROR,
              "msiNcRegGlobalAttr: input objPathParam is NULL");
            return (SYS_INTERNAL_NULL_INPUT_ERR);
        }

        if (strcmp (objPathParam->type, STR_MS_T) == 0) {
            /* str input */
            bzero (&ncRegGlobalAttrInp, sizeof (ncRegGlobalAttrInp));
            rstrcpy (ncRegGlobalAttrInp.objPath,
              (char*)objPathParam->inOutStruct, MAX_NAME_LEN);
        } else {
            rodsLog (LOG_ERROR,
              "msiNcRegGlobalAttr: Unsupported input objPathParam type %s",
              objPathParam->type);
            return (USER_PARAM_TYPE_ERR);
        }
        if (adminParam != NULL) {
            int adminFlag = parseMspForPosInt (adminParam);
            if (adminFlag > 0) {
                addKeyVal (&ncRegGlobalAttrInp.condInput, ADMIN_KW, "");
            }
        }
        rei->status = rsNcRegGlobalAttr (rsComm, &ncRegGlobalAttrInp);
        clearKeyVal (&ncRegGlobalAttrInp.condInput);
        fillIntInMsParam (outParam, rei->status);
        if (rei->status < 0) {
          rodsLogAndErrorMsg (LOG_ERROR, &rsComm->rError, rei->status,
            "msiNcRegGlobalAttr: rscNcRegGlobalAttr failed for %s, status = %d",
            ncRegGlobalAttrInp.objPath, rei->status);
        }

        return (rei->status);
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
        msvc->add_operation( "msiNcRegGlobalAttr", "msiNcRegGlobalAttr" );

        // =-=-=-=-=-=-=-
        // 6. return the newly created microservice plugin
        return msvc;
    }

}; // extern "C"



