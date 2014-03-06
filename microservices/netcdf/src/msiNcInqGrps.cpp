// =-=-=-=-=-=-=-
#include "msParam.hpp"
#include "reGlobalsExtern.hpp"
#include "irods_ms_plugin.hpp"
#include "netcdfMS.hpp"
#include "ncInqGrps.hpp"

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

        rei->status = rsNcInqGrps (rsComm, &ncInqGrpsInp, &ncInqGrpsOut);

        clearKeyVal (&ncInqGrpsInp.condInput);
        if (rei->status >= 0) {
            fillMsParam (outParam, NULL, NcInqGrpsOut_MS_T, ncInqGrpsOut, NULL);
        } else {
          rodsLogAndErrorMsg (LOG_ERROR, &rsComm->rError, rei->status,
            "msiNcInqGrps: rsNcInqGrps failed for ncid %d, status = %d",
            ncInqGrpsInp.ncid, rei->status);
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
        irods::ms_table_entry* msvc = new irods::ms_table_entry( 2 );

        // =-=-=-=-=-=-=-
        // 5. add the microservice function as an operation to the plugin
        //    the first param is the name / key of the operation, the second
        //    is the name of the function which will be the microservice
        msvc->add_operation( "msiNcInqGrps", "msiNcInqGrps" );

        // =-=-=-=-=-=-=-
        // 6. return the newly created microservice plugin
        return msvc;
    }

}; // extern "C"



