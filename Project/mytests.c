/* 
 * File: mytests.c
 * Author: Elijah Sorensen
 * Date: Sat Nov 27 2021 7:35PM MDT
 * Description: Unit tests for ProcEntry object in CS 253
 * myps final project
 */

#include<stdio.h>
#include<stdlib.h>
#include <string.h>

#include "ProcEntry.h"

int testCreateDestroy(void) {

   char testName[] = "Unit Test - Create/Destroy ProcEntry:";
   ProcEntry * testEntry = CreateProcEntry();

   if (testEntry == NULL) {
      fprintf(stderr, "%s failed\n", testName);
      return 1;
   }

   DestroyProcEntry(testEntry);
   fprintf(stdout, "%s passed\n", testName);
   return 0;

}

int testCreateFromFileDestroy(void) {

   char testName[] = "Unit Test - Create/Destroy ProcEntry From File:";
   char statFile[] = "test_data/onyx_proc/1/stat";

   ProcEntry * testEntry = CreateProcEntryFromFile(statFile);

   if (testEntry == NULL){
       fprintf(stderr, "%s failed\n", testName);
       return 1;
   }

   DestroyProcEntry(testEntry);
   fprintf(stdout, "%s passed\n", testName);

   return 0;

}

int testCreateFromFilePrintDestroy(void) {

   char testName[] = "Unit Test - Create/Print/Destroy ProcEntry From File:";
   char statFile[] = "test_data/onyx_proc/1/stat";

   ProcEntry * testEntry = CreateProcEntryFromFile(statFile);

   if (testEntry == NULL){
       fprintf(stderr, "%s failed\n", testName);
       return 1;
   }

   printf("EXPECTED OUTPUT:\n");
   printf("      1       0     S     1     3       1 (systemd)                 test_data/onyx_proc/1/stat\n");

   printf("ACTUAL OUTPUT:\n");
   PrintProcEntry(testEntry);

   DestroyProcEntry(testEntry);
   fprintf(stdout, "%s passed\n", testName);

   return 0;

}

int testCreateFromFileNull(void) {

    char testName[] = "Unit Test - Create ProcEntry From NULL statFile:";

    char statFile[10];
    statFile[0] = '\0';

    ProcEntry * testEntry = CreateProcEntryFromFile(statFile);

    if (testEntry != NULL) {
        fprintf(stderr, "%s failed\n", testName);
        return 1;
    }

    fprintf(stdout, "%s passed\n", testName);

    return 0;

}

int testCreateFromInvalidFile(void) {

    char testName[] = "Unit Test - Create ProcEntry: No such file or directory";

    char invalidStatFile[] = "some/fake/file/path";

    ProcEntry * testEntry = CreateProcEntryFromFile(invalidStatFile);

    if (testEntry != NULL) {
        fprintf(stderr, "%s failed\n", testName);
        return 1;
    }

    fprintf(stdout, "%s passed\n", testName);

    return 0;

}

int testCreateFromFileInvalidFormat(void) {

    char testName[] = "Unit Test - Create ProcEntry From Invalid Format:";
    char statFile[] = "test_data/onyx_proc/invalid/stat";

    ProcEntry * testEntry = CreateProcEntryFromFile(statFile);

    if (testEntry != NULL) {
        PrintProcEntry(testEntry);
        fprintf(stderr, "%s failed\n", testName);
        return 1;
    }

    fprintf(stdout, "%s passed\n", testName);

    return 0;

}

int runAllTests(void) {

    int status = 0;

    while (status == 0) {

        status = testCreateDestroy();
        status = testCreateFromFileDestroy();
        status = testCreateFromFilePrintDestroy();
        status = testCreateFromFileNull();
        status = testCreateFromInvalidFile();
        status = testCreateFromFileInvalidFormat();
        break;

    }

    return status;

}

int main(int argc, char * argv[]) {

   int status = 0;

   if (argc != 3) {

      printf("Usage: %s [-t <test num>]\n", argv[0]);
      exit(1);

   } else {

      int testNum = atoi(argv[2]);

      switch(testNum) {

          case 1:

              status = testCreateDestroy();
              break;

          case 2:

              status = testCreateFromFileDestroy();
              break;

          case 3:
              status = testCreateFromFilePrintDestroy();
              break;

          case 4:

              status = testCreateFromFileNull();
              break;

          case 5:

             status = testCreateFromInvalidFile();
             break;

          case 6:

              status = testCreateFromFileInvalidFormat();
              break;

          case 7:

              status = runAllTests();
              break;

          default:

             printf("Usage: %s [-t <test num>]\n", argv[0]);
             exit(1);

      }

   }

   return status;

}