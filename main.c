#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdarg.h>
#define DEBUG

//Benutzeraufforderung zur Eingabe des Farb-String und speichert in input[], gibt die Anzahl der Wörter zurück
int getInput(char input[]);

//Wandelt alle Benutzereingaben in Kleinbuchstaben um
void stringToLower(char s[]);

//Prüft den eingegeben String auf die korrekte Syntax und gibt als Rückgabewert die Anzahl der Farbwörter zurück, gibt "" zurück, wenn der String nicht korrekt ist
int validate_input(char input_string[]);

//Sorts the input, so that every new Word is exactly at pos 8*colournumber
void sortInput(char input[], int count);

//Splittet den Eingabestring um, die verschiedenen Farben werden in die einzelnen Variablen gespeichert
void separate(char input[], int count, char colour1[], char colour2[], char colour3[], char colour4[], char colour5[], char colour6[]);

//Prüft je nach Ringnummer, ob die Farbe an dieser Position zulässig ist
int checkRing(int count,int number, char colour[]);

//Gibt den Zahlenwert einer Farbe zurück
int colourvalue(char colour[]);

//Wandelt bis zu sechs gegebene Wörter in die zugehörigen Ziffern um
void resistorDigits(int count, char colour1[], char colour2[], char colour3[], char colour4[], char colour5[], char colour6[], int* ring1, int* ring2, int* ring3, int* ring4, int* ring5, int* ring6);

//berechnet aus gegebenen Zahlen den Widerstandswert
int widerstandswert(int count, int wert1, int wert2, int wert3, int wert4);


int main() {

    //Deklarieren der Variablen für den Input und die Farbwörter
    char input[48] = "";
    char colour1[8] = "";
    char colour2[8] = "";
    char colour3[8] = "";
    char colour4[8] = "";
    char colour5[8] = "";
    char colour6[8] = "";

    //Variable für die Anzahl der Wörter im eingegebenen String
    int count = 0;

    //Variablen für die Farbwerte
    int ring1, ring2, ring3, ring4, ring5, ring6;

    //syntaktisch korrekte Eingabe abfragen und in input speichern
    count = getInput(input);

    //Hier haben wir in input "schwarz-rot-gelb-gruen" und die Anzahl der Wörter in count

    //Sortieren des Inputs, sodass jedes neue Wort an einer definierten Position beginnt
    sortInput(input, count);
   
    //Prüfen der Logik der jeweiligen Ringe



    //Schreiben der Zahlen je nach Farbwort
    //resistorDigits(count, colour1, colour2, colour3, colour4, colour5, colour6, &ring1, &ring2, &ring3, &ring4, &ring5, &ring6);



    //Berechnen des Widerstandswertes mit den

    /*
    printf("Farbe 1: %s\n", colour1);
    printf("Farbe 2: %s\n", colour2);
    printf("Farbe 3: %s\n", colour3);
    printf("Farbe 4: %s\n", colour4);
    printf("Farbe 5: %s\n", colour5);
    printf("Farbe 6: %s\n", colour6);
    /*
    printf("Farbwert 1: %i\n", colourvalue(colour1));
    printf("Farbwert 2: %i\n", colourvalue(colour2));
    printf("Farbwert 3: %i\n", colourvalue(colour3));
    printf("Farbwert 4: %i\n", colourvalue(colour4));
    printf("Farbwert 5: %i\n", colourvalue(colour5));
    printf("Farbwert 6: %i\n", colourvalue(colour6));
    printf("Wert: %i\n", widerstandswert(count, colourvalue(colour1), colourvalue(colour2), colourvalue(colour3), colourvalue(colour4)));

    int widerstand = (wert1 * 100 + wert2 * 10 + wert3 * 1)* pow(10, wert4);
    printf("Widerstand: %i Ohm\n", widerstand);
    */
    return 0;

}

int getInput(char input[]) {

    int count;

    //Aufforderung zur Eingabe
    printf("Bitte geben Sie den Eingabestring ein: ");

    //Eingabe abfragen
    do {
        //Speichern der Eingabe aus stdin in input (max. 48 Zeichen)
        fgets(input, 48, stdin);

        //Entfernen des Zeilenumbruchs am Ende der Eingabe und ersetzen durch '\0'
        input[strcspn(input, "\n")] = 0;

        // Alle Wörter zu Kleinbuchstaben
        stringToLower(input);

        //Prüfen auf syntaktische Korrektheit und Zählen der Farben (0 = Fehler, 3-6 = Anzahl der eingegebenen Farbringe
        count = validate_input(input);

        //Wenn fehlerhafte Eingabe, Aufforderung zur Korrektur ausgeben
        if(count == 0) printf("Ihre Eingabe ist Fehlerhaft, bitte erneut eingeben.\n");

        //Eingabeabfrage wiederholen, wenn Eingabe fehlerhaft
    }while(count == 0);

}

void stringToLower(char s[]) {
    int i;
    for(i = 0; s[i] != '\0'; i++) {
        s[i] = tolower(s[i]);
    }
}

int validate_input(char input[]) {

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
        if(count > 6) { //Ungültig, wenn über sechs Farben
        #ifdef DEBUG
            printf("Zu viele\n");
        #endif
            return false;
        }

        if(strcmp(token, "schwarz") == 0 || strcmp(token, "black") == 0 || strcmp(token, "bk") == 0 ||
           strcmp(token, "braun") == 0 || strcmp(token, "brown") == 0 || strcmp(token, "bn") == 0 ||
           strcmp(token, "rot") == 0 || strcmp(token, "red") == 0 || strcmp(token, "rd") == 0 ||
           strcmp(token, "orange") == 0 || strcmp(token, "orange") == 0 || strcmp(token, "og") == 0 ||
           strcmp(token, "gelb") == 0 || strcmp(token, "yellow") == 0 || strcmp(token, "yl") == 0 ||
           strcmp(token, "gruen") == 0 || strcmp(token, "grün") == 0 || strcmp(token, "green") == 0 || strcmp(token, "gn") == 0 ||
           strcmp(token, "blau") == 0 || strcmp(token, "blue") == 0 || strcmp(token, "bu") == 0 ||
           strcmp(token, "violett") == 0 || strcmp(token, "violet") == 0 || strcmp(token, "vi") == 0 ||
           strcmp(token, "grau") == 0 || strcmp(token, "grey") == 0 || strcmp(token, "gy") == 0 ||
           strcmp(token, "weiss") == 0 || strcmp(token, "white") == 0 || strcmp(token, "wh") == 0 ||
           strcmp(token, "silber") == 0 || strcmp(token, "silver") == 0 || strcmp(token, "ag") == 0 ||
           strcmp(token, "gold") == 0 || strcmp(token, "au") == 0 ) {
            // Farbe ist gültig
#ifdef DEBUG
            printf("Farbe gültig\n");
#endif
        } else {
            // Farbe ist ungültig
#ifdef DEBUG
            printf("Token: %s\n", token);
            printf("Farbe ungültig\n");
#endif
            return false;
        }
        token = strtok(NULL, "-");
    }

    if(count < 3) { //Ungültig, wenn unter drei Farben
#ifdef DEBUG
        printf("Zu wenige Farben\n");
        printf("Anzahl Farben: %i\n", count);
#endif
        return false;
    }
    printf("Anzahl: %i\n", count);
    return count;
}

void sortInput(char input[], int count) {
    //Sorts the input, so that every new Word is exactly at pos 8*colournumber

    char colour1[8] = "";
    char colour2[8] = "";
    char colour3[8] = "";
    char colour4[8] = "";
    char colour5[8] = "";
    char colour6[8] = "";

    separate(input, count, colour1, colour2, colour3, colour4, colour5, colour6);
    printf("Separated in sort\n");
    strcpy(input, colour1);
    strcpy(&input[8], colour2);
    strcpy(&input[16], colour3);
    switch (count) {
        case 4:
            strcpy(&input[24], colour4);
            break;
        case 5:
            strcpy(&input[24], colour4);
            strcpy(&input[32], colour5);
            break;
        case 6:
            strcpy(&input[24], colour4);
            strcpy(&input[32], colour5);
            strcpy(&input[40], colour6);
            break;
    }


}

void separate(char input[],int count, char colour1[], char colour2[], char colour3[], char colour4[], char colour5[], char colour6[])
{
#ifdef DEBUG
    printf("Separate\n");
#endif
    char *token;

    token = strtok(input, "-");
    strcpy(colour1, token);
    printf("Separate\n");

    token = strtok(NULL, "-");
    strcpy(colour2, token);
    printf("Separate\n");

    token = strtok(NULL, "-");
    strcpy(colour3, token);
    printf("Separate\n");


    switch (count) {
        case 4:
            token = strtok(NULL, "-");
            strcpy(colour4, token);
            printf("Separate\n");
            break;
        case 5:

            token = strtok(NULL, "-");
            strcpy(colour4, token);
            printf("Separate\n");

            token = strtok(NULL, "-");
            strcpy(colour5, token);
            printf("Separate\n");

            break;

        case 6:

            token = strtok(NULL, "-");
            strcpy(colour4, token);
            printf("Separate\n");

            token = strtok(NULL, "-");
            strcpy(colour5, token);
            printf("Separate\n");

            token = strtok(NULL, "-");
            strcpy(colour6, token);
            printf("Separate\n");

            break;

    }
}

int colourvalue(char colour[])
{
    //Ändern in zweidimensionales Array !!!
    //ISO Norm für Farbabkürzungen
    if(strcmp(colour, "schwarz") == 0 || strcmp(colour, "black") == 0 || strcmp(colour, "bk") == 0) return 0;
    if(strcmp(colour, "braun") == 0 || strcmp(colour, "brown") == 0 || strcmp(colour, "bn") == 0) return 1;
    if(strcmp(colour, "rot") == 0 || strcmp(colour, "red") == 0 || strcmp(colour, "rd") == 0 ) return 2;
    if(strcmp(colour, "orange") == 0 || strcmp(colour, "orange") == 0 || strcmp(colour, "og") == 0 ) return 3;
    if(strcmp(colour, "gelb") == 0 || strcmp(colour, "yellow") == 0 || strcmp(colour, "yl") == 0 )return 4;
    if(strcmp(colour, "gruen") == 0 || strcmp(colour, "green") == 0 || strcmp(colour, "gn") == 0 ) return 5;
    if(strcmp(colour, "blau") == 0 || strcmp(colour, "blue") == 0 || strcmp(colour, "bu") == 0 ) return 6;
    if(strcmp(colour, "violett") == 0 || strcmp(colour, "violet") == 0 || strcmp(colour, "vi") == 0 ) return 7;
    if(strcmp(colour, "grau") == 0 || strcmp(colour, "grey") == 0 || strcmp(colour, "gy") == 0 ) return 8;
    if(strcmp(colour, "weiss") == 0 || strcmp(colour, "white") == 0 || strcmp(colour, "wh") == 0 ) return 9;
    if(strcmp(colour, "silber") == 0 || strcmp(colour, "silver") == 0 || strcmp(colour, "sr") == 0 ) return -1;
    if(strcmp(colour, "gold") == 0 || strcmp(colour, "gd") == 0 ) return -10;
    return 100;
}

int checkResistorLogic(char input[], int count)
{
    switch (count) {
        
    }
    return 0;
}

int checkRing(int count, int number, char colour[]) {

    //Holger
    return 1;

}


void resistorDigits(int count, char colour1[], char colour2[], char colour3[], char colour4[], char colour5[], char colour6[], int* ring1, int* ring2, int* ring3, int* ring4, int* ring5, int* ring6)
{
    //Die ersten drei Ringe sind immer vorhanden, die jeweiligen Farbwerte werden mit Colourvalue zugeordnet
    *ring1 = colourvalue(colour1);
    *ring2 = colourvalue(colour2);
    *ring3 = colourvalue(colour3);

    //Je nach Anzahl der Ringe werden die Ringe 4-6 auch in Zahlen übersetzt
    switch (count) {
        case 4:
            *ring4 = colourvalue(colour4);
            break;
        case 5:
            *ring4 = colourvalue(colour4);
            *ring5 = colourvalue(colour5);
            break;
        case 6:
            *ring4 = colourvalue(colour4);
            *ring5 = colourvalue(colour5);
            *ring6 = colourvalue(colour6);
            break;
    }
}

int widerstandswert(int count, int wert1, int wert2, int wert3, int wert4) {

    switch (count) {
        case 3:
        case 4:
            return (wert1*10+wert2*1)*pow(10,wert3);
            break;
        case 5:
        case 6:
            return (wert1*100 + wert2*10 + wert3*1)*pow(10, wert4);
            break;
    }
}