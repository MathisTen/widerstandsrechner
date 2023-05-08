#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdarg.h>
#include <locale.h>

//#define DEBUG

//Wendet die Spracheinstellungen je nach Anwendersystem an
void setLanguage(char language[]);

//Prueft, ob das Programm in der Konsole oder als CGI-Script aufgerufen wurde (0 als Konsole, 1 als CGI)
int checkCall();

//Benutzeraufforderung zur Eingabe des Farb-String und speichert in input[], gibt die Anzahl der Woerter zurueck
int getInputConsole(char input[]);

//Auslesen der Eingabe 
int getInputCGI(char input[]);

//Wandelt alle Benutzereingaben in Kleinbuchstaben um
void stringToLower(char s[]);

//Prueft den eingegeben String auf die korrekte Syntax und gibt als Rueckgabewert die Anzahl der Farbwoerter zurueck, gibt "FALSE" zurueck, wenn der String nicht korrekt ist
int validateConsoleInput(char input_string[]);

//Prueft den ans CGI-Script uebergebenen String auf die korrekte Syntax und gibt als Rueckgabewert die Anzahl der Farbwoerter zurueck, gibt "FALSE" zurueck, wenn der String nicht korrekt ist
int validateHtmlInput(char input[]);

//Sorts the input, so that every new Word is exactly at pos 8*colournumber
void sortInput(char input[], int count);

//Splittet den Eingabestring um, die verschiedenen Farben werden in die einzelnen Variablen gespeichert
void separate(char input[], int count, char colour1[], char colour2[], char colour3[], char colour4[], char colour5[], char colour6[]);

//Gibt den Zahlenwert einer Farbe zurueck
int colourValue(char colour[]);

//Prueft, ob alle eingegebenen Farben an der jeweiligen Stelle moeglich sind
int checkResistorLogic(char input[], int count);

//Prueft je nach Ringnummer, ob die Farbe an dieser Position zulaessig ist
int checkRing(int count,int number, char colour[]);

//Wandelt bis zu sechs gegebene Woerter in die zugehoerigen Ziffern um
int* resistorDigits(int count, char input[]);

//berechnet aus gegebenen Zahlen den Widerstandswert
double calcResistorValue(int count, char input[]);

//gibt aus dem gegebenen Ring die Toleranz des Widerstands zurueck
double toleranceValue(int count, char input[]);

//gibt aus dem gegebenen Ring den Temperaturkoeffizienten des Widerstands zurueck
int temperatureCoefficientValue(char colour[]);

//Gibt die berechneten Werte in der Konsole aus
void printConsoleResult(double resValue, double tolerance, int tempCoefficient);

//Initialisert den CGI-Output, um einen Output im Browser zu ermoeglichen
void initHtmlOutput();

//Gibt den Widerstandswert als HTML aus
void printHtmlResult(double resValue, double tolerance, int tempCoefficient);

//Gibt die Fehlermeldung im Browser aus
void printHtmlError();

//Beendet den CGI-Output
void closeHtmlOutput();

char language[] = "de"; 


int main(void) {

    //Einstellen der Sprache
    setLanguage(language);

    //Abfragen der Aufrufumgebung
    int env = checkCall();
    
    //Wenn als CGI-Script, dann starte HTML-Ausgabe
    if(env) initHtmlOutput();       
    #ifdef DEBUG
    else printf("Konsole - Start\n");
    #endif

    //Deklarieren der Variable fuer den Input
    char input[48] = "";
    
    //Variable fuer die Anzahl der Woerter im eingegebenen String
    int count = 0;
    
    //Variablen fuer den Widerstand, die Toleranz und den Koeffizienten
    double resistorValue = 0;
    double tolerance = 0;
    int tempCoeff = 0;

    //Wenn in der Konsole, dann "normale" Ausfuehrung
    if(!env)
    {
        //syntaktisch korrekte Eingabe abfragen und in input speichern
        count = getInputConsole(input);

        //Sortieren des Inputs, sodass jedes neue Wort an einer definierten Position beginnt
        sortInput(input, count);
    
        //Pruefen der Logik der jeweiligen Ringe und erneute Eingabeaufforderung, wenn falsch
        while(!checkResistorLogic(input, count)) 
        {
            //Ausgabe der Fehlermeldung
            if(strcmp(language, "de")==0)printf("Bitte geben sie einen korrekten Widerstand ein.\n");
            else printf("Please enter a correct resistor.\n");

            //Erneute Eingabeaufforderung
            count = getInputConsole(input);

            //Erneutes Sortieren der Eingabe
            sortInput(input, count);
        }

        //Berechnung des Widerstandswert und der Toleranz
        resistorValue = calcResistorValue(count, input);
        tolerance = toleranceValue(count, input);

        //Wenn sechs Ringe, dann auch den Temperaturkoeffizienten
        if (count == 6) tempCoeff = temperatureCoefficientValue(&input[40]);

        //Konsolenausgabe
        printConsoleResult(resistorValue, tolerance, tempCoeff);
        
    }else   //Sonst Ausfuehrung als CGI-Script
    {
        //Abfrage der Eingabe aus den uebergebenen Parametern
        count = getInputCGI(input);
        
        if(!count) 
        {
            printHtmlError();
            closeHtmlOutput();
            return 0;  
        }

        //Pruefen der Logik der jeweiligen Ringe und erneute Eingabeaufforderung mit Programmende, wenn falsch
        if(!checkResistorLogic(input, count)) 
        {
            printHtmlError();
            closeHtmlOutput();
            return 0;
        }

        //Berechnung des Widerstandswert und der Toleranz
        resistorValue = calcResistorValue(count, input);
        tolerance = toleranceValue(count, input);
        //Wenn sechs Ringe, dann auch den Temperaturkoeffizienten
        if (count == 6) tempCoeff = temperatureCoefficientValue(&input[40]);

        //Ausgabe des Widerstandswertes 
        printHtmlResult(resistorValue, tolerance, tempCoeff);

        //Beenden der HTML-Ausgabe
        closeHtmlOutput(); 
    }  

    return 0;
}

void setLanguage(char language[])
{
    char* lang = getenv("LANG");
    if (lang != NULL) {
        
        language[0] = lang[0];
        language[1] = lang[1];
        #ifdef DEBUG
        printf("Sprache des Anwendersystems: %s\n", lang);
        printf("Language: %s\n", language);
        #endif
    } else {
        #ifdef DEBUG
        printf("Sprache des Anwendersystems nicht gefunden, setze auf Englisch\n");
        #endif
        strcpy(language, "en");
    }
    setlocale(LC_ALL, getenv( "LANG" ));
}

int checkCall()
{
    char* term = getenv("TERM");
    char* gateway = getenv("GATEWAY_INTERFACE");

    if (term != NULL)
    {
        #ifdef DEBUG
            //printf("Das Programm wurde ueber die Konsole gestartet.\n");
        #endif
        //Aufruf ueber die Konsole
        return 0;
    }
    else if (gateway != NULL)
    {
        #ifdef DEBUG
            //printf("Das Programm wurde als CGI-Skript gestartet.\n");
        #endif
        //Aufruf als CGI-Script
        return 1;
    }
    else
    {
        #ifdef DEBUG
            //printf("Die Ausfuehrungsumgebung konnte nicht erkannt werden.\n");
        #endif
        //Keine Ahnung, wie aufgerufen wurde, also Konsolenausgabe
        return 0;
    }
    return 0;
}

int getInputConsole(char input[]) {

    int count = 0;

    //Eingabe abfragen
    do {

        //Aufforderung zur Eingabe
        if(strcmp(language, "de")==0)printf("Bitte geben Sie den Eingabestring ein: ");
        else printf("Please enter your input-string: ");

        //Speichern der Eingabe aus stdin in input (max. 48 Zeichen)
        fgets(input, 48, stdin);

        //Entfernen des Zeilenumbruchs am Ende der Eingabe und ersetzen durch '\0'
        input[strcspn(input, "\n")] = 0;

        // Alle Woerter zu Kleinbuchstaben
        stringToLower(input);

        count = 0;

        //Pruefen auf syntaktische Korrektheit und Zaehlen der Farben (0 = Fehler, 3-6 = Anzahl der eingegebenen Farbringe
        count = validateConsoleInput(input);

        //Wenn fehlerhafte Eingabe, Aufforderung zur Korrektur ausgeben
        if(count == 0) 
        {   
            if(strcmp(language, "de")==0)printf("Ihre Eingabe ist Fehlerhaft, bitte erneut eingeben.\n");
            else printf("Your input was incorrect, pleas enter a correct input.\n");
        }
        
        //Eingabeabfrage wiederholen, wenn Eingabe fehlerhaft
    }while(count == 0);

    return count;
}

int getInputCGI(char input[])
{
    int count = 0;

    char *query = getenv("QUERY_STRING");

    if (query == NULL) return 0;

    for(int i = 0; i < 48; i++)
    {
        input[i] = '\0';
    }
   
    sscanf(query, "R1=%7[^&]&R2=%7[^&]&R3=%7[^&]&R4=%7[^&]&R5=%7[^&]&R6=%7s", &input[0], &input[8], &input[16], &input[24], &input[32], &input[40]); 
    
    stringToLower(input);
    count = validateHtmlInput(input);
    
    if(count == 0) 
            {   
                return 0;
            }
    return count;
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

int validateHtmlInput(char input[])
{
    int count = 0;
    int empty = 0;
    for (int i = 0; i < 6; i++)
    {
        if(input[(8*i)+7] != '\0') return false;
    }
    for (int i = 0; i < 6; i++)
    {
        if(input[8*i] != '\0')
        {
            if(empty == 1)return false;
            count++;
        }
        else empty = 1;
    }

    if (count<3)return false;
    else return count;
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
    #ifdef DEBUG
        printf("Separated in sort\n");
    #endif
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
    #ifdef DEBUG
        printf("Separate\n");
    #endif
    token = strtok(NULL, "-");
    strcpy(colour2, token);
    #ifdef DEBUG
        printf("Separate\n");
    #endif

    token = strtok(NULL, "-");
    strcpy(colour3, token);
    #ifdef DEBUG
        printf("Separate\n");
    #endif


    switch (count) {
        case 4:
            token = strtok(NULL, "-");
            strcpy(colour4, token);
            #ifdef DEBUG
                printf("Separate\n");
            #endif
            break;
        case 5:

            token = strtok(NULL, "-");
            strcpy(colour4, token);
            #ifdef DEBUG
                printf("Separate\n");
            #endif

            token = strtok(NULL, "-");
            strcpy(colour5, token);
            #ifdef DEBUG
                printf("Separate\n");
            #endif

            break;

        case 6:

            token = strtok(NULL, "-");
            strcpy(colour4, token);
            #ifdef DEBUG
                printf("Separate\n");
            #endif

            token = strtok(NULL, "-");
            strcpy(colour5, token);
            #ifdef DEBUG
                printf("Separate\n");
            #endif

            token = strtok(NULL, "-");
            strcpy(colour6, token);
            #ifdef DEBUG
                printf("Separate\n");
            #endif

            break;

    }
}

int colourValue(char colour[])
{
    //ISO Norm fuer Farbabkuerzungen
    if(strcmp(colour, "schwarz") == 0 || strcmp(colour, "black") == 0 || strcmp(colour, "bk") == 0 || strcmp(colour, "sw") == 0) return 0;
    if(strcmp(colour, "braun") == 0 || strcmp(colour, "brown") == 0 || strcmp(colour, "bn") == 0 || strcmp(colour, "br") == 0) return 1;
    if(strcmp(colour, "rot") == 0 || strcmp(colour, "red") == 0 || strcmp(colour, "rd") == 0  || strcmp(colour, "rt") == 0) return 2;
    if(strcmp(colour, "orange") == 0 || strcmp(colour, "orange") == 0 || strcmp(colour, "og") == 0 || strcmp(colour, "or") == 0) return 3;
    if(strcmp(colour, "gelb") == 0 || strcmp(colour, "yellow") == 0 || strcmp(colour, "ye") == 0 || strcmp(colour, "ge" ) == 0) return 4;
    if(strcmp(colour, "gruen") == 0 || strcmp(colour, "green") == 0 || strcmp(colour, "gn") == 0 ) return 5;
    if(strcmp(colour, "blau") == 0 || strcmp(colour, "blue") == 0 || strcmp(colour, "bu") == 0 || strcmp(colour, "bl") == 0 ) return 6;
    if(strcmp(colour, "violett") == 0 || strcmp(colour, "violet") == 0 || strcmp(colour, "vt") == 0 || strcmp(colour, "vi") == 0 ) return 7;
    if(strcmp(colour, "grau") == 0 || strcmp(colour, "grey") == 0 || strcmp(colour, "gy") == 0 || strcmp(colour, "gr") == 0 ) return 8;
    if(strcmp(colour, "weiss") == 0 || strcmp(colour, "white") == 0 || strcmp(colour, "wh") == 0 || strcmp(colour, "ws") == 0 ) return 9;
    if(strcmp(colour, "silber") == 0 || strcmp(colour, "silver") == 0 || strcmp(colour, "sr") == 0 || strcmp(colour, "si") == 0 ) return -2;
    if(strcmp(colour, "gold") == 0 || strcmp(colour, "go") == 0 ) return -1;
    return 100;
}

int checkResistorLogic(char input[], int count)
{
    //test the first ring on logic
    if(checkRing(count, 1, &input[0]))
    {
        //test the second ring on logic
        if(checkRing(count, 2, &input[8]))
        {  
            //test the third ring on logic
            if(checkRing(count, 3, &input[16]))
            {
                //If more than three rings
                if(count >3)
                {
                    //check the fourth ring on logic
                    if(checkRing(count, 4, &input[24]))
                    {
                        //If more than four rings 
                        if(count >4)
                        {   
                            //check the fifth ring on logic
                            if(checkRing(count, 5, &input[32]))
                            {
                                //If more than five rings
                                if(count>5)
                                {
                                    //check the sixth ring on logic
                                    if(checkRing(count, 6, &input[40]))return true;
                                }else return true;
                            }
                        }else return true;
                    }
                }else return true;
            }
        }
    }
    return false;
}

int checkRing(int count, int number, char colour[]) {

    if ( count == 3 || count == 4 )
    { 
        if(strcmp(colour, "") == 0) return 0;
        switch( number)
        {
        case 1: 
            if(strcmp(colour, "schwarz") == 0 || strcmp(colour, "black") == 0 || strcmp(colour, "bk") == 0) return 0;
        case 2:
            //test fuer ring 1 und 2
            if(strcmp(colour, "gold") == 0 || strcmp(colour, "au") == 0) return 0;
            if(strcmp(colour, "silber") == 0 || strcmp(colour, "silver") == 0 || strcmp(colour, "ag") == 0) return 0;
            break;
        case 3:
            // test fuer Ring 3: Alle Farben erlaubt
            break;
        case 4:
        // test fuer Ring 4
        if(strcmp(colour, "schwarz") == 0 || strcmp(colour, "black") == 0 || strcmp(colour, "bk") == 0) return 0;
        if(strcmp(colour, "orange") == 0 || strcmp(colour, "orange") == 0 || strcmp(colour, "og") == 0) return 0;
        if(strcmp(colour, "gelb") == 0 || strcmp(colour, "yellow") == 0 || strcmp(colour, "yl") == 0) return 0;
        if(strcmp(colour, "weiss") == 0 || strcmp(colour, "white") == 0 || strcmp(colour, "wh") == 0) return 0;
        break;
        }
        
    }else if(count == 5 || count == 6 )
    {
        if(strcmp(colour, "") == 0) return 0;
        switch (number)
        { case 1:
            if(strcmp(colour, "schwarz") == 0 || strcmp(colour, "black") == 0 || strcmp(colour, "bk") == 0) return 0;
          case 2:
          case 3:
            // test fuer ring 1,2,3)
            if(strcmp(colour, "gold") == 0 || strcmp(colour, "au") == 0) return 0;
            if(strcmp(colour, "silber") == 0 || strcmp(colour, "silver") == 0 || strcmp(colour, "ag") == 0) return 0;
            break;
         case 4:
            // test ring 4
            if(strcmp(colour, "violett") == 0 || strcmp(colour, "violet") == 0 || strcmp(colour, "vi") == 0) return 0;
            if( strcmp(colour, "grau") == 0 || strcmp(colour, "grey") == 0 || strcmp(colour, "gy") ==0) return 0;
            if(strcmp(colour, "weiss") == 0 || strcmp(colour, "white") == 0 || strcmp(colour, "wh") == 0) return 0;
            break;
         case 5:
         // test fuer ring 5
            if(strcmp(colour, "gold") == 0 || strcmp(colour, "au") == 0) return 0;
            if(strcmp(colour, "silber") == 0 || strcmp(colour, "silver") == 0 || strcmp(colour, "ag") == 0) return 0;
            if(strcmp(colour, "schwarz") == 0 || strcmp(colour, "black") == 0 || strcmp(colour, "bk") == 0) return 0;
            if(strcmp(colour, "orange") == 0 || strcmp(colour, "orange") == 0 || strcmp(colour, "og") == 0) return 0;
            if(strcmp(colour, "gelb") == 0 || strcmp(colour, "yellow") == 0 || strcmp(colour, "yl") == 0) return 0;
            if(strcmp(colour, "weiss") == 0 || strcmp(colour, "white") == 0 || strcmp(colour, "wh") == 0) return 0;
         break;
         case 6:
         // test fuer ring 6
            if(strcmp(colour, "gold") == 0 || strcmp(colour, "au") == 0) return 0;
            if(strcmp(colour, "silber") == 0 || strcmp(colour, "silver") == 0 || strcmp(colour, "ag") == 0) return 0;
            if(strcmp(colour, "weiss") == 0 || strcmp(colour, "white") == 0 || strcmp(colour, "wh") == 0) return 0;
            if( strcmp(colour, "grau") == 0 || strcmp(colour, "grey") == 0 || strcmp(colour, "gy") ==0) return 0;
            if(strcmp(colour, "gruen") == 0 || strcmp(colour, "gruen") == 0 || strcmp(colour, "green") == 0 || strcmp(colour, "gn") == 0) return 0;
         break;
        }
    
    }
    
    return 1;

}

int* resistorDigits(int count, char input[])
{
    
    int* ringvalues = malloc(count * sizeof(int));
    //Die ersten drei Ringe sind immer vorhanden, die jeweiligen Farbwerte werden mit Colourvalue zugeordnet
    ringvalues[0] = colourValue(&input[0]);
    ringvalues[1] = colourValue(&input[8]);
    ringvalues[2] = colourValue(&input[16]);

    //Je nach Anzahl der Ringe werden die Ringe 4-6 auch in Zahlen uebersetzt
    switch (count) {
        case 4:
            ringvalues[3] = colourValue(&input[24]);
            break;
        case 5:
            ringvalues[3] = colourValue(&input[24]);
            ringvalues[4] = colourValue(&input[32]);
            break;
        case 6:
            ringvalues[3] = colourValue(&input[24]);
            ringvalues[4] = colourValue(&input[32]);
            ringvalues[5] = colourValue(&input[40]);
            break;
    }
    return ringvalues;
}

double calcResistorValue(int count, char input[]) {

    #ifdef DEBUG
        printf("Calculating...\n");
    #endif
    int* values = resistorDigits(count, input);

    #ifdef DEBUG
        printf("Calculating 2...\n");
        for(int i = 0; i<count;i++) {
            printf("Value %i: %i\n", i, values[i]);
        }
    #endif
    double multiplicator;
    switch (count) {
        case 3:
            #ifdef DEBUG
                printf("Case 3\n");
            #endif
        case 4:
            #ifdef DEBUG
                printf("Case 4\n");
            #endif    
            multiplicator = pow(10, (double)values[2]);
            return (values[0]*10+values[1]*1)*multiplicator;
            break;
        case 5:
        case 6:
            multiplicator = pow(10, (double)values[3]);
            return (values[0]*100 + values[1]*10 + values[2]*1)*multiplicator;
            break;
        default:
            return 0;
            break;
    }
}

double toleranceValue(int count, char input[])
{
    char colour[8];
    switch (count)
    {
        case 3:
            return 20;
            break;
        case 4: 
            strcpy(colour, &input[24]);
            break;
        case 5:
        case 6:

            strcpy(colour, &input[32]);
            break;
        default:
            break;
    }

    int ringValue = colourValue(colour);
    switch(ringValue) 
    {
        case -1:
            return 5;
            break;
        case -2:
            return 10;
            break;
        case 1:
            return 1;
            break;
        case 2:
            return 2;
            break;
        case 5:
            return 0.5;
            break;
        case 6:
            return 0.25;
            break;
        case 7:
            return 0.1;
            break;
        case 8:
            return 0.05;
            break;
        default:
        printf("default\n");
            return 0;
            break;
    }
    return 13;
}

int temperatureCoefficientValue(char colour[])
{
    int ringValue = colourValue(colour);
    switch(ringValue)
    {
        case 0:
            return 200;
            break;
        case 1:
            return 100;
            break;
        case 2:
            return 50;
            break;
        case 3:
            return 15;
            break;
        case 4: 
            return 25;
            break;
        case 6:
            return 10;
            break;
        case 7:
            return 5;
            break; 
        default:
            break;
    }

}

void printConsoleResult(double resValue, double tolerance, int tempCoefficient)
{
    
    int multiplicator = 0;
    while(resValue > 1000 || resValue < 1)
    {
        if(resValue >1000)
        {
            resValue /= 1000;
            multiplicator ++;
        }
        if(resValue < 1)
        {
            resValue *= 1000;
            multiplicator --;            
        }
    }
    
    //Ausgabe des Wiederstandswertes
    if(strcmp(language, "de")==0)printf("Widerstandswert:");
    else printf("Resistor Value:");
    printf("%.lf", resValue);
    
    switch (multiplicator)
    {
        case -1:
            printf("m");
            break;
        case 0:
            break;
        case 1:
            printf("k");
            break;
        case 2:
            printf("M");
            break;
        case 3:
            printf("G");
            break;
        default:
            break;
    }
    
    printf("Ohm\n");

    //Ausgabe der Toleranz
    if(strcmp(language, "de")==0)printf("Toleranz:");
    else printf("Tolerance:");
    printf("%.2lf", tolerance);
    printf("%%\n");

    //Wenn sechs Ringe, dann gib Koeffizienten aus
    if(tempCoefficient != 0) printf("Temp: %d ppm/K\n", tempCoefficient);
}

void initHtmlOutput()
{
    //initialisieren der Web-Oberflaeche
    printf("Content-Type: text/html\n");   
}

void printHtmlResult(double resValue, double tolerance, int tempCoefficient)
{
    char output[1000] = "http://localhost/result.html?R=";
    char intStr[10];
    int multiplicator = 0;
    while(resValue > 1000 || resValue < 1)
    {
        if(resValue >1000)
        {
            resValue /= 1000;
            multiplicator ++;
        }
        if(resValue < 1)
        {
            resValue *= 1000;
            multiplicator --;            
        }
    }

    sprintf(intStr, "%.2lf", resValue);
    
    strcat(output, intStr);

    switch (multiplicator)
    {
        case -1:
            strcat(output, " m");
            break;
        case 0:
            break;
        case 1:
            strcat(output, " k");
            break;
        case 2:
            strcat(output, " M");
            break;
        case 3:
            strcat(output, " G");
            break;
        default:
            break;
    }
    strcat(output, "&T=");
    
    if(!tolerance)strcat(output,"20");
    else 
    {
        sprintf(intStr, "%.2lf", tolerance);
        strcat(output, intStr);
        strcat(output, "&tK=");

    }
    if(tempCoefficient)
    {
        sprintf(intStr, "%d", tempCoefficient);
        strcat(output, intStr);
    }else
    {
        strcat(output, "0");
    }
    printf("Location: %s\n", output);
    printf("Status: 302 Found\n\n");
    printf("<html><head><title>Weiterleitung</title></head>\n");
    printf("<body><h1>Ergebnisse werden berechnet</h1>\n");
}

void printHtmlError()
{
    //printf("Content-Type: text/html\n"); 
    printf("\n<html>\n");
    printf("<head>\n");
    printf("<title>Fehler bei der Eingabe</title>\n");
    printf("</head>\n");
    printf("<body>\n");
    printf("Keine Korrekte Eingabelogik. </br>");
    printf("Bitte kehren sie zur <a href=\"index.html\" >Eingabeseite</a> zurueck.</br>");
}

void closeHtmlOutput()
{
    printf("</body>\n");
    printf("</html>\n");
}
