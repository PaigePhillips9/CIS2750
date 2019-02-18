#include <regex.h>
#include "GEDCOMparser.h"
#include "ctype.h"

typedef struct {
    //Number at front of line.  Must not be NULL/empty.
    int lineNum;

    int lineDepth;

    //Field in line.  Must not be NULL/empty.
    Field* field;

} FileLine;

typedef struct {

    void *data;

    char *tag;

} Tags;

int validateLine(char* text);

Field* parseField(char *line);

char* printFileLine(void* toBePrinted);

int compareFileLine(const void* first,const void* second);

void deleteFileLine(void* toBeDeleted);

void fakeDelete(void* toBeDeleted);

void deleteFromFront(List *list);

void deleteFromBack(List *list);

char* print(void* toBePrinted);

Field* copyField(Field *field);

bool compareFindIndiTag(const void* first, const void* second);

bool compareFindFamTag(const void* first, const void* second);

bool compareFindTags(const void* first,const void* second);

Individual * copyIndividual(Individual toCopy);

void getDescendentsRecursive(List *descendents, Individual parent);

bool isEvent(char *tag);

bool isFamEvent(char *tag);

void parseIndiName(Individual *indi, char *text);

int validateHeader(char *text);

char *coolerfgets(char *string, int max, FILE *file);

bool compareTagPointer(const void *first, const void *second);

void deleteTags(void *toBeDeleted);

int compareTags(const void *first, const void *second);

char *createTag(char type, int num);

bool compareReferences(const void* first,const void* second);

void getDescendentsRecursiveN(List *descendents, Individual parent, int maxGen, int curGen);

void getAncestorsRecursiveN(List *ancestors, const Individual *child, int maxGen, int curGen);

int compareNames(const void* first, const void * second);

bool checkForCopies(const void *first, const void *second);

int getFamSize(const Individual *indi);

GEDCOMobject* betterJSONtoGEDCOM(const char* str);

Individual* betterJSONtoInd(const char* str);

char* betterIndToJSON(const Individual* ind);

char * betterGEDCOMtoJSON(GEDCOMobject *obj);

char *JSONfileNamesToInfo (char *jsonNames);

char *printHello (char * string);

char* betterIListToJSON(List iList);

char getSex(const Individual * indi);

char * JSONfileToGen( char * fileName, char * givn, char * surn, int max, int type);

bool sameName(const void *first, const void *second);

char *JSONtoGEDCOMfile(char *fileName, char *JSON);

char * JSONtoAddIndi(char *fileName, char *addIndi);

char * fileNameToIndiList(char *fileName);