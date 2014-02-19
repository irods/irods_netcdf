#include "rods.hpp"
#include "rcMisc.hpp"
#include "procApiRequest.hpp"
#include "apiNumber.hpp"
#include "initServer.hpp"
#include "dataObjInpOut.hpp"
#include "ncGetAggInfo.hpp"
#include "apiHeaderAll.hpp"

int readAggInfo( rsComm_t *rsComm, char *aggColl, keyValPair_t *condInput, ncAggInfo_t **ncAggInfo );
int _rsNcGetVarsByType( int ncid, ncGetVarInp_t *ncGetVarInp, ncGetVarOut_t **ncGetVarOut );
                    


