Installation
------------

This fork builds the native code of PyCryptodome as a single **static
library** with **CMake**. There is no ``setup.py``/``pip`` build anymore.

Requirements
~~~~~~~~~~~~

* CMake 3.20 or newer
* Visual Studio 2019, 2022 or 2026 (MSVC, x64 or x86)
* a CPython installation (for the headers: ``Python.h``,
  ``PyMODINIT_FUNC``); Python 3.14 is fine

The CMake build also works with GCC/Clang on Linux (the flags that
``compiler_opt.py`` used to detect are mirrored in ``CMakeLists.txt``),
even though the primary target is MSVC.

Build steps
~~~~~~~~~~~

#. Configure and build::

       cmake -S . -B build -DPython3_ROOT_DIR="C:/Program Files/Python314"
       cmake --build build --config Release

   If CMake already finds the right interpreter, ``Python3_ROOT_DIR`` can
   be omitted; any other Python can be selected with
   ``-DPython3_EXECUTABLE=...``.

   The build produces:

   * ``build/Release/pycryptodome_static.lib`` — the static library with
     all C primitives and all 42 ``PyInit_*()`` entry points
   * ``build/Release/pycryptodome_link_check.exe`` — example consumer
     that links the whole library and runs a smoke test
   * ``build/Release/pycryptodome_link_repl_test.exe`` — example embedder
     that registers all ``PyInit_*()`` entry points in the inittab, starts
     CPython and opens an interactive REPL

#. (Optional) Install the library and the public header::

       cmake --install build --config Release

   This copies ``pycryptodome_static.lib`` to ``<prefix>/lib`` and
   ``src/pycryptodome_init.h`` to ``<prefix>/include``.

#. (Optional) Run the link check through CTest::

       ctest --test-dir build -C Release

Consuming the library
~~~~~~~~~~~~~~~~~~~~~

* Include directories for direct use of the C primitives:
  ``src`` and ``src/libtom``.
* Entry points of the former ``.pyd`` modules: include
  ``pycryptodome_init.h`` and use the ``PYCRYPTODOME_INIT_TABLE``
  X-macro (see ``examples/consume_static.c``).
* In a CMake project::

      add_subdirectory(pycryptodome-static)
      target_link_libraries(my_app PRIVATE pycryptodome_static)

* In a Visual Studio project: add ``pycryptodome_static.lib`` to the
  linker inputs. Make sure the runtime library option (``/MT`` or
  ``/MD``) matches the one used to build the library (CMake defaults
  to ``/MD`` in ``Release`` and ``/MDd`` in ``Debug``).

Documentation
~~~~~~~~~~~~~

Project documentation is written in reStructuredText and it is stored
under ``Doc/src``. To publish it as HTML files, you need to install
`sphinx <http://www.sphinx-doc.org/en/stable/>`_ and use::

    > make -C Doc/ html

It will then be available under ``Doc/_build/html/``.
