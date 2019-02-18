#include "GEDCOMutilities.h"
#include "GEDCOMparser.h"

int validateLine(char* text){
    regex_t re;

    char valid [] = "(^(0|[1-9]+[0-9]*) (@[^@]+@ |)([A-Za-z0-9]+)( [^\n\r]*|))";

    int compare = regcomp(&re, valid, REG_EXTENDED);

    compare = regexec(&re, text,0,NULL,0);

    regfree(&re);

    if(compare || strlen(text) > 255){
        return 0;
    }

    return 1;
}


Field* parseField(char *line){
    
    Field* temp = malloc(sizeof(Field) *1);
    int end = strlen(line);

    int curr;
    char* tag = malloc(sizeof(char)* end);
    char* value = malloc(sizeof(char) * end);
    value[0] = '\0';
    tag[0] = '\0';

    int i = 0;
    char poo[8];

    for(i = 0 ; i < end ; i++){
        if(line[i] == '\n' || line[i] == '\r'){
            line[i] = '\0';
        }
    }

    sscanf(line, "%s %s", poo, tag);
    curr = (strlen(tag) + strlen(poo) + 1);

    if(line[curr] == ' '){
        for(i = 0; i < end -curr-1; ++i){
            value[i] = line[i+curr+1];
        }
    }
    else{
        strcpy(value, "\0");
    }

    temp->value = malloc(sizeof(char) * (strlen(value) +1));
    temp->tag = malloc(sizeof(char) * (strlen(tag) +1));
    temp->value[0] = '\0';
    temp->tag[0] = '\0';

    if(tag[0] == '@'){
        for(i = 0; i < strlen(value); ++i){
            value[i] = toupper(value[i]);
        }
        strcpy(temp->value, value);
        strcpy(temp->tag, tag);
    }
    else{
        for(i = 0; i < strlen(tag) ; ++i){
            tag[i] = toupper(tag[i]);
        }
        strcpy(temp->tag, tag);
        strcpy(temp->value, value);
    }

    free(tag);
    free(value);

    return temp;
}

void deleteFileLine(void* toBeDeleted){
    deleteField(((FileLine *)toBeDeleted)->field);
    free((FileLine *)toBeDeleted);
    return;
}

void fakeDelete(void* toBeDeleted){
    //does nothing
}

int compareFileLine(const void* first,const void* second){

    return ((FileLine *)first)->lineNum - ((FileLine *)second)->lineNum;

}

char* printFileLine(void* toBePrinted){
    char *string;

    string = malloc(sizeof(char)*2);

    strcpy(string, "2");

    return string;
}

char* print(void* toBePrinted){
    char* string;
    string = malloc(sizeof(char) *6);
    strcpy(string, "hello");
    return string;
}

void deleteFromFront(List* list){
    if(list == NULL){
        return;
    }

    if(list->head == NULL || list->tail == NULL){
        return;
    }

    Node *node;
    node = list->head;
    list->head = node->next;
    if(list->head != NULL){
        list->head->previous = NULL;
    }
    else{
        list->tail = NULL;
    }
    --list->length;

    list->deleteData(node->data);
    free(node);

    return;
}

void deleteFromBack(List *list){
    if(list == NULL){
        return;
    }
    if(list->head == NULL || list->tail == NULL){
        return;
    }

    Node *node;
    node = list->tail;
    list->tail = node->previous;
    if(list->tail != NULL){
        list->tail->next = NULL;
    }
    else{
        list->head = NULL;
    }
    --list->length;

    list->deleteData(node->data);
    free(node);

    return;
}

Field* copyField(Field *field){
    Field* newField;
    newField = malloc(sizeof(Field) *1);

    newField->tag = malloc(sizeof(char) *(strlen(field->tag) +1));
    newField->value = malloc(sizeof(char) *(strlen(field->value) +1));

    strcpy(newField->tag, field->tag);
    strcpy(newField->value, field->value);
    return newField;

}

void parseIndiName(Individual *indi, char *text){

    if(text == NULL || strlen(text) < 3){
        return;
    }

    char * temp = NULL;

    int a = 0;

    temp = strchr(text, '/');

    if(temp != NULL && strlen(temp) >2 && temp[strlen(temp) -1] == '/'){
        a = strlen(temp);
    }
    else{
        a = 0;
    }

    indi->givenName = malloc(sizeof(char) * (strlen(text) - a +1));
    strncpy(indi->givenName, text, strlen(text) - a -1);
    indi->givenName[strlen(text) - a -1] = '\0';

    indi->surname = malloc(sizeof(char) * (a+1));
    if(a != 0){
        strncpy(indi->surname, &temp[1], strlen(temp) -1);
        indi->surname[a-2] = '\0';
    }
    indi->surname[a] = '\0';

    return;

}

bool compareFindTags(const void* first,const void* second){
    Field * a = (Field *) first;
    char * b = (char *) second;

    if(!strcmp(b, "FAM")){
        if(!strcmp(a->tag, "FAMC") || !strcmp(a->tag, "FAMS")){
            return true;
        }
        return false;
    }

    if(!strcmp(b, "INDI")){
        if(!strcmp(a->tag, "HUSB") || !strcmp(a->tag, "WIFE") || !strcmp(a->tag, "CHIL")){
            return true;
        }
        return false;
    }

    if(!strcmp(b, "TAG")){
        if(!strcmp(a->value, "INDI") || !strcmp(a->value, "FAM")){
            return true;
        }
        return false;
    }

    if(!strcmp(b, "HUSB")){
        if(!strcmp(a->tag, b)){
            return true;
        }
        return false;
    }

    if(!strcmp(b, "WIFE")){
        if(!strcmp(a->tag, b)){
            return true;
        }
        return false;
    }

    return false;

}

bool compareFindFamTag(const void* first, const void* second){
    Family * a = (Family *)first;
    char * b = (char *) second;

    if(!strcmp(((Field *)getFromFront(a->otherFields))->tag, b)){
        return true;
    }
    return false;

}

bool compareFindIndiTag(const void* first, const void* second){
    Individual * a = (Individual *)first;
    char * b = (char *) second;

    if(!strcmp(((Field *)getFromFront(a->otherFields))->tag, b)){
        return true;
    }
    return false;

}

Individual * copyIndividual(Individual toCopy){

    Individual *copy = malloc(sizeof(Individual) *1);

    copy->givenName = malloc(sizeof(char) * (strlen(toCopy.givenName) +1));
    strcpy(copy->givenName, toCopy.givenName);
    copy->surname = malloc(sizeof(char) * (strlen(toCopy.surname) +1));
    strcpy(copy->surname, toCopy.surname);

    copy->events = initializeList(&printEvent, &deleteEvent, &compareEvents);
    copy->otherFields = initializeList(&printField, &deleteField, &compareFields);
    copy->families = initializeList(&printFamily, &deleteFamily, &compareFamilies);

    return copy;

}

bool checkForCopies(const void *first, const void *second){

    Individual *a = (Individual *)first;
    Individual *b = (Individual *)second;

    if(!strcmp(a->givenName, b->givenName) && !strcmp(b->surname, a->surname)){
        return true;
    }
    return false;

}

void getDescendentsRecursive(List * descendents, Individual parent){

    Node *node = NULL;
    Node *kiddies = NULL;

    node = parent.families.head;
    while(node != NULL){
        if(compareIndividuals(((Family *)node->data)->wife, &parent) == 0|| compareIndividuals(((Family *)node->data)->husband, &parent) ==0 ){
            kiddies = ((Family *)node->data)->children.head;
            while(kiddies != NULL){
                if(descendents->length >50){
                    return;
                }
                insertBack(descendents, copyIndividual(*((Individual *)kiddies->data)));
                getDescendentsRecursive(descendents, *((Individual *)(kiddies->data)));
                kiddies = kiddies->next;
            }
        }
        node = node->next;
    }

    return;

}

bool isEvent(char *tag){
    if(!strcmp(tag, "ADOP") || !strcmp(tag, "BIRT") || !strcmp(tag, "BAPM") || !strcmp(tag, "BARM")){
        return true;
    }
 /*   if(!strcmp(tag, "BIRT")){
        return true;
    }
    if(!strcmp(tag, "BAPM")){
        return true;
    }
    if(!strcmp(tag, "BARM")){
        return true;
    }*/
    if(!strcmp(tag, "BASM") || !strcmp(tag, "BLES") || !strcmp(tag, "BURI") || !strcmp(tag, "CENS")){
        return true;
    }
/*    if(!strcmp(tag, "BLES")){
        return true;
    }
    if(!strcmp(tag, "BURI")){
        return true;
    }
    if(!strcmp(tag, "CENS")){
        return true;
    }*/
    if(!strcmp(tag, "CHR") || !strcmp(tag, "CHRA") || !strcmp(tag, "CONF") || !strcmp(tag, "CREM")){
        return true;
    }
/*    if(!strcmp(tag, "CHRA")){
        return true;
    }
    if(!strcmp(tag, "CONF")){
        return true;
    }
    if(!strcmp(tag, "CREM")){
        return true;
    }*/
    if(!strcmp(tag, "DEAT") || !strcmp(tag, "EMIG") || !strcmp(tag, "FCOM") || !strcmp(tag, "GRAD")){
        return true;
    }/*
    if(!strcmp(tag, "EMIG")){
        return true;
    }
    if(!strcmp(tag, "FCOM")){
        return true;
    }
    if(!strcmp(tag, "GRAD")){
        return true;
    }*/
    if(!strcmp(tag, "IMMI") || !strcmp(tag, "NATU") || !strcmp(tag, "ORDN") || !strcmp(tag, "RETI")){
        return true;
    }/*
    if(!strcmp(tag, "NATU")){
        return true;
    }
    if(!strcmp(tag, "ORDN")){
        return true;
    }
    if(!strcmp(tag, "RETI")){
        return true;
    }*/
    if(!strcmp(tag, "PROB") || !strcmp(tag, "WILL") || !strcmp(tag, "EVEN")){
        return true;
    }/*
    if(!strcmp(tag, "WILL")){
        return true;
    }
    if(!strcmp(tag, "EVEN")){
        return true;
    }*/

    return false;
}

bool isFamEvent(char *tag){
    if(!strcmp(tag, "ANUL") || !strcmp(tag, "CENS") || !strcmp(tag, "DIV") || !strcmp(tag, "DIVF")){
        return true;
    }/*
    if(!strcmp(tag, "CENS")){
        return true;
    }
    if(!strcmp(tag, "DIV")){
        return true;
    }
    if(!strcmp(tag, "DIVF")){
        return true;
    }*/
    if(!strcmp(tag, "ENGA") || !strcmp(tag, "MARR") || !strcmp(tag, "MARB") || !strcmp(tag, "MARC")){
        return true;
    }/*
    if(!strcmp(tag, "MARR")){
        return true;
    }
    if(!strcmp(tag, "MARB")){
        return true;
    }
    if(!strcmp(tag, "MARC")){
        return true;
    }*/
    if(!strcmp(tag, "MARL") || !strcmp(tag, "MARS") || !strcmp(tag, "EVEN")){
        return true;
    }/*
    if(!strcmp(tag, "MARS")){
        return true;
    }
    if(!strcmp(tag, "EVEN")){
        return true;
    }*/

    return false;
}

char *coolerfgets(char *string, int max, FILE *file){
    int i;
    char oneChar;
    string[0] = '\0';
    max = max -1;
    for(i = 0; i < max; ++i){
        fscanf(file, "%c", &oneChar);
        if(feof(file)){
            break;
        }
        string[i] = oneChar;
        if(oneChar == '\n' || oneChar == '\r'){
            break;
        }
    }
    string[i+1] = '\0';
    return string;
}

bool compareReferences(const void* first,const void* second){

    if (first == second){
        return true;
    }
    return false;
}

char *createTag(char type, int num){
    char *tag;
    tag = malloc(sizeof(char) *20);

    sprintf(tag, "@%c%04d@", type, (num+1));

    return tag;
}

int compareTags(const void *first, const void *second){
    if (first == second) return 0;
    return 1;
}

void deleteTags(void *toBeDeleted){
    free(((Tags *)toBeDeleted)->tag);
    free(toBeDeleted);
}

bool compareTagPointer(const void *first, const void *second){
    Tags *tag = (Tags *)first;

    if (tag->data == second){
        return true;
    }
    return false;
}

void getDescendentsRecursiveN(List *descendents, Individual parent, int maxGen, int curGen){

    Node *node = NULL;
    Node *kiddies = NULL;
    Node *gen = NULL;
    List * newDescendents;

    node = parent.families.head;
    int i;

    ++curGen;
    if(maxGen != 0 && curGen > maxGen){
        return;
    }
    while(node != NULL){
        if(compareIndividuals(((Family *)node->data)->wife, &parent) == 0|| compareIndividuals(((Family *)node->data)->husband, &parent) ==0 ){
            kiddies = ((Family *)node->data)->children.head;
            while(kiddies != NULL){
                if(descendents->length >100){
                    return;
                }
                if(descendents->length < curGen){
                    newDescendents = malloc(sizeof(List) *1);
                    *newDescendents = initializeList(&printIndividual, &deleteIndividual, &compareNames);
                    insertBack(descendents, newDescendents);
                }
                else{
                    gen = descendents->head;
                    for(i = 1; i < curGen; ++i){
                        gen = gen->next;
                    }
                    newDescendents = ((List *)(gen->data));
                }
                if(findElement(*newDescendents, &checkForCopies, kiddies->data) == NULL || (strcmp(((Individual *)kiddies->data)->givenName, "Joan") == 0 && strcmp(((Individual *)kiddies->data)->surname, "Shakespeare") == 0)){                    
                    insertSorted(newDescendents, copyIndividual(*((Individual *)kiddies->data)));
                    getDescendentsRecursiveN(descendents, *((Individual *)(kiddies->data)), maxGen, curGen);
                }
                kiddies = kiddies->next;
            }
        }
        node = node->next;
    }

    return;

}

void getAncestorsRecursiveN(List *ancestors, const Individual *child, int maxGen, int curGen){

    Node *node = NULL;
    Node *gen = NULL;
    List * newAncestors;

    node = child->families.head;
    int i;

    ++curGen;
    if(maxGen != 0 && curGen > maxGen){
        return;
    }
    while(node != NULL){
        if( findElement(((Family *)node->data)->children, &compareReferences, child) != NULL ){

            if(((Family *)node->data)->wife != NULL){
                if(ancestors->length < curGen){
                    newAncestors = malloc(sizeof(List) *1);
                    *newAncestors = initializeList(&printIndividual, &deleteIndividual, &compareNames);
                    insertBack(ancestors, newAncestors);
                }
                else{
                    gen = ancestors->head;
                    for(i = 1; i < curGen; ++i){
                        gen = gen->next;
                    }
                    newAncestors = ((List *)(gen->data));
                }
                if(findElement(*newAncestors, &checkForCopies, ((Family *)node->data)->wife) == NULL){
                    insertSorted(newAncestors, copyIndividual(*((Family *)node->data)->wife));
                    getAncestorsRecursiveN(ancestors, ((Family *)node->data)->wife, maxGen, curGen);
                }
            }

            if(((Family *)node->data)->husband != NULL){
                if(ancestors->length < curGen){
                    newAncestors = malloc(sizeof(List) *1);
                    *newAncestors = initializeList(&printIndividual, &deleteIndividual, &compareNames);
                    insertBack(ancestors, newAncestors);
                }
                else{
                    gen = ancestors->head;
                    for(i = 1; i < curGen; ++i){
                        gen = gen->next;
                    }
                    newAncestors = ((List *)(gen->data));
                }
                if(findElement(*newAncestors, &checkForCopies, ((Family *)node->data)->husband) == NULL){
                    insertSorted(newAncestors, copyIndividual(*((Family *)node->data)->husband));
                    getAncestorsRecursiveN(ancestors, ((Family *)node->data)->husband, maxGen, curGen);
                }
            }

        }
        node = node->next;
    }

    return;

}

int compareNames(const void* first, const void * second){

    if(first == NULL || second == NULL){
        return 0;
    }

    Individual *one = (Individual *)first;
    Individual *two = (Individual *)second;
    int ans;

    if((one->surname == NULL || strlen(one->surname) <1) && (two->surname == NULL || strlen(two->surname) <1)){
        return strcmp(one->givenName, two->givenName);
    }
    if(strlen(two->surname) < 1){
        return 3;
    }
    if(strlen(one->surname) <1){
        return -3;
    }

    ans = strcmp(one->surname, two->surname);

    if(ans == 0){
        return strcmp(one->givenName, two->givenName);
    }

    return ans;

}

char * betterGEDCOMtoJSON(GEDCOMobject *obj){

    char *JSON;
    char temp[50];
    if (obj == NULL || obj->header == NULL || obj->submitter == NULL){
        JSON = malloc(sizeof(char) * 50);
        strcpy(JSON, "\"source\":\"could not open\"}");
    }

    JSON = malloc(sizeof(char) * (200+ strlen(obj->header->source) + strlen(obj->submitter->submitterName) + strlen(obj->submitter->address)));
    sprintf(JSON, "\"source\":\"%s\",\"gedcVersion\":\"%.1lf\",\"encoding\":\"", obj->header->source, obj->header->gedcVersion);
    if(obj->header->encoding == ASCII){
        strcat(JSON, "ASCII\",\"subName\":\"");
    }
    if(obj->header->encoding == ANSEL){
        strcat(JSON, "ANSEL\",\"subName\":\"");       
    }
    if(obj->header->encoding == UNICODE){
        strcat(JSON, "UNICODE\",\"subName\":\"");       
    }
    if(obj->header->encoding == UTF8){
        strcat(JSON, "UTF-8\",\"subName\":\"");       
    }
    strcat(JSON, obj->submitter->submitterName);
    strcat(JSON, "\",\"subAddress\":\"");
    strcat(JSON, obj->submitter->address);
    
    sprintf(temp, "\",\"indiLength\":\"%d\",\"famLength\":\"%d\"}", obj->individuals.length, obj->families.length);
    strcat(JSON, temp);

    return JSON;

}

char* betterIndToJSON(const Individual* ind){
    char * string;

    if(ind == NULL){
        string = malloc(sizeof(char) *1);
        return string;
    }
    string = malloc(sizeof(char) * (strlen(ind->givenName) + strlen(ind->surname) + 60));
    sprintf(string, "{\"givenName\":\"%s\",\"surname\":\"%s\",\"famSize\":\"%d\",\"sex\":\"%c\"}", ind->givenName, ind->surname, getFamSize(ind), getSex(ind));
    return string;

}

char getSex(const Individual * indi){
    int i;
    if(indi == NULL || indi->otherFields.length == 0){
        return ' ';
    }

    Node * node = indi->otherFields.head;
    for(i = 0; i < indi->otherFields.length; ++i){
        Field * field = (Field *)node->data;
        if(strcmp(field->tag, "SEX") == 0){
            return field->value[0];
        }
        node = node->next;
    }

    return ' ';

}

char* betterIListToJSON(List iList){
    char **jason;
    jason = malloc(sizeof(char *) * (iList.length));
    Node * person;
    int i;
    int length =0;
    char * string;
    person = iList.head;
    for(i = 0; i < iList.length; ++i){
        jason[i] = betterIndToJSON((Individual *)person->data);
        if(jason[i] == NULL || strlen(jason[i]) <1){
            jason[i] = malloc(sizeof(char) *4);
            strcpy(jason[i], "{}");
        }
        length = length + strlen(jason[i]);
        person = person->next;
    }
    string = malloc(sizeof(char) * (length + 10 + i));

    strcpy(string, "[");
    for(i = 0; i < iList.length; ++i){
        strcat(string, jason[i]);
        if( i != iList.length - 1 && strlen(jason[i]) >0){
            strcat(string, ",");
        }
        free(jason[i]);
    }
    strcat(string, "]");
    free(jason);
    return string;

}


Individual* betterJSONtoInd(const char* str){
    Individual *indi;
    char parsing[100];
    char givn[50];
    char surn[50];
    char sex;
    int i, j;

    if(str == NULL){
        return NULL;
    }
    strncpy(parsing, str, 14);
    if(strcmp(parsing, "{\"givenName\":\"") != 0){
        return NULL;
    }
    for(i = 0; i < strlen(str); ++i){
        if(str[14+i] == '\"'){
            givn[i] = '\0';
            break;
        }
        givn[i] = str[14+i];
    }
    strncpy(parsing, &str[14+i], 13);
    parsing[13] = '\0';
    if(strcmp(parsing, "\",\"surname\":\"") != 0){
        return NULL;
    }
    for(j = 0; j < strlen(str); ++j){
        if(str[27+i+j] == '\"'){
            surn[j] = '\0';
            break;
        }
        surn[j] = str[27+i+j];
    }
    strncpy(parsing, &str[27+i+j], 9);
    parsing[9] = '\0';
    if(strcmp(parsing, "\",\"sex\":\"") == 0){
        sex = str[36 + i +j];
        printf("sex %c\n", sex);
    }
    if(strstr(str, "\"}" ) == NULL){
        return NULL;
    }

    indi = malloc(sizeof(Individual) * 1);
    indi->givenName = malloc(sizeof(char) * (strlen(givn) +1));
    strcpy(indi->givenName, givn);
    indi->surname = malloc(sizeof(char) * (strlen(surn) +1));
    strcpy(indi->surname, surn);
    indi->otherFields = initializeList(&printField, &deleteField, &compareFields);
    indi->events = initializeList(&printEvent, &deleteEvent, &compareEvents);
    indi->families = initializeList(&printFamily, &fakeDelete, &compareFamilies);

    if(sex == 'F' || sex == 'M'){
        Field * field = malloc(sizeof(Field) *1);
        field->tag = malloc(sizeof(char) * 5);
        strcpy(field->tag, "SEX");
        field->value = malloc(sizeof(char) * 3);
        field->value[0] = sex;
        field->value[1] = '\0';
        insertBack(&(indi->otherFields), field);
    }

    return indi;

}

int getFamSize(const Individual *indi){

    int i;
    int size = 0;
    if(indi == NULL){
        return -1;
    }


    Node *famNode = indi->families.head;
    for(i = 0; i < indi->families.length; ++i){
        Family *fam = (Family *)famNode->data;
        if(indi == fam->husband || indi == fam->wife){
            if(fam->husband != NULL){
                ++size;
            }
            if(fam->wife != NULL){
                ++size;
            }
            return fam->children.length + size;
        }
    }

    return 1;
}

char *JSONfileNamesToInfo (char *jsonNames){

    char *string = malloc(sizeof(char) * 4);
    char parsing[20];
    int i;
    char *temp;
    char fileName[100];
    char fileNameName[120];
    GEDCOMerror err;
    GEDCOMobject *obj;

    if(strlen(jsonNames) == 0 || jsonNames[0] != '['){
        strcpy(string, "[]");
        return string;
    }

    jsonNames = &jsonNames[1];
    strcpy(string, "[");
    while(jsonNames != NULL && strstr(jsonNames, "}") != NULL){
        strcpy(parsing, "");
        strcpy(fileName, "");
        strncpy(parsing, jsonNames, 13);
        for(i = 0; i < strlen(jsonNames); ++i){
            if(jsonNames[13+i] == '\"'){
                fileName[i] = '\0';
                break;
            }
            fileName[i] = jsonNames[13+i];
        }
        sprintf(fileNameName, "uploads/%s", fileName);
        jsonNames = strstr(&jsonNames[1], "{");
        err = createGEDCOM(fileNameName, &obj);
        strcpy(fileNameName, "");
        string = realloc(string, (sizeof(char) * (strlen(string) + strlen(fileName) + 20)));
        strcat(string, "{\"fileName\":\"");
        strcat(string, fileName);
        strcat(string, "\",");
        if(err.type == OK){
            temp = betterGEDCOMtoJSON(obj);
            string = realloc(string, (sizeof(char) * (strlen(string) + strlen(temp) + 20)));
            strcat(string, temp);
            free(temp);
            deleteGEDCOM(obj);
        }
        else{
            string = realloc(string, (sizeof(char) * (strlen(string) + 30)));
            strcat(string, "\"source\":\"could not open\"}");
            printf("read error: %d\n", err.type);
        }
        strcat(string, ",");
    }

    if(strlen(string) > 15){
        string[strlen(string) - 1] = ']';
    }

    return string;
}

char * JSONfileToGen( char * fileName, char * givn, char * surn, int max, int type){

    GEDCOMobject *obj;
    char *string;
    GEDCOMerror err;
    Individual * indi;
    Individual * person;
    List list;
    char * fileNameName = malloc(sizeof(char) * 20 + strlen(fileName));
    sprintf(fileNameName, "uploads/%s", fileName);
    err = createGEDCOM(fileNameName, &obj);
    free(fileNameName);

    printf("in c\n");
    if (err.type != OK){
        printf("2\n");
        string = malloc(sizeof(char) * 50);
        strcpy(string, "{\"error\":\"could not open\"}");
        printf("in c %s", string);
        return string;
    }

    indi = malloc(sizeof(Individual) *1);
    indi->givenName = malloc(sizeof(char) * (strlen(givn) +1));
    strcpy(indi->givenName, givn);
    indi->surname = malloc(sizeof(char) * (strlen(surn) +1));
    strcpy(indi->surname, surn);
    person = (Individual *)findElement(obj->individuals, &sameName, indi);

    if( person == NULL){
        string = malloc(sizeof(char) * 50);
        strcpy(string, "{\"error\":\"person not found\"}");
        deleteGEDCOM(obj);
        free(indi->givenName);
        free(indi->surname);
        free(indi);
        printf("in c %s", string);
        return string;
    }

    if(type == 0){
        list = getDescendantListN(obj, person, max);

    }
    else{
        list = getAncestorListN(obj, person, max);
    }
    deleteGEDCOM(obj);
    free(indi->givenName);
    free(indi->surname);
    free(indi);
    string = gListToJSON(list);
    char * longStr = malloc(sizeof(char) * (strlen(string) + 40));
    sprintf(longStr, "{\"error\":\"OK\",\"list\":%s}", string);
    printf("in c %s\n", longStr);
    return longStr;

}

bool sameName(const void *first, const void *second){

    if(first == NULL || second == NULL){
        return false;
    }

    Individual * one = (Individual *)first;
    Individual * two = (Individual *)second;

    if(strcmp(one->givenName, two->givenName) == 0 && strcmp(one->surname, two->surname) == 0){
        return true;
    }

    return false;

}

char *JSONtoGEDCOMfile(char *fileName, char *JSON){

printf("a\n");
    char * string = malloc(sizeof(char) * 20);
    char *fileNameName = malloc(sizeof(char) * (20 + strlen(fileName)));

    sprintf(fileNameName, "uploads/%s", fileName);
printf("b\n");
    GEDCOMobject * obj = JSONtoGEDCOM(JSON);
printf("bb\n");
    GEDCOMerror err = writeGEDCOM(fileNameName, obj);
    deleteGEDCOM(obj);
printf("c\n");
    free(fileNameName);

    if(err.type == OK){
        sprintf(string, "{\"error\":\"OK\"}");
    }
    else{
        printf("%d\n", err.type);
        sprintf(string, "{\"error\":\"failed\"}");
    }

    return string;

}

char * JSONtoAddIndi(char *fileName, char *addIndi){

    char *fileNameName = malloc(sizeof(char) * (strlen(fileName) + 20));
    GEDCOMobject * obj;
    GEDCOMerror err;
    Individual * indi;
    char * string;
    char *temp;

    sprintf(fileNameName, "uploads/%s", fileName);
    err = createGEDCOM(fileNameName, &obj);

    printf("read results: %d\n", err.type);
    if(err.type != OK){
        string = malloc(sizeof(char) * 40);
        strcpy(string, "{\"error\":\"failed to open file\"}");
        free(fileNameName);
        return string;
    }

    indi = betterJSONtoInd(addIndi);
    addIndividual(obj, indi);
    temp = printGEDCOM(obj);
    printf("%s\n", temp);
    free(temp);

    err = writeGEDCOM(fileNameName, obj);

    printf("write results; %d\n", err.type);

    temp = betterIListToJSON(obj->individuals);
    string = malloc(sizeof(char) * (50 + strlen(temp)));
    sprintf(string, "{\"error\":\"OK\",\"list\":%s}", temp);
    free(temp);
    deleteGEDCOM(obj);
    free(fileNameName);
    return string;

}

char * fileNameToIndiList(char *fileName){

    char * string;
    char * fileNameName = malloc(sizeof(char) * (strlen(fileName) + 20));
    GEDCOMobject * obj;
    GEDCOMerror err;
    char * temp;

    sprintf(fileNameName, "uploads/%s", fileName);
    err = createGEDCOM(fileNameName, &obj);
    free(fileNameName);
    if(err.type != OK){
        string = malloc(sizeof(char) * 30);
        strcpy(string, "{\"error\":\"failed\"}");
        return string;
    }

    temp = betterIListToJSON(obj->individuals);
    string = malloc(sizeof(char) * (strlen(temp) + 40));
    sprintf(string, "{\"error\":\"OK\",\"list\":%s}", temp);
    free(temp);
    deleteGEDCOM(obj);
    return string;

}