/***********
 ID: 214007346
 NAME: Yahali Mashiach
***********/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INVALID_INDEX -1

typedef struct Episode {
    char *name;
    char *length;
    struct Episode *next;
} Episode;

typedef struct Season {
    char *name;
    Episode *episodes;
    struct Season *next;
} Season;

typedef struct TVShow {
    char *name;
    Season *seasons;
} TVShow;

TVShow ***database = NULL;
int dbSize = 0;

char *getString();
int getInt();

int validLength(char *s);
int countShows();

void shrinkDB();
void expandDB();
void shiftDBRightFromIndex(int index);
void shiftDBLeftFromIndex(int index);

void freeEpisode(Episode *e);
void freeSeason(Season *s);
void freeShow(TVShow *show);
void freeAll();

TVShow *findShow(char *name);
Season *findSeason(TVShow *show, char *name);
Episode *findEpisode(Season *season, char *name);

void addShow();
int getIndexForNewShowByOrder(char *showName);
void addSeason();
void addEpisode();

void deleteShow();
void deleteSeason();
void deleteEpisode();

void printEpisode();
void printShow();
void printArray();

int doesShowExist(char *showName);
int doesSeasonExist(TVShow *show, char *seasonName);
int doesEpisodeExist(Season *season, char *episodeName);

void addMenu() {
    int choice;
    printf("Choose an option:\n");
    printf("1. Add a TV show\n");
    printf("2. Add a season\n");
    printf("3. Add an episode\n");
    scanf("%d", &choice);
    getchar();
    switch (choice) {
        case 1: addShow(); break;
        case 2: addSeason(); break;
        case 3: addEpisode(); break;
    }
}

void deleteMenu() {
    int choice;
    printf("Choose an option:\n");
    printf("1. Delete a TV show\n");
    printf("2. Delete a season\n");
    printf("3. Delete an episode\n");
    scanf("%d", &choice);
    getchar();
    switch (choice) {
        case 1: deleteShow(); break;
        case 2: deleteSeason(); break;
        case 3: deleteEpisode(); break;
    }
}

void printMenuSub() {
    int choice;
    printf("Choose an option:\n");
    printf("1. Print a TV show\n");
    printf("2. Print an episode\n");
    printf("3. Print the array\n");
    scanf("%d", &choice);
    getchar();
    switch (choice) {
        case 1: printShow(); break;
        case 2: printEpisode(); break;
        case 3: printArray(); break;
    }
}

void mainMenu() {
    printf("Choose an option:\n");
    printf("1. Add\n");
    printf("2. Delete\n");
    printf("3. Print\n");
    printf("4. Exit\n");
}

int main() {
    int choice;
    do {
        mainMenu();
        scanf("%d", &choice);
        getchar();
        switch (choice) {
            case 1: addMenu(); break;
            case 2: deleteMenu(); break;
            case 3: printMenuSub(); break;
            case 4: freeAll(); break;
        }
    } while (choice != 4);
    return 0;
}

char *getString() {
    char *string = NULL;
    int length = 0;
    char character;

    do {
        character = getchar();

        if (character != '\n') {
            string = (char *)realloc(string, (length + 1) * sizeof(char));

            if (string == NULL)
                return NULL;

            string[length] = character;
            length++;
        }
    } while (character != '\n');

    string = (char *)realloc(string, (length + 1) * sizeof(char));
    
    if (string == NULL)
        return NULL;

    string[length] = '\0';

    return string;
}

int getInt() {
    int num;
    
    // if input of number isnt successful
    if (scanf("%d", &num) != 1) {
        // clear buffer
        while (getchar() != '\n');

        return INVALID_INDEX;
    }

    // clear buffer
     while (getchar() != '\n');

    return num;
}

void expandDB() {    
    int newSize = (dbSize + 1) * (dbSize + 1);
    int currentSize = dbSize * dbSize;

    // Allocate space for n+1 x n+1 tv shows
    TVShow **newDatabase = (TVShow **)realloc(*database, newSize * sizeof(TVShow *));
    
    if (newDatabase == NULL) {
        return; 
    }

    *database = newDatabase;

    // Reset new cells to null
    for (int i = currentSize; i < newSize; i++) {
        (*database)[i] = NULL;
    }

    dbSize++; 
}

void shrinkDB() {
    if (dbSize == 0) {
        return;
    }

    int newSize = (dbSize - 1) * (dbSize - 1);
    int currentSize = dbSize * dbSize;

    for (int i = newSize; i < currentSize; i++) {
        if ((*database)[i] != NULL) {
            return;
        };
    }

    // Allocate space for n-1 x n-1 tv shows
    TVShow **newDatabase = (TVShow **)realloc(*database, newSize * sizeof(TVShow *));
    *database = newDatabase;
    dbSize--; 
}

void shiftDBRightFromIndex(int index) {
    int totalDbSize = dbSize * dbSize;

    for (int j = totalDbSize - 1; j > index; j--) {
        (*database)[j] = (*database)[j - 1];
    }
}

void shiftDBLeftFromIndex(int index) {
    int totalDbSize = dbSize * dbSize;

    for (int j = index; j < totalDbSize - 1; j++) {
        (*database)[j] = (*database)[j + 1];
    }

    (*database)[totalDbSize - 1] = NULL;
}

int getIndexForNewShowByOrder(char *showName) {
    int totalDbSize = dbSize * dbSize;
    int indexToAddShowIn = 0;

    while (indexToAddShowIn < totalDbSize) {
         // if cell is empty or show name is bigger then cell thats the index we want to add the show to
        if ((*database)[indexToAddShowIn] == NULL ||
         strcmp((*database)[indexToAddShowIn]->name, showName) > 0) {
            return indexToAddShowIn;
        }

        indexToAddShowIn++;
    }

    return INVALID_INDEX;
}

void freeEpisode(Episode *e) {
    if (e == NULL) {
        return;
    }

    free(e->name);
    free(e->length);

    freeEpisode(e->next);
    free(e);
}

void freeSeason(Season *s) {
    if (s == NULL) {
        return;
    }

    free(s->name);
    freeEpisode(s->episodes);
    freeSeason(s->next);
    free(s);
}

void freeShow(TVShow *show) {
    free(show->name);
    freeSeason(show->seasons);
    free(show);
}

// Return index of show if it exists, otherise return -1
int doesShowExist(char *showName) {
    int dbTotalSize = dbSize * dbSize;
    
    for (int i=0; i < dbTotalSize; i++) {        
        if ((*database)[i] == NULL) {
            continue;
        }

        int isShowEqual = strcmp((*database)[i]->name, showName);

        if (isShowEqual == 0) {            
            return i;
        }
    }

    return INVALID_INDEX;
}

// Return index of season if it exists, otherise return -1
int doesSeasonExist(TVShow *show, char *seasonName) {
    Season *season = show->seasons;
    int index = 0;

    while (season != NULL) {
        int isSeasonEqual = strcmp(season->name, seasonName);

        if (isSeasonEqual == 0) {
            return index;
        }

        season = season->next;
        index++;
    }

    return INVALID_INDEX;
}

// Return index of episode if it exists, otherise return -1
int doesEpisodeExist(Season *season, char *episodeName) {
    Episode *episode = season->episodes;
    int index = 0;

    while (episode != NULL) {
        int isEpisodeEqual = strcmp(episode->name, episodeName);

        if (isEpisodeEqual == 0) {
            return index;
        }

        episode = episode->next;
        index++;
    }

    return INVALID_INDEX;
}

void addShow() {
    char *showName;
    int dbTotalSize = dbSize * dbSize;

    if (database == NULL) {
        database = (TVShow ***)malloc(sizeof(TVShow**));
        if (database == NULL) 
            return;

        *database = NULL; 
    }
    
    printf("Enter the name of the show:\n");
    showName = getString();

    if (showName == NULL) 
        return;

    if (doesShowExist(showName) != INVALID_INDEX) {
        printf("Show already exists\n");
        free(showName);

        return;
    }

    if (dbTotalSize == 0 || (*database)[dbTotalSize - 1] != NULL) {
        expandDB();
        // Update total size after expanding
        dbTotalSize = dbSize * dbSize;
    }

    int indexToAddShowIn = getIndexForNewShowByOrder(showName);

    shiftDBRightFromIndex(indexToAddShowIn);

    TVShow *newShow = (TVShow *)malloc(sizeof(TVShow));

    if (newShow == NULL)
        return;

    newShow->name = showName;
    newShow->seasons = NULL;
    (*database)[indexToAddShowIn] = newShow;
}

void deleteShow() {
    char *showName;
    int dbTotalSize = dbSize * dbSize;
    
    printf("Enter the name of the show:\n");
    showName = getString();

    if (showName == NULL) 
        return;

    int indexOfExistingShow = doesShowExist(showName);

    if (indexOfExistingShow == INVALID_INDEX) {
        printf("Show not found.\n");
        free(showName);
        
        return;
    }
    
    TVShow *showToRemove = (*database)[indexOfExistingShow];

    free(showName);
    freeShow(showToRemove);

    shiftDBLeftFromIndex(indexOfExistingShow);
    
    if (dbTotalSize == 0 || (*database)[dbTotalSize - 1] == NULL) {
        shrinkDB();
        // Update total size after expanding
        dbTotalSize = dbSize * dbSize;
    }
} 

void addSeason() {
    printf("Enter the name of the show:\n");
    char *showName = getString();

    if (showName == NULL) 
        return;

    int showIndex = doesShowExist(showName);
    
    if (showIndex == INVALID_INDEX) {
        printf("Show not found.\n");
        free(showName);

        return;
    }

    free(showName);

    TVShow *show = (*database)[showIndex];
    
    printf("Enter the name of the season:\n");
    char *seasonName = getString();

    if (seasonName == NULL) 
        return;

    if (doesSeasonExist(show, seasonName) != INVALID_INDEX) {
        printf("Season already exists\n");
        free(seasonName);

        return;
    }

    free(seasonName);

    int position;
    printf("Enter the position:\n");
    position = getInt();

    Season *season = show->seasons;
    Season *newSeason = (Season *)malloc(sizeof(Season));

    if (newSeason == NULL) {
        free(seasonName);

        return;
    }

    newSeason->name = seasonName;
    newSeason->episodes = NULL;
    Season temporarySeason;
    temporarySeason.next = show->seasons;
    Season *iterator = &temporarySeason;
    int seasonIndex = 0;

    while (seasonIndex < position && iterator->next != NULL) {
        iterator = iterator->next;
        seasonIndex++;
    }

    newSeason->next = iterator->next;
    iterator->next = newSeason;

    show->seasons = temporarySeason.next;
}

void deleteSeason() {
    printf("Enter the name of the show:\n");
    char *showName = getString();

    if (showName == NULL) 
        return;

    int showIndex = doesShowExist(showName);
    
    if (showIndex == INVALID_INDEX) {
        printf("Show not found.\n");
        free(showName);

        return;
    }

    free(showName);

    TVShow *show = (*database)[showIndex];
    
    printf("Enter the name of the season:\n");
    char *seasonName = getString();

    if (seasonName == NULL) 
        return;

    int seasonIndex = doesSeasonExist(show, seasonName);

    if (seasonIndex == INVALID_INDEX) {
        printf("Season not found.\n");
        free(seasonName);

        return;
    }

    Season updatedSeason; 
    updatedSeason.next = show->seasons;
    Season *previousSeason = &updatedSeason;
    Season *temporarySeason;

    int index = 0;

    while (index < seasonIndex) {
        previousSeason = previousSeason->next;
        index++;
    }

    temporarySeason = previousSeason->next;
    previousSeason->next = temporarySeason->next;
    temporarySeason->next = NULL;
    freeSeason(temporarySeason);
    free(seasonName);
    show->seasons = updatedSeason.next;
}