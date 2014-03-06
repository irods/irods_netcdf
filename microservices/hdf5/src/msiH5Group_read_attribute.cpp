// =-=-=-=-=-=-=-
#include "msParam.hpp"
#include "reGlobalsExtern.hpp"
#include "irods_ms_plugin.hpp"
#include "h5Group.hpp"
#include "hdf5MS.hpp"

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
    * \fn msiH5Group_read_attribute (msParam_t *inpH5GroupParam, msParam_t *outH5GroupParam, ruleExecInfo_t *rei)
    *
    * \brief This microservice is for reading attribute of a dataset from an opened H5Group.
    *
    * \module hdf5
    *
    * \since pre-2.1
    *
    * \author uiuc.edu Mike wan
    * \date   2008
    *
    * \usage See clients/icommands/test/rules3.0/
    *
    * \param[in] inpH5GroupParam - The input H5Group. Must be h5Dataset_MS_T.
    * \param[out] outH5GroupParam - The output H5Group - Must be h5Dataset_MS_T.
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
    msiH5Group_read_attribute (msParam_t *inpH5GroupParam,
        msParam_t *outH5GroupParam, ruleExecInfo_t *rei)
    {
        rsComm_t *rsComm;
        H5Group *ing;
        H5Group *outg;
        int l1descInx;
        dataObjInfo_t *dataObjInfo;
        int remoteFlag;
        rodsServerHost_t *rodsServerHost;

        RE_TEST_MACRO ( ( char * )"    Calling msiH5Group_read_attribute")

        if (rei == NULL || rei->rsComm == NULL) {
            rodsLog (LOG_ERROR,
              "msiH5Group_read_attribute: input rei or rsComm is NULL");
            return (SYS_INTERNAL_NULL_INPUT_ERR);
        }

        rsComm = rei->rsComm;

        if (inpH5GroupParam == NULL || outH5GroupParam == NULL) {
            rei->status = SYS_INTERNAL_NULL_INPUT_ERR;
            rodsLogAndErrorMsg (LOG_ERROR, &rsComm->rError, rei->status,
              ( char * )"msiH5Group_read_attribute: NULL input/output Param");
            return (rei->status);
        }

        if (strcmp (inpH5GroupParam->type, h5Group_MS_T) == 0) {
            ing = (H5Group*)inpH5GroupParam->inOutStruct;
        } else {
            rei->status = USER_PARAM_TYPE_ERR;
            rodsLogAndErrorMsg (LOG_ERROR, &rsComm->rError, rei->status,
              ( char * )"msiH5Group_read_attribute: wrong inpH5GroupParam type %s",
        inpH5GroupParam->type);
            return (rei->status);
        }

        l1descInx = getL1descInxByFid (ing->fid);
        if (l1descInx < 0) {
            rei->status = SYS_BAD_FILE_DESCRIPTOR;
            rodsLogAndErrorMsg (LOG_ERROR, &rsComm->rError, rei->status,
              ( char * )"msiH5Group_read_attribute: bad fid %d", ing->fid);
            return (rei->status);
        }


        dataObjInfo = L1desc[l1descInx].dataObjInfo;

        remoteFlag = resolveHostByDataObjInfo (dataObjInfo,
          &rodsServerHost);

        if (remoteFlag == LOCAL_HOST) {
            outg = (H5Group*)malloc (sizeof (H5Group));
            rei->status = H5Group_read_attribute (ing, outg);
        } else {
            /* do the remote close */
            outg = NULL;
            if ((rei->status = svrToSvrConnect (rsComm, rodsServerHost)) >= 0) {
                rei->status = _clH5Group_read_attribute (rodsServerHost->conn, 
            ing, &outg);
            }
        }

        /* prepare the output */

        fillMsParam (outH5GroupParam, NULL, ( char * )h5Group_MS_T, outg, NULL);

        if (ing) H5Group_dtor(ing);

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
        msvc->add_operation( "msiH5Group_read_attribute", "msiH5Group_read_attribute" );

        // =-=-=-=-=-=-=-
        // 6. return the newly created microservice plugin
        return msvc;
    }

}; // extern "C"



