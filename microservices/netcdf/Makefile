SRCDIR=src

SRCS=$(SRCDIR)/msiAddToNcArray.cpp \
		$(SRCDIR)/msiFloatToString.cpp \
		$(SRCDIR)/msiFreeNcStruct.cpp \
		$(SRCDIR)/msiNcClose.cpp \
		$(SRCDIR)/msiNcCreate.cpp \
		$(SRCDIR)/msiNcGetArrayLen.cpp \
		$(SRCDIR)/msiNcGetAttNameInInqOut.cpp \
		$(SRCDIR)/msiNcGetAttValStrInInqOut.cpp \
		$(SRCDIR)/msiNcGetDataType.cpp \
		$(SRCDIR)/msiNcGetDimLenInInqOut.cpp \
		$(SRCDIR)/msiNcGetDimNameInInqOut.cpp \
		$(SRCDIR)/msiNcGetElementInArray.cpp \
		$(SRCDIR)/msiNcGetFormatInInqOut.cpp \
		$(SRCDIR)/msiNcGetGrpInInqOut.cpp \
		$(SRCDIR)/msiNcGetNattsInInqOut.cpp \
		$(SRCDIR)/msiNcGetNdimsInInqOut.cpp \
		$(SRCDIR)/msiNcGetNGrpsInInqOut.cpp \
		$(SRCDIR)/msiNcGetNumDim.cpp \
		$(SRCDIR)/msiNcGetNvarsInInqOut.cpp \
		$(SRCDIR)/msiNcGetVarIdInInqOut.cpp \
		$(SRCDIR)/msiNcGetVarNameInInqOut.cpp \
		$(SRCDIR)/msiNcGetVarsByType.cpp \
		$(SRCDIR)/msiNcGetVarTypeInInqOut.cpp \
		$(SRCDIR)/msiNcInq.cpp \
		$(SRCDIR)/msiNcInqGrps.cpp \
		$(SRCDIR)/msiNcInqId.cpp \
		$(SRCDIR)/msiNcInqWithId.cpp \
		$(SRCDIR)/msiNcIntDataTypeToStr.cpp \
		$(SRCDIR)/msiNcOpen.cpp \
		$(SRCDIR)/msiNcOpenGroup.cpp \
		$(SRCDIR)/msiNcRegGlobalAttr.cpp \
		$(SRCDIR)/msiNcSubsetVar.cpp \
		$(SRCDIR)/msiNcVarStat.cpp

CFLAGS += -DRODS_SERVER

EXTRAINCS+= -I/usr/include/irods/netcdf/include -I../../api/include

EXTRALIBS+= -lnetcdf ../../api/libirods_netcdf_server.a

.PHONY: default clean

default: agnostic-base

clean: clean-base

include ../../Makefile.base
