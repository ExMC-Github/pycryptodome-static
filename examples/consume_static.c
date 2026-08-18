/* ===================================================================
 * consume_static.c
 *
 * Example of a host program consuming pycryptodome_static.lib.
 *
 * - It references every PyInit_*() entry point through the
 *   PYCRYPTODOME_INIT_TABLE X-macro, so the linker pulls in all
 *   objects of the static library (this is also the duplicate-symbol
 *   check for the library itself).
 * - It calls a C primitive (strxor) to prove the code is functional.
 * =================================================================== */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "pycryptodome_init.h"

/* Implemented in the static library (src/strxor.c) */
void strxor(const uint8_t *in1, const uint8_t *in2, uint8_t *out, size_t len);

int main(void)
{
    int count = 0;
    int failures = 0;

    /* Force the address of every PyInit_*() function to be resolved */
#define FORCE_LINK(mod_name, init_fn) \
    do { \
        if (init_fn == NULL) { \
            failures++; \
        } \
        count++; \
    } while (0);
    PYCRYPTODOME_INIT_TABLE(FORCE_LINK)
#undef FORCE_LINK

    printf("Resolved %d PyInit entry points (%d missing)\n", count, failures);

    /* Smoke test on a C primitive */
    {
        const uint8_t a[4] = { 0xAA, 0x55, 0xF0, 0x0F };
        const uint8_t b[4] = { 0x0F, 0xF0, 0x55, 0xAA };
        const uint8_t expected[4] = { 0xA5, 0xA5, 0xA5, 0xA5 };
        uint8_t out[4];

        strxor(a, b, out, 4);
        if (memcmp(out, expected, 4) != 0) {
            printf("strxor smoke test FAILED\n");
            failures++;
        } else {
            printf("strxor smoke test OK\n");
        }
    }

    return failures ? 1 : 0;
}
