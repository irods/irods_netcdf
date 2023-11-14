#ifndef NETCDF_PARSE_UTIL_HPP__
#define NETCDF_PARSE_UTIL_HPP__
#include <irods/msParam.h>
#include "ncInq.hpp"

extern "C" {

    int
    parseMspForNcInqIdInpName (msParam_t *inpParam, ncInqIdInp_t *ncInqIdInp);
    int
    parseMspForNcInqIdInpId (msParam_t *inpParam, ncInqIdInp_t *ncInqIdInp);
    int
    parseMspForNcGetVarInp (msParam_t *inpParam, ncGetVarInp_t *ncGetVarInp);
    int
    parseStrToNcType (char *myStr);
    int
    parseStrMspForLongArray (msParam_t *inpParam, int *ndimOut,
    rodsLong_t **longArrayOut);

    int
    parseMspForNcInqIdInpName (msParam_t *inpParam, ncInqIdInp_t *ncInqIdInp)
    {
        if (strcmp (inpParam->type, STR_MS_T) == 0) {
            /* str input */
            bzero (ncInqIdInp, sizeof (ncInqIdInp_t));
            rstrcpy (ncInqIdInp->name, (char*)inpParam->inOutStruct,
              MAX_NAME_LEN);
        } else  if (strcmp (inpParam->type, NcInqIdInp_MS_T) == 0) {
            *ncInqIdInp = *((ncInqIdInp_t *) inpParam->inOutStruct);
            replKeyVal (&((ncInqIdInp_t *) inpParam->inOutStruct)->condInput,
              &ncInqIdInp->condInput);
        } else {
            rodsLog (LOG_ERROR,
              "parseMspForNcInqIdInp: Unsupported input Param1 type %s",
              inpParam->type);
            return (USER_PARAM_TYPE_ERR);
        }
        return 0;
    }

    int
    parseMspForNcInqIdInpId (msParam_t *inpParam, ncInqIdInp_t *ncInqIdInp)
    {
        if (strcmp (inpParam->type, STR_MS_T) == 0) {
            /* str input */
            bzero (ncInqIdInp, sizeof (ncInqIdInp_t));
            ncInqIdInp->myid = atoi ((const char*)inpParam->inOutStruct);
        } else if (strcmp (inpParam->type, INT_MS_T) == 0) {
            bzero (ncInqIdInp, sizeof (ncInqIdInp_t));
            ncInqIdInp->myid = *(int *)inpParam->inOutStruct;
        } else if (strcmp (inpParam->type, NcInqIdInp_MS_T) == 0) {
            *ncInqIdInp = *((ncInqIdInp_t *) inpParam->inOutStruct);
            replKeyVal (&((ncInqIdInp_t *) inpParam->inOutStruct)->condInput,
              &ncInqIdInp->condInput);
        } else {
            rodsLog (LOG_ERROR,
              "parseMspForNcInqIdInp: Unsupported input Param1 type %s",
              inpParam->type);
            return (USER_PARAM_TYPE_ERR);
        }
        return 0;
    }

    int
    parseMspForNcGetVarInp (msParam_t *inpParam, ncGetVarInp_t *ncGetVarInp)
    {
        if (strcmp (inpParam->type, STR_MS_T) == 0) {
            /* str input */
            bzero (ncGetVarInp, sizeof (ncGetVarInp_t));
            ncGetVarInp->dataType = parseStrToNcType ((char*)inpParam->inOutStruct);
            if (ncGetVarInp->dataType < 0) return ncGetVarInp->dataType;
        } else if (strcmp (inpParam->type, INT_MS_T) == 0) {
            bzero (ncGetVarInp, sizeof (ncGetVarInp_t));
            ncGetVarInp->dataType = *(int *)inpParam->inOutStruct;
        } else if (strcmp (inpParam->type, NcGetVarInp_MS_T) == 0) {
            *ncGetVarInp = *((ncGetVarInp_t *) inpParam->inOutStruct);
            replKeyVal (&((ncGetVarInp_t *) inpParam->inOutStruct)->condInput,
              &ncGetVarInp->condInput);
        } else {
            rodsLog (LOG_ERROR,
              "parseMspForNcGetVarInp: Unsupported input Param1 type %s",
              inpParam->type);
            return (USER_PARAM_TYPE_ERR);
        }
        return 0;
    }

    int
    parseStrToNcType (char *myStr)
    {
        if (strcmp (myStr, "NC_BYTE") == 0) {
            return NC_BYTE;
        } else if (strcmp (myStr, "NC_CHAR") == 0) {
            return NC_CHAR;
        } else if (strcmp (myStr, "NC_SHORT") == 0) {
            return NC_SHORT;
        } else if (strcmp (myStr, "NC_INT") == 0) {
            return NC_INT;
        } else if (strcmp (myStr, "NC_FLOAT") == 0) {
            return NC_FLOAT;
        } else if (strcmp (myStr, "NC_DOUBLE") == 0) {
            return NC_DOUBLE;
        } else if (strcmp (myStr, "NC_UBYTE") == 0) {
            return NC_UBYTE;
        } else if (strcmp (myStr, "NC_USHORT") == 0) {
            return NC_USHORT;
        } else if (strcmp (myStr, "NC_UINT") == 0) {
            return NC_UINT;
        } else if (strcmp (myStr, "NC_INT64") == 0) {
            return NC_INT64;
        } else if (strcmp (myStr, "NC_UINT64") == 0) {
            return NC_UINT64;
        } else if (strcmp (myStr, "NC_STRING") == 0) {
            return NC_STRING;
        } else {
            rodsLog (LOG_ERROR,
              "parseStrToNcType: Unknow dataType %s", myStr);
            return (NETCDF_INVALID_DATA_TYPE);
        }
    }

    int
    parseStrMspForLongArray (msParam_t *inpParam, int *ndimOut,
    rodsLong_t **longArrayOut)
    {
        char *strPtr;
        strArray_t strArray;
        char *value;
        int status, i;
        rodsLong_t *longArray;

        if (strcmp (inpParam->type, STR_MS_T) == 0) {
            strPtr = (char *) inpParam->inOutStruct;
            memset (&strArray, 0, sizeof (strArray));

            status = parseMultiStr (strPtr, &strArray);

            if (status <= 0) return (SYS_INVALID_INPUT_PARAM);

            *ndimOut = strArray.len;
            value = strArray.value;
            *longArrayOut = longArray = (rodsLong_t *) 
              calloc (1, strArray.len * sizeof (rodsLong_t));
            for (i = 0; i < strArray.len; i++) {
          longArray[i] = atoi (&value[i * strArray.size]);
            }
            if (value != NULL) free (value);
        } else if (strcmp (inpParam->type, NcGetVarOut_MS_T) == 0) {
            ncGetVarOut_t *ncArray;
      int *inArray;
      int len;
      ncArray = (ncGetVarOut_t *) inpParam->inOutStruct;
      if (ncArray == NULL || ncArray->dataArray == NULL || 
        ncArray->dataArray->buf == NULL) return USER__NULL_INPUT_ERR;
      inArray = (int *) ncArray->dataArray->buf;
      len = ncArray->dataArray->len;
      if (len <= 0) return SYS_INVALID_INPUT_PARAM;
      *longArrayOut = longArray = (rodsLong_t *) 
              calloc (1, len * sizeof (rodsLong_t));
      for (i = 0; i < len; i++) {
          longArray[i] = inArray[i];
      }
      *ndimOut = len;
        } else {
            rodsLog (LOG_ERROR,
              "parseStrMspForLongArray: Unsupported input Param type %s",
              inpParam->type);
            return (USER_PARAM_TYPE_ERR);
        }

        return 0;
    }

} //extern "C"

#endif // NETCDF_PARSE_UTIL_HPP__
