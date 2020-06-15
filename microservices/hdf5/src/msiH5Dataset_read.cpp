// =-=-=-=-=-=-=-
#include "netcdf_port.hpp"
#include "msParam.h"
//#include "reGlobalsExtern.hpp"
#include "irods_ms_plugin.hpp"
#include "hdf5MS.hpp"
#include "h5Dataset.hpp"

// =-=-=-=-=-=-=-
// STL Includes
#include <iostream>

extern "C" {

    // =-=-=-=-=-=-=-
    // 1. Set the version of the plugin.  1.0 is currently the standard

    // =-=-=-=-=-=-=-
    // 2. Write a standard issue microservice

    /**
    * \fn msiH5Dataset_read (msParam_t *inpH5DatasetParam, msParam_t *outH5DatasetParam,
    * ruleExecInfo_t *rei)
    *
    * \brief This microservice is for reading a dataset from an opened HDF5 file.
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
    * \param[in] inpH5DatasetParam - The input H5Dataset. Must be h5Dataset_MS_T.
    * \param[out] outH5DatasetParam - The output H5Dataset - Must be h5Dataset_MS_T.
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
    * \sa none
    **/
    int
    msiH5Dataset_read (msParam_t *inpH5DatasetParam, msParam_t *outH5DatasetParam,
    ruleExecInfo_t *rei)
    {
        rsComm_t *rsComm;
        H5Dataset *ind;
        H5Dataset *outd;
        int l1descInx;
        dataObjInfo_t *dataObjInfo;
        int remoteFlag;
        rodsServerHost_t *rodsServerHost;

        RE_TEST_MACRO ( ( char * )"    Calling msiH5Dataset_read")

        if (rei == NULL || rei->rsComm == NULL) {
            rodsLog (LOG_ERROR,
              "msiH5Dataset_read: input rei or rsComm is NULL");
            return (SYS_INTERNAL_NULL_INPUT_ERR);
        }

        rsComm = rei->rsComm;

        if (inpH5DatasetParam == NULL || outH5DatasetParam == NULL) {
            rei->status = SYS_INTERNAL_NULL_INPUT_ERR;
            rodsLogAndErrorMsg (LOG_ERROR, &rsComm->rError, rei->status,
              ( char * )"msiH5Dataset_read: NULL input/output Param");
            return (rei->status);
        }

        if (strcmp (inpH5DatasetParam->type, h5Dataset_MS_T) == 0) {
            ind = (H5Dataset*)inpH5DatasetParam->inOutStruct;
        } else {
            rei->status = USER_PARAM_TYPE_ERR;
            rodsLogAndErrorMsg (LOG_ERROR, &rsComm->rError, rei->status,
              ( char * )"msiH5Dataset_read: wrong inpH5DatasetParam type %s",
              inpH5DatasetParam->type);
            return (rei->status);
        }

        l1descInx = getL1descInxByFid (ind->fid);
        if (l1descInx < 0) {
            rei->status = SYS_BAD_FILE_DESCRIPTOR;
            rodsLogAndErrorMsg (LOG_ERROR, &rsComm->rError, rei->status,
              ( char * )"msiH5Dataset_read: bad fid %d", ind->fid);
            return (rei->status);
        }

        dataObjInfo = L1desc[l1descInx].dataObjInfo;

        remoteFlag = resolveHostByDataObjInfo (dataObjInfo,
          &rodsServerHost);

        if (remoteFlag == LOCAL_HOST) {
            outd = (H5Dataset*)malloc (sizeof (H5Dataset));
            rei->status = H5Dataset_read (ind, outd);
        } else {
            /* do the remote close */
            outd = NULL;
            if ((rei->status = svrToSvrConnect (rsComm, rodsServerHost)) >= 0) {
                rei->status = _clH5Dataset_read (rodsServerHost->conn, ind, &outd);
            }
        }

        /* prepare the output */

        fillMsParam (outH5DatasetParam, NULL, ( char * )h5Dataset_MS_T, outd, NULL);

        if (ind) H5Dataset_dtor(ind);

        return rei->status;

    }

    // =-=-=-=-=-=-=-
    // 3.  Create the plugin factory function which will return a microservice
    //     table entry
    irods::ms_table_entry*  plugin_factory( ) {

        ADD_IRODS_MS_TABLE_ENTRY( 2, msiH5Dataset_read );
    }

}; // extern "C"



