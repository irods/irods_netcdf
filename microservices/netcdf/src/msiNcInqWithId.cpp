// =-=-=-=-=-=-=-
#include "netcdf_port.hpp"
#include <irods/msParam.h>
//#include "reGlobalsExtern.hpp"
#include <irods/irods_ms_plugin.hpp>
#include "netcdfMS.hpp"
#include "netcdfParseUtilities.hpp"
#include "ncInqWithId.hpp"
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
    * \fn msiNcInqWithId (msParam_t *inpParam1, msParam_t *inpParam2, msParam_t *inpParam3, msParam_t *outParam, ruleExecInfo_t *rei)
    * \brief General netcdf inquiry with id (equivalent nc_inq_dim, nc_inq_dim, nc_inq_var ....) This API is superceded by the more comprehensive rcNcInq API.
    * \module core
    *
    * \since 3.2
    *
    * \author  Mike Wan
    * \date    2012
    *
    * \usage See clients/icommands/test/rules3.0/netcdfTest1.r, netcdfTest2.r and netcdfTest3.r.
    * \param[in] inpParam1 - A STR_MS_T, INT_MS_T or NcInqIdInp_MS_T. If it is a STR_MS_T or INT_MS_T, it contains the id of the inquiry obtained from msiNcInqId.
    * \param[in] inpParam2 - If inpParam1 is a STR_MS_T or INT_MS_T, it is a INT_MS_T containing the paramType - what to inquire - valid values are defined in ncInqId.h - 0 (NC_VAR_T) or 1 (NC_DIM_T).
    NC_VAR_T or NC_DIM_T.
    * \param[in] inpParam3 - If inpParam1 is a STR_MS_T or INT_MS_T, it is a INT_MS_T containing ncid of the opened object for the inquiry.
    * \param[out] outParam - A NcInqWithIdOut_MS_T containing a ncInqWithIdOut_t.
    Elements of ncInqWithIdOut_t:
    *    \li rodsLong_t \b mylong - Content depends on paramType.For NC_DIM_T, this is arrayLen. not used for NC_VAR_T.
    *    \li int \b dataType - data type for NC_VAR_T.
    *    \li int \b natts - number of attrs for NC_VAR_T.
    *    \li char \b name[MAX_NAME_LEN] - name of the parameter.
    *    \li int \b ndim - number of dimensions (rank) for NC_VAR_T.
    *    \li int \b *intArray - int array of dimIds and ndim length for NC_VAR_T.
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
    msiNcInqWithId (msParam_t *inpParam1, msParam_t *inpParam2,
    msParam_t *inpParam3, msParam_t *outParam, ruleExecInfo_t *rei)
    {
        rsComm_t *rsComm;
        ncInqIdInp_t ncInqWithIdInp;
        ncInqWithIdOut_t *ncInqWithIdOut = NULL;

        RE_TEST_MACRO ("    Calling msiNcInqWithId")

        if (rei == NULL || rei->rsComm == NULL) {
          rodsLog (LOG_ERROR,
            "msiNcInqWithId: input rei or rsComm is NULL");
          return (SYS_INTERNAL_NULL_INPUT_ERR);
        }
        rsComm = rei->rsComm;

        if (inpParam1 == NULL) {
            rodsLog (LOG_ERROR,
              "msiNcInqWithId: input inpParam1 is NULL");
            return (SYS_INTERNAL_NULL_INPUT_ERR);
        }

        /* parse for myid or ncInqWithIdInp_t */
        rei->status = parseMspForNcInqIdInpId (inpParam1, &ncInqWithIdInp);

        if (rei->status < 0) return rei->status;

        if (inpParam2 != NULL) {
            /* parse for paramType */
            ncInqWithIdInp.paramType = parseMspForPosInt (inpParam2);
            if (ncInqWithIdInp.paramType != NC_VAR_T &&
              ncInqWithIdInp.paramType != NC_DIM_T) {
                rodsLog (LOG_ERROR,
                  "msiNcInqWithId: Unknow paramType %d for %s ",
                  ncInqWithIdInp.paramType, ncInqWithIdInp.name);
                return (NETCDF_INVALID_PARAM_TYPE);
            }
        }

        if (inpParam3 != NULL) {
            /* parse for ncid */
            ncInqWithIdInp.ncid = parseMspForPosInt (inpParam3);
        }

        rei->status = irods::server_api_call ( NC_INQ_WITH_ID_AN, rsComm, &ncInqWithIdInp, &ncInqWithIdOut);
        clearKeyVal (&ncInqWithIdInp.condInput);
        if (rei->status >= 0) {
            fillMsParam (outParam, NULL, NcInqWithIdOut_MS_T, ncInqWithIdOut, NULL);
        } else {
          rodsLogAndErrorMsg (LOG_ERROR, &rsComm->rError, rei->status,
            "msiNcInqWithId: api call to ncInqWithId failed for %s, status = %d",
            ncInqWithIdInp.name, rei->status);
        }

        return (rei->status);
    }



    irods::ms_table_entry*  plugin_factory( ) {
      ADD_IRODS_MS_TABLE_ENTRY(4,msiNcInqWithId );
    }

}; // extern "C"



