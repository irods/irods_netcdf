ifndef $(CC)
CC=g++
endif
netcdfMSObjDir=obj
netcdfMSIncDir=include
netcdfMSSrcDir=src
netcdfApiIncDir=../../api/include

INCLUDES+= -I/usr/include/irods -I/usr/include/irods/netcdf/include -I$(netcdfMSIncDir) -I$(netcdfApiIncDir)
LDFLAGS+= -L/usr/lib/irods
LDLIBS+= libirods.a

CFLAGS+= $(INCLUDES)
CFLAGS+= -DRODS_SERVER -DNETCDF_API
CFLAGS+= -Wall -Werror

NETCDF_MS_OBJS=$(netcdfMSObjDir)/msiAddToNcArray.o \
		$(netcdfMSObjDir)/msiFloatToString.o \
		$(netcdfMSObjDir)/msiFreeNcStruct.o \
		$(netcdfMSObjDir)/msiNcClose.o \
		$(netcdfMSObjDir)/msiNcCreate.o \
		$(netcdfMSObjDir)/msiNcGetArrayLen.o \
		$(netcdfMSObjDir)/msiNcGetAttNameInInqOut.o \
		$(netcdfMSObjDir)/msiNcGetAttValStrInInqOut.o \
		$(netcdfMSObjDir)/msiNcGetDataType.o \
		$(netcdfMSObjDir)/msiNcGetDimLenInInqOut.o \
		$(netcdfMSObjDir)/msiNcGetDimNameInInqOut.o \
		$(netcdfMSObjDir)/msiNcGetElementInArray.o \
		$(netcdfMSObjDir)/msiNcGetFormatInInqOut.o \
		$(netcdfMSObjDir)/msiNcGetGrpInInqOut.o \
		$(netcdfMSObjDir)/msiNcGetNattsInInqOut.o \
		$(netcdfMSObjDir)/msiNcGetNdimsInInqOut.o \
		$(netcdfMSObjDir)/msiNcGetNGrpsInInqOut.o \
		$(netcdfMSObjDir)/msiNcGetNumDim.o \
		$(netcdfMSObjDir)/msiNcGetNvarsInInqOut.o \
		$(netcdfMSObjDir)/msiNcGetVarIdInInqOut.o \
		$(netcdfMSObjDir)/msiNcGetVarNameInInqOut.o \
		$(netcdfMSObjDir)/msiNcGetVarsByType.o \
		$(netcdfMSObjDir)/msiNcGetVarTypeInInqOut.o \
		$(netcdfMSObjDir)/msiNcInq.o \
		$(netcdfMSObjDir)/msiNcInqGrps.o \
		$(netcdfMSObjDir)/msiNcInqId.o \
		$(netcdfMSObjDir)/msiNcInqWithId.o \
		$(netcdfMSObjDir)/msiNcIntDataTypeToStr.o \
		$(netcdfMSObjDir)/msiNcOpen.o \
		$(netcdfMSObjDir)/msiNcOpenGroup.o \
		$(netcdfMSObjDir)/msiNcRegGlobalAttr.o \
		$(netcdfMSObjDir)/msiNcSubsetVar.o \
		$(netcdfMSObjDir)/msiNcVarStat.o

.PHONY : default all clean

default: $(NETCDF_MS_OBJS)

$(NETCDF_MS_OBJS) : $(netcdfMSObjDir)/%.o : $(netcdfMSSrcDir)/%.cpp
	@echo "Compile netcdf microservice `basename $@`..."
	$(CC) -c $(CFLAGS) -o $@ $<

clean :
	@rm -f $(netcdfMSObjDir)/*
