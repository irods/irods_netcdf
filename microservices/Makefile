.PHONY: all clean netcdf hdf5

all: netcdf hdf5

MAKEFLAGS += --no-print-directory

netcdf:
	@${MAKE} -C netcdf/

hdf5:
	@${MAKE} -C hdf5/

clean:
	@${MAKE} -C netcdf/ clean
	@${MAKE} -C hdf5/ clean
