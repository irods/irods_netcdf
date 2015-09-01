#ifndef NC_API_INDEX_HPP__
#define NC_API_INDEX_HPP__

/* 1000 - 1059 - NETCDF API calls */
#define NC_OPEN_AN                      1000
#define NC_CREATE_AN                    1001
#define NC_CLOSE_AN                     1002
#define NC_INQ_ID_AN                    1003
#define NC_INQ_WITH_ID_AN               1004
#define NC_GET_VARS_BY_TYPE_AN          1005
#define NC_INQ_AN                       1007
#define NC_OPEN_GROUP_AN                1008
#define NC_INQ_GRPS_AN                  1009
#define NC_REG_GLOBAL_ATTR_AN           1010
#define NC_GET_AGG_ELEMENT_AN           1011
#define NC_GET_AGG_INFO_AN              1012
#define NC_ARCH_TIME_SERIES_AN          1013

/* 1060 - OOI API calls */
#define OOI_GEN_SERV_REQ_AN             1060

#define NATIVE_NETCDF_CALL_KW "nativeNetcdfCall"
#define NEW_NETCDF_ARCH_KW    "newNetcdfArch"

#endif // NC_API_INDEX_HPP__
