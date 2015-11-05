// =-=-=-=-=-=-=-
#include "msParam.hpp"
#include "reGlobalsExtern.hpp"
#include "irods_ms_plugin.hpp"
#include "h5File.hpp"
#include "hdf5MS.hpp"
#include "dataObjClose.hpp"
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
    * \fn msiH5File_close (msParam_t *inpH5FileParam, msParam_t *outH5FileParam, ruleExecInfo_t *rei)
    *
    * \brief This microservice closes an HDF5 file.
    *
    * \module hdf5
    *
    * \since pre-2.1
    *
    * \author uiuc.edu Mike Wan
    * \date  2008
    *
    * \usage See clients/icommands/test/rules3.0/
    *
    * \param[in] inpH5FileParam - The input H5File to close. Must be h5File_MS_T.
    * \param[out] outH5FileParam - the output H5File - Must be h5File_MS_T.
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
    * \retval 0 upon success
    * \pre none
    * \post none
    * \sa msiH5File_open
    **/
    int
    msiH5File_close (msParam_t *inpH5FileParam, msParam_t *outH5FileParam,
    ruleExecInfo_t *rei)
    {
        rsComm_t *rsComm;
        H5File *inf = 0;
        H5File *outf;
        openedDataObjInp_t dataObjCloseInp;
        dataObjInfo_t *dataObjInfo;
        int remoteFlag;
        rodsServerHost_t *rodsServerHost;
        int l1descInx;

        RE_TEST_MACRO ( ( char * )"    Calling msiH5File_close")

        if (rei == NULL || rei->rsComm == NULL) {
            rodsLog (LOG_ERROR,
              "msiH5File_close: input rei or rsComm is NULL");
            return (SYS_INTERNAL_NULL_INPUT_ERR);
        }

        rsComm = rei->rsComm;

        if (inpH5FileParam == NULL || outH5FileParam == NULL) {
            rei->status = SYS_INTERNAL_NULL_INPUT_ERR;
            rodsLogAndErrorMsg (LOG_ERROR, &rsComm->rError, rei->status,
              ( char * )"msiH5File_close: NULL input/output Param");
            return (rei->status);
        }

        if (strcmp (inpH5FileParam->type, h5File_MS_T) == 0) {
            inf = (H5File*)inpH5FileParam->inOutStruct;
        } else {
            rei->status = USER_PARAM_TYPE_ERR;
            rodsLogAndErrorMsg (LOG_ERROR, &rsComm->rError, rei->status,
              ( char * )"msiH5File_close: wrong inpH5FileParam type %s",
              inpH5FileParam->type);
            return (rei->status);
        }

        l1descInx = getL1descInxByFid (inf->fid);
        if (l1descInx < 0) {
            rei->status = SYS_BAD_FILE_DESCRIPTOR;
            rodsLogAndErrorMsg (LOG_ERROR, &rsComm->rError, rei->status,
              ( char * )"msiH5File_close: bad fid %d", inf->fid);
            return (rei->status);
        }

        dataObjCloseInp.l1descInx = l1descInx;
        dataObjInfo = L1desc[l1descInx].dataObjInfo;

        remoteFlag = resolveHostByDataObjInfo (dataObjInfo,
          &rodsServerHost);

        if (remoteFlag == LOCAL_HOST) {
            outf = (H5File*)malloc (sizeof (H5File));
            /* switch the fid */
            inf->fid = L1desc[l1descInx].l3descInx;
            rei->status = H5File_close (inf, outf);
        } else {
            /* do the remote close */
            if ((rei->status = svrToSvrConnect (rsComm, rodsServerHost)) >= 0) {
                rei->status = _clH5File_close (rodsServerHost->conn, inf, &outf);
            }
        }

        if (rei->status < 0) {
            rodsLogAndErrorMsg (LOG_ERROR, &rsComm->rError, rei->status,
              ( char * )"msiH5File_close: H5File_close error for %d", l1descInx);
        }

        L1desc[l1descInx].l3descInx = 0;
        irods::server_api_call ( DATA_OBJ_CLOSE_AN,rsComm, &dataObjCloseInp);

        /* prepare the output */

        fillMsParam (outH5FileParam, NULL, ( char * )h5File_MS_T, outf, NULL);

        if (inf) H5File_dtor(inf);

        return rei->status;
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
        msvc->add_operation( "msiH5File_close", "msiH5File_close" );

        // =-=-=-=-=-=-=-
        // 6. return the newly created microservice plugin
        return msvc;
    }

}; // extern "C"



