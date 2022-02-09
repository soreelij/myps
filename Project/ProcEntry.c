/*
 * File: ProcEntry.c
 * Author: Elijah Sorensen
 * Date: Wed 8 Dec 2021 11:43 AM MDT
 * Description: ProcEntry object representing the entry
 * of a process for the CS 253 myps final project. Can
 * initialize a ProcEntry with empty fields, or parse
 * a stat file to populate and return a ProcEntry containing
 * its data.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#include "ProcEntry.h"

#define STRING_SIZE 4096

ProcEntry * CreateProcEntry(void) {

    /* Allocate space for new entry in heap and validate */
    ProcEntry * newEntry = (ProcEntry *) malloc(sizeof(ProcEntry));
    if (newEntry == NULL) {
        return NULL;
    }

    /* Initialize all fields to 0 or NULL */
    newEntry->process_id = 0;
    newEntry->parent_process_id = 0;
    newEntry->comm = NULL;
    newEntry->state = '\0';
    newEntry->utime = 0;
    newEntry->stime = 0;
    newEntry->path = NULL;

    return newEntry;

}

ProcEntry * CreateProcEntryFromFile(const char statFile[]) {

    if (statFile == NULL) {
        return NULL;
    }

    /* Check if the stat file exists */
    FILE * file;

    file = fopen(statFile, "r");

    if (file) {

        /* Allocate space for new entry in heap and validate */
        ProcEntry * newEntry = (ProcEntry *) malloc(sizeof(ProcEntry));
        if (newEntry == NULL) {
            return NULL;
        }

        /* Read all 7 values from the input stat file,
        skipping over unneeded ones */
        if (fscanf(file, "%7d", &newEntry->process_id) != 1) {

            free(newEntry);
            fclose(file);
            return NULL;

        }

        char comm[STRING_SIZE];

        if (fscanf(file, "%s", comm) != 1) {

            free(newEntry);
            fclose(file);
            return NULL;

        } else {

            /* Allocate space for comm */
            newEntry->comm = (char *) malloc(sizeof(char) * STRING_SIZE);
            strcpy(newEntry->comm, comm);

            bool commParsed;

            if (comm[strlen(comm) - 1] != ')') {

                commParsed = false;

                while (!commParsed) {

                    if (fscanf(file, "%s", comm) != 1) {

                        free(newEntry);
                        fclose(file);
                        return NULL;

                    } else {

                        strcat(newEntry->comm, comm);

                        if (comm[strlen(comm) - 1] != ')') {
                            commParsed = true;
                        }
                        
                    }

                }

            }

        }

        if (fscanf(file, " %c", &newEntry->state) != 1) {

            free(newEntry->comm);
            free(newEntry);
            fclose(file);
            return NULL;

        }

        if (fscanf(file, "%7d", &newEntry->parent_process_id) != 1) {

            free(newEntry->comm);
            free(newEntry);
            fclose(file);
            return NULL;
            
        }

        /* Skip next 4 integer values */
        for (int i = 0; i < 4; i++) {
            fscanf(file, "%*d");
        }

        /* Skip next unsigned value */
        fscanf(file, "%*u");

        /* Skip next 4 unsigned long values */
        for (int i = 0; i < 4; i++) {
            fscanf(file, "%*u");
        }

        if (fscanf(file, "%5lu", &newEntry->utime) != 1) {
            
            free(newEntry->comm);
            free(newEntry);
            fclose(file);
            return NULL;

        }

        if (fscanf(file, "%5lu", &newEntry->stime) != 1) {

            free(newEntry->comm);
            free(newEntry);
            fclose(file);
            return NULL;

        }

        /* Skip next 4 long values */
        for (int i = 0; i < 4; i++) {
            fscanf(file, "%*d");
        }

        if (fscanf(file, "%7ld", &newEntry->number_of_threads) != 1) {

            free(newEntry->comm);
            free(newEntry);
            fclose(file);
            return NULL;

        }

        /* Allocate space for path and copy from statFile parameter */
        newEntry->path = (char *) malloc(sizeof(char) * STRING_SIZE);
        strcpy(newEntry->path, statFile);

        fclose(file);

        return newEntry;

    } else {
        /* File does not exist */
        return NULL;
    }

}

void DestroyProcEntry(ProcEntry * entry) {

    if (entry != NULL) {

        if (entry->comm) {
            free(entry->comm);
        }

        if (entry->path) {
            free(entry->path);
        }

        free(entry);

    }

}

void PrintProcEntry(ProcEntry *entry) {
    unsigned long int utime = entry->utime / sysconf(_SC_CLK_TCK);
    unsigned long int stime = entry->stime / sysconf(_SC_CLK_TCK);
    fprintf(stdout, "%7d %7d %5c %5lu %5lu %7ld %-25s %-20s\n",
            entry->process_id,
            entry->parent_process_id,
            entry->state,
            utime,
            stime,
            entry->number_of_threads,
            entry->comm,
            entry->path);
}