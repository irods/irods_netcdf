// =-=-=-=-=-=-=-
#include "msParam.hpp"
#include "reGlobalsExtern.hpp"
#include "irods_ms_plugin.hpp"
#include "ncGetVarsByType.hpp"
#include "netcdfMS.hpp"

// =-=-=-=-=-=-=-
// STL Includes
#include <iostream>

extern "C" {

    // =-=-=-=-=-=-=-
    // 1. Set the version of the plugin.  1.0 is currently the standard

    // =-=-=-=-=-=-=-
    // 2. Write a standard issue microservice

    /**
    * \fn msiNcVarStat (msParam_t *ncGetVarOutParam, msParam_t *statOutStr,
    ruleExecInfo_t *rei)
    * \brief Compute the maximum, minimum and average of a variable in a NcGetVarOut_MS_T
    * \module core
    *
    * \since 3.2
    *
    * \author  Mike Wan
    * \date    2012
    *
    * \usage See clients/icommands/test/rules3.0/netcdfTest1.r, netcdfTest2.r and netcdfTest3.r.
    * \param[in] ncGetVarOutParam - A NcGetVarOut_MS_T containing the variables
    * \param[out] statOutStr - A STR_MS_T containing the  maximum, minimum and average values
    *
    * \return integer
    * \retval status of the call. success if greater or equal 0. error if negative.
    * \sideeffect none
    * \pre none
    * \post none
    * \sa none
    * \bug  no known bugs
    **/
    int
    msiNcVarStat (msParam_t *ncGetVarOutParam, msParam_t *statOutStr,
    ruleExecInfo_t *rei)
    {
        ncGetVarOut_t *ncGetVarOut;
        char outStr[MAX_NAME_LEN];
        int i;
        float mymax = 0;
        float mymin = 0;
        float mytotal = 0;
        short *myshortArray;
        unsigned short *myushortArray;
        int *myintArray;
        unsigned int *myuintArray;
        long long *mylongArray;
        unsigned long long *myulongArray;
        float myfloat, *myfloatArray;
        double *mydoubleArray;


        RE_TEST_MACRO ("    Calling msiNcVarStat")

        if (rei == NULL) {
            rodsLog (LOG_ERROR, "msiNcVarStat: input rei is NULL");
          return (SYS_INTERNAL_NULL_INPUT_ERR);
        }

        if (ncGetVarOutParam == NULL) {
            rodsLog (LOG_ERROR,
              "msiNcVarStat: input ncidParam is NULL");
            return (SYS_INTERNAL_NULL_INPUT_ERR);
        } else {
            ncGetVarOut = (ncGetVarOut_t *) ncGetVarOutParam->inOutStruct;
            if (ncGetVarOut == NULL || ncGetVarOut->dataArray == NULL)
                return USER__NULL_INPUT_ERR;
        }

        if (ncGetVarOut->dataArray->type == NC_CHAR ||
          ncGetVarOut->dataArray->type == NC_STRING ||
          ncGetVarOut->dataArray->type == NC_BYTE ||
          ncGetVarOut->dataArray->type == NC_UBYTE) {
            rodsLog (LOG_ERROR,
              "msiNcVarStat: cannot compute max/min/ave of chr or string");
            return (NETCDF_INVALID_DATA_TYPE);
        }
        switch (ncGetVarOut->dataArray->type) {
          case NC_SHORT:
            myshortArray = (short *) ncGetVarOut->dataArray->buf;
            mymax = mymin = (float) myshortArray[0];
            for (i = 0; i < ncGetVarOut->dataArray->len; i++) {
                myfloat = (float) myshortArray[i];
                procMaxMinAve (myfloat, &mymax, &mymin, &mytotal);
            }
            break;
          case NC_USHORT:
            myushortArray = (unsigned short *) ncGetVarOut->dataArray->buf;
            mymax = mymin = (float) myushortArray[0];
            for (i = 0; i < ncGetVarOut->dataArray->len; i++) {
                myfloat = (float) myushortArray[i];
                procMaxMinAve (myfloat, &mymax, &mymin, &mytotal);
            }
            break;
          case NC_INT:
            myintArray = (int *) ncGetVarOut->dataArray->buf;
            mymax = mymin = (float) myintArray[0];
            for (i = 0; i < ncGetVarOut->dataArray->len; i++) {
                myfloat = (float) myintArray[i];
                procMaxMinAve (myfloat, &mymax, &mymin, &mytotal);
            }
            break;
          case NC_UINT:
            myuintArray = (unsigned int *) ncGetVarOut->dataArray->buf;
            mymax = mymin = (float) myuintArray[0];
            for (i = 0; i < ncGetVarOut->dataArray->len; i++) {
                myfloat = (float) myuintArray[i];
                procMaxMinAve (myfloat, &mymax, &mymin, &mytotal);
            }
            break;
          case NC_INT64:
            mylongArray = (long long *) ncGetVarOut->dataArray->buf;
            mymax = mymin = (float) mylongArray[0];
            for (i = 0; i < ncGetVarOut->dataArray->len; i++) {
                myfloat = (float) mylongArray[i];
                procMaxMinAve (myfloat, &mymax, &mymin, &mytotal);
            }
            break;
          case NC_UINT64:
            myulongArray = (unsigned long long *) ncGetVarOut->dataArray->buf;
            mymax = mymin = (float) myulongArray[0];
            for (i = 0; i < ncGetVarOut->dataArray->len; i++) {
                myfloat = (float) myulongArray[i];
                procMaxMinAve (myfloat, &mymax, &mymin, &mytotal);
            }
            break;
          case NC_FLOAT:
            myfloatArray = (float *) ncGetVarOut->dataArray->buf;
            mymax = mymin = myfloatArray[0];
            for (i = 0; i < ncGetVarOut->dataArray->len; i++) {
                myfloat = myfloatArray[i];
                procMaxMinAve (myfloat, &mymax, &mymin, &mytotal);
            }
            break;
          case NC_DOUBLE:
            mydoubleArray = (double *) ncGetVarOut->dataArray->buf;
            mymax = mymin = (float) mydoubleArray[0];
            for (i = 0; i < ncGetVarOut->dataArray->len; i++) {
                myfloat = (float) mydoubleArray[i];
                procMaxMinAve (myfloat, &mymax, &mymin, &mytotal);
            }
            break;
          default:
            rodsLog (LOG_ERROR,
              "msiNcVarStat: Unknow dataType %d", ncGetVarOut->dataArray->type);
            return (NETCDF_INVALID_DATA_TYPE);
        }
        snprintf (outStr, MAX_NAME_LEN,
          "      array length = %d max = %.4f min = %.4f ave = %.4f\n",
          ncGetVarOut->dataArray->len, mymax, mymin,
          mytotal / ncGetVarOut->dataArray->len);
        fillMsParam (statOutStr, NULL, STR_MS_T, outStr, NULL);
        return 0;
    }

    int
    procMaxMinAve (float myfloat, float *mymax, float *mymin, float *mytotal)
    {
        *mytotal += myfloat;
        if (myfloat < *mymin) *mymin = myfloat;
        if (myfloat > *mymax) *mymax = myfloat;
        return 0;
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
        msvc->add_operation( "msiNcVarStat", "msiNcVarStat" );

        // =-=-=-=-=-=-=-
        // 6. return the newly created microservice plugin
        return msvc;
    }

}; // extern "C"



