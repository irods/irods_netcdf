from __future__ import print_function

import optparse
import os
import shutil
import glob
import time
import irods_python_ci_utilities

def install_cmake_and_add_to_front_of_path():
    irods_python_ci_utilities.install_os_packages(['irods-externals-cmake3.11.4-0'])
    os.environ['PATH'] = '/opt/irods-externals/cmake3.11.4-0/bin' + os.pathsep + os.environ['PATH']

def get_build_prerequisites_all():
    return ['bats']

def get_build_prerequisites_apt():
    addl_prereqs_by_ubuntu_version_major = {
        '16': ['libnetcdf11' , 'libhdf5-10'],
        '18': ['libnetcdf13' , 'libhdf5-100']
    }
    try:
        return addl_prereqs_by_ubuntu_version_major[
                   irods_python_ci_utilities.get_distribution_version_major()
               ] + get_build_prerequisites_all()
    except KeyError:
        raise RuntimeError('Unsupported OS platform.')

def get_build_prerequisites_yum():
    return ['netcdf', 'hdf5'] + get_build_prerequisites_all()

def get_build_prerequisites():
    dispatch_map = {
        'Ubuntu': get_build_prerequisites_apt,
        'Centos': get_build_prerequisites_yum,
        'Centos linux': get_build_prerequisites_yum,
        'Opensuse ': get_build_prerequisites_yum,
    }
    try:
        return dispatch_map[irods_python_ci_utilities.get_distribution()]()
    except KeyError:
        irods_python_ci_utilities.raise_not_implemented_for_distribution()

def install_build_prerequisites_apt():
    irods_python_ci_utilities.install_os_packages(get_build_prerequisites())
    

def install_build_prerequisites_yum():
    irods_python_ci_utilities.install_os_packages(get_build_prerequisites())

def install_build_prerequisites():
    dispatch_map = {
        'Ubuntu': install_build_prerequisites_apt,
        'Centos': install_build_prerequisites_yum,
        'Centos linux': install_build_prerequisites_yum,
        'Opensuse ': install_build_prerequisites_yum,
    }
    try:
        return dispatch_map[irods_python_ci_utilities.get_distribution()]()
    except KeyError:
        irods_python_ci_utilities.raise_not_implemented_for_distribution()

def main():
    parser = optparse.OptionParser()
    parser.add_option('--output_root_directory')
    parser.add_option('--built_packages_root_directory')
    parser.add_option('--mungefs_packages_directory')

    options, _ = parser.parse_args()

    # dwm: 1 should always be called if system packages are needed ?
    #      2 maybe should be called install_test_prerequisites ?

    install_build_prerequisites()

    output_root_directory = options.output_root_directory
    built_packages_root_directory = options.built_packages_root_directory
    package_suffix = irods_python_ci_utilities.get_package_suffix()
    os_specific_directory = irods_python_ci_utilities.append_os_specific_directory(built_packages_root_directory)
    irods_python_ci_utilities.install_os_packages_from_files(glob.glob(os.path.join(os_specific_directory, 'irods-netcdf*.{0}'.format(package_suffix))))

    irods_python_ci_utilities.install_irods_core_dev_repository()

    time.sleep(10)

    try:
        test_output_file = 'log/test_output.log'
        irods_python_ci_utilities.subprocess_get_output(['sudo', 'su', '-', 'irods', '-c', 'bats ~/netcdf-plugin-tests'], check_rc=True)
    finally:
        if output_root_directory:
            irods_python_ci_utilities.gather_files_satisfying_predicate('/var/lib/irods/log', output_root_directory, lambda x: True)
            try: 
                shutil.copy('/var/lib/irods/log/test_output.log', output_root_directory)
            except Exception as e: 
                print("Could not copy out test_output.log:\n\t{}",e)

if __name__ == '__main__':
    main()
