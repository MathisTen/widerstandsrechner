#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdarg.h>
#include <locale.h>

int validateConsoleInput(char input[]);

char language[] = "de";
int main(void)
{
    char input[48] = "";
    while(true)
    {
        fgets(input, 48, stdin);
        input[strcspn(input, "\n")] = 0;
        int count = validateConsoleInput(input);
        printf("Count: %d\n", count);
        for(int i = 0; i <48; i++)input[i] = '\0';
    }
    return 0;
}

void stringToLower(char s[]) {
    for(int i = 0; s[i] != '\0'; i++) {
        s[i] = tolower(s[i]);
    }
}
int validateConsoleInput(char input[]) {

    char *token;
    char tmpInput[48];
    strcpy(tmpInput, input);

    int count = 0;

    token = strtok(tmpInput, "-");
    while(token != NULL) {
        #ifdef DEBUG
            printf("Durchlauf nummer: %i\n", count);
        #endif
        count++;
        if(count > 6) { //Ungueltig, wenn ueber sechs Farben
            #ifdef DEBUG
                printf("Zu viele\n");
            #endif
            return false;
        }

        if(strcmp(token, "schwarz") == 0 || strcmp(token, "black") == 0 || strcmp(token, "bk") == 0 || strcmp(token, "sw") == 0 ||
           strcmp(token, "braun") == 0 || strcmp(token, "brown") == 0 || strcmp(token, "bn") == 0 || strcmp(token, "br") == 0 ||
           strcmp(token, "rot") == 0 || strcmp(token, "red") == 0 || strcmp(token, "rd") == 0 || strcmp(token, "rt") == 0 ||
           strcmp(token, "orange") == 0 || strcmp(token, "orange") == 0 || strcmp(token, "og") == 0 || strcmp(token, "or") == 0 ||
           strcmp(token, "gelb") == 0 || strcmp(token, "yellow") == 0 || strcmp(token, "ye") == 0 || strcmp(token, "ge") == 0 ||
           strcmp(token, "gruen") == 0 || strcmp(token, "green") == 0 || strcmp(token, "gn") == 0 || 
           strcmp(token, "blau") == 0 || strcmp(token, "blue") == 0 || strcmp(token, "bu") == 0 || strcmp(token, "bl") == 0 ||
           strcmp(token, "violett") == 0 || strcmp(token, "violet") == 0 || strcmp(token, "vt") == 0 || strcmp(token, "vi") == 0 ||
           strcmp(token, "grau") == 0 || strcmp(token, "grey") == 0 || strcmp(token, "gy") == 0 || strcmp(token, "gr") == 0 ||
           strcmp(token, "weiss") == 0 || strcmp(token, "white") == 0 || strcmp(token, "wh") == 0 || strcmp(token, "ws") == 0 ||
           strcmp(token, "silber") == 0 || strcmp(token, "silver") == 0 || strcmp(token, "sr") == 0 || strcmp(token, "si") == 0 ||
           strcmp(token, "gold") == 0 || strcmp(token, "gd") == 0 || strcmp(token, "go") == 0 )
        {
            // Farbe ist gueltig
            #ifdef DEBUG
                printf("Farbe gueltig\n");
            #endif
        } else {
            // Farbe ist ungueltig
            #ifdef DEBUG
                printf("Token: %s\n", token);
                printf("Farbe ungueltig\n");
            #endif
            return false;
        }
        token = strtok(NULL, "-");
    }

    if(count < 3) { //Ungueltig, wenn unter drei Farben
        #ifdef DEBUG
            printf("Zu wenige Farben\n");
            printf("Anzahl Farben: %i\n", count);
        #endif
        return false;
    }
    #ifdef DEBUG
        printf("Anzahl: %i\n", count);
    #endif
    return count;
}