cmake_minimum_required(VERSION 3.0)

PROJECT(Example CXX)
SET(SOURCE exe.cpp)
SET(EXECUTABLE  exe_25)

INCLUDE_DIRECTORIES(${PROJECT_INCL_DIR})
INCLUDE_DIRECTORIES(${KERNEL_INCL_DIR})
INCLUDE_DIRECTORIES(${BLAZE_INCL_DIR})

LINK_DIRECTORIES(${PROJECT_LIB_DIR})
LINK_DIRECTORIES(${KERNEL_LIB_DIR})

ADD_EXECUTABLE(${EXECUTABLE} ${SOURCE})

TARGET_LINK_LIBRARIES(${EXECUTABLE} cubicengine)
TARGET_LINK_LIBRARIES(${EXECUTABLE} kernellib)

IF(USE_PYTORCH)
    TARGET_LINK_LIBRARIES(${EXECUTABLE} "${TORCH_LIBRARIES}")
ENDIF()

TARGET_LINK_LIBRARIES(${EXECUTABLE} openblas)
TARGET_LINK_LIBRARIES(${EXECUTABLE} pthread)


IF(USE_TRILINOS)
    TARGET_LINK_LIBRARIES(${EXECUTABLE} epetra)
    TARGET_LINK_LIBRARIES(${EXECUTABLE} aztecoo)
    TARGET_LINK_LIBRARIES(${EXECUTABLE} amesos)
ENDIF()

