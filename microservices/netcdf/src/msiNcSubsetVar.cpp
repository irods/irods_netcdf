// =-=-=-=-=-=-=-
#include "msParam.hpp"
#include "reGlobalsExtern.hpp"
#include "irods_ms_plugin.hpp"
#include "netcdfMS.hpp"

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
    * \fn msiNcSubsetVar (msParam_t *varNameParam, msParam_t *ncidParam,
    msParam_t *ncInqOutParam, msParam_t *subsetStrParam,
    msParam_t *outParam, ruleExecInfo_t *rei)
    * \brief NETCDF subsetting operation
    * \module core
    *
    * \since 3.2
    *
    * \author  Mike Wan
    * \date    2012
    *
    * \usage See clients/icommands/test/rules3.0/netcdfTest1.r, netcdfTest2.r and netcdfTest3.r.
    * \param[in] varNameParam - A STR_MS_T containing the name of a variable for subsetting.
    * \param[in] ncidParam - An INT_MS_T containing the ncid
    * \param[in] ncInqOutParam - A NcInqOut_MS_T which is the output of msiNcInq.
    * \param[in] subsetStrParam - A STR_MS_T containing the subsetting string. The format of the string is dimVariable[start:stride:end]. e.g., *subsetStr="time[10:1:12] depth[3:1:3] lat[20:1:21] lon[30:1:34]"
    * \param[out] outParam - A NcGetVarOut_MS_T containing the output of the subsetting
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
    msiNcSubsetVar (msParam_t *varNameParam, msParam_t *ncidParam,
    msParam_t *ncInqOutParam, msParam_t *subsetStrParam,
    msParam_t *outParam, ruleExecInfo_t *rei)
    {
        rsComm_t *rsComm;
        ncVarSubset_t ncVarSubset;
        int ncid;
        ncInqOut_t *ncInqOut;
        char *name;
        char *subsetStr;
        ncGetVarOut_t *ncGetVarOut = NULL;

        RE_TEST_MACRO ("    Calling msiNcSubsetVar")

        if (rei == NULL || rei->rsComm == NULL) {
          irods::log ( ERROR ( SYS_INTERNAL_NULL_INPUT_ERR,
            "msiNcSubsetVar: input rei or rsComm is NULL" ) );
          return (SYS_INTERNAL_NULL_INPUT_ERR);
        }
        rsComm = rei->rsComm;
        if (ncidParam == NULL) {
          irods::log ( ERROR ( SYS_INTERNAL_NULL_INPUT_ERR,
              "msiNcSubsetVar: input ncidParam is NULL" ) );
            return (SYS_INTERNAL_NULL_INPUT_ERR);
        } else {
            ncid = parseMspForPosInt (ncidParam);
            if (ncid < 0) return ncid;
        }
        if (ncInqOutParam == NULL) {
          irods::log ( ERROR ( SYS_INTERNAL_NULL_INPUT_ERR,
              "msiNcSubsetVar: input ncInqOutParam is NULL" ) );
            return (SYS_INTERNAL_NULL_INPUT_ERR);
        } else {
            ncInqOut = (ncInqOut_t *) ncInqOutParam->inOutStruct;

        }

        if (varNameParam == NULL) {
          irods::log ( ERROR ( SYS_INTERNAL_NULL_INPUT_ERR,
              "msiNcSubsetVar: input varNameParam is NULL" ) );
            return (SYS_INTERNAL_NULL_INPUT_ERR);
        } else {
            name = (char*) varNameParam->inOutStruct;
        }
        if (subsetStrParam == NULL) {
          irods::log ( ERROR ( SYS_INTERNAL_NULL_INPUT_ERR,
              "msiNcSubsetVar: input subsetStrParam is NULL" ) );
            return (SYS_INTERNAL_NULL_INPUT_ERR);
        } else {
            subsetStr = (char*) subsetStrParam->inOutStruct;
        }
        bzero (&ncVarSubset, sizeof (ncVarSubset));
        ncVarSubset.numVar = 1;
        rstrcpy ((char *) ncVarSubset.varName, name, LONG_NAME_LEN);
        rei->status = parseSubsetStr (subsetStr, &ncVarSubset);
        if (rei->status < 0) return rei->status;
        rei->status = ncSubsetVar (rsComm, ncid, ncInqOut, &ncVarSubset,
          &ncGetVarOut);
        if (rei->status >= 0) {
            fillMsParam (outParam, NULL, NcGetVarOut_MS_T, ncGetVarOut, NULL);
        } else {
          rodsLogAndErrorMsg (LOG_ERROR, &rsComm->rError, rei->status,
            "msiNcSubsetVar: ncSubsetVar failed, status = %d",
            rei->status);
        }

        return rei->status;
    }

    int
    ncSubsetVar (rsComm_t *rsComm, int ncid, ncInqOut_t *ncInqOut,
    ncVarSubset_t *ncVarSubset, ncGetVarOut_t **ncGetVarOut)
    {
        int i, j, k, status;
        rodsLong_t start[NC_MAX_DIMS], stride[NC_MAX_DIMS], count[NC_MAX_DIMS];
        ncGetVarInp_t ncGetVarInp;
        int varInx = -1;

        for (i = 0; i < ncInqOut->nvars; i++) {
            if (strcmp ((char *)ncVarSubset->varName, ncInqOut->var[i].name) == 0) {
                varInx = i;
                break;
            }
        }
        if (varInx < 0) {
            rodsLog (LOG_ERROR,
              "ncSubsetVar: unmatched input var name %s", ncVarSubset->varName);
            return NETCDF_UNMATCHED_NAME_ERR;
        }
        /* the following code is almost identical to getSingleNcVarData on
        * the client side */
        for (j = 0; j < ncInqOut->var[varInx].nvdims; j++) {
            int dimId = ncInqOut->var[varInx].dimId[j];
            int doSubset = False;
            if (ncVarSubset != NULL && ncVarSubset->numSubset > 0) {
                for (k = 0; k < ncVarSubset->numSubset; k++) {
                    if (strcmp (ncInqOut->dim[dimId].name,
                      ncVarSubset->ncSubset[k].subsetVarName) == 0) {
                        doSubset = True;
                        break;
                    }
                }
            }
          if (doSubset == True) {
                if (ncVarSubset->ncSubset[k].start >=
                  ncInqOut->dim[dimId].arrayLen ||
                  ncVarSubset->ncSubset[k].end >=
                  ncInqOut->dim[dimId].arrayLen ||
                  ncVarSubset->ncSubset[k].start >
                  ncVarSubset->ncSubset[k].end) {
                    rodsLog (LOG_ERROR,
                    "ncSubsetVar:start %d or end %d for %s outOfRange %lld",
                    ncVarSubset->ncSubset[k].start,
                    ncVarSubset->ncSubset[k].end,
                    ncVarSubset->ncSubset[k].subsetVarName,
                    ncInqOut->dim[dimId].arrayLen);
                    return NETCDF_DIM_MISMATCH_ERR;
                }
                start[j] = ncVarSubset->ncSubset[k].start;
                stride[j] = ncVarSubset->ncSubset[k].stride;
                count[j] = ncVarSubset->ncSubset[k].end -
                  ncVarSubset->ncSubset[k].start + 1;
            } else {
                start[j] = 0;
                count[j] = ncInqOut->dim[dimId].arrayLen;
                stride[j] = 1;
            }
        }
        bzero (&ncGetVarInp, sizeof (ncGetVarInp));
        ncGetVarInp.dataType = ncInqOut->var[varInx].dataType;
        ncGetVarInp.ncid = ncid;
        ncGetVarInp.varid =  ncInqOut->var[varInx].id;
        ncGetVarInp.ndim =  ncInqOut->var[varInx].nvdims;
        ncGetVarInp.start = start;
        ncGetVarInp.count = count;
        ncGetVarInp.stride = stride;

        status = rsNcGetVarsByType (rsComm, &ncGetVarInp, ncGetVarOut);

        if (status < 0) {
          std::string msg = "ncSubsetVar: rcNcGetVarsByType error for ";
          msg += std::string( ncInqOut->var[varInx].name );
          irods::log ( ERROR( status, msg.c_str() ) );
        }
        return status;
    }
    // =-=-=-=-=-=-=-
    // 3.  Create the plugin factory function which will return a microservice
    //     table entry
    irods::ms_table_entry*  plugin_factory( ) {
        // =-=-=-=-=-=-=-
        // 4.  allocate a microservice plugin which takes the number of function
        //     params as a parameter to the constructor
        irods::ms_table_entry* msvc = new irods::ms_table_entry( 5 );

        // =-=-=-=-=-=-=-
        // 5. add the microservice function as an operation to the plugin
        //    the first param is the name / key of the operation, the second
        //    is the name of the function which will be the microservice
        msvc->add_operation( "msiNcSubsetVar", "msiNcSubsetVar" );

        // =-=-=-=-=-=-=-
        // 6. return the newly created microservice plugin
        return msvc;
    }

}; // extern "C"



