# Static and Dynamic Libraries under Unix - Prog Sys

This tutorial introduces the different types of binary libraries, which are collections of object files (.o) grouped together in a single entity. It is then left to the linker to find, in the library, the modules containing the functions it needs. This is the resolution of external references, which was mentioned in class.

GNU/Linux (as well as most modern Unixes) knows two types of libraries, static (.a extension) and shared (.so, as in shared object), and three ways of editing links:
- statically,
- dynamically with automatic loading,
- dynamically with explicit loading.

This tutorial explores these three possibilities in the following three sections.

---


## Exercise 1

This script will in the first place generate a list of 5 numbers and assign random numbers to it.

Then we record the start time.

We call the sort function of the imported sort file (either bubble, insertion, merge or quick).

Each sort file has the sort(int list[], int size) function that implements it's sort algorithm.

And at last we print the total time it took.

## Exercise 2

We run `ldd`:
```
linux-vdso.so.1 (0x00007ffdbb75a000)
libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f25e4ff5000)
/lib64/ld-linux-x86-64.so.2 (0x00007f25e522c000)
```

By analyzing the output we can deduct that 3 libraries have been imported:
- linux-vdso.so: A virtual shared object imported by time.h
- libc.so: A shared object lib that provides the fundamental building blocks for C programs to run on the operating system.
- /lib64/ld-linux-x86-64.so: Is the dynamic linker lib

## Exercise 3

We add this to the makefile
```makefile=
STATIC_EXE=$(EXE:.exe=-staticExe.exe)

tri_%-staticExe.exe: main.o %.o timer.o utils.o unused.o
	$(CC) -o $@ $^ --static
```


By running `ldd` we see `not a dynamic executable` that means that all libs have been included in the exe.

```bash
ldd tri_bubble-staticExe.exe 
        not a dynamic executable
```

We can thus see a big increase in size.
```
24K     tri_bubble-basicExe.exe
896K    tri_bubble-staticExe.exe
```


## Exercise 4

We add this to the makefile

```makefile=
STATIC_LIB=$(EXE:.exe=-staticLib.exe)

tri_%-staticLib.exe: main.o timer.o utils.o libTri_%-staticLib.a
	$(CC) -o $@ $^

libTri_%-staticLib.a: %.o unused.o
	ar -r $@ $^
```

We see a small size decrease because of the inclusion of unused.o in a library.
```
 22504 Feb 14 10:23 tri_bubble-basicExe.exe
915768 Feb 14 10:23 tri_bubble-staticExe.exe
 21640 Feb 14 10:23 tri_bubble-staticLib.exe
```

We can see that in the xxx-basicExe.exe the functions of unused.o are included
```bash
nm tri_bubble-basicExe.exe 
⋮
0000000000001946 T bar
0000000000001990 T baz
⋮
00000000000018f2 T foo
⋮
```
But theses functions are not included when the library is linked statically.


## Exercise 5

We add this to the makefile

```makefile=
DYNAMIC_LIB=$(EXE:.exe=-dynamicLib.exe)

tri_%-dynamicLib.exe: main.o timer.o utils.o libTri_%-dynamicLib.so
    $(CC) -o $@ $^

libTri_%-dynamicLib.so: %.o unused.o
    $(CC) -shared -o $@ $^

%.o: %.c
    $(CC) $(CFLAGS) -o $@ -c $<

# Added fpic flag
CFLAGS=-Wall -Wextra -std=gnu99 -g -fpic
```

Dont forget to add `export LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH` to your env vars.

## Exercise 6

We update the makefile

```makefile=
EXE=tri_bubble.exe tri_insertion.exe tri_merge.exe tri_quick.exe
```

## Exercise 7

### load_library.c

```c=
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

void (*sort_func)(int[], int);


void sort(int list[], int size) {
    sort_func(list,size);
};

void load_library(char *library_name) {
    char *error;
    void* handle = dlopen(library_name, RTLD_LAZY);
    if (!handle) {
        fputs(dlerror(), stderr);
        exit(1);
    }

    sort_func = dlsym(handle, "sort");
    if ((error = dlerror()) != NULL)  {
        fputs(error, stderr);
        exit(1);
    }
}
```

We update the makefile to take in account of the run time dynamic library loading
```makefile=
tri.exe: main_dynload.o load_library.o timer.o utils.o
	$(CC) -rdynamic -o $@ $^ -ldl

# Variables de libraries dynamiques
DYN_LIBS=libTri_bubble-dynamicLib.so libTri_quick-dynamicLib.so libTri_merge-dynamicLib.so libTri_insertion-dynamicLib.so


perf: all
    @echo "*** Début des tests des programmes générés"
    @for lib in $(DYN_LIBS); do \
        echo "Lancement avec la lib: $$lib" ;\
        ./tri.exe -p -s 1000 -d $$lib; \
        echo ;\
    done
```


```
22592 Feb 28 10:30 tri_bubble-basicExe.exe
24576 Feb 28 10:45 tri.exe
```
We notice that tri.exe is bigger than tri_bubble-basicExe.exe this is because we import a lib that allows to do linkage at runtime. Indeed `tri_bubble-basicExe.exe` dosen't need thoses libraries as its libraries are linked at compile time.

By doing a nm diff between `tri` and `tri_bubble-basicExe.exe` we notice that dlerror, dlopen and dlsym have been loaded:
```
<                  U dlerror@GLIBC_2.34                    
<                  U dlopen@GLIBC_2.34                     
<                  U dlsym@GLIBC_2.34 
```

## Exercise 8

We wouldn't need to recompile 'tri.exe', it would be enough to make a shared object that would contain the implementation of the new sorting algorithm.

## Exercise 10

ldd /usr/bin/java
```
    linux-vdso.so.1 (0x00007fff710d1000)
    libjli.so => not found
    libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f8af0cad000)
    /lib64/ld-linux-x86-64.so.2 (0x00007f8af0ecd000)
```

ldd /usr/bin/python3
```
    linux-vdso.so.1 (0x00007ffe12b78000)
    libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f893c5fe000)
    libpthread.so.0 => /lib/x86_64-linux-gnu/libpthread.so.0 (0x00007f893c5db000)
    libdl.so.2 => /lib/x86_64-linux-gnu/libdl.so.2 (0x00007f893c5d5000)
    libutil.so.1 => /lib/x86_64-linux-gnu/libutil.so.1 (0x00007f893c5d0000)
    libm.so.6 => /lib/x86_64-linux-gnu/libm.so.6 (0x00007f893c481000)
    libexpat.so.1 => /lib/x86_64-linux-gnu/libexpat.so.1 (0x00007f893c453000)
    libz.so.1 => /lib/x86_64-linux-gnu/libz.so.1 (0x00007f893c435000)
    /lib64/ld-linux-x86-64.so.2 (0x00007f893c819000)
```

From this output, we can conclude that both /usr/bin/java and /usr/bin/python3 rely on a number of shared libraries to function correctly and that they even have shared libraries between them even if they are completely different languages.