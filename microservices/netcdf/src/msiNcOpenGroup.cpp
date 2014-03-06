// =-=-=-=-=-=-=-
#include "msParam.hpp"
#include "reGlobalsExtern.hpp"
#include "irods_ms_plugin.hpp"
#include "netcdfMS.hpp"
#include "ncOpenGroup.hpp"

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
        rei->status = rsNcOpenGroup (rsComm, &ncOpenInp, &grpNcid);

        clearKeyVal (&ncOpenInp.condInput);
        if (rei->status >= 0) {
            fillIntInMsParam (outParam, *grpNcid);
            free (grpNcid);
        } else {
          rodsLogAndErrorMsg (LOG_ERROR, &rsComm->rError, rei->status,
            "msiNcOpenGroup: rsNcOpenGroup failed for rootNcid %d, status = %d",
            ncOpenInp.rootNcid, rei->status);
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
        msvc->add_operation( "msiNcOpenGroup", "msiNcOpenGroup" );

        // =-=-=-=-=-=-=-
        // 6. return the newly created microservice plugin
        return msvc;
    }

}; // extern "C"



