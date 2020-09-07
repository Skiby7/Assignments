#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_LEN 15
#define TEXT_LEN 1001

// Lista che contiene tutte le frasi tradotte fino all'EOF
typedef struct text{
    char line_[TEXT_LEN];
    struct text *next;    
} line;

line* insCoda(line *head, char* line_)
{
    if (head == NULL)
    {
        line *new = (line *)malloc(sizeof(line));
        strcpy(new->line_, line_);
        new->next = NULL;
        
        return new;

    }
    line *scanner;
    scanner = head;
    while(scanner->next != NULL)
        scanner = scanner->next;
    
    scanner->next = (line *)malloc(sizeof(line));
    strcpy(scanner->next->line_, line_);
    scanner->next->next = NULL;
    
    return head;
}

void printlist(line *head)
{
    if(head == NULL) return;

    printf("%s\n", head->line_);
    printlist(head->next);
    
}

char* make_morse(char *text, char **table)
{   
    
    char *morse = (char *)malloc(TEXT_LEN*sizeof(char));
    memset(morse, 0, TEXT_LEN);
    int length = 0;
    int index = 0;
    for (int i = 0; text[i] != '\n'; i++){
        index = text[i];
        if ((index >= 48 && index <= 57) || (index >= 65 && index <= 90) || (index >= 97 && index <= 122) || text[i] == ' '){
            
            if (text[i] != ' '){
                for(int j = 0; table[index][j] != '\0'; j++){
                    morse[length] =  table[index][j];
                    length++;
                }
                if (text[i+1] != ' '){
                    for (int k = 0; k < 3; k++){
                        morse[length] = ' ';
                        length++;
                    }
                }
            }
            else{
                for (int l = 0; l < 7; l++){
                    morse[length] = ' ';
                    length++;
                }
            }
        }
        else{
            strcpy(morse, "Errore nell'input");
            return morse;
        }
    
        
    }
    return morse;
    
}

int main()
{
    FILE *file;

    if ((file = fopen("input.txt", "r")) == NULL) {
        puts("Impossibile aprire il file");
        exit(1);    // Se il file non esiste esco con un errore
    }
    else{

        /*  array_alphabet è un array di stringhe che contiene l'alfabeto in codice morse.
            In particolare, ogni indice dell'array corrisponde al codice ASCII del carattere
            di cui si ha la traduzione (ovviamente per i caratteri [A-Z],[a-z] e [0-9]) */
        char **array_alphabet = (char **)malloc(256*sizeof(char *));
        for (int mem = 0; mem < 256; mem++){
            array_alphabet[mem] = (char *)malloc(MAX_LEN*sizeof(char));
            memset(array_alphabet[mem], 0, MAX_LEN);
        }
        char tmp[TEXT_LEN]; // Qui salvo la stringa da stdin per poi tradurla e inserirla nella lista
        memset(tmp, 0, TEXT_LEN);
        line *lines = NULL; // Lista di frasi tradotte
        int i = 0;
        int i_upper = 65; // Indice di partenza dei caratteri maiuscoli
        int i_lower = 97; // Indice di partenza dei caratteri minuscoli
        int i_number = 48; // // Indice di partenza dei numeri
        char *res = NULL;
        char *morse = NULL;
        while(1) {
            // Inserisco nell'alfabeto i 26 caratteri maiuscoli e minuscoli
            if (i < 26){
                while(fgetc(file) != ':');
                fscanf(file, "%[^\n]s", tmp);
                strcpy(array_alphabet[i_upper], tmp);
                strcpy(array_alphabet[i_lower],tmp);
                i++;     
                i_lower++;
                i_upper++; 
            }
            // Inserisco i numeri
            if (i >= 26 && i < 36){
                while(fgetc(file)!=':');
                fscanf(file, "%[^\n]s", tmp);
                strcpy(array_alphabet[i_number], tmp);
                i++;      
                i_number++;
            }
            // Inserisco le frasi tradotte con make_morse nella lista
            if (i >= 36){
                res = fgets(tmp, TEXT_LEN, file);
                if (res == NULL)
                    break;

                if (strcmp(tmp, "****\n") != 0 && strcmp(tmp, "\n") != 0 ){
                    morse = make_morse(tmp, array_alphabet);
                    lines = insCoda(lines, morse);
                    free(morse);
                }
                                    
            }
            
        }
        // Stampo il risultato
        printlist(lines);
        for (int l = 0; l < 256; l++)
            free(array_alphabet[l]);
        free(array_alphabet);

        while(lines != NULL) {
            line* tmp = lines;
            lines = lines->next;
            free(tmp);
        }
        fclose(file);
    }
    
    return 0;
}