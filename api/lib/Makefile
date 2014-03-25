TARGET = libirods_netcdf.a

SRCS =  ncUtil.cpp \
				ncarchUtil.cpp  \
				ncattrUtil.cpp \
				ooiCi.cpp \
				readAggInfo.cpp \
				_rsNcGetVarsByType.cpp \
				_rsNcInqDataObj.cpp \
				rcNcArchTimeSeries.cpp \
				rcNcClose.cpp  \
				rcNcCreate.cpp  \
				rcNcGetAggElement.cpp  \
				rcNcGetAggInfo.cpp  \
				rcNcGetVarsByType.cpp  \
				rcNcInq.cpp  \
				rcNcInqGrps.cpp  \
				rcNcInqId.cpp  \
				rcNcInqWithId.cpp  \
				rcNcOpen.cpp  \
				rcNcOpenGroup.cpp  \
				rcNcRegGlobalAttr.cpp

HEADERS =

EXTRALIBS = /usr/lib/libirods.a

EXTRAINCS = -I/usr/include/irods/jansson/src/

######################################################################
# General settings below.
# These should not need to be changed in most cases.

SODIR = ..
OBJDIR = .objs
SVR_OBJDIR = .svr_objs
CLI_OBJDIR = .cli_objs
DEPDIR = .deps

FULLTARGET = ${SODIR}/${TARGET}
SVR_TARGET= ${SODIR}/libirods_netcdf_server.a
CLI_TARGET= ${SODIR}/libirods_netcdf_client.a
DEPFILE = .depends

OBJS = $(patsubst %.cpp, ${OBJDIR}/%.o, ${SRCS})
SVR_OBJS = $(patsubst %.cpp, ${SVR_OBJDIR}/%.o, ${SRCS})
CLI_OBJS = $(patsubst %.cpp, ${CLI_OBJDIR}/%.o, ${SRCS})
DEPS = $(patsubst %.cpp, ${DEPDIR}/%.d, ${SRCS})

GCC = g++

CFLAGS = -Wno-write-strings -DNETCDF_API

INC = -I../include/ -I/usr/include/irods -I/usr/include/irods/netcdf/include ${EXTRAINCS}

.PHONY: default clean client-library server-library

default: ${FULLTARGET}

client-library: ${CLI_TARGET}

server-library: ${SVR_TARGET}

clean:
	@-rm -f ${FULLTARGET} > /dev/null 2>&1
	@-rm -f ${SVR_TARGET} > /dev/null 2>&1
	@-rm -f ${CLI_TARGET} > /dev/null 2>&1
	@-rm -f ${OBJS} > /dev/null 2>&1
	@-rm -f ${CLI_OBJS} > /dev/null 2>&1
	@-rm -f ${SVR_OBJS} > /dev/null 2>&1
	@-rm -f ${DEPS} > /dev/null 2>&1
	@-rm -f ${DEPFILE} > /dev/null 2>&1

${FULLTARGET}: ${OBJS} ${DEPFILE}
	ar  -r -c -s ${FULLTARGET} ./${OBJDIR}/*.o

${SVR_TARGET}: ${SVR_OBJS} ${DEPFILE}
	ar  -r -c -s ${SVR_TARGET} ./${SVR_OBJDIR}/*.o

${CLI_TARGET}: ${CLI_OBJS} ${DEPFILE}
	ar  -r -c -s ${CLI_TARGET} ./${CLI_OBJDIR}/*.o

${DEPFILE}: ${DEPS}
	@-rm -f ${DEPFILE} > /dev/null 2>&1
	@for dep in ${DEPS}; do \
	cat $$dep >> ${DEPFILE}; \
	done

$(DEPS): ${SRCS} ${HEADERS}
	@-mkdir -p ${DEPDIR} > /dev/null 2>&1
	${GCC} ${INC} -MM $< -MT $(patsubst %.cpp, ${OBJDIR}/%.o, $<) -MF $@

$(OBJS): ${OBJDIR}/%.o: %.cpp
	@-mkdir -p ${OBJDIR} > /dev/null 2>&1
	${GCC} ${CFLAGS} ${INC} -fPIC -c -g -o $@ $<

$(SVR_OBJS): ${SVR_OBJDIR}/%.o: %.cpp
	@-mkdir -p ${SVR_OBJDIR} > /dev/null 2>&1
	${GCC} ${CFLAGS} -DRODS_SERVER ${INC} -fPIC -c -g -o $@ $<

$(CLI_OBJS): ${CLI_OBJDIR}/%.o: %.cpp
	@-mkdir -p ${CLI_OBJDIR} > /dev/null 2>&1
	${GCC} ${CFLAGS} ${INC} -fPIC -c -g -o $@ $<


include ${DEPFILE}

