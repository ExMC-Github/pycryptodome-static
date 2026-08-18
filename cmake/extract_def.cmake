# ===================================================================
# extract_def.cmake
#
# Generate a .def file exporting every externally visible symbol of
# pycryptodome_static.lib. The .def is linked into the host executable
# (/DEF:) together with /WHOLEARCHIVE so that the Python frontend can
# resolve the raw C primitives through ctypes.CDLL(None) even though
# no .pyd exists on disk.
#
# Invoked as a build-time script:
#   cmake -DDUMPBIN=<dumpbin> -DSTATIC_LIB=<lib> -DDEF_FILE=<out> -P ...
# ===================================================================

execute_process(
    COMMAND "${DUMPBIN}" /SYMBOLS "${STATIC_LIB}"
    OUTPUT_VARIABLE symbols_out
    RESULT_VARIABLE rc
)
if(NOT rc EQUAL 0)
    message(FATAL_ERROR "dumpbin /SYMBOLS failed with exit code ${rc}")
endif()

# Keep only defined external symbols:
#   "00B 00000000 SECT5  notype ()    External     | aes_start_operation"
# UNDEF entries are references, not definitions, and symbols starting
# with "__" are MSVC/CRT internals that must not be re-exported.
string(REPLACE ";" "\\;" symbols_out "${symbols_out}")
string(REGEX MATCHALL "[^\r\n]+" lines "${symbols_out}")

set(names "")
foreach(line IN LISTS lines)
    if(line MATCHES "UNDEF")
        continue()
    endif()
    if(NOT line MATCHES "External")
        continue()
    endif()
    string(REGEX MATCH "\\|[ \t]+([^ \t]+)$" tail "${line}")
    if(NOT tail)
        continue()
    endif()
    set(sym "${CMAKE_MATCH_1}")
    if(sym MATCHES "^__")
        continue()
    endif()
    list(APPEND names "${sym}")
endforeach()

list(REMOVE_DUPLICATES names)
list(SORT names)

list(LENGTH names count)
if(count EQUAL 0)
    message(FATAL_ERROR "No external symbols found in ${STATIC_LIB}")
endif()

file(WRITE "${DEF_FILE}" "EXPORTS\n")
foreach(sym IN LISTS names)
    file(APPEND "${DEF_FILE}" "    ${sym}\n")
endforeach()
message(STATUS "Generated ${DEF_FILE}: ${count} exported symbols")
