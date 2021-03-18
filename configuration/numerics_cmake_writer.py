import os
from pathlib import Path
from configuration import INFO
from configuration.cmake_file_writer import CMakeFileWriter


class NumericsCMakeWriter(CMakeFileWriter):
    def __init__(self, configuration: dict, kernel_dirs: list, kernel_dir: Path) -> None:
        super(NumericsCMakeWriter, self).__init__(configuration=configuration,
                                                  project_name="kernel_numerics")

        self.kernel_dirs = kernel_dirs
        self.kernel_dir = kernel_dir
        self.dirs = ['statistics', 'direct_solvers', 'fvm', 'optimization',
                     'solvers', 'krylov_solvers']

    def write_cmake_lists(self):
        """
        Write CMakeLists for the project
        :return:
        """
        print("{0} Writing CMakeLists for project {1}".format(INFO, self.project_name))
        current_dir = Path(os.getcwd())
        print("Current dir {0}".format(current_dir))

        with open("kernel/numerics/CMakeLists.txt", 'w', newline="\n") as fh:

            fh = self.write_basic_lists(fh=fh)
            fh.write('INCLUDE_DIRECTORIES(${BLAZE_INCL_DIR})\n')

            for kdir in self.kernel_dirs:
                fh.write('INCLUDE_DIRECTORIES({0})\n'.format(self.kernel_dir / kdir / 'src'))

            fh.write('INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/)\n')
            #for directory in self.dirs:
            #    fh.write('INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/%s)\n' % (directory))

            if self.configuration["trilinos"]["USE_TRILINOS"]:
                fh.write('INCLUDE_DIRECTORIES(${TRILINOS_INCL_DIR})\n')

            fh.write('INCLUDE_DIRECTORIES(${BOOST_INCLUDEDIR})\n')
            fh.write('\n')
            fh.write('ADD_LIBRARY({0} SHARED "")\n'.format(self.project_name))
            fh.write('\n')

            src_dir = current_dir / "kernel" / "numerics" / "src" / "kernel" / "numerics"
            fh.write('ADD_SUBDIRECTORY(%s)\n' % src_dir)

            with open(src_dir / 'CMakeLists.txt', 'w', newline='\n') as local_fh:
                #dir_upper = directory.upper() + 'SRCS'
                local_fh.write('IF(COMMAND cmake_policy)\n')
                local_fh.write('\tCMAKE_POLICY(SET CMP0076 NEW)\n')
                local_fh.write('ENDIF(COMMAND cmake_policy)\n')
                local_fh.write('\n')
                local_fh.write('FILE(GLOB SRCS %s/*.cpp '
                               '%s/*/*.cpp)\n' % (src_dir, src_dir))
                local_fh.write('TARGET_SOURCES(%s PUBLIC ${%s})\n' % (self.project_name, 'SRCS'))

            """
            with open(src_dir / 'CMakeLists.txt', 'w', newline='\n') as local_fh:
                dir_upper = directory.upper() + '_SRCS'
                local_fh.write('IF(COMMAND cmake_policy)\n')
                local_fh.write('\tCMAKE_POLICY(SET CMP0076 NEW)\n')
                local_fh.write('ENDIF(COMMAND cmake_policy)\n')
                local_fh.write('\n')
                local_fh.write('FILE(GLOB %s ${PROJECT_SOURCE_DIR}/%s/src/%s/%s/*.cpp '
                               '${PROJECT_SOURCE_DIR}/%s/src/%s/%s/*/*.cpp)\n' % (
                               dir_upper, directory, self.project_name, directory,
                               directory, self.project_name, directory))
                local_fh.write('TARGET_SOURCES(%s PUBLIC ${%s})\n' % (self.project_name, dir_upper))
            """
            fh.write('SET_TARGET_PROPERTIES({0} PROPERTIES LINKER_LANGUAGE CXX)\n'.format(self.project_name))
            fh.write('INSTALL(TARGETS %s DESTINATION ${CMAKE_INSTALL_PREFIX})\n' % self.project_name)
            fh.write('MESSAGE(STATUS "Installation destination at: ${CMAKE_INSTALL_PREFIX}")\n')

        print("{0} Done...".format(INFO))

    def _write_configure_files(self, fh):
        """
        Do nothing as the project will use kernel/base/config.h
        """
        return fh

    def _write_project_variables(self, fh):

        if self.configuration["trilinos"]["USE_TRILINOS"]:
            fh.write('SET(USE_TRILINOS {0})\n'.format(self.configuration["trilinos"]["USE_TRILINOS"]))
            fh.write('SET(USE_TRILINOS_LONG_LONG_TYPE {0})\n'.format(
                self.configuration["trilinos"]["USE_TRILINOS_LONG_LONG_TYPE"]))
            fh.write('SET(TRILINOS_INCL_DIR {0})\n'.format(self.configuration["trilinos"]["TRILINOS_INCL_DIR"]))
            fh.write('SET(TRILINOS_LIB_DIR {0})\n'.format(self.configuration["trilinos"]["TRILINOS_LIB_DIR"]))
        return fh
