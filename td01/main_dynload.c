#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <dlfcn.h>

#include "utils.h"
#include "timer.h"
#include "load_library.h"
#include "string.h"
#include "glob.h"

/* ------------------------------------------------------------------------------------
 * Valeurs par défaut des variables globales qui suivent 
 * ------------------------------------------------------------------------------------
 */

#define SIZE 5
#define FALSE 0
#define TRUE 1

/* ------------------------------------------------------------------------------------
 * Variables globales 
 * ------------------------------------------------------------------------------------
*/

static int Size_Array = SIZE;
static int Verbose = FALSE;
static int Perf = FALSE;
char LIBRARY_NAME[50] = "";

/* ------------------------------------------------------------------------------------
 * Prototypes de fonctions définies plus tard 
 * ------------------------------------------------------------------------------------
*/
static void Scan_Args(int argc, char *argv[]);

static void Usage(const char *execname);


void do_job() {
    int list[Size_Array];

    // Initialize a ramdom list of numbers;
    srand(0);
    for (int i = 0; i < Size_Array; i++) {
        list[i] = rand() % 1000;
    }

    if (Verbose) {
        printf("Array to sort:");
        print_list(list, Size_Array);
    }

    struct timespec vartime = timer_start();
    sort(list, Size_Array);
    long time_elapsed_nanos = timer_end(vartime);

    if (Perf) {
        printf("Time taken for sorting (nanoseconds): ");
        printf("%ld\n", time_elapsed_nanos);
    }

    if (Verbose) {
        printf("Sorted array:");
        print_list(list, Size_Array);
    }
}

int main(int argc, char *argv[]) {
    /* Décodage des arguments de la ligne de commande */
    Scan_Args(argc, argv);
    if (strcmp(LIBRARY_NAME, "") != 0) {
        if (Verbose) {
            printf("loading lib: %s\n", LIBRARY_NAME);
        }
        load_library(LIBRARY_NAME);
        do_job();

    } else {
        fprintf(stderr, "No lib specified\n");
        printf("Searching libs\n");
        char **found;
        glob_t gstruct;
        int r;

        r = glob("libTri*.so", GLOB_ERR , NULL, &gstruct);
        /* check for errors */
        if( r!=0 )
        {
            if( r==GLOB_NOMATCH )
                fprintf(stderr,"No matches\n");
            else
                fprintf(stderr,"Some kinda glob error\n");
            exit(1);
        }

        /* success, output found filenames */
        printf("Found %zu libs matches\n", gstruct.gl_pathc);
        found = gstruct.gl_pathv;
        while(*found)
        {
            load_library(*found);
            do_job();
            found++;
            close_library();
        }
    }
}

/* Analyse des arguments 
 * ----------------------
 */
static void Scan_Args(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
                case 'h':
                    Usage(argv[0]);
                    exit(1);
                case 's':
                    Size_Array = atoi(argv[++i]);
                    break;
                case 'v':
                    Verbose = TRUE;
                    break;
                case 'd': {
                    char const *lib = argv[++i];
                    strcpy(LIBRARY_NAME, lib);
                    break;
                }
                case 'p':
                    Perf = TRUE;
                    break;
                default:
                    fprintf(stderr, "Unexpected argument");
                    break;
            }
        }
    }
}

/* Information d'utilisation
 * -------------------------
 */
static void Usage(const char *execname) {
    printf("\nUsage: %s [options]\n", execname);
    printf("\nOptions générales\n"
           "-----------------\n"
           "-h\tce message\n"
           "-s\ttaille du tableau à trier"
           "-v\taffiche plus d'informations"
           "-d\tselectionne la bibliotèque à utiliser (libTri_quick-dynamiLib.so, libTri_bubble-dynamiLib.so, libTri_insertion-dynamiLib.so, libTri_merge-dynamiLib.so)"
           "-p\tsupprime la sotie standard des chiffres");
}