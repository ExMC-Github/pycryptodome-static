# ===================================================================
# CMake build for pycryptodome-static
#
# Builds every native module of PyCryptodome into a single static
# library (pycryptodome_static.lib with MSVC). The library contains
# all the C primitives and, for each former .pyd module, the
# PyInit_*() entry point (see src/common.h, FAKE_INIT macro).
# A host program only needs to link this library and register the
# PyInit_*() functions it is interested in.
#
# Supported and primary target: MSVC on Windows. The flags for
# GCC/Clang are kept so that the C sources also build elsewhere
# (e.g. CodeQL on Linux).
# ===================================================================

cmake_minimum_required(VERSION 3.20)

# Keep the library version in sync with the Python package.
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/lib/Crypto/__init__.py" PCT_INIT_PY)
string(REGEX MATCH "version_info[^=]*= *\\(([0-9]+), *([0-9]+), *'([0-9]+)" PCT_VERSION_MATCH "${PCT_INIT_PY}")
if(NOT PCT_VERSION_MATCH)
    string(REGEX MATCH "version_info[^=]*= *\\(([0-9]+), *([0-9]+), *([0-9]+)" PCT_VERSION_MATCH "${PCT_INIT_PY}")
endif()
if(NOT PCT_VERSION_MATCH)
    message(FATAL_ERROR "Cannot parse version_info from lib/Crypto/__init__.py")
endif()
set(PCT_VERSION_MAJOR ${CMAKE_MATCH_1})
set(PCT_VERSION_MINOR ${CMAKE_MATCH_2})
set(PCT_VERSION_PATCH ${CMAKE_MATCH_3})

project(pycryptodome_static
    VERSION ${PCT_VERSION_MAJOR}.${PCT_VERSION_MINOR}.${PCT_VERSION_PATCH}
    DESCRIPTION "PyCryptodome native code as a single static library"
    LANGUAGES C)

# The C code only includes Python.h (for the PyInit_* stubs); a static
# library is never linked against pythonXY.lib itself. Development.Module
# provides the header directory only. Development.Embed is needed by the
# REPL example, which starts an interpreter of its own.
find_package(Python3 REQUIRED COMPONENTS Development.Module Development.Embed)

option(PYCRYPTODOME_BUILD_EXAMPLES "Build the example that links the static library" ON)
include(CTest)

# -------------------------------------------------------------------
# Source files
#
# This is the list of modules that setup.py used to turn into one
# .pyd extension each. Files that are textually #included by others
# (block_common.c, blake2.c, blowfish_init.c, hash_SHA2_template.c,
# multiply_32.c, multiply_64.c, modexp_utils.c, libtom/tomcrypt_des.c)
# must NOT be compiled on their own.
#
# mont1.c, mont2.c and mont3.c only exist to work around an old
# distutils race condition (same source shared by several extensions);
# in this project mont.c is compiled once and shared by ec_ws.c,
# curve448.c, ed448.c and modexp.c.
# -------------------------------------------------------------------
set(PCT_SOURCES
    # Hash functions
    src/MD2.c
    src/MD4.c
    src/MD5.c
    src/SHA1.c
    src/SHA224.c
    src/SHA256.c
    src/SHA384.c
    src/SHA512.c
    src/RIPEMD160.c
    src/keccak.c
    src/blake2b.c
    src/blake2s.c
    src/ghash_portable.c
    src/ghash_clmul.c
    src/poly1305.c

    # Block ciphers
    src/AES.c
    src/AESNI.c
    src/ARC2.c
    src/blowfish.c
    src/blowfish_eks.c
    src/CAST.c
    src/DES.c
    src/DES3.c

    # Block cipher modes
    src/raw_ecb.c
    src/raw_cbc.c
    src/raw_cfb.c
    src/raw_ofb.c
    src/raw_ctr.c
    src/raw_ocb.c

    # Stream ciphers
    src/ARC4.c
    src/Salsa20.c
    src/chacha20.c

    # Others
    src/pkcs1_decode.c
    src/scrypt.c
    src/cpuid.c
    src/strxor.c

    # ECC / math (mont.c is shared, see note above)
    src/ec_ws.c
    src/mont.c
    src/p256_table.c
    src/p384_table.c
    src/p521_table.c
    src/curve25519.c
    src/curve448.c
    src/ed25519.c
    src/ed448.c
    src/modexp.c
)

add_library(pycryptodome_static STATIC ${PCT_SOURCES})

add_library(Pycryptodome::Static ALIAS pycryptodome_static)

set_target_properties(pycryptodome_static PROPERTIES
    C_STANDARD 99
    OUTPUT_NAME_DEBUG pycryptodome_static)

target_include_directories(pycryptodome_static
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/libtom>)

target_link_libraries(pycryptodome_static
    PUBLIC Python3::Module)

# -------------------------------------------------------------------
# Feature macros (mirroring what compiler_opt.py used to detect)
# -------------------------------------------------------------------
target_compile_definitions(pycryptodome_static PRIVATE
    HAVE_STDINT_H
    LTC_NO_ASM)

if(CMAKE_C_BYTE_ORDER STREQUAL "BIG_ENDIAN")
    target_compile_definitions(pycryptodome_static PRIVATE PYCRYPTO_BIG_ENDIAN)
else()
    target_compile_definitions(pycryptodome_static PRIVATE PYCRYPTO_LITTLE_ENDIAN)
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    target_compile_definitions(pycryptodome_static PRIVATE SYS_BITS=64)
else()
    target_compile_definitions(pycryptodome_static PRIVATE SYS_BITS=32)
endif()

if(MSVC)
    target_compile_definitions(pycryptodome_static PRIVATE
        HAVE_INTRIN_H
        USE_SSE2)
    if(CMAKE_SIZEOF_VOID_P EQUAL 4)
        # SSE2 is not the default on 32-bit MSVC targets
        target_compile_options(pycryptodome_static PRIVATE /arch:SSE2)
    endif()
else()
    # GCC / Clang on x86
    target_compile_definitions(pycryptodome_static PRIVATE
        HAVE_CPUID_H
        HAVE_POSIX_MEMALIGN
        HAVE_X86INTRIN_H
        USE_SSE2
        HAVE_UINT128)
    if(CMAKE_SIZEOF_VOID_P EQUAL 4 AND
       (CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86|i[3-6]86)$" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "AMD64"))
        target_compile_options(pycryptodome_static PRIVATE
            -msse2 -mstackrealign)
    endif()
    # PCLMUL/SSSE3 for GHASH with CLMUL; a no-op on x86-64 baselines
    # where both are always available.
    set_source_files_properties(src/ghash_clmul.c PROPERTIES
        COMPILE_OPTIONS "-mpclmul;-mssse3")
endif()

# GHASH with the CLMUL instruction (both MSVC and GCC)
set_source_files_properties(src/ghash_clmul.c PROPERTIES
    COMPILE_DEFINITIONS "HAVE_WMMINTRIN_H;HAVE_TMMINTRIN_H")

if(MSVC)
    # The sources contain UTF-8 characters; without /utf-8 MSVC would
    # decode them with the local code page (warning C4819).
    target_compile_options(pycryptodome_static PRIVATE /utf-8)
    # The sources are not warning-clean under /W4
    target_compile_options(pycryptodome_static PRIVATE
        /wd4244   # conversion, possible loss of data
        /wd4267   # conversion from size_t, possible loss of data
        /wd4996)  # 'function': was declared deprecated
endif()

# -------------------------------------------------------------------
# Installation
# -------------------------------------------------------------------
include(GNUInstallDirs)

install(TARGETS pycryptodome_static
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(FILES src/pycryptodome_init.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# -------------------------------------------------------------------
# Example / link check: forces every object of the static library to
# be pulled in (references to all PyInit_* symbols plus a couple of
# C calls), which also proves that there are no duplicate symbols.
# -------------------------------------------------------------------
if(PYCRYPTODOME_BUILD_EXAMPLES)
    add_executable(pycryptodome_link_check examples/consume_static.c)
    target_link_libraries(pycryptodome_link_check PRIVATE pycryptodome_static)
    if(MSVC)
        target_compile_options(pycryptodome_link_check PRIVATE /wd4996)
    endif()
    if(BUILD_TESTING)
        add_test(NAME link_check COMMAND pycryptodome_link_check)
    endif()

    # Embeds CPython: registers every PyInit_*() entry point in the
    # inittab, then starts an interactive REPL.
    add_executable(pycryptodome_link_repl_test examples/consume_repl.c)
    target_link_libraries(pycryptodome_link_repl_test PRIVATE
        pycryptodome_static
        Python3::Python)
    if(BUILD_TESTING)
        # With piped input the REPL reads EOF and exits cleanly.
        add_test(NAME link_repl_test COMMAND pycryptodome_link_repl_test)
    endif()
endif()
