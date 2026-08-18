/* ===================================================================
 * consume_repl.c
 *
 * Example of a host program that embeds CPython and consumes
 * pycryptodome_static.lib:
 *
 * 1. it registers every PyInit_*() entry point of the static library
 *    with PyImport_AppendInittab(), before the interpreter starts;
 * 2. it starts CPython;
 * 3. it drops into an interactive REPL.
 *
 * All 42 modules become visible to the import machinery through
 * sys.builtin_module_names. Note that the PyInit_*() stubs return
 * NULL by design (see FAKE_INIT in src/common.h): they exist so that
 * a host program can wire the native code into its own environment,
 * not to provide fully importable modules.
 * =================================================================== */

#include <stdio.h>

#include <Python.h>

#include "pycryptodome_init.h"

int main(void)
{
    int registered = 0;
    int failures = 0;

    /* PyImport_AppendInittab() must be called before Py_Initialize() */
#define REGISTER(mod_name, init_fn) \
    do { \
        if (PyImport_AppendInittab(mod_name, init_fn) != 0) { \
            fprintf(stderr, "Failed to register inittab entry '%s'\n", mod_name); \
            failures++; \
        } else { \
            registered++; \
        } \
    } while (0);
    PYCRYPTODOME_INIT_TABLE(REGISTER)
#undef REGISTER

    if (failures) {
        fprintf(stderr, "%d module(s) could not be registered\n", failures);
        return 1;
    }
    printf("Registered %d PyCryptodome modules in the inittab\n", registered);

    Py_Initialize();

    /* Show what the interpreter sees; the entries registered above are
     * all visible through sys.builtin_module_names. */
    PyRun_SimpleString(
        "import sys\n"
        "mods = sorted(m for m in sys.builtin_module_names if m.startswith('Crypto'))\n"
        "print('Embedded Python %s, %d Crypto modules visible to imports'\n"
        "      % (sys.version.split()[0], len(mods)))\n"
    );

    printf("Starting the REPL (exit() or Ctrl-Z + Enter to quit)\n\n");
    PyRun_InteractiveLoop(stdin, "<stdin>");

    if (Py_FinalizeEx() < 0) {
        return 120;
    }
    return 0;
}
