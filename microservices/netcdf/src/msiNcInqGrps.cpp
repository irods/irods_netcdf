// =-=-=-=-=-=-=-
#include <netcdf_port.hpp>
#include <irods/msParam.h>
//#include <irods/reGlobalsExtern.hpp>
#include <irods/irods_ms_plugin.hpp>
#include "netcdfMS.hpp"
#include "ncInqGrps.hpp"
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
    * \fn msiNcInqGrps (msParam_t *ncidParam, msParam_t *outParam, ruleExecInfo_t *rei)
    * \brief  Given the group ncid, returns all full group paths. On the server, the nc_inq_grpname_len and nc_inq_grpname_full are called
    * \module core
    *
    * \since 3.2
    *
    * \author  Mike Wan
    * \date    2012
    *
    * \usage See clients/icommands/test/rules3.0/netcdfTest1.r, netcdfTest2.r and netcdfTest3.r.
    * \param[in] ncidParam - An INT_MS_T containing the group ncid obtained from msiNcOpenGroup
    * \param[out] outParam - A NcInqGrpsOut_MS_T containing the output of the inquiry
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
    msiNcInqGrps (msParam_t *ncidParam, msParam_t *outParam, ruleExecInfo_t *rei)
    {
        rsComm_t *rsComm;
        ncInqGrpsInp_t ncInqGrpsInp;
        ncInqGrpsOut_t *ncInqGrpsOut = NULL;

        RE_TEST_MACRO ("    Calling msiNcInqGrps")

        if (rei == NULL || rei->rsComm == NULL) {
          rodsLog (LOG_ERROR,
            "msiNcInqGrps: input rei or rsComm is NULL");
          return (SYS_INTERNAL_NULL_INPUT_ERR);
        }
        rsComm = rei->rsComm;

        if (ncidParam == NULL) {
            rodsLog (LOG_ERROR,
              "msiNcInqGrps: input ncidParam is NULL");
            return (SYS_INTERNAL_NULL_INPUT_ERR);
        }
        bzero (&ncInqGrpsInp, sizeof (ncInqGrpsInp));

        ncInqGrpsInp.ncid = parseMspForPosInt (ncidParam);

        rei->status = irods::server_api_call( NC_INQ_GRPS_AN, rsComm, &ncInqGrpsInp, &ncInqGrpsOut);

        clearKeyVal (&ncInqGrpsInp.condInput);
        if (rei->status >= 0) {
            fillMsParam (outParam, NULL, NcInqGrpsOut_MS_T, ncInqGrpsOut, NULL);
        } else {
          rodsLogAndErrorMsg (LOG_ERROR, &rsComm->rError, rei->status,
            "msiNcInqGrps: api call to ncInqGrps failed for ncid %d, status = %d",
            ncInqGrpsInp.ncid, rei->status);
        }
        return (rei->status);
    }

    irods::ms_table_entry*  plugin_factory( ) {


	ADD_IRODS_MS_TABLE_ENTRY(2,msiNcInqGrps );

    }

}; // extern "C"



