/*** Copyright (c), The Regents of the University of California            ***
 *** For more information please refer to files in the COPYRIGHT directory ***/
/* This is script-generated code (for the most part).  */
/* See ooiGenServReq.h for a description of this API call.*/

#include <string>

#include "myjansson.h"
#include "netcdf_port.hpp"
#include "ooiGenServReq.hpp"
#include <irods/rodsLog.h>
#include <irods/rsGlobalExtern.hpp>
#include <irods/rcGlobalExtern.h>
#include <irods/rsApiHandler.hpp>
#include <irods/specColl.hpp>
#include <irods/resource.hpp>
#include <irods/miscServerFunct.hpp>
#include <irods/irods_resource_backport.hpp>
#include "ncApiIndex.hpp"
#include <boost/format.hpp>


#ifdef RODS_SERVER
#define IRODS_QUERY_ENABLE_SERVER_SIDE_API
#endif
#include <irods/irods_query.hpp>
#undef  IRODS_QUERY_ENABLE_SERVER_SIDE_API


extern "C" {
    double get_plugin_interface_version() { return 1.0; }
#ifdef RODS_SERVER
    int rsOoiGenServReq(
        rsComm_t *rsComm,
        ooiGenServReqInp_t *ooiGenServReqInp,
        ooiGenServReqOut_t **ooiGenServReqOut ) {
        int remoteFlag = 0;
        rodsServerHost_t* rodsServerHost = 0;
        irods::error ret = irods::get_host_for_hier_string(
                               ooiGenServReqInp->irodsRescName,
                               remoteFlag,
                               rodsServerHost );
        if ( !ret.ok() ) {
            irods::log( PASSMSG( "failed in call to irods::get_host_for_hier_string", ret ) );
            return SYS_INVALID_INPUT_PARAM;
        }

        int status = 0;
        if ( remoteFlag == LOCAL_HOST ) {
            status = _rsOoiGenServReq(
                         rsComm,
                         ooiGenServReqInp,
                         ooiGenServReqOut );
        }
        else if ( remoteFlag == REMOTE_HOST ) {
            status = remoteOoiGenServReq(
                         rsComm,
                         ooiGenServReqInp,
                         ooiGenServReqOut,
                         rodsServerHost );
        }
        else if ( remoteFlag < 0 ) {
            status = remoteFlag;
        }

        return status;
    }
#endif

    // =-=-=-=-=-=-=-
    // factory function to provide instance of the plugin
    irods::api_entry* plugin_factory(
        const std::string& _inst_name,
        const std::string& _context ) {
        // =-=-=-=-=-=-=-
        // create a api def object

        irods::apidef_t def = { OOI_GEN_SERV_REQ_AN,
                                RODS_API_VERSION,
                                REMOTE_USER_AUTH,
                                REMOTE_USER_AUTH,
                                "OoiGenServReqInp_PI", 0,
                                "OoiGenServReqOut_PI", 0,
#ifdef RODS_SERVER
                                CPP_14_FUNCTION( rsOoiGenServReq ),
#else
                                CPP_14_NOOPFUNC( rsComm_t*,   ooiGenServReqInp_t*, ooiGenServReqOut_t** ),
#endif // RODS_SERVER
                                "api_ooi_gen_serv_req",
                                [](void*){},
                                (funcPtr) RODS_SERVER_ENABLE(( irods::netcdf::api_call_wrapper< ooiGenServReqInp_t*, ooiGenServReqOut_t** > ))
                              };
        // =-=-=-=-=-=-=-
        // create an api object
        irods::api_entry* api = new irods::api_entry( def );

        // =-=-=-=-=-=-=-
        // assign the fcn which will handle the api call
#ifdef RODS_SERVER
//        api->fcn_name_ = "";
#endif // RODS_SERVER

        // =-=-=-=-=-=-=-
        // assign the pack struct key and value
        api->in_pack_key   = "OoiGenServReqInp_PI";
        api->in_pack_value = OoiGenServReqInp_PI;

        api->out_pack_key   = "OoiGenServReqOut_PI";
        api->out_pack_value = OoiGenServReqOut_PI;

        // =-=-=-=-=-=-=-
        // and... we're done.
        return api;

    } // plugin_factory


};

#ifdef RODS_SERVER
int
remoteOoiGenServReq( rsComm_t *rsComm, ooiGenServReqInp_t *ooiGenServReqInp,
                     ooiGenServReqOut_t **ooiGenServReqOut, rodsServerHost_t *rodsServerHost ) {
    int status;

    if ( rodsServerHost == NULL ) {
        rodsLog( LOG_NOTICE,
                 "remoteOoiGenServReq: Invalid rodsServerHost" );
        return SYS_INVALID_SERVER_HOST;
    }

    if ( ( status = svrToSvrConnect( rsComm, rodsServerHost ) ) < 0 ) {
        return status;
    }


    status = rcOoiGenServReq( rodsServerHost->conn, ooiGenServReqInp,
                              ooiGenServReqOut );

    if ( status < 0 ) {
        rodsLogError( LOG_ERROR, status,
                      "remoteOoiGenServReq: rcOoiGenServReq failed for %s, status = %d" );
    }

    return status;
}

int _rsOoiGenServReq(
    rsComm_t*            rsComm,
    ooiGenServReqInp_t*  ooiGenServReqInp,
    ooiGenServReqOut_t** ooiGenServReqOut ) {
    CURL *easyhandle;
    CURLcode res;
    char myUrl[MAX_NAME_LEN];
    int status;
    char *postStr = NULL;
    ooiGenServReqStruct_t ooiGenServReqStruct;

    if ( ooiGenServReqInp == NULL || ooiGenServReqOut == NULL ) {
        return USER__NULL_INPUT_ERR;
    }

    easyhandle = curl_easy_init();
    if ( !easyhandle ) {
        rodsLog( LOG_ERROR,
                 "_rsOoiGenServReq: curl_easy_init error" );
        return OOI_CURL_EASY_INIT_ERR;
    }

    /* the vault path must be a url e.g., http://localhost:5000 */
    std::string resc_vault_path;

    rodsLong_t resc_id = -1;

    std::string query_str {
        boost::str(boost::format("select RESC_ID where RESC_NAME = '%s'") % ooiGenServReqInp->irodsRescName)
    };


    for (auto&& row : irods::query<rsComm_t> {rsComm, query_str}) {
        resc_id =  std::stol(row[0]);
    }

    if (resc_id < 0)
    {
        return USER_INVALID_RESC_INPUT; 
    }

    irods::error ret = irods::get_resource_property< std::string >( resc_id, irods::RESOURCE_PATH, resc_vault_path );

    snprintf(
        myUrl,
        MAX_NAME_LEN,
        "%s/%s/%s/%s",
        resc_vault_path.c_str(),
        ION_SERVICE_STR,
        ooiGenServReqInp->servName,
        ooiGenServReqInp->servOpr );

    if ( ooiGenServReqInp->params.len > 0 ) {
        /* do POST */
        status = jsonPackOoiServReqForPost( ooiGenServReqInp->servName,
                                            ooiGenServReqInp->servOpr,
                                            &ooiGenServReqInp->params, &postStr );
        if ( status < 0 ) {
            rodsLogError( LOG_ERROR, status,
                          "_rsOoiGenServReq: jsonPackOoiServReq error" );
            return status;
        }
        curl_easy_setopt( easyhandle, CURLOPT_POSTFIELDS, postStr );
    }
    curl_easy_setopt( easyhandle, CURLOPT_URL, myUrl );
    curl_easy_setopt( easyhandle, CURLOPT_WRITEFUNCTION, ooiGenServReqFunc );
    bzero( &ooiGenServReqStruct, sizeof( ooiGenServReqStruct ) );
    ooiGenServReqStruct.outType = ooiGenServReqInp->outType;
    ooiGenServReqStruct.flags = ooiGenServReqInp->flags;

    curl_easy_setopt( easyhandle, CURLOPT_WRITEDATA, &ooiGenServReqStruct );

    res = curl_easy_perform( easyhandle );
    free( postStr );

    if ( res != CURLE_OK ) {
        /* res is +ive for error */
        rodsLog( LOG_ERROR,
                 "_rsOoiGenServReq: curl_easy_perform error: %d", res );
        free( *ooiGenServReqOut );
        *ooiGenServReqOut = NULL;
        return OOI_CURL_EASY_PERFORM_ERR - res;
    }
    *ooiGenServReqOut = ooiGenServReqStruct.ooiGenServReqOut;
    curl_easy_cleanup( easyhandle );

    return 0;
}

size_t
ooiGenServReqFunc( void *buffer, size_t size, size_t nmemb, void *userp ) {
    char *type_PI = "";
    int status;
    void *ptr = NULL;
    Json_t  *dataObj, *responseObj;
    Json_error_t jerror;

    ooiGenServReqStruct_t *ooiGenServReqStruct =
        ( ooiGenServReqStruct_t * ) userp;

    Json_t* root_p { Json_loads( ( const char* ) buffer, 0, &jerror ) };

    if ( !root_p ) {
        rodsLog( LOG_ERROR,
                 "jsonUnpackOoiRespStr: Json_loads error. %s", jerror.text );
        return OOI_JSON_LOAD_ERR;
    }

    PTR(Json_t) root { root_p };

    dataObj = Json_object_get( GET(root), OOI_DATA_TAG );
    if ( !dataObj ) {
        rodsLog( LOG_ERROR,
                 "jsonUnpackOoiRespStr: Json_object_get data failed." );
        Json_decref( root );
        return OOI_JSON_GET_ERR;
    }
    responseObj = Json_object_get( dataObj, OOI_GATEWAY_RESPONSE_TAG );
    if ( !responseObj ) {
        responseObj = Json_object_get( dataObj, OOI_GATEWAY_ERROR_TAG );
        if ( !responseObj ) {
            Json_decref( root );
            rodsLog( LOG_ERROR,
                     "jsonUnpackOoiRespStr: Json_object_get GatewayResponse failed." );
            return OOI_JSON_GET_ERR;
        }
        else {
            rodsLog( LOG_ERROR,
                     "jsonUnpackOoiRespStr: Gateway returns %s", ( char * ) buffer );
            ooiGenServReqStruct->ooiGenServReqOut =
                ( ooiGenServReqOut_t * ) calloc( 1, sizeof( ooiGenServReqOut_t ) );

            rstrcpy( ooiGenServReqStruct->ooiGenServReqOut->type_PI,
                     STR_MS_T, NAME_LEN );
            ooiGenServReqStruct->ooiGenServReqOut->ptr =
                strdup( ( char * )buffer );
            return OOI_JSON_GET_ERR;
        }
    }

    switch ( ooiGenServReqStruct->outType ) {
    case OOI_STR_TYPE:
        type_PI = STR_MS_T;
        status = jsonUnpackOoiRespStr( responseObj, ( char ** )( static_cast< void * >( &ptr ) ) );
        break;
    case OOI_DICT_TYPE:
        type_PI = Dictionary_MS_T;
        status = jsonUnpackOoiRespDict( responseObj, ( dictionary_t ** )( static_cast< void * >( &ptr ) ) );
        break;
    case OOI_ARRAY_TYPE:
        type_PI = GenArray_MS_T;
        status = jsonUnpackOoiRespArray( responseObj, ( genArray_t ** )( static_cast< void * >( &ptr ) ) );
        break;
    case OOI_INT_TYPE:
        type_PI = INT_MS_T;
        status = jsonUnpackOoiRespInt( responseObj, ( int ** )( static_cast< void * >( &ptr ) ) );
        break;
    case OOI_FLOAT_TYPE:
        type_PI = FLOAT_MS_T;
        status = jsonUnpackOoiRespFloat( responseObj, ( float ** )( static_cast< void * >( &ptr ) ) );
        break;
    case OOI_BOOL_TYPE:
        type_PI = BOOL_MS_T;
        status = jsonUnpackOoiRespBool( responseObj, ( int ** )( static_cast< void * >( &ptr ) ) );
        break;
    default:
        rodsLog( LOG_ERROR,
                 "ooiGenServReqFunc: outType %d not supported",
                 ooiGenServReqStruct->outType );
        status = OOI_JSON_TYPE_ERR;
    }
    Json_decref( root );
    if ( status < 0 ) {
        return 0;
    }

    ooiGenServReqStruct->ooiGenServReqOut =
        ( ooiGenServReqOut_t * ) calloc( 1, sizeof( ooiGenServReqOut_t ) );

    rstrcpy( ooiGenServReqStruct->ooiGenServReqOut->type_PI, type_PI, NAME_LEN );
    ooiGenServReqStruct->ooiGenServReqOut->ptr = ptr;

    return nmemb * size;
}
#endif //RODS_SERVER
