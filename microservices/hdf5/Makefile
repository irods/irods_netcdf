ifndef $(CC)
CC=g++
endif
hdf5MSObjDir=obj
hdf5MSIncDir=include
hdf5MSSrcDir=src

INCLUDES+= -I/usr/include/irods -I/usr/include/irods/hdf5/src -I$(hdf5MSIncDir)
LDFLAGS+= -L/usr/lib/irods
LDLIBS+= libirods.a

CFLAGS+= $(INCLUDES)
CFLAGS+= -DRODS_SERVER
CFLAGS+= -Wall -Werror

HDF5_MS_OBJS=$(hdf5MSObjDir)/msiH5Dataset_read.o \
		$(hdf5MSObjDir)/msiH5Dataset_read_attribute.o \
		$(hdf5MSObjDir)/msiH5Group_read_attribute.o \
		$(hdf5MSObjDir)/msiH5File_close.o \
		$(hdf5MSObjDir)/msiH5File_open.o
.PHONY : default all clean

default: $(HDF5_MS_OBJS)

$(HDF5_MS_OBJS) : $(hdf5MSObjDir)/%.o : $(hdf5MSSrcDir)/%.cpp
	@echo "Compile hdf5 microservice `basename $@`..."
	$(CC) -c $(CFLAGS) -o $@ $<

clean :
	@rm -f $(hdf5MSObjDir)/*
