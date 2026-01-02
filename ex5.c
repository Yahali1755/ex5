/***********
 ID: 214007346
 NAME: Yahali Mashiach
***********/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INVALID_INDEX -1
#define MAX_HOURS 99
#define MINUTES_IN_HOUR 60
#define SECONDS_IN_MINUTE 60
#define EPISODE_STRING_LENGTH 8


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
char* getEpisodeLength();

void shrinkDB();
void expandDB();
void shiftDBRightFromIndex(int index);
void shiftDBLeftFromIndex(int index);

void freeEpisode(Episode *e);
void freeSeason(Season *s);
void freeShow(TVShow *show);
void freeAll();

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
void insertSeasonAtPosition(TVShow *show, Season *newSeason, int position);

TVShow* getShowFromUser();
Season* getSeasonFromUser(TVShow *show);

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

        string = (char *)realloc(string, (length + 1) * sizeof(char));

        if (string == NULL)
            return NULL;

        if (character != '\n') {
            string[length] = character;
            length++;
        } else {
            string[length] = '\0';
        }
    } while (character != '\n');

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

    // Ensure no shows are lost in the area we are removing
    for (int i = newSize; i < currentSize; i++) {
        if ((*database)[i] != NULL) {
            return;
        };
    }

    // If db is shrinked to be empty, reset database completely
    if (dbSize - 1 == 0) {
        free(*database);
        *database = NULL;
        dbSize = 0;

        return;
    }

    // Allocate space for n-1 x n-1 tv shows
    TVShow **newDatabase = (TVShow **)realloc(*database, newSize * sizeof(TVShow *));
    *database = newDatabase;
    dbSize--; 
}

TVShow* getShowFromUser() {
    printf("Enter the name of the show:\n");
    char *showName = getString();

    if (showName == NULL) 
        return NULL;

    int index = doesShowExist(showName);
    free(showName);

    if (index == INVALID_INDEX) {
        printf("Show not found.\n");

        return NULL;
    }

    return (*database)[index];
}

Season* getSeasonFromUser(TVShow *show) {
    printf("Enter the name of the season:\n");
    char *seasonName = getString();

    if (seasonName == NULL) 
        return NULL;

    Season *season = findSeason(show, seasonName);

    free(seasonName);

    if (season == NULL) {
        printf("Season not found.\n");

        return NULL;
    }

    return season;
}

Episode* getEpisodeFromUser(Season *season) {
    printf("Enter the name of the episode:\n");
    char *episodeName = getString();

    if (episodeName == NULL) 
        return NULL;

    Episode *episode = findEpisode(season, episodeName);

    free(episodeName);

    if (episode == NULL) {
        printf("Episode not found.\n");

        return NULL;
    }

    return episode;
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

Season* findSeason(TVShow *show, char *seasonName) {
    Season *season = show->seasons;

    while (season != NULL) {
        int isSeasonEqual = strcmp(season->name, seasonName);

        if (isSeasonEqual == 0) {
            return season;
        }

        season = season->next;
    }

    return NULL;
}

Episode* findEpisode(Season *season, char *episodeName) {
    Episode *episode = season->episodes;

    while (episode != NULL) {
        int isEpisodeEqual = strcmp(episode->name, episodeName);

        if (isEpisodeEqual == 0) {
            return episode;
        }

        episode = episode->next;
    }

    return NULL;
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
        printf("Show already exists.\n");
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

    if (newShow == NULL) {
        free(showName);

        return;
    }

    newShow->name = showName;
    newShow->seasons = NULL;
    (*database)[indexToAddShowIn] = newShow;
}

void deleteShow() {
    int dbTotalSize = dbSize * dbSize;
    
    TVShow *showToRemove = getShowFromUser();

    if (showToRemove == NULL) {        
        return;
    }

    int indexOfExistingShow = doesShowExist(showToRemove->name);
    freeShow(showToRemove);

    shiftDBLeftFromIndex(indexOfExistingShow);
    
    if (dbTotalSize == 0 || (*database)[dbTotalSize - 1] == NULL) {
        shrinkDB();
    }
} 

void insertSeasonAtPosition(TVShow *show, Season *newSeason, int position) {
    Season temporarySeason;
    temporarySeason.next = show->seasons;
    
    Season *iterator = &temporarySeason;
    int index = 0;

    while (index < position && iterator->next != NULL) {
        iterator = iterator->next;
        index++;
    }

    newSeason->next = iterator->next;
    iterator->next = newSeason;

    show->seasons = temporarySeason.next;
}


void insertEpisodeAtPosition(Season *season, Episode *newEpisode, int position) {
    Episode temporaryEpisode;
    temporaryEpisode.next = season->episodes;
    
    Episode *iterator = &temporaryEpisode;
    int index = 0;

    while (index < position && iterator->next != NULL) {
        iterator = iterator->next;
        index++;
    }

    newEpisode->next = iterator->next;
    iterator->next = newEpisode;

    season->episodes = temporaryEpisode.next;
}

void addSeason() {
    TVShow *show = getShowFromUser();

    if (show == NULL) {
        return;
    }
    
    printf("Enter the name of the season:\n");
    char *seasonName = getString();

    if (seasonName == NULL)
        return;

    if (findSeason(show, seasonName) != NULL) {
        printf("Season already exists.\n");
        free(seasonName);

        return;
    }

    Season *newSeason = (Season *)malloc(sizeof(Season));

    if (newSeason == NULL) {
        free(seasonName); 

        return;
    }

    newSeason->name = seasonName;
    newSeason->episodes = NULL;

    int position;
    printf("Enter the position:\n");
    position = getInt();

    insertSeasonAtPosition(show, newSeason, position);
}

void deleteSeason() {
    TVShow *show = getShowFromUser();

    if (show == NULL) 
        return;

    Season *season = getSeasonFromUser(show);

    if (season == NULL)
        return;

    Season updatedSeason; 
    updatedSeason.next = show->seasons;
    Season *previousSeason = &updatedSeason;

    while (previousSeason->next != season) {
        previousSeason = previousSeason->next;
    }

    previousSeason->next = season->next;
    season->next = NULL;
    freeSeason(season);
    show->seasons = updatedSeason.next;
}

// Validates that the episode length string matches the exact format 0-99:0-59:0-59.
int validLength(char *lengthString) {
    int maxValues[3] = {MAX_HOURS, MINUTES_IN_HOUR - 1, SECONDS_IN_MINUTE - 1};
    int index = 0;
    int part;

    // Ensure length is according to format
    if (strlen(lengthString) != EPISODE_STRING_LENGTH) {
        return 0;
    }

    for (int part = 0; part < 3; part++) {
        // Ensure exactly two characters at the current position are digits
        if (lengthString[index] < '0' || lengthString[index] > '9' ||
            lengthString[index + 1] < '0' || lengthString[index + 1] > '9') {
            return 0;
        }

        // Convert the two-digit string to integer
        int value = atoi(lengthString + index);
        
        // Validate the value based on the part
        if (value < 0 || value > maxValues[part]) {
            return 0;
        }

        index += 2;

        if (part < 2) {
            // Ensure hours and minutes are followed by a colon
            if (lengthString[index] != ':') {
                return 0;
            }

            index++; // Advance past the colon
        }
    }

    return 1;
}

char* getEpisodeLength() {
    char *episodeLength;
    printf("Enter the length (xx:xx:xx):\n");

    do {
        episodeLength = getString();

        if (episodeLength == NULL)
            return NULL;

        if (validLength(episodeLength)) {
            return episodeLength;
        } else {
           printf("Invalid length, enter again:\n");

           free(episodeLength);
        }
    } while (1);
}

void addEpisode() {
    TVShow *show = getShowFromUser();

    if (show == NULL)
        return;

    Season *season = getSeasonFromUser(show);

    if (season == NULL)
        return;

    printf("Enter the name of the episode:\n");
    char *episodeName = getString();

    if (episodeName == NULL) 
        return;

    if (findEpisode(season, episodeName) != NULL) {
        printf("Episode already exists.\n");
        free(episodeName);

        return;
    }

    char *episodeLength = getEpisodeLength();

    if (episodeLength == NULL) {
        free(episodeName);

        return;
    }

    Episode *newEpisode= (Episode *)malloc(sizeof(Episode));

    if (newEpisode == NULL) {
        free(episodeName);
        free(episodeLength);

        return;
    }

    newEpisode->name = episodeName;
    newEpisode->length = episodeLength;
    newEpisode->next = NULL;

    int position;
    printf("Enter the position:\n");
    position = getInt();

    insertEpisodeAtPosition(season, newEpisode, position);
}

void deleteEpisode() {
    TVShow *show = getShowFromUser();

    if (show == NULL) 
        return;

    Season *season = getSeasonFromUser(show);

    if (season == NULL) 
        return;

    Episode *episode = getEpisodeFromUser(season); 

    if (episode == NULL) 
        return;

    Episode updatedEpisode; 
    updatedEpisode.next = season->episodes;
    Episode *previousEpisode = &updatedEpisode;

    while (previousEpisode->next != episode) {
        previousEpisode = previousEpisode->next;
    }

    previousEpisode->next = episode->next;
    episode->next = NULL;
    
    freeEpisode(episode);
    
    season->episodes = updatedEpisode.next;
}

void freeAll() {
    if (database == NULL) {
        return;
    }

    // free content of each show and its place in the array the shows array
    if (*database != NULL) {
        int totalSize = dbSize * dbSize;

        for (int i = 0; i < totalSize; i++) {
            TVShow *show = (*database)[i];

            if (show != NULL) {
                freeShow(show);
            }
        }
        
        free(*database); 
    }

    free(database);
}

void printShow() {
    TVShow *show = getShowFromUser();

    if (show == NULL) {        
        return;
    }

    printf("Name: %s\n", show->name);
    printf("Seasons:\n");

    Season *season = show->seasons;
    int seasonIndex = 0;

    while (season != NULL) {
        printf("\tSeason %d: %s\n", seasonIndex, season->name);

        Episode *episode = season->episodes;
        int episodeIndex = 0;

        while (episode != NULL) {
            printf("\t\tEpisode %d: %s (%s)\n", episodeIndex, episode->name, episode->length);

            episodeIndex++;
            episode = episode->next;
        }

        seasonIndex++;
        season = season->next;
    }
}

void printEpisode() {
    TVShow *show = getShowFromUser();

    if (show == NULL) 
        return;

    Season *season = getSeasonFromUser(show);

    if (season == NULL) 
        return;

    Episode *episode = getEpisodeFromUser(season); 

    if (episode == NULL) 
        return;

    printf("Name: %s\n", episode->name);
    printf("Length: %s\n", episode->length);
}

void printArray() {
    if (database == NULL || *database == NULL) 
        return;

    for (int i = 0; i< dbSize; i++) {
        for (int j = 0; j< dbSize; j++) {
            TVShow *show = (*database)[(i * dbSize) + j];

            if (show == NULL) {
                printf("[NULL] ");
            } else {
                printf("[%s] ", show->name);
            }
        }

        printf("\n");
    }
}