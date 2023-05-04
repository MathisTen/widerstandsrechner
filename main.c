#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdarg.h>
//#define DEBUG


//Benutzeraufforderung zur Eingabe des Farb-String und speichert in input[], gibt die Anzahl der Wörter zurück
int getInputConsole(char input[]);

//Auslesen der Eingabe 
int getInputCGI(char input[]);

//Wandelt alle Benutzereingaben in Kleinbuchstaben um
void stringToLower(char s[]);

//Prüft, ob die Eingabe eine Einstellung ist
int checkForSetting(char input[]);

//Wendet die Einstellung an
int applySetting(char input[]);

//Zählt die eingegebenen Farben
int countInput(char input[]);

//Prüft den eingegeben String auf die korrekte Syntax und gibt als Rückgabewert die Anzahl der Farbwörter zurück, gibt "FALSE" zurück, wenn der String nicht korrekt ist
int validateConsoleInput(char input_string[]);

//Prüft den ans CGI-Script übergebenen String auf die korrekte Syntax und gibt als Rückgabewert die Anzahl der Farbwörter zurück, gibt "FALSE" zurück, wenn der String nicht korrekt ist
int validateHtmlInput(char input[]);

//Sorts the input, so that every new Word is exactly at pos 8*colournumber
void sortInput(char input[], int count);

//Splittet den Eingabestring um, die verschiedenen Farben werden in die einzelnen Variablen gespeichert
void separate(char input[], int count, char colour1[], char colour2[], char colour3[], char colour4[], char colour5[], char colour6[]);

//Gibt den Zahlenwert einer Farbe zurück
int colourValue(char colour[]);

//Prüft, ob alle eingegebenen Farben an der jeweiligen Stelle möglich sind
int checkResistorLogic(char input[], int count);

//Prüft je nach Ringnummer, ob die Farbe an dieser Position zulässig ist
int checkRing(int count,int number, char colour[]);

//Wandelt bis zu sechs gegebene Wörter in die zugehörigen Ziffern um
int* resistorDigits(int count, char input[]);

//berechnet aus gegebenen Zahlen den Widerstandswert
double calcResistorValue(int count, char input[]);

//gibt aus dem gegebenen Ring die Toleranz des Widerstands zurück
double toleranceValue(int count, char input[]);

//gibt aus dem gegebenen Ring den Temperaturkoeffizienten des Widerstands zurück
int temperatureCoefficientValue(char colour[]);

//Gibt die berechneten Werte in der Konsole aus
void printConsoleResult(float resValue, float tolerance, int tempCoefficient);

//Prüft, ob das Programm in der Konsole oder als CGI-Script aufgerufen wurde (0 als Konsole, 1 als CGI)
int checkCall();

//Gibt den Widerstandswert auf der Konsole aus
void printResValue(int resValue);

//Initialisert den CGI-Output, um einen Output im Browser zu ermöglichen
void initHtmlOutput();

//Gibt den Widerstandswert als HTML aus
void printHtmlResult(float resValue, float tolerance, int tempCoefficient);

//Beendet den CGI-Output
void closeHtmlOutput();

char language[] = "de"; 


int main() {

    //Abfragen der Aufrufumgebung
    int env = checkCall();
    
    //Wenn als CGI-Script, dann starte HTML-Ausgabe
    if(env) initHtmlOutput();       
    else printf("Konsole - Start\n");

    //Deklarieren der Variable für den Input
    char input[48] = "";
    
    //Variable für die Anzahl der Wörter im eingegebenen String
    int count = 0;
    
    //Variablen für den Widerstand, die Toleranz und den Koeffizienten
    double resistorValue = 0;
    double tolerance = 12;
    int tempCoeff = 0;

    //Wenn in der Konsole, dann "normale" Ausführung
    if(!env)
    {
        //syntaktisch korrekte Eingabe abfragen und in input speichern
        count = getInputConsole(input);

        //Sortieren des Inputs, sodass jedes neue Wort an einer definierten Position beginnt
        sortInput(input, count);
    
        //Prüfen der Logik der jeweiligen Ringe und erneute Eingabeaufforderung, wenn falsch
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
        
    }else 
    {
        //Sonst Ausführung als CGI-Script

        //Abfrage der Eingabe aus den übergebenen Parametern
        count = getInputCGI(input);
        printf("Count: %d</br>", count);
        printf("Input: ");
        for(int i = 0; i < 48; i++)
        {
            printf("%c", input[i]);
            if((i + 1) % 8 == 0)printf("</br>");
        }
        printf("</br>");
        //Wenn Count 0, dann kein Korrekter String.
        // TODO: Fehlerhafte Eingabe zurückmelden
        if(!count) return 0;  

        //Prüfen der Logik der jeweiligen Ringe une erneute Eingabeaufforderung mit Programmende, wenn falsch
        if(!checkResistorLogic(input, count)) 
        {
            printf("Keine Korrekte Eingabelogik. </br>");
            printf("Bitte kehren sie zur <a href=\"test_cgi.html\" >Eingabeseite</a> zurück.</br>");
            return 0;
        }
        //Ausgabe des Widerstandswertes 
        resistorValue = calcResistorValue(count, input);
        printf("Der Widerstand beträgt %lf Ohm.</br>", resistorValue);

        closeHtmlOutput(); 
    }  

    return 0;
}



int getInputConsole(char input[]) {

    int count = 0;

    //Aufforderung zur Eingabe
    if(strcmp(language, "de")==0)printf("Bitte geben Sie den Eingabestring ein: ");
    else printf("Please enter your input-string: ");

    //Eingabe abfragen
    do {
        //Speichern der Eingabe aus stdin in input (max. 48 Zeichen)
        fgets(input, 48, stdin);

        //Entfernen des Zeilenumbruchs am Ende der Eingabe und ersetzen durch '\0'
        input[strcspn(input, "\n")] = 0;

        // Alle Wörter zu Kleinbuchstaben
        stringToLower(input);

        count = 0;

        //Wenn Eingabe ist Einstellung -> Einstellung ändern
        if(checkForSetting(input)) 
        {
            //Einstellungen ändern ...
            if(applySetting(input))
            {                
                //Gültige Einstellung
                if(strcmp(language, "de")==0)
                {
                    printf("Einstellungen geändert.\n");
                    printf("Bitte geben Sie einen Eingabestring ein: ");
                }else
                {
                    printf("Settings changed.\n");
                    printf("Please enter an input-string: ");
                }
            }else
            {   
                //Ungültige Einstellung
                if(strcmp(language, "de")==0)
                {
                    printf("Keine gültige Einstellung.\n");
                    printf("Bitte geben Sie eine gültige Einstellung oder einen Eingabestring ein: ");
                }else
                {
                    printf("No vaild setting.\n");
                    printf("Please enter a valid setting or an input-string: ");
                }
            }
        }else
        {            
            //Prüfen auf syntaktische Korrektheit und Zählen der Farben (0 = Fehler, 3-6 = Anzahl der eingegebenen Farbringe
            count = validateConsoleInput(input);

            //Wenn fehlerhafte Eingabe, Aufforderung zur Korrektur ausgeben
            if(count == 0) 
            {   
                if(strcmp(language, "de")==0)printf("Ihre Eingabe ist Fehlerhaft, bitte erneut eingeben.\n");
                else printf("Your input was incorrect, pleas enter a correct input.\n");
            }
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

    printf("Eingegebener Query:</br>%s</br>", query);
    /*
    input[7] = '-';
    input[15] = '-';
    input[23] = '-';
    */
   for(int i = 0; i < 48; i++)
   {
    input[i] = '\0';
   }
   
    sscanf(query, "R1=%7[^&]&R2=%7[^&]&R3=%7[^&]&R4=%7[^&]&R5=%7[^&]&R6=%7s", &input[0], &input[8], &input[16], &input[24], &input[32], &input[40]); 
    for(int i = 0; i < 48; i++)
        {
            if(input[i] == '\0')
            {
                printf(".");
            }else{
                printf("%c", input[i]);
            }
            
            //if((i + 1) % 8 == 0)printf("</br>");
        }
        printf("</br>");
    stringToLower(input);
    count = validateHtmlInput(input);

    for(int i = 0; i < 48; i++)
    {
        if(input[i] == '\0')
            {
                printf(" ");
            }else{
                printf("%c", input[i]);
            }
        //if((i + 1) % 8 == 0)printf("</br>");
    }
    printf("</br>");

    if(count == 0) 
            {   
                printf("Keine Korrekte Eingabe. </br>");
                printf("Bitte kehren sie zur <a href=\"test_cgi.html\" >Eingabeseite</a> zurück.</br>");
                return 0;
            }
    return count;
}

void stringToLower(char s[]) {
    int i;
    for(i = 0; s[i] != '\0'; i++) {
        s[i] = tolower(s[i]);
    }
}

int checkForSetting(char input[])
{
    if(input[0] == '-')return 1;
    return 0;
}

int applySetting(char input[])
{
    if(input[1] == 'l' && input[2] == 'e' && input[3] == 'n')
    {
        if(input[5] == 'd' && input[6] == 'e')
        {
            //Sprache in Deutsch geändert
            strcpy(language, "de");
            printf("Sprachauswahl: Deutsch.\n");
            return 1;
        }else if (input[5] == 'e' && input[6] == 'n')
        {
            //Sprache in Englisch geändert
            strcpy(language, "en");
            printf("Language: english\n");
            return 1;
        }
    }
    return 0;
}

int countInput(char input[])
{

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
           strcmp(token, "silber") == 0 || strcmp(token, "silver") == 0 || strcmp(token, "sr") == 0 ||
           strcmp(token, "gold") == 0 || strcmp(token, "au") == 0 )
        {
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
    #ifdef DEBUG
        printf("Anzahl: %i\n", count);
    #endif
    return count;
}

int validateHtmlInput(char input[])
{
    int count = 0;
    for (int i = 0; i < 6; i++)
    {
        if(input[(8*i)+7] != '\0') return false;
        if(input[8*i] != '\0')count++;

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
    if(strcmp(colour, "silber") == 0 || strcmp(colour, "silver") == 0 || strcmp(colour, "sr") == 0 ) return -2;
    if(strcmp(colour, "gold") == 0 || strcmp(colour, "gd") == 0 ) return -1;
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
    return 0;
}

int checkRing(int count, int number, char colour[]) {

    //Holger
    if ( count == 3 || count == 4 )
    { 
        switch( number)
        {
        case 1: 
        case 2:
            //test für ring 1 und 2
            if(strcmp(colour, "gold") == 0 || strcmp(colour, "au") == 0) return 0;
            if(strcmp(colour, "silber") == 0 || strcmp(colour, "silver") == 0 || strcmp(colour, "ag") == 0) return 0;
            break;
        case 3:
            // test für Ring 3
            if( strcmp(colour, "grau") == 0 || strcmp(colour, "grey") == 0 || strcmp(colour, "gy") ==0) return 0;
            if(strcmp(colour, "weiss") == 0 || strcmp(colour, "white") == 0 || strcmp(colour, "wh") == 0) return 0;
            break;
        case 4:
        // test für Ring 4
        if(strcmp(colour, "schwarz") == 0 || strcmp(colour, "black") == 0 || strcmp(colour, "bk") == 0) return 0;
        if(strcmp(colour, "orange") == 0 || strcmp(colour, "orange") == 0 || strcmp(colour, "og") == 0) return 0;
        if(strcmp(colour, "gelb") == 0 || strcmp(colour, "yellow") == 0 || strcmp(colour, "yl") == 0) return 0;
        if(strcmp(colour, "weiss") == 0 || strcmp(colour, "white") == 0 || strcmp(colour, "wh") == 0) return 0;
        break;
        }
        
    }else if(count == 5 || count == 6 )
    {
        switch (number)
        { case 1:
            if(strcmp(colour, "schwarz") == 0 || strcmp(colour, "black") == 0 || strcmp(colour, "bk") == 0) return 0;
          case 2:
          case 3:
            // test für ring 1,2,3)
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
         // test für ring 5
            if(strcmp(colour, "gold") == 0 || strcmp(colour, "au") == 0) return 0;
            if(strcmp(colour, "silber") == 0 || strcmp(colour, "silver") == 0 || strcmp(colour, "ag") == 0) return 0;
            if(strcmp(colour, "schwarz") == 0 || strcmp(colour, "black") == 0 || strcmp(colour, "bk") == 0) return 0;
            if(strcmp(colour, "orange") == 0 || strcmp(colour, "orange") == 0 || strcmp(colour, "og") == 0) return 0;
            if(strcmp(colour, "gelb") == 0 || strcmp(colour, "yellow") == 0 || strcmp(colour, "yl") == 0) return 0;
            if(strcmp(colour, "weiss") == 0 || strcmp(colour, "white") == 0 || strcmp(colour, "wh") == 0) return 0;
         break;
         case 6:
         // test für ring 6
            if(strcmp(colour, "gold") == 0 || strcmp(colour, "au") == 0) return 0;
            if(strcmp(colour, "silber") == 0 || strcmp(colour, "silver") == 0 || strcmp(colour, "ag") == 0) return 0;
            if(strcmp(colour, "weiss") == 0 || strcmp(colour, "white") == 0 || strcmp(colour, "wh") == 0) return 0;
            if( strcmp(colour, "grau") == 0 || strcmp(colour, "grey") == 0 || strcmp(colour, "gy") ==0) return 0;
            if(strcmp(colour, "gruen") == 0 || strcmp(colour, "grün") == 0 || strcmp(colour, "green") == 0 || strcmp(colour, "gn") == 0) return 0;
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

    //Je nach Anzahl der Ringe werden die Ringe 4-6 auch in Zahlen übersetzt
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
        case -10:
            return 5;
            break;
        case -1:
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

void printConsoleResult(float resValue, float tolerance, int tempCoefficient)
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
    printf("%.2f", tolerance);
    printf("%%\n");

    //Wenn sechs Ringe, dann gib Koeffizienten aus
    if(tempCoefficient != 0) printf("Temp: %d ppm/K\n", tempCoefficient);
}

void initHtmlOutput()
{
    //initialisieren der Web-Oberfläche
    printf("Content-type: text/html\n\n");
    printf("<html>\n");
    printf("<head>\n");
    printf("<title>Hallo Welt</title>\n");
    printf("</head>\n");
    printf("<body>\n");
    printf("<h1>Hallo Holger!</h1>\n");
}

void printHtmlResult(float resValue, float tolerance, int tempCoefficient)
{
    //Ausgabe der HTML-Informationen und des Ergebnisses der Widerstandsberechnung
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

    printf("<p>");
    //Ausgabe des Wiederstandswertes
    if(strcmp(language, "de")==0)printf("Widerstandswert:");
    else printf("Resistor Value:");
    printf("%.3lf", resValue);
    
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

    if(!tolerance)printf("Die Toleranz beträgt 20%% </br>");
    else printf("Die Toleranz beträgt %lf %%</br>", tolerance);
    if(tempCoefficient)printf("Der Temperaturkoeffizient beträgt %d ppm/K </br>", tempCoefficient);
    printf("</p>");
}

void closeHtmlOutput()
{
    printf("</body>\n");
    printf("</html>\n");
}

int checkCall()
{
    char* term = getenv("TERM");
    char* gateway = getenv("GATEWAY_INTERFACE");

    if (term != NULL)
    {
        #ifdef DEBUG
            //printf("Das Programm wurde über die Konsole gestartet.\n");
        #endif
        //Aufruf über die Konsole
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
            //printf("Die Ausführungsumgebung konnte nicht erkannt werden.\n");
        #endif
        //Keine Ahnung, wie aufgerufen wurde, also Konsolenausgabe
        return 0;
    }
    return 0;
}
