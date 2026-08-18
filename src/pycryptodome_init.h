/* ===================================================================
 * pycryptodome_init.h
 *
 * Entry points of the static PyCryptodome library.
 *
 * The static library contains the code of every native module that
 * used to ship as a separate .pyd, together with the corresponding
 * PyInit_*() stubs (see FAKE_INIT in common.h). A host program only
 * needs to link pycryptodome_static.lib and to register the PyInit_*
 * functions below; the C primitives (md5_init(), strxor(),
 * AES_start_operation(), ...) can also be called directly.
 * =================================================================== */

#ifndef PYCRYPTODOME_INIT_H
#define PYCRYPTODOME_INIT_H

#include <Python.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Hash functions */
PyMODINIT_FUNC PyInit__MD2(void);
PyMODINIT_FUNC PyInit__MD4(void);
PyMODINIT_FUNC PyInit__MD5(void);
PyMODINIT_FUNC PyInit__SHA1(void);
PyMODINIT_FUNC PyInit__SHA224(void);
PyMODINIT_FUNC PyInit__SHA256(void);
PyMODINIT_FUNC PyInit__SHA384(void);
PyMODINIT_FUNC PyInit__SHA512(void);
PyMODINIT_FUNC PyInit__RIPEMD160(void);
PyMODINIT_FUNC PyInit__keccak(void);
PyMODINIT_FUNC PyInit__BLAKE2b(void);
PyMODINIT_FUNC PyInit__BLAKE2s(void);
PyMODINIT_FUNC PyInit__ghash_portable(void);
PyMODINIT_FUNC PyInit__ghash_clmul(void);
PyMODINIT_FUNC PyInit__poly1305(void);

/* Block ciphers */
PyMODINIT_FUNC PyInit__raw_aes(void);
PyMODINIT_FUNC PyInit__raw_aesni(void);
PyMODINIT_FUNC PyInit__raw_arc2(void);
PyMODINIT_FUNC PyInit__raw_blowfish(void);
PyMODINIT_FUNC PyInit__raw_eksblowfish(void);
PyMODINIT_FUNC PyInit__raw_cast(void);
PyMODINIT_FUNC PyInit__raw_des(void);
PyMODINIT_FUNC PyInit__raw_des3(void);

/* Block cipher modes */
PyMODINIT_FUNC PyInit__raw_ecb(void);
PyMODINIT_FUNC PyInit__raw_cbc(void);
PyMODINIT_FUNC PyInit__raw_cfb(void);
PyMODINIT_FUNC PyInit__raw_ofb(void);
PyMODINIT_FUNC PyInit__raw_ctr(void);
PyMODINIT_FUNC PyInit__raw_ocb(void);

/* Stream ciphers */
PyMODINIT_FUNC PyInit__ARC4(void);
PyMODINIT_FUNC PyInit__Salsa20(void);
PyMODINIT_FUNC PyInit__chacha20(void);

/* Others */
PyMODINIT_FUNC PyInit__pkcs1_decode(void);
PyMODINIT_FUNC PyInit__scrypt(void);
PyMODINIT_FUNC PyInit__cpuid_c(void);
PyMODINIT_FUNC PyInit__strxor(void);

/* ECC / math */
PyMODINIT_FUNC PyInit__ec_ws(void);
PyMODINIT_FUNC PyInit__curve25519(void);
PyMODINIT_FUNC PyInit__curve448(void);
PyMODINIT_FUNC PyInit__ed25519(void);
PyMODINIT_FUNC PyInit__ed448(void);
PyMODINIT_FUNC PyInit__modexp(void);

/* -------------------------------------------------------------------
 * X-macro table mapping each former extension module to its entry
 * point. Use it to register every module in one go, for instance
 * with PyImport_AppendInittab() before Py_Initialize():
 *
 *     #define REGISTER(name, fn) PyImport_AppendInittab(name, fn);
 *     PYCRYPTODOME_INIT_TABLE(REGISTER)
 *
 * or simply to force the linker to pull in the whole library.
 * ------------------------------------------------------------------- */
#define PYCRYPTODOME_INIT_TABLE(F) \
    F("Crypto.Hash._MD2",             PyInit__MD2)             \
    F("Crypto.Hash._MD4",             PyInit__MD4)             \
    F("Crypto.Hash._MD5",             PyInit__MD5)             \
    F("Crypto.Hash._SHA1",            PyInit__SHA1)            \
    F("Crypto.Hash._SHA224",          PyInit__SHA224)          \
    F("Crypto.Hash._SHA256",          PyInit__SHA256)          \
    F("Crypto.Hash._SHA384",          PyInit__SHA384)          \
    F("Crypto.Hash._SHA512",          PyInit__SHA512)          \
    F("Crypto.Hash._RIPEMD160",       PyInit__RIPEMD160)       \
    F("Crypto.Hash._keccak",          PyInit__keccak)          \
    F("Crypto.Hash._BLAKE2b",         PyInit__BLAKE2b)         \
    F("Crypto.Hash._BLAKE2s",         PyInit__BLAKE2s)         \
    F("Crypto.Hash._ghash_portable",  PyInit__ghash_portable)  \
    F("Crypto.Hash._ghash_clmul",     PyInit__ghash_clmul)     \
    F("Crypto.Hash._poly1305",        PyInit__poly1305)        \
    F("Crypto.Cipher._raw_aes",       PyInit__raw_aes)         \
    F("Crypto.Cipher._raw_aesni",     PyInit__raw_aesni)       \
    F("Crypto.Cipher._raw_arc2",      PyInit__raw_arc2)        \
    F("Crypto.Cipher._raw_blowfish",  PyInit__raw_blowfish)    \
    F("Crypto.Cipher._raw_eksblowfish", PyInit__raw_eksblowfish) \
    F("Crypto.Cipher._raw_cast",      PyInit__raw_cast)        \
    F("Crypto.Cipher._raw_des",       PyInit__raw_des)         \
    F("Crypto.Cipher._raw_des3",      PyInit__raw_des3)        \
    F("Crypto.Cipher._raw_ecb",       PyInit__raw_ecb)         \
    F("Crypto.Cipher._raw_cbc",       PyInit__raw_cbc)         \
    F("Crypto.Cipher._raw_cfb",       PyInit__raw_cfb)         \
    F("Crypto.Cipher._raw_ofb",       PyInit__raw_ofb)         \
    F("Crypto.Cipher._raw_ctr",       PyInit__raw_ctr)         \
    F("Crypto.Cipher._raw_ocb",       PyInit__raw_ocb)         \
    F("Crypto.Cipher._ARC4",          PyInit__ARC4)            \
    F("Crypto.Cipher._Salsa20",       PyInit__Salsa20)         \
    F("Crypto.Cipher._chacha20",      PyInit__chacha20)        \
    F("Crypto.Cipher._pkcs1_decode",  PyInit__pkcs1_decode)    \
    F("Crypto.Protocol._scrypt",      PyInit__scrypt)          \
    F("Crypto.Util._cpuid_c",         PyInit__cpuid_c)         \
    F("Crypto.Util._strxor",          PyInit__strxor)          \
    F("Crypto.PublicKey._ec_ws",      PyInit__ec_ws)           \
    F("Crypto.PublicKey._curve25519", PyInit__curve25519)      \
    F("Crypto.PublicKey._curve448",   PyInit__curve448)        \
    F("Crypto.PublicKey._ed25519",    PyInit__ed25519)         \
    F("Crypto.PublicKey._ed448",      PyInit__ed448)           \
    F("Crypto.Math._modexp",          PyInit__modexp)

#ifdef __cplusplus
}
#endif

#endif /* PYCRYPTODOME_INIT_H */
