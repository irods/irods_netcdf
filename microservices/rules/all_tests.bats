INSTANCE="irods_rule_engine_plugin-irods_rule_language-instance"

init_data_collections_for_tests() {
    [ ils hdf5 netcdf  >/dev/null 2>&1 ] || {
        for dir in netcdf hdf5 ; do
            echo "(--- putting '${dir}' collection for tests ---)"
            iput -fr $dir
        done
    }
}

@test "NetCDF rules test: 1" {
    cd "${BATS_TEST_DIRNAME}"; init_data_collections_for_tests
    irule -r "${INSTANCE}" -F netcdfTest1.r
}

@test "NetCDF rules test: 2" {
    cd "${BATS_TEST_DIRNAME}"; init_data_collections_for_tests
    irule -r "${INSTANCE}" -F netcdfTest2.r
}

@test "NetCDF rules test: 3" {
    cd "${BATS_TEST_DIRNAME}"; init_data_collections_for_tests
    irule -r "${INSTANCE}" -F netcdfTest3.r
}


@test "NetCDF rules test: H5 Groups" {
    cd "${BATS_TEST_DIRNAME}"; init_data_collections_for_tests
    irule -r "${INSTANCE}" -F netcdfTestGroup_limited.r
}
