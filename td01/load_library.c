#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

void (*sort_func)(int[], int);
void* handle;


void sort(int list[], int size) {
    sort_func(list,size);
};

void load_library(char *library_name) {
    char *error;
    handle = dlopen(library_name, RTLD_LAZY);
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

void close_library() {
    dlclose(handle);
    sort_func = NULL;
}