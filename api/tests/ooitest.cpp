/*** Copyright (c), The Regents of the University of California            ***
 *** For more information please refer to files in the COPYRIGHT directory ***/
/* packtest.c - test the basic packing routines */

#include "rodsClient.h"
#include <curl/curl.h>
//#include <jansson.h>
#include "myjansson.h"

#define NEW_ACC_PAYLOAD "payload={\"serviceRequest\": { \"serviceName\": \"bank\", \"serviceOp\": \"new_account\", \"params\": { \"name\": \"kurt\" }}}"

#define DEPOS_PAYLOAD "payload={\"serviceRequest\": { \"serviceName\": \"bank\", \"serviceOp\": \"deposit\", \"params\": { \"account_id\": \"1c231090c69b4eceae1c469f85da10ea\", \"amount\": 125.00 }}}"

#define BUY_BOND_PAYLOAD "payload={\"serviceRequest\": { \"serviceName\": \"bank\", \"serviceOp\": \"buy_bonds\", \"params\": { \"account_id\": \"1c231090c69b4eceae1c469f85da10ea\", \"cash_amount\": 115.00 }}}"

#define OOI_GATEWAY_URL "http://localhost"
#define OOI_GATEWAY_PORT 		"5000"

#define ION_SERVICE			"ion-service"
#define BANK_SERVICE_NAME    		"bank"
#define NEW_ACCOUNT_OP			"new_account"
#define DEPOSIT_OP			"deposit"
#define BUY_BOND_OP			"buy_bonds"
#define LIST_ACCOUNTS_OP		"list_accounts"
#define ACCOUNT_ID_KW			"account_id"
#define ACCOUNT_TYPE_KW			"account_type"
#define NAME_KW				"name"
#define AMOUNT_KW			"amount"
#define CASH_AMOUNT_KW			"cash_amount"

typedef struct {
    char account_id[NAME_LEN];
    char reponseStr[MAX_NAME_LEN];
    float cashDeposit;
    float bondAmount;
} bankOprOut_t;


int
runBankTest();
int
doNewAccount( CURL *easyhandle, char *account_type, char *name,
              char **accountIDOut );
int
doDeposit( CURL *easyhandle, char *account_id, float ammount );
int
doBuyBond( CURL *easyhandle, char *account_id, float cash_amount );
int
doListAccounts( CURL *easyhandle );
size_t
decodeDepositOut( void *buffer, size_t size, size_t nmemb, void *userp );
size_t
newAccountOut( void *buffer, size_t size, size_t nmemb, void *userp );
size_t
listAccountsOut( void *buffer, size_t size, size_t nmemb, void *userp );
int
printKeyValJsonObj( Json_t *keyValObj );

int
main( int argc, char **argv ) {
    int status;

    status = runBankTest();

    if ( status < 0 ) {
        exit( 0 );
    }
    else {
        exit( 1 );
    }
}

int
runBankTest() {
    CURL *easyhandle;
    int status;
    char *account_id;

    easyhandle = curl_easy_init();
    if ( !easyhandle ) {
        printf( "Curl Error: Initialization failed\n" );
        return( -1 );
    }

    status = doNewAccount( easyhandle, "Savings", "John", &account_id );

    if ( status < 0 ) {
        free( account_id );
        return status;
    }

    status = doDeposit( easyhandle, account_id, 500.0 );

    if ( status < 0 ) {
        return status;
    }

    status = doBuyBond( easyhandle, account_id, 200.0 );
    free( account_id );

    if ( status < 0 ) {
        return status;
    }

    status = doListAccounts( easyhandle );


    return status;
}

int
doNewAccount( CURL *easyhandle, char *account_type, char *name,
              char **accountIDOut ) {
    /* account_type can be Checking or Savings */
    Json_t *obj_p;
    CURLcode res;
    char myUrl[MAX_NAME_LEN];
    char postStr[MAX_NAME_LEN];
    char *objStr;
    bankOprOut_t bankOprOut;
    int status;

    PTR(Json_t) paramObj { Json_object() };

    status = Json_object_set_new( GET(paramObj), ACCOUNT_TYPE_KW,
                                  PTR(Json_t){ Json_string(account_type) } );

     if ( status != 0 ) {
        rodsLog( LOG_ERROR, "doNewAccount: son_object_set paramObj error" );
        Json_decref( paramObj );
        return -2;
    }
    status = Json_object_set_new( GET(paramObj), NAME_KW,
                                  PTR(Json_t){ Json_string(name) } );

    if ( status != 0 ) {
        rodsLog( LOG_ERROR, "doNewAccount: son_object_set paramObj error" );
        Json_decref( paramObj );
        return -2;
    }

    obj_p = Json_pack( "{s:{s:s,s:s,s:o}}",
                       "serviceRequest",
                       "serviceName", BANK_SERVICE_NAME,
                       "serviceOp", NEW_ACCOUNT_OP,
                       "params", Json_deep_copy(GET(paramObj)) );

    if ( obj_p == NULL ) {
        rodsLog( LOG_ERROR, "doNewAccount: json_pack error" );
        return -2;
    }

    PTR(Json_t) obj { obj_p };

    objStr = Json_dumps( GET(obj), 0 );

    if ( objStr == NULL ) {
        rodsLog( LOG_ERROR, "doNewAccount: json_dumps error" );
        return -2;
    }

    printf( "jsonStr : %s\n", objStr );

    snprintf( postStr, MAX_NAME_LEN, "payload=%s", objStr );
    /* deposit */
    snprintf( myUrl, MAX_NAME_LEN, "%s:%s/%s/%s/%s",
              OOI_GATEWAY_URL, OOI_GATEWAY_PORT, ION_SERVICE, BANK_SERVICE_NAME,
              NEW_ACCOUNT_OP );
    printf( "%s\n", myUrl );

    curl_easy_setopt( easyhandle, CURLOPT_POSTFIELDS, postStr );
    curl_easy_setopt( easyhandle, CURLOPT_URL, myUrl );
    curl_easy_setopt( easyhandle, CURLOPT_WRITEFUNCTION, newAccountOut );
    bzero( &bankOprOut, sizeof( bankOprOut ) );
    curl_easy_setopt( easyhandle, CURLOPT_WRITEDATA, &bankOprOut );

    res = curl_easy_perform( easyhandle );
    Json_decref( obj );
    free( objStr );
    if ( res != CURLE_OK ) {
        rodsLog( LOG_ERROR, "doNewAccount: curl_easy_perform error: %d", res );
        return -1;
    }
    printf( "output = %s\n", bankOprOut.account_id );
    *accountIDOut = strdup( bankOprOut.account_id );


    return 0;
}

int
doDeposit( CURL *easyhandle, char *account_id, float ammount ) {
    Json_t *obj;
    CURLcode res;
    char myUrl[MAX_NAME_LEN];
    char postStr[MAX_NAME_LEN];
    char *objStr;
    bankOprOut_t bankOprOut;

    printf( "Depositing $ %10.2f\n", ammount );

    obj = Json_pack( "{s:{s:s,s:s,s:{s:s,s:f}}}",
                     "serviceRequest",
                     "serviceName", BANK_SERVICE_NAME,
                     "serviceOp", DEPOSIT_OP,
                     "params",
                     ACCOUNT_ID_KW, account_id,
                     AMOUNT_KW, ammount );
    if ( obj == NULL ) {
        rodsLog( LOG_ERROR, "doDeposit: json_pack error" );
        return -2;
    }

    objStr = Json_dumps( obj, 0 );

    if ( objStr == NULL ) {
        rodsLog( LOG_ERROR, "doDeposit: Json_dumps error" );
        return -2;
    }

    snprintf( postStr, MAX_NAME_LEN, "payload=%s", objStr );
    /* deposit */
    snprintf( myUrl, MAX_NAME_LEN, "%s:%s/%s/%s/%s",
              OOI_GATEWAY_URL, OOI_GATEWAY_PORT, ION_SERVICE, BANK_SERVICE_NAME,
              DEPOSIT_OP );
    printf( "%s\n", myUrl );

    curl_easy_setopt( easyhandle, CURLOPT_POSTFIELDS, postStr );
    curl_easy_setopt( easyhandle, CURLOPT_URL, myUrl );
    curl_easy_setopt( easyhandle, CURLOPT_WRITEFUNCTION, decodeDepositOut );
    bzero( &bankOprOut, sizeof( bankOprOut ) );
    curl_easy_setopt( easyhandle, CURLOPT_WRITEDATA, &bankOprOut );

    res = curl_easy_perform( easyhandle );
    free( obj );
    free( objStr );
    if ( res != CURLE_OK ) {
        rodsLog( LOG_ERROR, "doDeposit: curl_easy_perform error: %d", res );
        return -1;
    }
    printf( "doDeposit output = %s\n", bankOprOut.reponseStr );

    return 0;
}

int
doBuyBond( CURL *easyhandle, char *account_id, float cash_amount ) {
    Json_t *obj;
    CURLcode res;
    char myUrl[MAX_NAME_LEN];
    char postStr[MAX_NAME_LEN];
    char *objStr;
    bankOprOut_t bankOprOut;

    printf( "Buying $ %10.2f bonds\n", cash_amount );

    obj = Json_pack( "{s:{s:s,s:s,s:{s:s,s:f}}}",
                     "serviceRequest",
                     "serviceName", BANK_SERVICE_NAME,
                     "serviceOp", BUY_BOND_OP,
                     "params",
                     ACCOUNT_ID_KW, account_id,
                     CASH_AMOUNT_KW, cash_amount );
    if ( obj == NULL ) {
        rodsLog( LOG_ERROR, "doBuyBond: json_pack error" );
        return -2;
    }
    objStr = Json_dumps( obj, 0 );

    if ( objStr == NULL ) {
        rodsLog( LOG_ERROR, "doBuyBond: json_dumps error" );
        return -2;
    }

    snprintf( postStr, MAX_NAME_LEN, "payload=%s", objStr );
    /* deposit */
    snprintf( myUrl, MAX_NAME_LEN, "%s:%s/%s/%s/%s",
              OOI_GATEWAY_URL, OOI_GATEWAY_PORT, ION_SERVICE, BANK_SERVICE_NAME,
              BUY_BOND_OP );
    printf( "%s\n", myUrl );

    curl_easy_setopt( easyhandle, CURLOPT_POSTFIELDS, postStr );
    curl_easy_setopt( easyhandle, CURLOPT_URL, myUrl );
    curl_easy_setopt( easyhandle, CURLOPT_WRITEFUNCTION, decodeDepositOut );
    bzero( &bankOprOut, sizeof( bankOprOut ) );
    curl_easy_setopt( easyhandle, CURLOPT_WRITEDATA, &bankOprOut );

    res = curl_easy_perform( easyhandle );
    free( obj );
    free( objStr );
    if ( res != CURLE_OK ) {
        rodsLog( LOG_ERROR, "doBuyBond: curl_easy_perform error: %d", res );
        return -1;
    }
    printf( "doBuyBond output = %s\n", bankOprOut.reponseStr );

    return 0;
}

size_t
decodeDepositOut( void *buffer, size_t size, size_t nmemb, void *userp ) {
    __Json_string_value_support__;
    Json_t *root_p, *dataObj, *responseObj;
    Json_error_t jerror;
    const char *responseStr;
    bankOprOut_t *bankOprOut;

    root_p = Json_loads( ( const char* ) buffer, 0, &jerror );

    if ( !root_p ) {
        rodsLog( LOG_ERROR,
                 "decodeDepositOut: json_loads error. %s", jerror.text );
        return 0;
    }

    PTR(Json_t) root {root_p};

    dataObj = Json_object_get( GET(root), "data" );
    if ( !dataObj ) {
        rodsLog( LOG_ERROR,
                 "decodeDepositOut: json_object_get data failed." );
        Json_decref( root );
        return 0;
    }
    responseObj = Json_object_get( dataObj, "GatewayResponse" );
    if ( !responseObj ) {
        rodsLog( LOG_ERROR,
                 "decodeDepositOut: json_object_get GatewayResponse failed." );
        Json_decref( root );
        return 0;
    }
    responseStr = Json_string_value( responseObj );
    bankOprOut = ( bankOprOut_t * ) userp;
    strncpy( bankOprOut->reponseStr, responseStr, MAX_NAME_LEN );
    Json_decref( root );

    return nmemb * size;
}

size_t
newAccountOut( void *buffer, size_t size, size_t nmemb, void *userp ) {
    __Json_string_value_support__;
    Json_t *root_p, *dataObj, *responseObj;
    Json_error_t jerror;
    const char *responseStr;
    bankOprOut_t *bankOprOut;

    root_p = Json_loads( ( const char* ) buffer, 0, &jerror );
    if ( !root_p ) {
        rodsLog( LOG_ERROR,
                 "decodeDepositOut: json_loads error. %s", jerror.text );
        return 0;
    }
    PTR(Json_t) root { root_p };

    dataObj = Json_object_get( GET(root), "data" );
    if ( !dataObj ) {
        rodsLog( LOG_ERROR,
                 "decodeDepositOut: json_object_get data failed." );
        Json_decref( root );
        return 0;
    }
    responseObj = Json_object_get( dataObj, "GatewayResponse" );
    if ( !responseObj ) {
        rodsLog( LOG_ERROR,
                 "decodeDepositOut: json_object_get GatewayResponse failed." );
        Json_decref( root );
        return 0;
    }
    responseStr = Json_string_value( responseObj );
    bankOprOut = ( bankOprOut_t * ) userp;
    strncpy( bankOprOut->account_id, responseStr, NAME_LEN );
    Json_decref( root );

    return nmemb * size;
}

int
doListAccounts( CURL *easyhandle ) {
    CURLcode res;
    char myUrl[MAX_NAME_LEN];
    bankOprOut_t *bankOprOut;

    snprintf( myUrl, MAX_NAME_LEN, "%s:%s/%s/%s/%s",
              OOI_GATEWAY_URL, OOI_GATEWAY_PORT, ION_SERVICE, BANK_SERVICE_NAME,
              LIST_ACCOUNTS_OP );
    printf( "%s\n", myUrl );

    curl_easy_reset( easyhandle );	/* reset to clean out POST option */
    curl_easy_setopt( easyhandle, CURLOPT_URL, myUrl );
    curl_easy_setopt( easyhandle, CURLOPT_WRITEFUNCTION, listAccountsOut );
    bzero( &bankOprOut, sizeof( bankOprOut ) );
    curl_easy_setopt( easyhandle, CURLOPT_WRITEDATA, &bankOprOut );

    res = curl_easy_perform( easyhandle );
    if ( res != CURLE_OK ) {
        rodsLog( LOG_ERROR, "listAccounts: curl_easy_perform error: %d", res );
        return -1;
    }
    return 0;
}
size_t
listAccountsOut( void *buffer, size_t size, size_t nmemb, void *userp ) {
    Json_t *root_p, *dataObj, *responseObj, *keyValObj;
    Json_error_t jerror;
    int i;

    root_p = Json_loads( ( const char* ) buffer, 0, &jerror );
    if ( !root_p ) {
        rodsLog( LOG_ERROR,
                 "listAccountsOut: json_loads error. %s", jerror.text );
        return 0;
    }

    PTR(Json_t) root { root_p };

    dataObj = Json_object_get( GET(root), "data" );
    if ( !dataObj ) {
        rodsLog( LOG_ERROR,
                 "listAccountsOut: json_object_get data failed." );
        Json_decref( root );
        return 0;
    }
    responseObj = Json_object_get( dataObj, "GatewayResponse" );
    if ( !responseObj ) {
        rodsLog( LOG_ERROR,
                 "listAccountsOut: json_object_get GatewayResponse failed." );
        Json_decref( root );
        return 0;
    }

    if ( !Json_is_array( responseObj ) ) {
        rodsLog( LOG_ERROR,
                 "listAccountsOut: responseObj is not an array." );
        Json_decref( root );
        return 0;
    }

    for ( i = 0; i < ( int ) Json_array_size( responseObj ); i++ ) {
        keyValObj = Json_array_get( responseObj, i );
        if ( !Json_is_object( keyValObj ) ) {
            rodsLog( LOG_ERROR,
                     "listAccountsOut: responseObj is not an array." );
            Json_decref( root );
            return 0;
        }
        printKeyValJsonObj( keyValObj );
    }
    return nmemb * size;
}

int
printKeyValJsonObj( Json_t *keyValObj ) {
    __Json_string_value_support__;
    const char *key;
    Json_t *value;

    printf( "keys: Value Object\n  {\n" );
    auto iter { Json_object_iter( keyValObj ) };

    while (GOOD( keyValObj, iter )) {
        Json_type_t myType;
        char valueStr[NAME_LEN];

        key = Json_object_iter_key( iter );
        value = Json_object_iter_value( iter );
        myType = Json_typeof( value );
        switch ( myType ) {
        case JSON__STRING:
            snprintf( valueStr, NAME_LEN, "%s", Json_string_value( value ) );
            break;
        case JSON__INTEGER:
            snprintf( valueStr, NAME_LEN, "%d",
                      ( int ) Json_integer_value( value ) );
            break;
        case JSON__REAL:
            snprintf( valueStr, NAME_LEN, "%f",
                      ( float ) Json_real_value( value ) );
            break;
        case JSON__TRUE:
            snprintf( valueStr, NAME_LEN, "TRUE" );
            break;
        case JSON__FALSE:
            snprintf( valueStr, NAME_LEN, "FALSE" );
            break;
        default:
            snprintf( valueStr, NAME_LEN, "Unknown json_type: %d", myType );
        }
        printf( "    %s: %s\n", key, valueStr );
        iter = Json_object_iter_next( keyValObj, iter );
    }
    printf( "  }\n" );

    return 0;
}
