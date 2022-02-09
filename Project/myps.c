/* 
 * Author: Elijah Sorensen
 * Date: Wed 8 Dec 2021 11:43 AM MDT
 * Description: myps implementation of the ps command for 
 * the CS 253 final project. Has the ability to sort and output
 * all currently running processes, or stat files which
 * represent processes given a directory containing properly
 * formatted stat files.
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>

#define UNUSED(x) (void)x

#include "ProcEntry.h"

#define MAX_FILEPATH_SIZE 4096

int FilterFunction(const struct dirent *current);

int noDirentSort(const struct dirent **first, const struct dirent **second);

int CompareByPID(const void * first, const void * second);

int CompareByCMD(const void * first, const void * second);

void PrintUsage(void);

int main(int argc, char *argv[]) {

    /* Define dirent array and size */
    struct dirent ** directories;
    int n;

    /* Define default function pointers */
    int (*filterFunction)(const struct dirent *);
    filterFunction = FilterFunction;

    int (*sortFunction)(const struct dirent **, const struct dirent **);
    sortFunction = noDirentSort;

    int (*compareFunction)(const void * first, const void * second);
    compareFunction = CompareByPID;

    /* Allocate space on the stack for dirPath
     * on the stack and initialize to default
     * ProcEntry location */
    char dirPath[MAX_FILEPATH_SIZE] = "/proc";

    int opt;
    bool zombiesOnly = false;

    while ((opt = getopt(argc, argv, "d:pczh")) != -1) {

        switch (opt) {

            case 'd':

                /* Copy user-specified directory to dirPath */
                strncpy(dirPath, optarg, MAX_FILEPATH_SIZE);
                break;

            case 'p':

                /* Compare ProcEntries by PID */
                compareFunction = CompareByPID;
                break;

            case 'c':

                /* Compare ProcEntries by CMD */
                compareFunction = CompareByCMD;
                break;

            case 'z':

                /* Show only zombie ProcEntries */
                zombiesOnly = true;
                break;

            case 'h':

                /* Print help message */
                PrintUsage();
                exit(0);

            default:

                /* Invalid args/syntax */
                PrintUsage();
                exit(1);

        }

    }

    /* Read directories from provided path */
    n = scandir(dirPath, &directories, filterFunction, sortFunction);

    /* validate directory was opened successfully */
    if (n < 0) {
        perror("scandir: ");
        exit(1);
    }

    /* Create space on the heap for the ProcEntry array */
    ProcEntry ** allEntries = malloc(sizeof(ProcEntry *) * n);

    bool readError = false;

    for (int index = 0; index < n; index++) {

        /* Create string for next directory path */
        char nextDirPath[MAX_FILEPATH_SIZE];

        /* Copy pre-existing path into next */
        strcpy(nextDirPath, dirPath);

        /* Concatenate directory name and stat file */
        strcat(nextDirPath, "/");
        strcat(nextDirPath, directories[index]->d_name);
        strcat(nextDirPath, "/stat");

        nextDirPath[strlen(nextDirPath)] = '\0';

        /* Read stat file from nextDirPath and create ProcEntry */
        ProcEntry * nextEntry = CreateProcEntryFromFile(nextDirPath);

        /* Ensure ProcEntry was created correctly */
        if (nextEntry != NULL) {

            allEntries[index] = nextEntry;

        } else {

            /* Error while reading stat file, abort */
            readError = true;
            break;

        }

    }

    if (readError) {

        /* Free entries to prevent memory leaks */
        for (int index = 0; index < n; index++) {

            free(directories[index]);
            DestroyProcEntry(allEntries[index]);

        }

        /* Free arrays to prevent memory leaks */
        free(allEntries);
        free(directories);

        fprintf(stderr, "Error encountered while reading stat file.\n");
        exit(1);

    }

    qsort(allEntries, n, sizeof(ProcEntry *), compareFunction);

    fprintf(stdout,"%7s %7s %5s %5s %5s %7s %-25s %-20s\n","PID","PPID","STATE","UTIME","STIME","THREADS","CMD","STAT_FILE");

    for (int index = 0; index < n; index++) {

        if (zombiesOnly) {

            if (allEntries[index]->state == 'Z') {

                PrintProcEntry(allEntries[index]);

            }

        } else {

            PrintProcEntry(allEntries[index]);

        }
        
        /* Free entries to prevent memory leaks */
        DestroyProcEntry(allEntries[index]);
        free(directories[index]);

    }

    /* Free arrays to prevent memory leaks */
    free(allEntries);
    free(directories);

}

int FilterFunction(const struct dirent *current) {

    /* Check if current is a directory */
    if (current->d_type == DT_DIR) {

        /* Check if the name of the directory begins with a number */
        if (current->d_name[0] < 58 && current->d_name[0] > 47) {

            /* Current matches the requirements */
            return 1;

        }

    }

    return 0;

}

/* Placeholder sort for scandir()--doesn't actually sort directories */
int noDirentSort(const struct dirent **entryA, const struct dirent **entryB) {

    UNUSED(entryA);
    UNUSED(entryB);

    return 0;

}

/* Compare two ProcEntry structs by their PID */
int CompareByPID(const void * first, const void * second) {

     ProcEntry * f = * (ProcEntry **) first;
     ProcEntry * s = * (ProcEntry **) second;
     int rval = f->process_id - s->process_id;
     return rval;

}

/* Compare two ProcEntry structs by their PID */
int CompareByCMD(const void * first, const void * second) {

    ProcEntry * f = * (ProcEntry **) first;
    ProcEntry * s = * (ProcEntry **) second;

    return (strcmp(f->comm, s->comm));

}

/* Print program usage */
void PrintUsage(void) {

    fprintf(stderr, "Usage: ./myps [-d <path>] [-p] [-c] [-z] [-h]\n"
                    "\t-d <path> Directory containing proc entries (default: /proc)\n"
                    "\t-p \t  Display proc entries sorted by pid (default)\n"
                    "\t-c \t  Display proc entries sorted by command lexicographically\n"
                    "\t-z \t  Display ONLY proc entries in the zombie state \n"
                    "\t-h \t  Display this help message\n");

}
