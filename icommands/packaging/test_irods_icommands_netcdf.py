import sys
if sys.version_info >= (2, 7):
    import unittest
else:
    import unittest2 as unittest
import os
import tempfile
import time
import shutil

import configuration
import lib
import resource_suite


class Test_ICommands_Netcdf(resource_suite.ResourceBase, unittest.TestCase):

    def setUp(self):
        super(Test_ICommands_Netcdf, self).setUp()
        self.testing_tmp_dir = '/tmp/irods-test-icommands-netcdf'
        shutil.rmtree(self.testing_tmp_dir, ignore_errors=True)
        os.mkdir(self.testing_tmp_dir)

    def tearDown(self):
        shutil.rmtree(self.testing_tmp_dir)
        super(Test_ICommands_Netcdf, self).tearDown()

    def test_inc_header(self):
        filename = 'sfc_pres_temp.nc'
        self.admin.assert_icommand(['iput', '-f', filename])
        self.admin.assert_icommand(['inc', '--header', filename], 'STDOUT_SINGLELINE', 'netcdf sfc_pres_temp')


