# Install script for directory: /home/semes/rsa_tool/Source_20/01.SOEM_20/SOEM-master

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/home/semes/rsa_tool/Source_20/01.SOEM_20/SOEM-master/install")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/semes/rsa_tool/Source_20/01.SOEM_20/SOEM-master/build/libsoem.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/soem/cmake/soemConfig.cmake")
    file(DIFFERENT EXPORT_FILE_CHANGED FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/soem/cmake/soemConfig.cmake"
         "/home/semes/rsa_tool/Source_20/01.SOEM_20/SOEM-master/build/CMakeFiles/Export/share/soem/cmake/soemConfig.cmake")
    if(EXPORT_FILE_CHANGED)
      file(GLOB OLD_CONFIG_FILES "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/soem/cmake/soemConfig-*.cmake")
      if(OLD_CONFIG_FILES)
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/soem/cmake/soemConfig.cmake\" will be replaced.  Removing files [${OLD_CONFIG_FILES}].")
        file(REMOVE ${OLD_CONFIG_FILES})
      endif()
    endif()
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/soem/cmake" TYPE FILE FILES "/home/semes/rsa_tool/Source_20/01.SOEM_20/SOEM-master/build/CMakeFiles/Export/share/soem/cmake/soemConfig.cmake")
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^()$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/soem/cmake" TYPE FILE FILES "/home/semes/rsa_tool/Source_20/01.SOEM_20/SOEM-master/build/CMakeFiles/Export/share/soem/cmake/soemConfig-noconfig.cmake")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/soem" TYPE FILE FILES
    "/home/semes/rsa_tool/Source_20/01.SOEM_20/SOEM-master/soem/ethercat.h"
    "/home/semes/rsa_tool/Source_20/01.SOEM_20/SOEM-master/soem/ethercatbase.h"
    "/home/semes/rsa_tool/Source_20/01.SOEM_20/SOEM-master/soem/ethercatcoe.h"
    "/home/semes/rsa_tool/Source_20/01.SOEM_20/SOEM-master/soem/ethercatconfig.h"
    "/home/semes/rsa_tool/Source_20/01.SOEM_20/SOEM-master/soem/ethercatconfiglist.h"
    "/home/semes/rsa_tool/Source_20/01.SOEM_20/SOEM-master/soem/ethercatdc.h"
    "/home/semes/rsa_tool/Source_20/01.SOEM_20/SOEM-master/soem/ethercateoe.h"
    "/home/semes/rsa_tool/Source_20/01.SOEM_20/SOEM-master/soem/ethercatfoe.h"
    "/home/semes/rsa_tool/Source_20/01.SOEM_20/SOEM-master/soem/ethercatmain.h"
    "/home/semes/rsa_tool/Source_20/01.SOEM_20/SOEM-master/soem/ethercatprint.h"
    "/home/semes/rsa_tool/Source_20/01.SOEM_20/SOEM-master/soem/ethercatsoe.h"
    "/home/semes/rsa_tool/Source_20/01.SOEM_20/SOEM-master/soem/ethercattype.h"
    "/home/semes/rsa_tool/Source_20/01.SOEM_20/SOEM-master/osal/linux/osal_defs.h"
    "/home/semes/rsa_tool/Source_20/01.SOEM_20/SOEM-master/osal/osal.h"
    "/home/semes/rsa_tool/Source_20/01.SOEM_20/SOEM-master/oshw/linux/nicdrv.h"
    "/home/semes/rsa_tool/Source_20/01.SOEM_20/SOEM-master/oshw/linux/oshw.h"
    )
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/semes/rsa_tool/Source_20/01.SOEM_20/SOEM-master/build/test/simple_ng/cmake_install.cmake")
  include("/home/semes/rsa_tool/Source_20/01.SOEM_20/SOEM-master/build/test/linux/slaveinfo/cmake_install.cmake")
  include("/home/semes/rsa_tool/Source_20/01.SOEM_20/SOEM-master/build/test/linux/eepromtool/cmake_install.cmake")
  include("/home/semes/rsa_tool/Source_20/01.SOEM_20/SOEM-master/build/test/linux/simple_test/cmake_install.cmake")
  include("/home/semes/rsa_tool/Source_20/01.SOEM_20/SOEM-master/build/test/linux/firm_update/cmake_install.cmake")
  include("/home/semes/rsa_tool/Source_20/01.SOEM_20/SOEM-master/build/test/linux/ebox/cmake_install.cmake")
  include("/home/semes/rsa_tool/Source_20/01.SOEM_20/SOEM-master/build/test/linux/red_test/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/home/semes/rsa_tool/Source_20/01.SOEM_20/SOEM-master/build/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
