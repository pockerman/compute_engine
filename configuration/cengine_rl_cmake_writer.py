from pathlib import Path
import os
from configuration.cmake_file_writer import CMakeFileWriter
from configuration import INFO


class RLCMakeWriter(CMakeFileWriter):

    @staticmethod
    def dir_path():
        current_dir = Path(os.getcwd())
        return current_dir / "cubic_engine" / "cengine"

    @staticmethod
    def module_dirs():
        return ['worlds', ]

    def __init__(self, configuration: dict, kernel_dir: Path, kernel_dirs: list,
                 cengine_dir: Path, cengine_dirs: list) -> None:
        super(RLCMakeWriter, self).__init__(configuration=configuration,
                                            project_name="cengine_rl",
                                            install_prefix=configuration["cengine"]["CMAKE_INSTALL_PREFIX"])

        self.kernel_dirs = kernel_dirs
        self.kernel_dir = kernel_dir
        self.cengine_dirs = cengine_dirs
        self.cengine_dir = cengine_dir
        self.dirs = RLCMakeWriter.module_dirs()

    def write_cmake_lists(self):

        print("{0} Writing CMakeLists for project {1}".format(INFO, self.project_name))

        # set the kernel path
        current_dir = Path(os.getcwd())
        with open(current_dir / "cubic_engine" / "rl" / "CMakeLists.txt", 'w', newline="\n") as fh:

            # write the basic set variables
            fh = self.write_basic_lists(fh=fh)

            # Blaze lib include
            fh.write('INCLUDE_DIRECTORIES(${BLAZE_INCL_DIR})\n')

            # library include files
            fh.write('INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/)\n')

            for kdir in self.kernel_dirs:
                fh.write('INCLUDE_DIRECTORIES({0})\n'.format(self.kernel_dir / kdir / 'src'))

            # cengine includes
            fh.write('INCLUDE_DIRECTORIES({0}/src/)\n'.format(self.cengine_dir))

            # trilinos includes
            if self.configuration["trilinos"]["USE_TRILINOS"]:
                fh.write('INCLUDE_DIRECTORIES(${TRILINOS_INCL_DIR})\n')

            if self.configuration["opencv"]["USE_OPEN_CV"]:
                fh.write('INCLUDE_DIRECTORIES({0})\n'.format(self.configuration["opencv"]["OPENCV_INCL_DIR"]))

            # boost includes
            fh.write('INCLUDE_DIRECTORIES(${BOOST_INCLUDEDIR})\n')
            # NLOHMANN_JSON_INCL_DIR
            fh.write('INCLUDE_DIRECTORIES(${NLOHMANN_JSON_INCL_DIR})\n')
            fh.write('\n')
            fh.write('ADD_LIBRARY({0} SHARED "")\n'.format(self.project_name))
            fh.write('\n')

            src_dir = current_dir / "cubic_engine" / "rl" / "src" / "cubic_engine" / "rl"
            fh.write('ADD_SUBDIRECTORY(%s)\n' % src_dir)

            with open(src_dir / 'CMakeLists.txt', 'w', newline='\n') as local_fh:
                dir_upper = 'SRCS'
                local_fh.write('IF(COMMAND cmake_policy)\n')
                local_fh.write('\tCMAKE_POLICY(SET CMP0076 NEW)\n')
                local_fh.write('ENDIF(COMMAND cmake_policy)\n')
                local_fh.write('\n')
                local_fh.write('FILE(GLOB %s ${PROJECT_SOURCE_DIR}/src/cubic_engine/rl/*.cpp '
                               '${PROJECT_SOURCE_DIR}/src/cubic_engine/rl/*/*.cpp)\n' % dir_upper)
                local_fh.write('TARGET_SOURCES(%s PUBLIC ${%s})\n' % (self.project_name, dir_upper))

            fh.write('\n')
            fh.write('SET_TARGET_PROPERTIES({0} PROPERTIES LINKER_LANGUAGE CXX)\n'.format(self.project_name))
            fh.write('INSTALL(TARGETS %s DESTINATION ${CMAKE_INSTALL_PREFIX})\n' % self.project_name)
            fh.write('MESSAGE(STATUS "Installation destination at: ${CMAKE_INSTALL_PREFIX}")\n')

        if self.configuration["cengine"]["rl"]["BUILD_TESTS"]:
            self._write_tests_cmake()

        if self.configuration["cengine"]["rl"]["BUILD_EXAMPLES"]:
            self._write_examples_cmake()

        print("{0} Done...".format(INFO))

    def _write_configure_files(self, fh):
        # library uses kernel/config.h and cengine/config.h
        return fh

    def _write_project_variables(self, fh):
        # library uses cengine project variables
        return fh

    def _write_single_cmake(self, path: Path, directory: str, example: bool) -> None:

        with open(path / "CMakeLists.txt", "w", newline="\n") as tfh:
            # set the kernel path
            current_dir = Path(os.getcwd())
            tfh.write("cmake_minimum_required(VERSION 3.0)\n")
            tfh.write("PROJECT({0} CXX)\n".format(directory))
            tfh.write("SET(SOURCE {0}.cpp)\n".format(directory))
            tfh.write("SET(EXECUTABLE  {0})\n".format(directory))

            tfh = self._find_boost(fh=tfh)
            tfh = self._find_blas(fh=tfh)
            tfh = self._write_build_option(fh=tfh)

            tfh.write('INCLUDE_DIRECTORIES({0})\n'.format(self.configuration["BLAZE_INCL_DIR"]))
            for directory in self.dirs:
                tfh.write('INCLUDE_DIRECTORIES(%s/%s/src/)\n' % (current_dir / "kernel" / "kernel", directory))

            if self.configuration["trilinos"]["USE_TRILINOS"]:
                tfh.write(
                    'INCLUDE_DIRECTORIES({0})\n'.format(self.configuration["trilinos"]["TRILINOS_INCL_DIR"]))

            if self.configuration["opencv"]["USE_OPEN_CV"]:
                tfh.write('INCLUDE_DIRECTORIES({0})\n'.format(self.configuration["opencv"]["OPENCV_INCL_DIR"]))

            tfh.write('INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})\n')
            if example is False:
                tfh.write('INCLUDE_DIRECTORIES({0})\n'.format(self.configuration["testing"]["GTEST_INC_DIR"]))
            tfh.write('\n')

            if example:
                link_dirs = [self.configuration["kernel"]["CMAKE_INSTALL_PREFIX"],
                             "${Boost_LIBRARY_DIRS}"]
            else:
                link_dirs = [self.configuration["kernel"]["CMAKE_INSTALL_PREFIX"],
                             self.configuration["testing"]["GTEST_LIB_DIR"],
                             "${Boost_LIBRARY_DIRS}"]

            if self.configuration["trilinos"]["USE_TRILINOS"]:
                link_dirs.append(self.configuration["trilinos"]["TRILINOS_LIB_DIR"])

            for link_dir in link_dirs:
                tfh.write("LINK_DIRECTORIES({0})\n".format(link_dir))

            tfh.write("\n")
            tfh.write('ADD_EXECUTABLE(%s %s)\n' % ("${EXECUTABLE}", "${SOURCE}"))
            tfh.write("\n")
            tfh.write('TARGET_LINK_LIBRARIES(%s %s)\n' % ("${EXECUTABLE}", self.project_name))

            if example is False:
                tfh.write('TARGET_LINK_LIBRARIES(${EXECUTABLE} gtest)\n')
                tfh.write('TARGET_LINK_LIBRARIES(${EXECUTABLE} gtest_main) '
                          '# so that tests dont need to have a main\n')
            tfh.write('TARGET_LINK_LIBRARIES(${EXECUTABLE} pthread)\n')
            tfh.write('TARGET_LINK_LIBRARIES(${EXECUTABLE} openblas)\n')

            if self.configuration["trilinos"]["USE_TRILINOS"]:
                libs = ["epetra", "aztecoo", "amesos"]
                for lib in libs:
                    tfh.write('TARGET_LINK_LIBRARIES(${EXECUTABLE} %s)\n' % lib)

    def _write_multiple_cmakes(self, path: Path, example: bool) -> None:

        # set the kernel path
        current_dir = Path(os.getcwd())

        # get the test directories
        working_path = current_dir / path
        working_dirs = os.listdir(working_path)

        for w_dir in working_dirs:
            w_dir_path = Path(working_path / w_dir)

            if w_dir_path.is_dir():
                self._write_single_cmake(path=w_dir_path, directory=w_dir, example=example)

    def _write_examples_cmake(self) -> None:
        """
        Write the examples CMakeLists
        :return:
        """
        current_dir = Path(os.getcwd())

        # get the test directories
        examples_path = current_dir / "kernel/kernel/examples"
        self._write_multiple_cmakes(path=examples_path, example=True)

    def _write_tests_cmake(self) -> None:
        """
        Write the CMakeLists for tests
        """

        # set the kernel path
        current_dir = Path(os.getcwd())

        # get the test directories
        path = current_dir / "kernel/kernel/tests"
        self._write_multiple_cmakes(path=path, example=False)