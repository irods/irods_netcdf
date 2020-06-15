/*** Copyright (c), The Regents of the University of California            ***
 *** For more information please refer to files in the COPYRIGHT directory ***/
/* ncClose.h
 */

#ifndef NC_CLOSE_HPP
#define NC_CLOSE_HPP

/* This is a NETCDF API call */

#include "rods.h"
#include "rcMisc.h"
#include "procApiRequest.h"
#include "apiNumber.h"
#include "initServer.hpp"
#include "dataObjInpOut.h"
#include "ncOpen.hpp"
#include "ncOpenGroup.hpp"

typedef struct {
    int type;			/* not used */
    int ncid;
    keyValPair_t condInput;
} ncCloseInp_t;

#define NcCloseInp_PI "int type; int ncid; struct KeyValPair_PI;"

#if defined(RODS_SERVER)
#define RS_NC_CLOSE rsNcClose
/* prototype for the server handler */
extern "C" int
rsNcClose( rsComm_t *rsComm, ncCloseInp_t *ncCloseInp );
#else
#define RS_NC_CLOSE NULL
#endif

extern "C" {

    /* rcNcClose - netcdf close an iRODS data object (equivalent to nc_close.
     * Input -
     *   rcComm_t *conn - The client connection handle.
     *   int the ncid of the opened object - an integer descriptor.
     * OutPut -
     */
    /* prototype for the client call */
    int
    rcNcClose( rcComm_t *conn, ncCloseInp_t *ncCloseInp );


}

#endif	/* NC_CLOSE_H */
