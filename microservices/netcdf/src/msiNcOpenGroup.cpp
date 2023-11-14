// =-=-=-=-=-=-=-
#include <netcdf_port.hpp>
#include <irods/msParam.h>
//#include <irods/reGlobalsExtern.hpp>
#include <irods/irods_ms_plugin.hpp>
#include "netcdfMS.hpp"
#include "ncOpenGroup.hpp"
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
    * \fn msiNcOpenGroup (msParam_t *rootNcidParam, msParam_t *fullGrpNameParam,
    msParam_t *outParam, ruleExecInfo_t *rei)
    * \brief Open a fully qualified group name and get the group id.  nc_inq_grp_full_ncid is called to get the grpncid
    * \module core
    *
    * \since 3.2
    *
    * \author  Mike Wan
    * \date    2012
    *
    * \usage See clients/icommands/test/rules3.0/netcdfTest1.r, netcdfTest2.r and netcdfTest3.r.
    * \param[in] rootNcidParam - An INT_MS_T containing the rootNcid.
    * \param[in] fullGrpNameParam - A STR_MS_T containing the full group name
    * \param[out] outParam - An INT_MS_T containing the group ncid
    *
    **/
    int
    msiNcOpenGroup (msParam_t *rootNcidParam, msParam_t *fullGrpNameParam,
    msParam_t *outParam, ruleExecInfo_t *rei)
    {
        rsComm_t *rsComm;
        ncOpenInp_t ncOpenInp;
        int *grpNcid = NULL;

        RE_TEST_MACRO ("    Calling msiNcOpenGroup")

        if (rei == NULL || rei->rsComm == NULL) {
          rodsLog (LOG_ERROR,
            "msiNcOpenGroup: input rei or rsComm is NULL");
          return (SYS_INTERNAL_NULL_INPUT_ERR);
        }
        rsComm = rei->rsComm;

        bzero (&ncOpenInp, sizeof (ncOpenInp));
        if (rootNcidParam == NULL) {
            rodsLog (LOG_ERROR,
              "msiNcOpenGroup: input rootNcidParam is NULL");
            return (SYS_INTERNAL_NULL_INPUT_ERR);
        }
        ncOpenInp.rootNcid = parseMspForPosInt (rootNcidParam);
        if (strcmp (fullGrpNameParam->type, STR_MS_T) == 0) {
            rstrcpy (ncOpenInp.objPath, (char*)fullGrpNameParam->inOutStruct,
              MAX_NAME_LEN);
        } else {
            rodsLog (LOG_ERROR,
              "msiNcOpenGroup: Unsupported input fullGrpNameParam type %s",
              fullGrpNameParam->type);
            return (USER_PARAM_TYPE_ERR);
        }
        rei->status = irods::server_api_call ( NC_OPEN_GROUP_AN, rsComm, &ncOpenInp, &grpNcid);

        clearKeyVal (&ncOpenInp.condInput);
        if (rei->status >= 0) {
            fillIntInMsParam (outParam, *grpNcid);
            free (grpNcid);
        } else {
          rodsLogAndErrorMsg (LOG_ERROR, &rsComm->rError, rei->status,
            "msiNcOpenGroup: api call to ncOpenGroup failed for rootNcid %d, status = %d",
            ncOpenInp.rootNcid, rei->status);
        }
        return (rei->status);
    }

    irods::ms_table_entry*  plugin_factory( ) {
      ADD_IRODS_MS_TABLE_ENTRY(3,msiNcOpenGroup );
    }

}; // extern "C"



