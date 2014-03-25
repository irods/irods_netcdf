// =-=-=-=-=-=-=-
#include "msParam.hpp"
#include "reGlobalsExtern.hpp"
#include "irods_ms_plugin.hpp"
#include "netcdfMS.hpp"
#include "irods_server_api_call.hpp"
#include "ncApiIndex.hpp"

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
    * \fn msiNcOpen (msParam_t *inpParam1, msParam_t *inpParam2, msParam_t *outParam, ruleExecInfo_t *rei)
    * \brief Open an iRODS data object for netcdf operation (equivalent to nc_open)
    * \module core
    *
    * \since 3.2
    *
    * \author  Mike Wan
    * \date    2012
    *
    * \usage See clients/icommands/test/rules3.0/netcdfTest1.r, netcdfTest2.r and netcdfTest3.r.
    * \param[in] inpParam1 - A STR_MS_T for the dataObj path to open.
    * \param[in] inpParam2 - A STR_MS_T or INT_MS_T for the mode of the open. valid values are given in netcdf.h - NC_NOWRITE, NC_WRITE.
    * \param[out] outParam - a msParam of type INT_MS_T containing the ncid of the opened object.
    * \param[in,out] rei - The RuleExecInfo structure that is automatically
    *    handled by the rule engine. The user does not include rei as a
    *    parameter in the rule invocation.
    *
    * \DolVarDependence none
    * \DolVarModified none
    * \iCatAttrDependence none
    * \iCatAttrModified none
    * \sideeffect none
    *
    * \return integer
    * \retval positive on success
    * \pre none
    * \post none
    * \sa none
    **/
    int
    msiNcOpen (msParam_t *inpParam1, msParam_t *inpParam2, msParam_t *outParam,
    ruleExecInfo_t *rei)
    {
        rsComm_t *rsComm;
        ncOpenInp_t ncOpenInp;
        int *ncid;

        RE_TEST_MACRO ("    Calling msiNcOpen")

        if (rei == NULL || rei->rsComm == NULL) {
          rodsLog (LOG_ERROR,
            "msiNcOpen: input rei or rsComm is NULL");
          return (SYS_INTERNAL_NULL_INPUT_ERR);
        }

        rsComm = rei->rsComm;
        if (inpParam1 == NULL) {
            rodsLog (LOG_ERROR,
              "msiNcOpen: input inpParam1 is NULL");
            return (SYS_INTERNAL_NULL_INPUT_ERR);
        }

        if (strcmp (inpParam1->type, STR_MS_T) == 0) {
            /* str input */
            bzero (&ncOpenInp, sizeof (ncOpenInp));
            rstrcpy (ncOpenInp.objPath, (char*)inpParam1->inOutStruct,
              MAX_NAME_LEN);
        } else  if (strcmp (inpParam1->type, NcOpenInp_MS_T) == 0) {
            ncOpenInp = *((ncOpenInp_t *) inpParam1->inOutStruct);
            replKeyVal (&((ncOpenInp_t *) inpParam1->inOutStruct)->condInput,
              &ncOpenInp.condInput);
        } else {
            rodsLog (LOG_ERROR,
              "msiNcOpen: Unsupported input Param1 type %s",
              inpParam1->type);
            return (USER_PARAM_TYPE_ERR);
        }
        if (inpParam2 != NULL) {
            /* parse for mode */
            ncOpenInp.mode = parseMspForPosInt (inpParam2);
            if (ncOpenInp.mode < 0) return (ncOpenInp.mode);
        }

        rei->status = irods::server_api_call ( NC_OPEN_AN, rsComm, &ncOpenInp, &ncid);
        clearKeyVal (&ncOpenInp.condInput);
        if (rei->status >= 0) {
            fillIntInMsParam (outParam, *ncid);
            free (ncid);
        } else {
          rodsLogAndErrorMsg (LOG_ERROR, &rsComm->rError, rei->status,
            "msiNcOpen: api call to ncOpen failed for %s, status = %d",
            ncOpenInp.objPath, rei->status);
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
        msvc->add_operation( "msiNcOpen", "msiNcOpen" );

        // =-=-=-=-=-=-=-
        // 6. return the newly created microservice plugin
        return msvc;
    }

}; // extern "C"



