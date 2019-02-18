#include "GEDCOMparser.h"
#include "GEDCOMutilities.h"
#include "LinkedListAPI.h"
#include "ctype.h"

/** Function to create a GEDCOM object based on the contents of an GEDCOM file.
 *@pre File name cannot be an empty string or NULL.  File name must have the .ged extension.
 File represented by this name must exist and must be readable.
 *@post Either:
 A valid GEDCOM has been created, its address was stored in the variable obj, and OK was returned
 or
 An error occurred, the GEDCOM was not created, all temporary memory was freed, obj was set to NULL, and the
 appropriate error code was returned
 *@return the error code indicating success or the error encountered when parsing the GEDCOM
 *@param fileName - a string containing the name of the GEDCOM file
 *@param a double pointer to a GEDCOMobject struct that needs to be allocated
 **/
GEDCOMerror createGEDCOM(char* fileName, GEDCOMobject** obj){

    int count = 0;
    FileLine* newLine = NULL;
    FileLine* lineo = NULL;
    List families;
    List individuals;
    Header* header = NULL;
    Submitter* submitter = NULL;
    GEDCOMerror err;
    int spot = 0;
    Field *temp = NULL;
    int lineValue;
    int i =0;
    List read;
    List readLines;
    spot = 0; // 0 while in header;
    char * submTag = NULL;
    Individual *indi = NULL;
    Family *fam = NULL;
    Node * iterator = NULL;
    int eventLine = -1;
    Event *event = NULL;
    char line[261];
    int sourceFound = 0;

    FILE * file = NULL;
    err.line = 1;

    if(fileName == NULL || strlen(fileName) == 0){
        err.line = -1;
        err.type = INV_FILE;
        return err;
    }
    if(strstr(fileName, ".ged") == NULL){
        err.line = -1;
        err.type = INV_FILE;
        return err;
    }

    file = fopen(fileName, "r");

    if(file == NULL){
        err.line = -1;
        err.type = INV_FILE;
        return err;
    }
    readLines = initializeList(&printFileLine, &fakeDelete, &compareFileLine);
    while(!feof(file)){
        strcpy(line, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0");
        coolerfgets(line, 260, file);
        if((line[0] != '\r' && line[0] != '\n' && line[0] != '\0') && strlen(line) > 1) {
            ++count;
            if (validateLine(line) == 0) {
                err.line = count;
                err.type = INV_GEDCOM;
                printf("-2 (%s)\n", line);
                if (strlen(line) > 255) {
                    err.type = INV_RECORD;
                }
                clearList(&readLines);
                fclose(file);
                *obj = NULL;
                return err;
            }
            lineValue = line[0] - '0';
            temp = parseField(line);
            if (count == 1) {
                if (lineValue != 0) {
                    err.type = INV_HEADER;
                }
                if (strcmp(temp->tag, "HEAD") != 0) {
                    err.type = INV_GEDCOM;
                    printf("-1\n");
                }
                if (lineValue != 0 || strcmp(temp->tag, "HEAD") != 0) {
                    err.line = -1;
                    clearList(&readLines);
                    free(temp);
                    fclose(file);
                    *obj = NULL;
                    return err;
                }
            }
            newLine = malloc(sizeof(FileLine) * 1);
            newLine->field = temp;
            newLine->lineNum = count;
            newLine->lineDepth = lineValue;
            insertBack(&readLines, newLine);
        }
    }
    fclose(file);

    if(strcmp(((FileLine *)getFromBack(readLines))->field->tag, "TRLR") != 0){
        err.line = -1;
        err.type = INV_GEDCOM;
        printf("%s\n", ((FileLine *)getFromBack(readLines))->field->tag );
        clearList(&readLines);
        return err;
    }

    families = initializeList(&printFamily, &deleteFamily, &compareFamilies);
    individuals = initializeList(&printIndividual, &deleteIndividual, &compareIndividuals);
    read = initializeList(&printFileLine, &deleteFileLine, &compareFileLine);
    header = malloc(sizeof(Header) *1);
    header->gedcVersion = -1.0;
    header->submitter = NULL;
    header->encoding = -1;
    header->otherFields = initializeList(&printField, &deleteField, &compareFields);
    spot = 0;
    insertBack(&read, getFromFront(readLines));
    deleteFromFront(&readLines);
    while(readLines.length != 0){
        lineo = (FileLine *)getFromFront(readLines);
        if(((FileLine *)getFromFront(readLines))->lineDepth - ((FileLine *)getFromBack(read))->lineDepth > 1){
            err.line = ((FileLine *)getFromFront(readLines))->lineNum;
            err.type = INV_RECORD;
            clearList(&families);
            clearList(&individuals);
            free(header);
            if(header->submitter != NULL){
                free(submitter);
            }
            clearList(&header->otherFields);
            clearList(&read);
            clearList(&readLines);
            *obj = NULL;
            return err;
        }
        while(read.length != 0 && ((FileLine *)getFromFront(readLines))->lineDepth <= ((FileLine *)getFromBack(read))->lineDepth){
            deleteFromBack(&read);
        }
        deleteFromFront(&readLines);
        insertBack(&read, lineo);
        if(read.length == 1){
            if(spot == 0){
                if(sourceFound == 0 || header->gedcVersion == -1 || submTag == NULL || header->encoding == -1){
                    err.line = -1;
                    err.type = INV_HEADER;
                    clearList(&families);
                    clearList(&individuals);
                    clearList(&read);
                    if(header->submitter != NULL){
                        free(submitter);
                    }
                    clearList(&header->otherFields);
                    free(header);
                    clearList(&readLines);
                    *obj = NULL;
                    return err;
                }
            }
            if(spot == 3){
                if(strlen(submitter->submitterName) == 0){
                    err.line = lineo->lineNum -1;
                    err.type = INV_GEDCOM;
                    clearList(&families);
                    clearList(&individuals);
                    clearList(&read);
                    if(header->submitter != NULL){
                        free(submitter);
                    }
                    clearList(&header->otherFields);
                    free(header);
                    clearList(&readLines);
                    *obj = NULL;
                    return err;
                }
            }
            if(!strcmp(lineo->field->value, "INDI")){
                indi = malloc(sizeof(Individual) *1);
                indi->otherFields = initializeList(&printField, &deleteField, &compareFields);
                indi->families = initializeList(&printFamily, &fakeDelete, &compareFamilies);
                indi->events = initializeList(&printEvent, &deleteEvent, &compareEvents);
                insertBack(&individuals, indi);
                spot = 1;
                indi->givenName = NULL;
                indi->surname = NULL;
                eventLine = -1;
            }
            else if(!strcmp(lineo->field->value, "FAM")){
                fam = malloc(sizeof(Family) *1);
                fam->otherFields = initializeList(&printField, &deleteField, &compareFields);
                fam->children = initializeList(&printIndividual, &fakeDelete, &compareIndividuals);
                fam->events = initializeList(&printEvent, &deleteEvent, &compareEvents);
                insertBack(&families, fam);
                spot = 2;
                fam->wife = NULL;
                fam->husband = NULL;
                eventLine = -1;
            }
            else if(!strcmp(lineo->field->value, "SUBM") && !strcmp(lineo->field->tag, submTag) && header->submitter == NULL){
                header->submitter = submitter;
                strcpy(submitter->address, submTag);
                spot = 3;
            }
            else if(!strcmp(lineo->field->tag, "SUBM") && !strcmp(lineo->field->value, "") && !strcmp(submTag, "")){
                header->submitter = submitter;
                strcpy(submitter->address, submTag);
                spot = 3;
            }
            else{
                spot = 4;
            }

        }

        //HEADER PARSING
        if(spot == 0){
            if(lineo->lineDepth == 1){
                if(!strcmp(lineo->field->tag, "SOUR") && lineo->lineDepth == 1){
                    if(strlen(lineo->field->value) > 248 || sourceFound == 1){
                        err.line = lineo->lineNum;
                        err.type = INV_HEADER;
                        clearList(&families);
                        clearList(&individuals);
                        clearList(&read);
                        if(header->submitter != NULL){
                            free(submitter);
                        }
                        clearList(&header->otherFields);
                        free(header);
                        clearList(&readLines);
                        *obj = NULL;
                        return err;
                    }
                    strncpy(header->source, lineo->field->value, 248);
                    sourceFound = 1;
                }
                else if(!strcmp(lineo->field->tag, "CHAR")){
                    if(!strcmp(lineo->field->value, "ANSEL") && header->encoding == -1){
                        header->encoding = ANSEL;
                    }
                    else if(!strcmp(lineo->field->value, "UTF-8") && header->encoding == -1){
                        header->encoding = UTF8;
                    }
                    else if(!strcmp(lineo->field->value, "UNICODE") && header->encoding == -1){
                        header->encoding = UNICODE;
                    }
                    else if(!strcmp(lineo->field->value, "ASCII") && header->encoding == -1){
                        header->encoding = ASCII;
                    }
                    else{
                        err.line = lineo->lineNum;
                        err.type = INV_HEADER;
                        clearList(&families);
                        clearList(&individuals);
                        clearList(&read);
                        if(header->submitter != NULL){
                            free(submitter);
                        }
                        clearList(&header->otherFields);
                        free(header);
                        clearList(&readLines);
                        *obj = NULL;
                        return err;
                    }
                }
                else if(!strcmp(lineo->field->tag, "SUBM")){
                    if(submTag != NULL){
                        err.line = lineo->lineNum;
                        err.type = INV_HEADER;
                        clearList(&families);
                        clearList(&individuals);
                        clearList(&read);
                        if(header->submitter != NULL){
                            free(submitter);
                        }
                        clearList(&header->otherFields);
                        free(header);
                        clearList(&readLines);
                        *obj = NULL;
                        return err;
                    }
                    submTag = malloc(sizeof(char) *(strlen(lineo->field->value) +1));
                    strcpy(submTag, lineo->field->value);
                    submitter = malloc(sizeof(Submitter) *1 + (sizeof(char) * (strlen(submTag) +1)));
                    submitter->otherFields = initializeList(&printField, &deleteField, &compareFields);
                }
                else{
                    insertBack(&header->otherFields, copyField(lineo->field));
                }
            }
            else if(lineo->lineDepth == 2){
                if(!strcmp(((FileLine *)read.tail->previous->data)->field->tag, "GEDC") && !strcmp(lineo->field->tag, "VERS")){
                    if(header->gedcVersion != -1){
                        err.line = lineo->lineNum;
                        err.type = INV_HEADER;
                        clearList(&families);
                        clearList(&individuals);
                        clearList(&read);
                        if(header->submitter != NULL){
                            free(submitter);
                        }
                        clearList(&header->otherFields);
                        free(header);
                        clearList(&readLines);
                        *obj = NULL;
                        return err;
                    }
                    header->gedcVersion = atof(lineo->field->value);
                    if(header->gedcVersion == 0){
                        err.line = lineo->lineNum;
                        err.type = INV_HEADER;
                        clearList(&families);
                        clearList(&individuals);
                        clearList(&read);
                        if(header->submitter != NULL){
                            free(submitter);
                        }
                        clearList(&header->otherFields);
                        free(header);
                        clearList(&readLines);
                        *obj = NULL;
                        return err;
                    }
                }
                else{
                    insertBack(&header->otherFields, copyField(lineo->field));
                }
            }
            else{
                insertBack(&header->otherFields, copyField(lineo->field));
            }
        }
        //individual parsing
        if(spot == 1){
            if(lineo->lineDepth <= eventLine){
                if(event->date == NULL){
                    event->date = malloc(sizeof(char));
                    strcpy(event->date, "");
                }
                if(event->place == NULL){
                    event->place = malloc(sizeof(char));
                    strcpy(event->place, "");
                }
                eventLine = -1;
            }
            if(eventLine != -1){
                if(!strcmp(lineo->field->tag, "DATE")){
                    if(event->date != NULL){
                        err.line = lineo->lineNum;
                        err.type = INV_RECORD;
                        clearList(&families);
                        clearList(&individuals);
                        clearList(&read);
                        if(header->submitter != NULL){
                            free(submitter);
                        }
                        clearList(&header->otherFields);
                        free(header);
                        clearList(&readLines);
                        *obj = NULL;
                        return err;
                    }
                    event->date = malloc(sizeof(char) *(strlen(lineo->field->value) +1));
                    strcpy(event->date, lineo->field->value);
                }
                else if(!strcmp(lineo->field->tag, "PLAC")){
                    if(event->place != NULL){
                        err.line = lineo->lineNum;
                        err.type = INV_RECORD;
                        clearList(&families);
                        clearList(&individuals);
                        clearList(&read);
                        if(header->submitter != NULL){
                            free(submitter);
                        }
                        clearList(&header->otherFields);
                        free(header);
                        clearList(&readLines);
                        *obj = NULL;
                        return err;
                    }
                    event->place = malloc(sizeof(char) *(strlen(lineo->field->value) +1));
                    strcpy(event->place, lineo->field->value);
                }
                else{
                    insertBack(&event->otherFields, copyField(lineo->field));
                }
            }
            else if(isEvent(lineo->field->tag)){
                event = malloc(sizeof(Event) *1);
                insertBack(&indi->events, event);
                strcpy(event->type, lineo->field->tag);
                eventLine = lineo->lineDepth;
                event->date = NULL;
                event->place = NULL;
                event->otherFields = initializeList(&printField, &deleteField, &compareFields);
            }
            else if(!strcmp(lineo->field->tag, "NAME") && indi->givenName == NULL && indi->surname == NULL){
                parseIndiName(indi, lineo->field->value);
            }
            else if(!strcmp(lineo->field->tag, "GIVN") && indi->givenName == NULL){
                indi->givenName = malloc(sizeof(char) *(strlen(lineo->field->value)+1));
                strcpy(indi->givenName, lineo->field->value);
            }
            else if(!strcmp(lineo->field->tag, "SURN") && indi->surname == NULL){
                indi->surname = malloc(sizeof(char) *(strlen(lineo->field->value)+1));
                strcpy(indi->surname, lineo->field->value);
            }
            else{
                insertBack(&indi->otherFields, copyField(lineo->field));
            }
        }
        //family parsing
        if(spot == 2){
            if(lineo->lineDepth <= eventLine){
                if(event->date == NULL){
                    event->date = malloc(sizeof(char));
                    strcpy(event->date, "");
                }
                if(event->place == NULL){
                    event->place = malloc(sizeof(char));
                    strcpy(event->place, "");
                }
                eventLine = -1;
            }
            if(eventLine != -1){
                if(!strcmp(lineo->field->tag, "DATE")){
                    if(event->date != NULL){
                        err.line = lineo->lineNum;
                        err.type = INV_RECORD;
                        clearList(&families);
                        clearList(&individuals);
                        clearList(&read);
                        if(header->submitter != NULL){
                            free(submitter);
                        }
                        clearList(&header->otherFields);
                        free(header);
                        clearList(&readLines);
                        *obj = NULL;
                        return err;
                    }
                    event->date = malloc(sizeof(char) *(strlen(lineo->field->value) +1));
                    strcpy(event->date, lineo->field->value);
                }
                else if(!strcmp(lineo->field->tag, "PLAC")){
                    if(event->place != NULL){
                        err.line = lineo->lineNum;
                        err.type = INV_RECORD;
                        clearList(&families);
                        clearList(&individuals);
                        clearList(&read);
                        if(header->submitter != NULL){
                            free(submitter);
                        }
                        clearList(&header->otherFields);
                        free(header);
                        clearList(&readLines);
                        *obj = NULL;
                        return err;
                    }
                    event->place = malloc(sizeof(char) *(strlen(lineo->field->value) +1));
                    strcpy(event->place, lineo->field->value);
                }
                else{
                    insertBack(&event->otherFields, copyField(lineo->field));
                }
            }
            else if(isFamEvent(lineo->field->tag)){
                event = malloc(sizeof(Event) *1);
                insertBack(&fam->events, event);
                strcpy(event->type, lineo->field->tag);
                eventLine = lineo->lineDepth;
                event->date = NULL;
                event->place = NULL;
                event->otherFields = initializeList(&printField, &deleteField, &compareFields);
            }
            else if(!strcmp(lineo->field->tag, "HUSB") && findElement(fam->otherFields, &compareFindTags, "HUSB") != NULL){
                err.line = lineo->lineNum;
                err.type = INV_RECORD;
                clearList(&families);
                clearList(&individuals);
                clearList(&read);
                if(header->submitter != NULL){
                    free(submitter);
                }
                clearList(&header->otherFields);
                free(header);
                clearList(&readLines);
                *obj = NULL;
                return err;
            }
            else if(!strcmp(lineo->field->tag, "WIFE") && findElement(fam->otherFields, &compareFindTags, "WIFE") != NULL){
                err.line = lineo->lineNum;
                err.type = INV_RECORD;
                clearList(&families);
                clearList(&individuals);
                clearList(&read);
                clearList(&header->otherFields);
                if(header->submitter != NULL){
                    free(submitter);
                }
                free(header);
                clearList(&readLines);
                *obj = NULL;
                return err;

            }
            else{
                insertBack(&fam->otherFields, copyField(lineo->field));
            }
        }
        //submitter parsing
        if(spot == 3){
            if(lineo->lineDepth == 1 && !strcmp(lineo->field->tag, "NAME")){
                if(strlen(lineo->field->value) > 60){
                    err.line = lineo->lineNum;
                    err.type = INV_GEDCOM;
                    clearList(&families);
                    clearList(&individuals);
                    clearList(&read);
                    clearList(&header->otherFields);
                    if(header->submitter != NULL){
                        free(submitter);
                    }
                    free(header);
                    clearList(&readLines);
                    *obj = NULL;
                    return err;
                }
                strncpy(submitter->submitterName, lineo->field->value, 60);
            }
            else{
                insertBack(&submitter->otherFields, copyField(lineo->field));
            }
        }

    }
    clearList(&read);
    clearList(&readLines);
    if(header->submitter == NULL || strlen(submitter->submitterName) == 0){
        err.type = INV_GEDCOM;
        err.line = -1;
        if(header->submitter == NULL){
            printf("sub is NULL\n");
        }
        else{
            printf("sub name is NULL\n");
        }
        clearList(&families);
        clearList(&individuals);
        clearList(&(header->otherFields));
        free(header);
        *obj = NULL;
        printf("oh fuckkk\n");
        return err;
    }

    //get individuals families
    iterator = individuals.head;
    for(i=0; i < individuals.length; ++i){
        while(findElement(((Individual *)iterator->data)->otherFields, &compareFindTags, "FAM")){
            temp = findElement(((Individual *)iterator->data)->otherFields, &compareFindTags, "FAM");
            fam = findElement(families, &compareFindFamTag, temp->value);
            if(fam != NULL){
                insertBack(&((Individual *)iterator->data)->families, fam);
                deleteDataFromList(&((Individual *)iterator->data)->otherFields, temp);
            }
            else{
                err.type = INV_RECORD;
                clearList(&families);
                clearList(&individuals);
                clearList(&(header->otherFields));
                free(header);
                free(submitter);
                *obj = NULL;
                return err;
            }
        }
        iterator = iterator->next;
    }
    //get families individuals
    iterator = families.head;
    for(i=0; i < families.length; ++i){
        while(findElement(((Family *)iterator->data)->otherFields, &compareFindTags, "INDI")){
            temp = findElement(((Family *)iterator->data)->otherFields, &compareFindTags, "INDI");
            indi = findElement(individuals, &compareFindIndiTag, temp->value);
            if(indi != NULL){
                if(!strcmp(temp->tag, "WIFE")){
                    ((Family *)iterator->data)->wife = indi;
                    deleteDataFromList(&((Family *)iterator->data)->otherFields, temp);
                }
                else if(!strcmp(temp->tag, "HUSB")){
                    ((Family *)iterator->data)->husband = indi;
                    deleteDataFromList(&((Family *)iterator->data)->otherFields, temp);
                }
                else if(!strcmp(temp->tag, "CHIL")){
                    insertBack(&((Family *)iterator->data)->children, indi);
                    deleteDataFromList(&((Family *)iterator->data)->otherFields, temp);
                }
                else{
                    err.type = INV_RECORD;
                    clearList(&(header->otherFields));
                    free(header);
                    free(submitter);
                    *obj = NULL;
                    return err;
                }
            }
        }
        iterator = iterator->next;
    }
    iterator = individuals.head;
    for(i = 0; i < individuals.length; ++i){
        if(getFromFront(((Individual *)iterator->data)->otherFields) != NULL && !strcmp(((Field *)(getFromFront(((Individual *)iterator->data)->otherFields)))->value, "INDI")){
            deleteFromFront(&((Individual *)iterator->data)->otherFields);
        }
        iterator = iterator->next;
    }

    iterator = families.head;
    for(i = 0; i < families.length; ++i){
        if(getFromFront(((Family *)iterator->data)->otherFields) != NULL && !strcmp(((Field *)(getFromFront(((Family *)iterator->data)->otherFields)))->value, "FAM")){
            deleteFromFront(&((Family *)iterator->data)->otherFields);
        }
        iterator = iterator->next;
    }
    if(getFromFront(submitter->otherFields) != NULL && !strcmp(((Field *)getFromFront(submitter->otherFields))->value, "SUBM")){
        deleteFromFront(&submitter->otherFields);
    }

    *obj = (GEDCOMobject *) malloc(sizeof(GEDCOMobject));
    (*obj)->families = families;
    (*obj)->individuals = individuals;
    (*obj)->submitter = submitter;
    (*obj)->header = header;

    clearList(&read);
    clearList(&readLines);
    free(submTag);
    err.type = OK;

    return err;

}

/** Function to create a string representation of a GEDCOMobject.
 *@pre GEDCOMobject object exists, is not null, and is valid
 *@post GEDCOMobject has not been modified in any way, and a string representing the GEDCOM contents has been created
 *@return a string contaning a humanly readable representation of a GEDCOMobject
 *@param obj - a pointer to a GEDCOMobject struct
 **/
char* printGEDCOM(const GEDCOMobject* obj){

    char * gedcom = NULL;
    if(obj == NULL){
        gedcom = malloc(sizeof(char) * 25);
        strcpy(gedcom, "Invalid gedcom object\n");
        return gedcom;
    }
    gedcom = malloc(sizeof(char) *50000);
    char * temp;
    sprintf(gedcom, "GEDCOM OBJECT:\nSource: %s\nVersion: %.2f\nEncoding:", obj->header->source, obj->header->gedcVersion);
    if(obj->header->encoding == ANSEL){
        strcat(gedcom, "ANSEL\n");
    }
    if(obj->header->encoding == UTF8){
        strcat(gedcom, "UTF-8\n");
    }
    if(obj->header->encoding == UNICODE){
        strcat(gedcom, "UNICODE\n");
    }
    if(obj->header->encoding == ASCII){
        strcat(gedcom, "ASCII\n");
    }
    strcat(gedcom, "Other Header Fields:");

    temp = toString(obj->header->otherFields);
    strcat(gedcom, temp);
    free(temp);
    strcat(gedcom, "\n\nSubmitter: \nName: ");
    strcat(gedcom, obj->submitter->submitterName);
    strcat(gedcom, "\nAddress: ");
    strcat(gedcom, obj->submitter->address);
    strcat(gedcom, "\nOther Submitter Fields:");

    temp = toString(obj->submitter->otherFields);
    strcat(gedcom, temp);
    free(temp);
    strcat(gedcom, "\n\nIndividuals:");
    temp = toString(obj->individuals);
    strcat(gedcom, temp);
    free(temp);
    strcat(gedcom, "\n\nFamilies:");
    temp = toString(obj->families);
    strcat(gedcom, temp);
    free(temp);

    return gedcom;
}


/** Function to delete all GEDCOM object content and free all the memory.
 *@pre GEDCOM object exists, is not null, and has not been freed
 *@post GEDCOM object had been freed
 *@return none
 *@param obj - a pointer to a GEDCOMobject struct
 **/
void deleteGEDCOM(GEDCOMobject* obj){

    if(obj == NULL){
        return;
    }

    clearList(&(obj->header->otherFields));
    clearList(&(obj->families));
    clearList(&(obj->individuals));
    clearList(&(obj->submitter->otherFields));
    free(obj->submitter);
    free(obj->header);
    free(obj);

    return;
}


/** Function to "convert" the GEDCOMerror into a humanly redabale string.
 *@return a string contaning a humanly readable representation of the error code
 *@param err - an error struct
 **/
char* printError(GEDCOMerror err){

    char* message = malloc(sizeof(char) *(50));

    switch(err.type){
        case OK:
            strcpy(message, "GEDCOM file is OK");
            break;
        case INV_FILE:
            strcpy(message, "File is invald");
            break;
        case INV_GEDCOM:
            sprintf(message, "File is invalid GEDCOM format, see line: %d", err.line);
            break;
        case INV_HEADER:
            sprintf(message, "Header is invalid GEDCOM format, see line: %d", err.line);
            break;
        case INV_RECORD:
            sprintf(message, "File contains invalid GEDCOM record, see line: %d", err.line);
            break;
        case OTHER_ERROR:
            sprintf(message, "GEDCOM file contains unknown error, see line: %d", err.line);
            break;
        default:
            sprintf(message, "Invalid error");
            break;
    }

    return message;

}

/** Function that searches for an individual in the list using a comparator function.
 * If an individual is found, a pointer to the Individual record
 * Returns NULL if the individual is not found.
 *@pre GEDCOM object exists,is not NULL, and is valid.  Comparator function has been provided.
 *@post GEDCOM object remains unchanged.
 *@return The Individual record associated with the person that matches the search criteria.  If the Individual record is not found, return NULL.
 *If multiple records match the search criteria, return the first one.
 *@param familyRecord - a pointer to a GEDCOMobject struct
 *@param compare - a pointer to comparator fuction for customizing the search
 *@param person - a pointer to search data, which contains seach criteria
 *Note: while the arguments of compare() and person are all void, it is assumed that records they point to are
 *      all of the same type - just like arguments to the compare() function in the List struct
 **/
Individual* findPerson(const GEDCOMobject* familyRecord, bool (*compare)(const void* first, const void* second), const void* person){
    if(familyRecord == NULL){
        return NULL;
    }
    return findElement(familyRecord->individuals, compare, person);
}


/** Function to return a list of all descendants of an individual in a GEDCOM
 *@pre GEDCOM object exists, is not null, and is valid
 *@post GEDCOM object has not been modified in any way, and a list of descendants has been created
 *@return a list of descendants.  The list may be empty.  All list members must be of type Individual, and can appear in any order.
 *All list members must be COPIES of the Individual records in the GEDCOM file.  If the returned list is freed, the original GEDCOM
 *must remain unaffected.
 *@param familyRecord - a pointer to a GEDCOMobject struct
 *@param person - the Individual record whose descendants we want
 **/
List getDescendants(const GEDCOMobject* familyRecord, const Individual* person){
    List descendants = initializeList(&printIndividual, &deleteIndividual, &compareIndividuals);
    if(familyRecord == NULL || person == NULL){
        return descendants;
    }
    getDescendentsRecursive(&descendants, *person);
    return descendants;
}


//****************************************** List helper functions *******************************************
void deleteEvent(void* toBeDeleted){
    Event * eve = (Event *)toBeDeleted;

    free(eve->date);
    free(eve->place);
    clearList(&eve->otherFields);
    free(eve);

    return;
}

int compareEvents(const void* first,const void* second){
    Event* one = (Event *) first;
    Event* two = (Event *) second;

    int compare = strcmp(one->type, two->type);
    if(compare == 0){
        return strcmp(one->place, two->place);
    }
    return compare;
}

char* printEvent(void* toBePrinted){
    char* string;
    Event * print = (Event *)toBePrinted;
    char *temp;

    string = malloc(sizeof(char) *500);

    sprintf(string, "  Type: %s  Date: %s  Place: %s", print->type, print->date, print->place);
    if(print->otherFields.length != 0){
        strcat(string, "\n  Other Event Fields:");
        temp = toString(print->otherFields);
        strcat(string, temp);
        free(temp);
    }

    return string;

}

void deleteIndividual(void* toBeDeleted){
    Individual* indi = (Individual *)toBeDeleted;

    free(indi->givenName);
    free(indi->surname);
    clearList(&indi->events);
    clearList(&indi->families);
    clearList(&indi->otherFields);
    free(indi);
}

int compareIndividuals(const void* first,const void* second){

    if(first == NULL || second == NULL){
        return -1;
    }

    int compare;
    compare = strcmp(((Individual *)first)->surname, (((Individual *)second)->surname));
    if(compare != 0){
        return compare;
    }

    compare = strcmp(((Individual *)first)->givenName, (((Individual *)second)->givenName));
    if(compare != 0){
        return compare;
    }

    return(((Individual *)first)->families.length - ((Individual *)second)->families.length);

}

char* printIndividual(void* toBePrinted){

    Individual *person = (Individual *)toBePrinted;
    char *string = malloc(sizeof(char) * 800);
    char *temp;

    sprintf(string, "\nGiven Name: %s\nSurname: %s\nNumber of Families: %d\n", person->givenName, person->surname, person->families.length);
    if(person->events.length != 0){
        strcat(string, "Events:");
        temp = toString(person->events);
        strcat(string, temp);
        strcat(string, "\n");
        free(temp);
    }
    if(person->otherFields.length != 0){
        strcat(string, "Other Individual Fields: ");
        temp = toString(person->otherFields);
        strcat(string, temp);
        free(temp);
    }

    return string;
}

void deleteFamily(void* toBeDeleted){
    Family* fam = (Family *)toBeDeleted;

    fam->wife = NULL;
    fam->husband = NULL;
    clearList(&fam->children);
    clearList(&fam->otherFields);
    free(fam);
}
int compareFamilies(const void* first,const void* second){
    Family* one = (Family *)first;
    Family* two = (Family *)second;
    int a = 0;
    int b = 0;

    if(one->wife != NULL){
        ++a;
    }
    if(one->husband != NULL){
        ++a;
    }
    if(two->wife != NULL){
        ++b;
    }
    if(two->husband != NULL){
        ++b;
    }

    a = a+ one->children.length;
    b = b+ two->children.length;

    if(a > b){
        return 1;
    }
    if(a == b){
        return 0;
    }
    return -1;

}
char* printFamily(void* toBePrinted){

    char *string;
    Family* family = (Family *)toBePrinted;
    int i;
    Node* node;
    char * temp;

    string = malloc(sizeof(char) *500);
    sprintf(string, "\nWife: ");
    if(family->wife != NULL){
        strcat(string, family->wife->givenName);
        strcat(string, " ");
        strcat(string, family->wife->surname);
    }
    strcat(string, "\nHusband: ");
    if(family->husband != NULL){
        strcat(string, family->husband->givenName);
        strcat(string, " ");
        strcat(string, family->husband->surname);
    }
    strcat(string, "\nChildren:\n");
    node = family->children.head;
    for(i = 0; i < family->children.length; ++i){
        strcat(string, ((Individual *)(node->data))->givenName);
        strcat(string, " ");
        strcat(string, ((Individual *)(node->data))->surname);
        strcat(string, "\n");
        node = node->next;
    }
    if(family->otherFields.length != 0){
        strcat(string, "Other Family Fields:");
        temp = toString(family->otherFields);
        strcat(string, temp);
        free(temp);
    }
    return string;

}

void deleteField(void* toBeDeleted){
    Field * field = (Field *)toBeDeleted;

    free(field->tag);
    free(field->value);
    free(field);

    return;
}
int compareFields(const void* first,const void* second){

    Field* one = (Field *)first;
    Field* two = (Field *)second;
    int compare;

    compare = strcmp(one->tag, two->tag);
    if(compare != 0){
        return compare;
    }
    return strcmp(one->value, two->value);
}

char* printField(void* toBePrinted){
    char *string;
    Field* print = (Field *)toBePrinted;

    string = malloc(sizeof(char)*(strlen(print->tag) + strlen(print->value) +20));

    sprintf(string, "   %s: %s", print->tag, print->value);

    return string;
}


/** Function for writing a GEDCOMobject into a file in GEDCOM format.
*@pre GEDCOMobject object exists, is not null, and is valid
*@post GEDCOMobject has not been modified in any way, and a file
representing the GEDCOMobject contents in GEDCOM format has been created
*@return the error code indicating success or the error encountered when
writing the GEDCOM
*@param obj - a pointer to a GEDCOMobject struct
**/
GEDCOMerror writeGEDCOM(char* fileName, const GEDCOMobject* obj){

    FILE * file = NULL;
    Node *node;
    Node *yaNode;
    Node *nooode;
    Field *tempField;
    int i, j, k;
    List tags;
    GEDCOMerror err;
    err.line = -1;
    err.type = validateGEDCOM(obj);
    if(obj == NULL){
        err.type = WRITE_ERROR;
        return err;
    }
    if (err.type != OK){
        return err;
    }
    if (fileName == NULL || strstr(fileName, ".ged") == NULL){
        err.type = WRITE_ERROR;
    }

    file = fopen(fileName, "w");
    if(file == NULL){
        err.type = WRITE_ERROR;
        return err;
    }
    tags = initializeList(&print, &deleteTags, &compareTags);
    node = obj->individuals.head;
    for(i = 0; i < obj->individuals.length; ++i){
        Tags *tag = malloc(sizeof(Tags) *1);
        insertBack(&tags, tag);
        tag->tag = createTag('I', i);
        tag->data = node->data;
        node = node->next;
    }
    node = obj->families.head;
    for(i = 0; i < obj->families.length; ++i){
        Tags *tag = malloc(sizeof(Tags) *1);
        insertBack(&tags, tag);
        tag->tag = createTag('F', i);
        tag->data = node->data;
        node = node->next;
    }
    //**********************HEADER*****************************
    fprintf(file, "0 HEAD\n");
    fprintf(file, "1 SOUR %s\n", obj->header->source);
    node = obj->header->otherFields.head;
 /*   for(i = 0; i < obj->header->otherFields.length; ++i){
        tempField = (Field *)node->data;
        fprintf(file, "2 %s %s\n", tempField->tag, tempField->value);
        node = node->next;
    }*/
    fprintf(file, "1 GEDC\n2 VERS %.1f\n", obj->header->gedcVersion);
    if(obj->header->encoding == ANSEL) fprintf(file, "1 CHAR ANSEL\n");
    if(obj->header->encoding == UTF8) fprintf(file, "1 CHAR UTF-8\n");
    if(obj->header->encoding == UNICODE) fprintf(file, "1 CHAR UNICODE\n");
    if(obj->header->encoding == ASCII) fprintf(file, "1 CHAR ASCII\n");
    if(obj->submitter->address == NULL || strlen(obj->submitter->address) == 0){
        fprintf(file, "1 SUBM\n");
        fprintf(file, "0 SUBM\n");
    }
    else{
        fprintf(file, "1 SUBM %s\n", obj->submitter->address);
        fprintf(file, "0 %s SUBM\n", obj->submitter->address);
    }
    //********************SUBMITTER****************************

    fprintf(file, "1 NAME %s\n", obj->submitter->submitterName);
    node = obj->submitter->otherFields.head;
  /*  for(i = 0; i < obj->submitter->otherFields.length; ++i){
        tempField = (Field *)node->data;
        fprintf(file, "1 %s %s\n", tempField->tag, tempField->value);
        node = node->next;
    }*/
    //*******************INDIVIDUALS*****************************
    node = obj->individuals.head;
    for(i = 0; i <obj->individuals.length; ++i){
        Individual *tempIndi = (Individual *)node->data;
        char *temp = createTag('I', i);
        fprintf(file, "0 %s INDI\n", temp);
        free(temp);
        fprintf(file, "1 NAME %s /%s/\n", tempIndi->givenName, tempIndi->surname);
    /*    if(tempIndi->givenName != NULL && strlen(tempIndi->givenName) >0){
            fprintf(file, "2 GIVN %s\n", tempIndi->givenName);
        }
        if(tempIndi->surname != NULL && strlen(tempIndi->surname) >0){
            fprintf(file, "2 SURN %s\n", tempIndi->surname);
        }*/
        yaNode = tempIndi->events.head;
        for(j = 0; j < tempIndi->events.length; ++j){
            Event *tempEve = (Event *)yaNode->data;
            fprintf(file, "1 %s\n", tempEve->type);
            if(tempEve->place != NULL && strlen(tempEve->place) > 0){
                fprintf(file, "2 PLAC %s\n", tempEve->place);
            }
            if(tempEve->date != NULL &&  strlen(tempEve->date) > 0){
                fprintf(file, "2 DATE %s\n", tempEve->date);
            }
            nooode = tempEve->otherFields.head;
            for(k = 0; k < tempEve->otherFields.length; ++k){
                tempField = (Field *)nooode->data;
                fprintf(file, "2 %s %s\n", tempField->tag, tempField->value);
                nooode = nooode->next;
            }
            yaNode = yaNode->next;
        }
        yaNode = tempIndi->families.head;
        for(j = 0; j < tempIndi->families.length; ++j){
            Family *fam = (Family *)yaNode->data;
            char *temp;
            temp = ((Tags *)findElement(tags, &compareTagPointer, yaNode->data))->tag;
            if(fam->husband == tempIndi || fam->wife == tempIndi){
                fprintf(file, "1 FAMS %s\n", temp);
            }
            else{
                fprintf(file, "1 FAMC %s\n", temp);
            }
            yaNode = yaNode->next;
        }
        yaNode = tempIndi->otherFields.head;
        for(j = 0; j < tempIndi->otherFields.length; ++j){
            tempField = (Field *)yaNode->data;
            if(strcmp(tempField->tag, "GIVN") != 0 && strcmp(tempField->tag, "SURN") != 0) {
                fprintf(file, "1 %s %s\n", tempField->tag, tempField->value);
            }
            yaNode = yaNode->next;
        }
        node = node->next;
    }

    //******************FAMILIES*********************************
    node = obj->families.head;
    for(i = 0; i <obj->families.length; ++i){
        Family *tempFam = (Family *)node->data;
        fprintf(file, "0 %s FAM\n", createTag('F', i));

        if(tempFam->wife != NULL){
            fprintf(file, "1 WIFE %s\n", ((Tags *)findElement(tags, &compareTagPointer, tempFam->wife))->tag);
        }
        if(tempFam->husband != NULL){
            fprintf(file, "1 HUSB %s\n", ((Tags *)findElement(tags, &compareTagPointer, tempFam->husband))->tag);
        }

        yaNode = tempFam->children.head;
        for(j = 0; j < tempFam->children.length; ++j){
            char *tag = ((Tags *)findElement(tags, &compareTagPointer, yaNode->data))->tag;
            fprintf(file, "1 CHIL %s\n", tag);
            yaNode = yaNode->next;
        }
        yaNode = tempFam->events.head;
        for(j = 0; j < tempFam->events.length; ++j){
            Event *tempEve = (Event *)yaNode->data;
            fprintf(file, "1 %s\n", tempEve->type);
            if(tempEve->place != NULL &&  strlen(tempEve->place) > 0){
                fprintf(file, "2 PLAC %s\n", tempEve->place);
            }
            if(tempEve->date != NULL &&  strlen(tempEve->date) > 0){
                fprintf(file, "2 DATE %s\n", tempEve->date);
            }
            nooode = tempEve->otherFields.head;
            for(k = 0; k < tempEve->otherFields.length; ++k){
                tempField = (Field *)nooode->data;
                fprintf(file, "2 %s %s\n", tempField->tag, tempField->value);
                nooode = nooode->next;
            }
            yaNode = yaNode->next;
        }
    /*    yaNode = tempFam->otherFields.head;
        for(j = 0; j < tempFam->otherFields.length; ++j){
            tempField = (Field *)yaNode->data;
            fprintf(file, "1 %s %s\n", tempField->tag, tempField->value);
            yaNode = yaNode->next;
        }*/
        node = node->next;
    }

    fprintf(file, "0 TRLR\n");

    clearList(&tags);
    fclose(file);
    err.type = OK;

printf("PRINTED\n");

    return err;

}

/** Function for validating an existing GEDCOM object
 *@pre GEDCOM object exists and is not null
 *@post GEDCOM object has not been modified in any way
 *@return the error code indicating success or the error encountered when
validating the GEDCOM
 *@param obj - a pointer to a GEDCOMobject struct
 **/
ErrorCode validateGEDCOM(const GEDCOMobject* obj){

    int i, j;
    Node *node = NULL;
    Node *yaNode =NULL;
    if(obj == NULL){
        return INV_GEDCOM;
    }

    if(obj->header == NULL || obj->submitter == NULL){
        return INV_GEDCOM;
    }

    if(strlen(obj->header->source) < 1 || obj->header->gedcVersion <0 || obj->header->encoding < 0 || obj->header->encoding >3 || obj->header->submitter != obj->submitter){
        return INV_HEADER;
    }

    if(strlen(obj->submitter->submitterName) <1 ){
        return INV_RECORD;
    }

    yaNode = obj->submitter->otherFields.head;
    for(j = 0; j <obj->submitter->otherFields.length; ++j){
        Field *tempField = (Field *)yaNode->data;
        if(yaNode->data == NULL){
            return INV_RECORD;
        }
        if(tempField->tag == NULL){
            return INV_RECORD;
        }
        yaNode = yaNode->next;
    }

    node = obj->individuals.head;
    for(i = 0; i < obj->individuals.length; ++i){
        Individual *tempIndi = (Individual *)node->data;
        if(tempIndi == NULL){
            return INV_RECORD;
        }
        yaNode = tempIndi->families.head;
        for(j = 0; j < tempIndi->families.length; ++j) {
            if(findElement(obj->families, &compareReferences, yaNode->data))
                yaNode = yaNode->next;
        }

        yaNode = tempIndi->events.head;
        for(j = 0; j < tempIndi->events.length; ++j) {
            Event *tempEve = (Event *)yaNode->data;
            if(tempEve == NULL){
                return INV_RECORD;
            }
            if(!isEvent(tempEve->type )){
                return INV_RECORD;
            }
            yaNode = yaNode->next;
        }

        yaNode = tempIndi->otherFields.head;
        for(j = 0; j <tempIndi->otherFields.length; ++j){
            Field *tempField = (Field *)yaNode->data;
            if(yaNode->data == NULL){
                return INV_RECORD;
            }
            if(tempField->tag == NULL){
                return INV_RECORD;
            }
            yaNode = yaNode->next;
        }

        node = node->next;
    }

    node = obj->families.head;
    for(i = 0; i < obj->families.length; ++i){
        Family *tempFam = (Family *)node->data;
        if (tempFam == NULL) {
            return INV_RECORD;
        }
        if(tempFam->wife != NULL){
            if(findElement(obj->individuals, &compareReferences, tempFam->wife) == NULL){
                return INV_RECORD;
            }
        }
        if(tempFam->husband != NULL){
            if(findElement(obj->individuals, &compareReferences, tempFam->husband) == NULL){
                return INV_RECORD;
            }
        }
        yaNode = tempFam->children.head;
        for(j = 0; j < tempFam->children.length; ++j){
            if(yaNode->data == NULL){
                return INV_RECORD;
            }
            if(findElement(obj->individuals, &compareReferences, yaNode->data) == NULL){
                return INV_RECORD;
            }
            yaNode = yaNode->next;
        }
        yaNode = tempFam->events.head;
        for(j = 0; j < tempFam->events.length; ++j) {
            Event *tempEve = (Event *)yaNode->data;
            if(tempEve == NULL){
                return INV_RECORD;
            }
            if(!isFamEvent(tempEve->type )){
                return INV_RECORD;
            }
            yaNode = yaNode->next;
        }

        yaNode = tempFam->otherFields.head;
        for(j = 0; j <tempFam->otherFields.length; ++j){
            Field *tempField = (Field *)yaNode->data;
            if(yaNode->data == NULL){
                return INV_RECORD;
            }
            if(tempField->tag == NULL){
                return INV_RECORD;
            }
            yaNode = yaNode->next;
        }

        node = node->next;
    }

    return OK;

}

List getDescendantListN(const GEDCOMobject* familyRecord, const Individual* person, unsigned int maxGen){
    List descendants = initializeList(&printGeneration, &deleteGeneration, &compareGenerations);
    if(familyRecord == NULL || person == NULL){
        return descendants;
    }
    getDescendentsRecursiveN(&descendants, *person, maxGen, 0);
    return descendants;

}

List getAncestorListN(const GEDCOMobject* familyRecord, const Individual* person, int maxGen){
    List ancestors = initializeList(&printGeneration, &deleteGeneration, &compareGenerations);
    if(familyRecord == NULL || person == NULL){
        return ancestors;
    }
    getAncestorsRecursiveN(&ancestors, person, maxGen, 0);
    return ancestors;

}

char* indToJSON(const Individual* ind){
    char * string;

    if(ind == NULL){
        string = malloc(sizeof(char) *1);
        return string;
    }
    string = malloc(sizeof(char) * (strlen(ind->givenName) + strlen(ind->surname) + 30));
    sprintf(string, "{\"givenName\":\"%s\",\"surname\":\"%s\"}", ind->givenName, ind->surname);
    return string;

}

Individual* JSONtoInd(const char* str){
    Individual *indi;
    char parsing[100];
    char givn[50];
    char surn[50];
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
    strncpy(parsing, &str[27+i+j], 4);
    if(strcmp(parsing, "\"}") != 0){
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

    return indi;

}

GEDCOMobject* JSONtoGEDCOM(const char* str){

    GEDCOMobject * obj;
    char parsing[100];
    char sour[300];
    char versS[10];
    char encoding[20];
    char subName[100];
    char sub[100];
    int i, j, k, l, m;
    if(str == NULL){
        return NULL;
    }
    strncpy(parsing, str, 11);
    parsing[11] = '\0';
    if(strcmp(parsing, "{\"source\":\"") != 0){
        return NULL;
    }
    for(i = 0; i < strlen(str); ++i){
        if(str[11+i] == '\"' || str[11+i] == '\0'){
            sour[i] = '\0';
            break;
        }
        sour[i] = str[11+i];
    }
    strncpy(parsing, &str[11+i], 17);
    parsing[17] = '\0';
    if(strcmp(parsing, "\",\"gedcVersion\":\"") != 0){
        return NULL;
    }
    for(j = 0; j < strlen(str); ++j){
        if(str[28+i+j] == '\"' || str[28+i+j] == '\0'){
            versS[j] = '\0';
            break;
        }
        if(!isdigit(str[28+i+j]) && str[28+i+j] != '.'){

            return NULL;
        }
        versS[j] = str[27+i+j+1];
    }
    strncpy(parsing, &str[28+i+j], 14);
    parsing[14] = '\0';
    if(strcmp(parsing, "\",\"encoding\":\"") != 0){

        return NULL;
    }
    for(k = 0; k < strlen(str); ++k){
        if(str[42+i+j+k] == '\"' || str[42+i+j+k] == '\0'){
            encoding[k] = '\0';
            break;
        }
        encoding[k] = str[42+i+j+k];
    }
    strncpy(parsing, &str[42+i+j+k], 13);
    parsing[13] = '\0';
    if(strcmp(parsing, "\",\"subName\":\"") != 0){
        return NULL;
    }
    for(l = 0; l < strlen(str); ++l){
        if(str[55 +i+j+k+l] == '\"' || str[55 +i+j+k+l] == '\0'){
            subName[l] = '\0';
            break;
        }
        subName[l] = str[55+i+j+k+l];
    }
    strncpy(parsing, &str[55+i+j+k+l], 16);
    parsing[16] = '\0';
    if(strcmp(parsing, "\",\"subAddress\":\"") != 0){
        return NULL;
    }
    for(m = 0; m < strlen(str); ++m){
        if(str[71 +i+j+k+l+m] == '\"' || str[71 +i+j+k+l+m] == '\0'){
            sub[m] = '\0';
            break;
        }
        sub[m] = str[71+i+j+k+l+m];
    }
    strncpy(parsing, &str[71+i+j+k+l+m], 5);
    if(strcmp(parsing, "\"}") != 0){

        return NULL;
    }

    if(strlen(sour) > 248 || strlen(subName) > 60 || (strcmp(encoding, "ANSEL") != 0 && strcmp(encoding, "UTF-8") != 0 && strcmp(encoding, "UNICODE") != 0 && strcmp(encoding, "ASCII") != 0)){

        return NULL;
    }
    obj = malloc(sizeof(GEDCOMobject) *1);
    obj->header = malloc(sizeof(Header) *1);
    obj->header->otherFields = initializeList(&printField, &deleteField, &compareFields);
    obj->submitter = malloc(sizeof(Submitter) + sizeof(char) * (strlen(sub) +1));
    obj->submitter->otherFields = initializeList(&printField, &deleteField, &compareFields);

    obj->header->submitter = obj->submitter;
    obj->individuals = initializeList(&printIndividual, &deleteIndividual, &compareIndividuals);
    obj->families = initializeList(&printFamily, &deleteFamily, &compareFamilies);
    strcpy(obj->header->source, sour);
    obj->header->gedcVersion = atof(versS);
    if(strcmp(encoding, "ANSEL") == 0){
        obj->header->encoding = ANSEL;
    }
    if(strcmp(encoding, "UTF-8") == 0){
        obj->header->encoding = UTF8;
    }
    if(strcmp(encoding, "UNICODE") == 0){
        obj->header->encoding = UNICODE;
    }
    if(strcmp(encoding, "ASCII") == 0){
        obj->header->encoding = ASCII;
    }
    strcpy(obj->submitter->submitterName, subName);
    strcpy(obj->submitter->address, sub);
    return obj;

}

void addIndividual(GEDCOMobject* obj, const Individual* toBeAdded){

    if(obj == NULL || toBeAdded == NULL){
        return;
    }
    insertBack(&obj->individuals, (Individual *)toBeAdded);

}

char* iListToJSON(List iList){
    char **jason;
    jason = malloc(sizeof(char *) * (iList.length));
    Node * person;
    int i;
    int length =0;
    char * string;
    person = iList.head;
    for(i = 0; i < iList.length; ++i){
        jason[i] = indToJSON((Individual *)person->data);
        if(jason[i] == NULL || strlen(jason[i]) <1){
            jason[i] = malloc(sizeof(char) *1);
            strcpy(jason[i], "");
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

void deleteGeneration(void* toBeDeleted){
    List *list = (List *)toBeDeleted;
    clearList(list);
    free(list);
}

int compareGenerations(const void* first,const void* second){

    List *a = (List *)first;
    List *b = (List *)second;

    return a->length - b->length;

}

char* printGeneration(void* toBePrinted){

    char* string;
    char *prints;
    List *print = toBePrinted;

    string = toString(*print);

    prints = malloc(sizeof(char) *(strlen(string) + 25));

    strcpy(prints, "Generation\n");
    strcat(prints, string);

    free(string);

    return prints;

}

char* gListToJSON(List gList){

    char **jason;

    jason = malloc(sizeof(char *) *(gList.length));
    Node *iList;
    int i; 
    int length = 0;
    char * string;

    iList = gList.head;
    for(i = 0; i < gList.length; ++i){
        jason[i] = iListToJSON(*((List *)iList->data));
        if(jason[i] == NULL || strlen(jason[i]) <1){
            jason[i] = malloc(sizeof(char));
            strcpy(jason[i], "");
        }
        length = length + strlen(jason[i]);
        iList = iList->next;
    }

    string = malloc(sizeof(char) * (length +10 + i));

    strcpy(string, "[");
    for( i = 0; i < gList.length; ++i){
        strcat(string, jason[i]);
        if( i != gList.length - 1 && strlen(jason[i]) >0 ){
            strcat(string, ",");
        }
    }
    strcat(string, "]");

    for(i = 0; i < gList.length; ++i){
        free(jason[i]);
    }
    free(jason);

    return string;


}