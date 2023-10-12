// =-=-=-=-=-=-=-
#include "netcdf_port.hpp"
#include "msParam.h"
//#include "reGlobalsExtern.hpp"
#include "irods_ms_plugin.hpp"
#include "h5File.hpp"
#include "hdf5MS.hpp"
#include "irods_server_api_call.hpp"
#include "ncApiIndex.hpp"
#include "rsDataObjOpen.hpp"



// =-=-=-=-=-=-=-
// STL Includes
#include <iostream>

namespace {

// removed from 4.2.9, so copying here from the 4.2.8 release

int
requeDataObjInfoByReplNum( dataObjInfo_t **dataObjInfoHead, int replNum ) {
    dataObjInfo_t *tmpDataObjInfo, *prevDataObjInfo;
    int status = -1;

    if ( dataObjInfoHead == NULL || *dataObjInfoHead == NULL ) {
        return -1;
    }

    tmpDataObjInfo = *dataObjInfoHead;
    if ( tmpDataObjInfo->next == NULL ) {
        /* just one */
        if ( replNum == tmpDataObjInfo->replNum ) {
            return 0;
        }
        else {
            return -1;
        }
    }
    prevDataObjInfo = NULL;
    while ( tmpDataObjInfo != NULL ) {
        if ( replNum == tmpDataObjInfo->replNum ) {
            if ( prevDataObjInfo != NULL ) {
                prevDataObjInfo->next = tmpDataObjInfo->next;
                queueDataObjInfo( dataObjInfoHead, tmpDataObjInfo, 1, 1 );
            }
            status = 0;
            break;
        }
        prevDataObjInfo = tmpDataObjInfo;
        tmpDataObjInfo = tmpDataObjInfo->next;
    }

    return status;
}


}

extern "C" {

    // =-=-=-=-=-=-=-
    // 1. Set the version of the plugin.  1.0 is currently the standard

    // =-=-=-=-=-=-=-
    // 2. Write a standard issue microservice

    /**
    * \fn msiH5File_open (msParam_t *inpH5FileParam, msParam_t *inpFlagParam,
    * msParam_t *outH5FileParam, ruleExecInfo_t *rei)
    *
    * \brief This microservice opens an HDF5 file.
    *
    * \module hdf5
    *
    * \since pre-2.1
    *
    * \author uiuc.edu Mike Wan
    * \date  Feb 2008
    *
    * \usage See clients/icommands/test/rules3.0/
    *
    * \param[in] inpH5FileParam - The input H5File to open. Must be h5File_MS_T.
    * \param[in] inpFlagParam - Input flag - INT_MS_T.
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
    * \sa none
    **/

    int
    msiH5File_open (msParam_t *inpH5FileParam, msParam_t *inpFlagParam,
    msParam_t *outH5FileParam, ruleExecInfo_t *rei)
    {
        rsComm_t *rsComm;
        H5File *inf = 0;
        H5File *outf;
        int inpFlag;
        int l1descInx;
        dataObjInp_t dataObjInp;
        openedDataObjInp_t dataObjCloseInp;
        dataObjInfo_t *dataObjInfo, *tmpDataObjInfo;
        int remoteFlag;
        rodsServerHost_t *rodsServerHost;
        int fid;

        RE_TEST_MACRO ( ( char * )"    Calling msiH5File_open")

        if (rei == NULL || rei->rsComm == NULL) {
            rodsLog (LOG_ERROR,
              "msiH5File_open: input rei or rsComm is NULL");
            return (SYS_INTERNAL_NULL_INPUT_ERR);
        }

        rsComm = rei->rsComm;

        if (inpH5FileParam == NULL || inpFlagParam == NULL ||
          outH5FileParam == NULL) {
            rei->status = SYS_INTERNAL_NULL_INPUT_ERR;
            rodsLogAndErrorMsg (LOG_ERROR, &rsComm->rError, rei->status,
              ( char * )"msiH5File_open: NULL input/output Param");
            return (rei->status);
        }

        if (strcmp (inpH5FileParam->type, h5File_MS_T) == 0) {
            inf = (H5File*)inpH5FileParam->inOutStruct;
        } else {
            rei->status = USER_PARAM_TYPE_ERR;
            rodsLogAndErrorMsg (LOG_ERROR, &rsComm->rError, rei->status,
              ( char * )"msiH5File_open: inpH5FileParam must be h5File_MS_T");
            return (rei->status);
        }

        inpFlag = parseMspForPosInt (inpFlagParam);
        if (inpFlag < 0) {
            inpFlag = 0;
        }

        /* see if we need to do it remotely based on the objPath. Open the
        * obj but don't do it physically */

        memset (&dataObjInp, 0, sizeof (dataObjInp));
        rstrcpy (dataObjInp.objPath, inf->filename, MAX_NAME_LEN);
        dataObjInp.openFlags = O_RDONLY;
        addKeyVal (&dataObjInp.condInput, NO_OPEN_FLAG_KW, "");
        rei->status = l1descInx = 
          rsDataObjOpen (rsComm, &dataObjInp);

        if (rei->status < 0) {
            rodsLogAndErrorMsg (LOG_ERROR, &rsComm->rError, rei->status,
              ( char * )"msiH5File_open: _rsDataObjOpen of %s error",
              dataObjInp.objPath);
            return (rei->status);
        }

        dataObjCloseInp.l1descInx = l1descInx;
        dataObjInfo = L1desc[l1descInx].dataObjInfo;

        if ((inpFlag & LOCAL_H5_OPEN) != 0) {
            /* need to find a local copy */
            tmpDataObjInfo = dataObjInfo;
            while (tmpDataObjInfo != NULL) {
                remoteFlag = resolveHostByDataObjInfo (tmpDataObjInfo,
                  &rodsServerHost);
                if (remoteFlag == LOCAL_HOST) {
                    requeDataObjInfoByReplNum (&dataObjInfo,
                      tmpDataObjInfo->replNum);
                    /* have to update L1desc */
                    L1desc[l1descInx].dataObjInfo = dataObjInfo;
                    break;
                }
            }
            if (remoteFlag != LOCAL_HOST) {
                /* there is no local copy */
                rei->status = SYS_COPY_NOT_EXIST_IN_RESC;
                rodsLogAndErrorMsg (LOG_ERROR, &rsComm->rError, rei->status,
                  ( char * )"msiH5File_open: _local copy of %s does not exist",
                  dataObjInp.objPath);
                irods::server_api_call ( DATA_OBJ_CLOSE_AN,rsComm, &dataObjCloseInp);
                return (rei->status);
            }
        } else {
            remoteFlag = resolveHostByDataObjInfo (dataObjInfo,
              &rodsServerHost);
        }

        if (remoteFlag == LOCAL_HOST) {
            outf = (H5File*)malloc (sizeof (H5File));
            /* replace iRODS file with local file */
            if (inf->filename != NULL) {
                free (inf->filename);
            }
            inf->filename = strdup (dataObjInfo->filePath);
            fid = H5File_open(inf, outf);
        } else {
            /* do the remote open */
            if ((rei->status = svrToSvrConnect (rsComm, rodsServerHost)) < 0) {
                irods::server_api_call ( DATA_OBJ_CLOSE_AN,rsComm, &dataObjCloseInp);
                return rei->status;
            }

            outf = NULL;
            fid = _clH5File_open (rodsServerHost->conn, inf, &outf, LOCAL_H5_OPEN);
        }

        if (fid >= 0) {
            L1desc[l1descInx].l3descInx = outf->fid;
        } else {
            rei->status = fid;
            rodsLogAndErrorMsg (LOG_ERROR, &rsComm->rError, rei->status,
              ( char * )"msiH5File_open: H5File_open %s error",
              dataObjInp.objPath);
            irods::server_api_call ( DATA_OBJ_CLOSE_AN,rsComm, &dataObjCloseInp);
            return rei->status;
        }

        /* prepare the output */

        fillMsParam (outH5FileParam, NULL, ( char * )h5File_MS_T, outf, NULL);

        if (inf) H5File_dtor(inf);

        return (rei->status);
    }

    // =-=-=-=-=-=-=-
    // 3.  Create the plugin factory function which will return a microservice
    //     table entry
    irods::ms_table_entry*  plugin_factory( ) {
       ADD_IRODS_MS_TABLE_ENTRY( 3 , msiH5File_open );

    }

}; // extern "C"



