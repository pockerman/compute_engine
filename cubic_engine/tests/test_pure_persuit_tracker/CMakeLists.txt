cmake_minimum_required(VERSION 3.0)

PROJECT(test_pure_persuit_tracker CXX)
SET(SOURCE test.cpp)
SET(EXECUTABLE  test_pure_persuit_tracker)

INCLUDE_DIRECTORIES(${PROJECT_INCL_DIR}) 
INCLUDE_DIRECTORIES(${BOOST_INCLUDEDIR})
INCLUDE_DIRECTORIES(${KERNEL_INCL_DIR})
INCLUDE_DIRECTORIES(${BLAZE_INCL_DIR})
INCLUDE_DIRECTORIES(${GTEST_INC_DIR})

LINK_DIRECTORIES(${CMAKE_INSTALL_PREFIX})
LINK_DIRECTORIES(${KERNEL_LIB_DIR})
LINK_DIRECTORIES(${BOOST_LIBRARYDIR})
LINK_DIRECTORIES(${GTEST_LIB_DIR})

ADD_EXECUTABLE(${EXECUTABLE} ${SOURCE})


# Link the executable
TARGET_LINK_LIBRARIES(${EXECUTABLE} cubicengine)
TARGET_LINK_LIBRARIES(${EXECUTABLE} kernellib)

IF(USE_PYTORCH)
  TARGET_LINK_LIBRARIES(${EXECUTABLE} "${TORCH_LIBRARIES}")
ENDIF()

TARGET_LINK_LIBRARIES(${EXECUTABLE} openblas)
TARGET_LINK_LIBRARIES(${EXECUTABLE} gtest)
TARGET_LINK_LIBRARIES(${EXECUTABLE} gtest_main) # so that tests don't need to have a main
TARGET_LINK_LIBRARIES(${EXECUTABLE} pthread)

IF(USE_TRILINOS)
 TARGET_LINK_LIBRARIES(${EXECUTABLE} epetra)
 TARGET_LINK_LIBRARIES(${EXECUTABLE} aztecoo)
 TARGET_LINK_LIBRARIES(${EXECUTABLE} amesos)
ENDIF()

ADD_TEST(NAME ${EXECUTABLE} COMMAND ${EXECUTABLE})




