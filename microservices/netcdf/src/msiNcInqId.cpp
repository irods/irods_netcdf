// =-=-=-=-=-=-=-
#include "netcdf_port.hpp"
#include <irods/msParam.h>
//#include "reGlobalsExtern.hpp"
#include <irods/irods_ms_plugin.hpp>
#include "netcdfMS.hpp"
#include "netcdfParseUtilities.hpp"
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
    * \fn msiNcInqId (msParam_t *inpParam1, msParam_t *inpParam2, msParam_t *inpParam3, msParam_t *outParam, ruleExecInfo_t *rei)
    * \brief  This microservice performs general netcdf inquiry for id (equivalent to nc_inq_dimid, nc_inq_varid, .... This msi is superceded by the more comprehensive msiNcInq API.
    * \module core
    *
    * \since 3.2
    *
    * \author  Mike Wan
    * \date    2012
    *
    * \usage See clients/icommands/test/rules3.0/netcdfTest1.r, netcdfTest2.r and netcdfTest3.r.
    * \param[in] inpParam1 - A NcInqIdInp_MS_T or STR_MS_T. For STR_MS_T input, it contains the name of the item to inquire.
    * \param[in] inpParam2 - If inpParam1 is a STR_MS_T, it is a INT_MS_T containing the paramType - what to inquire - valid values are defined in ncInqId.h - 0 (NC_VAR_T) or 1 (NC_DIM_T).
    * \param[in] inpParam3 - If inpParam1 is a STR_MS_T, it is a INT_MS_T containing ncid of the opened object for the inquiry.
    * \param[out] outParam - An INT_MS_T containing the returned id of the inquiry.
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
    **/
    int
    msiNcInqId (msParam_t *inpParam1, msParam_t *inpParam2, msParam_t *inpParam3,
    msParam_t *outParam, ruleExecInfo_t *rei)
    {
        rsComm_t *rsComm;
        ncInqIdInp_t ncInqIdInp;
        int *outId;

        RE_TEST_MACRO ("    Calling msiNcInqId")

        if (rei == NULL || rei->rsComm == NULL) {
          rodsLog (LOG_ERROR,
            "msiNcInqId: input rei or rsComm is NULL");
          return (SYS_INTERNAL_NULL_INPUT_ERR);
        }
        rsComm = rei->rsComm;

        if (inpParam1 == NULL) {
            rodsLog (LOG_ERROR,
              "msiNcInqId: input inpParam1 is NULL");
            return (SYS_INTERNAL_NULL_INPUT_ERR);
        }

        /* parse for name or ncInqWithIdInp_t */
        rei->status = parseMspForNcInqIdInpName (inpParam1, &ncInqIdInp);

        if (rei->status < 0) return rei->status;

        if (inpParam2 != NULL) {
            /* parse for paramType */
            ncInqIdInp.paramType = parseMspForPosInt (inpParam2);
            if (ncInqIdInp.paramType != NC_VAR_T &&
              ncInqIdInp.paramType != NC_DIM_T) {
                rodsLog (LOG_ERROR,
                  "msiNcInqId: Unknow paramType %d for %s ",
                  ncInqIdInp.paramType, ncInqIdInp.name);
                return (NETCDF_INVALID_PARAM_TYPE);
            }
        }

        if (inpParam3 != NULL) {
            /* parse for ncid */
            ncInqIdInp.ncid = parseMspForPosInt (inpParam3);
        }

        rei->status = irods::server_api_call ( NC_INQ_ID_AN, rsComm, &ncInqIdInp, &outId);
        clearKeyVal (&ncInqIdInp.condInput);
        if (rei->status >= 0) {
            fillIntInMsParam (outParam, *outId);
            free (outId);
        } else {
          rodsLogAndErrorMsg (LOG_ERROR, &rsComm->rError, rei->status,
            "msiNcInqId: api call to ncInqId failed for %s, status = %d",
            ncInqIdInp.name, rei->status);
        }

        return (rei->status);
    }

       // =-=-=-=-=-=-=-
    // 3.  Create the plugin factory function which will return a microservice
    //     table entry
    irods::ms_table_entry*  plugin_factory( ) {

        ADD_IRODS_MS_TABLE_ENTRY(4,msiNcInqId  );

    }

}; // extern "C"



