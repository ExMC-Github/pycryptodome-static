# Smoke test run inside the embedded interpreter of
# pycryptodome_link_repl_test. The Crypto package comes from lib/ via
# PYTHONPATH; its native primitives are resolved with ctypes through a
# handle to the current process, where pycryptodome_static.lib has been
# linked with /WHOLEARCHIVE and re-exported via the generated .def.

import sys

from Crypto.Cipher import AES
from Crypto.Hash import SHA256
from Crypto.Protocol.KDF import scrypt

assert 'Crypto' in sys.modules

# AES-EAX round trip (exercises _raw_aes, _raw_ecb, _raw_cbc and the
# CPU feature detection that failed when no .pyd was present)
key = b'Sixteen byte key'
data = b'attack at dawn'
cipher = AES.new(key, AES.MODE_EAX)
ciphertext, tag = cipher.encrypt_and_digest(data)
cipher2 = AES.new(key, AES.MODE_EAX, nonce=cipher.nonce)
assert cipher2.decrypt_and_verify(ciphertext, tag) == data
print('AES-EAX roundtrip OK (%d bytes, tag %s)' % (len(ciphertext), tag.hex()[:16]))

# SHA-256 (exercises the _SHA256 primitives)
assert SHA256.new(b'abc').hexdigest().startswith('ba7816bf8f01cfea414140de5dae2223')

# scrypt (exercises the _scrypt primitives)
salt = b'\x00' * 16
derived = scrypt(b'password', salt, 16, N=1024, r=1, p=1)
assert len(derived) == 16

print('pycryptodome frontend fully functional inside the host process')
