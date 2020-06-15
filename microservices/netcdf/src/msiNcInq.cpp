// =-=-=-=-=-=-=-
#include "netcdf_port.hpp"
#include "msParam.h"
//#include "reGlobalsExtern.hpp"
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

    // =-=-=-=-=-=-=-
    // 2. Write a standard issue microservice

    /**
    * \fn msiNcInq (msParam_t *ncidParam, msParam_t *outParam, ruleExecInfo_t *rei)
    * \brief General netcdf inquiry (equivalent to nc_inq + nc_inq_format). Get all info including attributes, dimensions and variables assocaiated with a NETCDF file with a single call. This msi is more comprehensive and supercede the msiNcInqId and msiNcInqWithId msi.
    * \module core
    *
    * \since 3.2
    *
    * \author  Mike Wan
    * \date    2012
    *
    * \usage See clients/icommands/test/rules3.0/netcdfTest1.r, netcdfTest2.r and netcdfTest3.r.
    * \param[in] ncidParam - An INT_MS_T containing the ncid of the opened NETCDF file (using msiNcOpen).
    * \param[out] outParam - A NcInqOut_MS_T containing the results of the inquery.
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
    msiNcInq (msParam_t *ncidParam, msParam_t *outParam, ruleExecInfo_t *rei)
    {
        rsComm_t *rsComm;
        ncInqInp_t ncInqInp;
        ncInqOut_t *ncInqOut = NULL;;

        RE_TEST_MACRO ("    Calling msiNcInq")

        if (rei == NULL || rei->rsComm == NULL) {
          rodsLog (LOG_ERROR,
            "msiNcInq: input rei or rsComm is NULL");
          return (SYS_INTERNAL_NULL_INPUT_ERR);
        }
        rsComm = rei->rsComm;

        if (ncidParam == NULL) {
            rodsLog (LOG_ERROR,
              "msiNcInq: input ncidParam is NULL");
            return (SYS_INTERNAL_NULL_INPUT_ERR);
        }
        bzero (&ncInqInp, sizeof (ncInqInp));

        ncInqInp.ncid = parseMspForPosInt (ncidParam);

        rei->status = irods::server_api_call( NC_INQ_AN, rsComm, &ncInqInp, &ncInqOut);

        clearKeyVal (&ncInqInp.condInput);
        if (rei->status >= 0) {
            fillMsParam (outParam, NULL, NcInqOut_MS_T, ncInqOut, NULL);
        } else {
          rodsLogAndErrorMsg (LOG_ERROR, &rsComm->rError, rei->status,
            "msiNcInq: api call to ncInq failed for ncid %d, status = %d",
            ncInqInp.ncid, rei->status);
        }

        return (rei->status);
    }


    irods::ms_table_entry*  plugin_factory( ) {
        ADD_IRODS_MS_TABLE_ENTRY(2,msiNcInq );
    }

}; // extern "C"



