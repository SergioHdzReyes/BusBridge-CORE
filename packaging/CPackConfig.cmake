# ============================================================
# CPack configuration for packaging
# ============================================================

set(CPACK_PACKAGE_NAME "busbridge-core")
set(CPACK_PACKAGE_VENDOR "LowLevelLabs")
set(CPACK_PACKAGE_CONTACT "sergio.hdzreyes@gmail.com")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Cross-platform C++ middleware library for embedded communication buses.")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/SergioHdzReyes/BusBridge-CORE")

set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})

# -----------------------------
# Files and base names
# -----------------------------
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${PROJECT_VERSION}")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${PROJECT_VERSION}-src")

# -----------------------------
# Package types to generate
# -----------------------------
set(CPACK_GENERATOR "DEB;RPM;TGZ")
set(CPACK_SOURCE_GENERATOR "TGZ")

# -----------------------------
# License and resources
# -----------------------------
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")

# -----------------------------
# Installation by default
# -----------------------------
include(GNUInstallDirs)
set(CPACK_PACKAGE_INSTALL_DIRECTORY ${CPACK_PACKAGE_NAME})
set(CPACK_PACKAGING_INSTALL_PREFIX "/usr")


# -----------------------------
# Detect Linux distribution
# -----------------------------
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    if(EXISTS "/etc/os-release")
        file(READ "/etc/os-release" OS_RELEASE_CONTENTS)

        string(REGEX MATCH "ID=([A-Za-z0-9]+)" _ ${OS_RELEASE_CONTENTS})
        set(DISTRO_ID "${CMAKE_MATCH_1}")

        string(REGEX MATCH "ID_LIKE=([A-Za-z0-9]+)" _ ${OS_RELEASE_CONTENTS})
        set(DISTRO_LIKE "${CMAKE_MATCH_1}")

        string(TOLOWER "${DISTRO_ID}" DISTRO_ID)
        string(TOLOWER "${DISTRO_LIKE}" DISTRO_LIKE)
    endif()

    if(DISTRO_ID MATCHES "debian|ubuntu" OR DISTRO_LIKE MATCHES "debian")
        message(STATUS "Detected Debian-based system → Using DEB generator")
        set(CPACK_GENERATOR "DEB;TGZ")
    elseif(DISTRO_ID MATCHES "fedora|centos|rhel|rocky|alma" OR DISTRO_LIKE MATCHES "rhel|fedora")
        message(STATUS "Detected RPM-based system → Using RPM generator")
        set(CPACK_GENERATOR "RPM;TGZ")
    else()
        message(WARNING "Unknown Linux distribution, defaulting to TGZ")
        set(CPACK_GENERATOR "TGZ")
    endif()
else()
    set(CPACK_GENERATOR "TGZ")
endif()


# -----------------------------
# Ending CPack configuration
# -----------------------------
include(CPack)
