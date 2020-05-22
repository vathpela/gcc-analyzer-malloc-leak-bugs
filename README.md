Some gcc bug reproducers
========================

This is a repo of some GCC bug reproducers.  Run "make tests" to run them.

analyzer-malloc-leak
--------------------
This shows several ways to get `gcc -fanalyze` to trigger `-Wanalyzer-malloc-leak` where the following conditions are true:
- the code is a common pattern in C
- there is obviously no memory leak or incorrect access
- valgrind shows no memory leaks or incorrect accesses

Current status:
- without -flto:
  - `-DDEFINE_TEST0 -DDEFINE_TEST1 -DCALL_TEST0 -DCALL_TEST1`
    - everything defined and called
    - triggers `-Wanalyzer-malloc-leak` compiling `no_leak_0`
      - tells me something happened at location `cc1`, about which I know
        nothing, and code for which it does not show.
    - triggers `-Wanalyzer-malloc-leak` compiling `no_leak_1`
      - tells me something happened at location "cc1", about which I know
        nothing, and code for which it does not show.
    - all run fine
    - valgrind finds no problems

  - `-DDEFINE_TEST0 -DCALL_TEST0`:
    - only `no_leak_0` defined, only `no_leak_0` called
    - triggers `-Wanalyzer-malloc-leak` compiling `no_leak_0`
      - tells me something happened at location `cc1`, about which I know
        nothing, and code for which it does not show.
    - `no_leak_0` runs fine
    - valgrind finds no problems

  - `-DCALL_TEST0 -DDEFINE_TEST0 -DDEFINE_TEST1`
    - both defined, only `no_leak_0` called
    - both trigger
    - `no_leak_0` runs fine
    - valgrind finds no problems

  - `-DDEFINE_TEST1 -DCALL_TEST1`:
    - only `no_leak_1` defined, only `no_leak_1` called
    - triggers `-Wanalyzer-malloc-leak` compiling `no_leak_1`
      - tells me something happened at location `cc1`, about which I know
        nothing, and code for which it does not show.
    - `no_leak_1` runs fine
    - valgrind finds no problems

- with -flto:
  - `-DDEFINE_TEST0 -DDEFINE_TEST1 -DCALL_TEST0 -DCALL_TEST1`
     - everything defined and called
    - triggers `-Wanalyzer-malloc-leak` compiling `no_leak_1` but not `no_leak_0`
      - all the variables now have name like outp_25(D), instead of the actual
        variable names.
      - tells me something happened at location "lto1", about which I know
        nothing, and code for which it does not show.
    - all run fine
    - valgrind finds no problems

  - `-DCALL_TEST0 -DDEFINE_TEST0 -DDEFINE_TEST1`
    - both defined, only `no_leak_0` called
    - triggers `-Wanalyzer-malloc-leak` compiling `no_leak_1`
      - only the code path that should be completely optimized out triggers the analyzer!
      - all the variables now have name like outp_25(D), instead of the actual
        variable names.
      - tells me something happened at location "lto1", about which I know
        nothing, and code for which it does not show.
    - `no_leak_0` runs fine
    - valgrind finds no problems

  - `-DCALL_TEST1 -DDEFINE_TEST1`
    - only `no_leak_1` defined, only `no_leak_1` called
    - triggers `-Wanalyzer-malloc-leak` compiling `no_leak_1`
      - all the variables now have name like outp_25(D), instead of the actual
        variable names.
      - tells me something happened at location "lto1", about which I know
        nothing, and code for which it does not show.
    - `no_leak_1` runs fine
    - valgrind finds no problems

