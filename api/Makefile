SERVER_LIBRARY = libirods_netcdf_server.a
CLIENT_LIBRARY = libirods_netcdf_client.a
SRCDIR = src

SRCS= $(SRCDIR)/rsNcArchTimeSeries.cpp \
			$(SRCDIR)/rsNcClose.cpp \
			$(SRCDIR)/rsNcCreate.cpp \
			$(SRCDIR)/rsNcGetAggElement.cpp \
			$(SRCDIR)/rsNcGetAggInfo.cpp \
			$(SRCDIR)/rsNcGetVarsByType.cpp \
			$(SRCDIR)/rsNcInq.cpp \
			$(SRCDIR)/rsNcInqGrps.cpp \
			$(SRCDIR)/rsNcInqId.cpp \
			$(SRCDIR)/rsNcInqWithId.cpp \
			$(SRCDIR)/rsNcOpen.cpp \
			$(SRCDIR)/rsNcOpenGroup.cpp \
			$(SRCDIR)/rsNcRegGlobalAttr.cpp \
			$(SRCDIR)/rsOoiGenServReq.cpp

MAKEFLAGS += --no-print-directory

CFLAGS += -Wno-write-strings
CFLAGS += -L/usr/lib/irods

EXTRAINCS += -I/usr/include/irods/boost -I/usr/include/irods/netcdf/include -I/usr/include/irods/jansson/src/ -I/usr/include/irods

BOOST_LIBS = -L/usr/lib/irods/externals\
    -lboost_filesystem \
    -lboost_system \
    -lboost_regex \
    -lboost_thread

EXTRALIBS += \
					$(BOOST_LIBS) \
					-lnetcdf
#         -lhdf5_hl \
#         -lh5tools \
#         -lhdf5

SERVER_LIBS += $(SERVER_LIBRARY)
CLIENT_LIBS += $(CLIENT_LIBRARY)

SERVER_DEPS += $(SERVER_LIBRARY)
CLIENT_DEPS += $(CLIENT_LIBRARY)

.PHONY: default clean library

default: api-base

library: $(SERVER_LIBRARY) $(CLIENT_LIBRARY)

$(SERVER_LIBRARY):
	@${MAKE} -C lib server-library

$(CLIENT_LIBRARY):
	@${MAKE} -C lib client-library

clean: clean-base
	@${MAKE} -C lib clean

include ../Makefile.base
