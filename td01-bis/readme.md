# Static and Dynamic Libraries in Win32 - Prog Sys
The goal is to learn how to handle dll's in windows


## Exercise 1
```powershell
dumpbin /SYMBOLS .\StaticLib.lib
```
By listing the symbols in the lib we can find the `PrintStop` function that we have defined.

```
...
034 00000000 SECT4  notype ()    External     | _PrintStop
...
```


## Exercise 2

```powershell
dumpbin /IMPORTS .\Exe_StaticLib.exe
...
File Type: EXECUTABLE IMAGE
  Section contains the following imports:
    VCRUNTIME140D.dll
...
    ucrtbased.dll
...
    KERNEL32.dll
...
```
We can see that `Exe_StaticLib.exe` depends only on 3 imports which are all in C:\Windows\SysWOW64\ thus deleting everything  around the exe doesn't change the outcome of the program

## Exercise 3

To separate static and dynamic libs. We created a new project `DynamicLib` which will generate `DynamicLib.dll` with the same `PrintStop` function

```powershell
dumpbin /EXPORTS .\DynamicLib.dll
...
Dump of file .\DynamicLib.dll
File Type: DLL
  Section contains the following exports for DynamicLib.dll
    ...
    ordinal hint RVA      name
          1    0 00011050 PrintStop = @ILT+75(_PrintStop)
...
```

When we examine `Exe_DynamicLib.exe` we can see that it needs 4 libraries including `DynamicLib.dll` where the `PrintStop` function is stored.

```powershell
dumpbin /IMPORTS .\Exe_DynamicLib.exe
...
Dump of file .\Exe_DynamicLib.exe

File Type: EXECUTABLE IMAGE

  Section contains the following imports:

    DynamicLib.dll
                41B000 Import Address Table
                41B250 Import Name Table
                     0 time date stamp
                     0 Index of first forwarder reference

                    0 PrintStop
    VCRUNTIME140D.dll
...
    ucrtbased.dll
...
    KERNEL32.dll
...
```
Thus, we cannot run the program without `DynamicLib.dll`

## Exercise 4
There are several significant differences in the creation and use of libraries between the Linux and Windows operating systems, as well as some similarities.

- File format: Libraries are stored in different file formats in Linux and Windows. In Linux, libraries are typically stored as .so (Shared Object) files, while in Windows they are stored as .dll (Dynamic Link Library) files.

- Usage: Programs in Linux and Windows use libraries in the same way. They simply call the functions provided by the libraries when they need to use them.

- Location: In Linux, libraries are typically stored in directories such as /lib, /usr/lib, and /usr/local/lib. In Windows, libraries are often stored in the System32 directory or in the same directory as the executable that uses them.

In summary, while libraries work similarly in Linux and Windows, there are significant differences in how they are created, stored, and managed. Developers need to take these differences into account when creating programs that use libraries on either system.