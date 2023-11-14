// =-=-=-=-=-=-=-
#include <netcdf_port.hpp>
#include <irods/msParam.h>
//#include <irods/reGlobalsExtern.hpp>
#include <irods/irods_ms_plugin.hpp>
#include "netcdfMS.hpp"
#include "ncClose.hpp"
#include <irods/irods_server_api_call.hpp>
#include "ncApiIndex.hpp"

// =-=-=-=-=-=-=-
// STL Includes
#include <iostream>

extern "C" {

    // =-=-=-=-=-=-=-
    // 1. Set the version of the plugin.  1.0 is currently the standard

    // =-=-=-=-=-=-=-
    // 2. Write a standard issue microservice

    /**
    * \fn msiNcClose (msParam_t *inpParam1, ruleExecInfo_t *rei)
    * \brief Close an opened iRODS data object (equivalent to nc_close)
    * \module core
    *
    * \since 3.2
    *
    * \author  Mike Wan
    * \date    2012
    *
    * \usage See clients/icommands/test/rules3.0/netcdfTest1.r, netcdfTest2.r and netcdfTest3.r.
    * \param[in] inpParam1 - A INT_MS_T or NcCloseInp_MS_T containing the ncid to close.
    * \param[out] none
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
    msiNcClose (msParam_t *inpParam1, ruleExecInfo_t *rei)
    {
        rsComm_t *rsComm;
        ncCloseInp_t ncCloseInp;

        RE_TEST_MACRO ("    Calling msiNcClose")

        if (rei == NULL || rei->rsComm == NULL) {
          rodsLog (LOG_ERROR,
            "msiNcClose: input rei or rsComm is NULL");
          return (SYS_INTERNAL_NULL_INPUT_ERR);
        }
        rsComm = rei->rsComm;
        if (inpParam1 == NULL) {
            rodsLog (LOG_ERROR,
              "msiNcClose: input inpParam1 is NULL");
            return (SYS_INTERNAL_NULL_INPUT_ERR);
        }

        if (strcmp (inpParam1->type, INT_MS_T) == 0) {
            /* str input */
            bzero (&ncCloseInp, sizeof (ncCloseInp));
            ncCloseInp.ncid = *((int*) inpParam1->inOutStruct);
        } else  if (strcmp (inpParam1->type, NcCloseInp_MS_T) == 0) {
            ncCloseInp = *((ncCloseInp_t *) inpParam1->inOutStruct);
            replKeyVal (&((ncCloseInp_t *) inpParam1->inOutStruct)->condInput,
              &ncCloseInp.condInput);
        } else {
            rodsLog (LOG_ERROR,
              "msiNcClose: Unsupported input Param1 type %s",
              inpParam1->type);
            return (USER_PARAM_TYPE_ERR);
        }

        rei->status = irods::server_api_call ( NC_CLOSE_AN, rsComm, &ncCloseInp );
        clearKeyVal (&ncCloseInp.condInput);
        if (rei->status < 0) {
          rodsLogAndErrorMsg (LOG_ERROR, &rsComm->rError, rei->status,
            "msiNcClose: api call to ncClose failed for %d, status = %d",
            ncCloseInp.ncid, rei->status);
        }

        return (rei->status);
    }

    irods::ms_table_entry*  plugin_factory( ) 
    {
        ADD_IRODS_MS_TABLE_ENTRY( 1, msiNcClose);
    }

}; // extern "C"



