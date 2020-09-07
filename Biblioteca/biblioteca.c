#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX_LEN 201
#define DELIMITER ","

typedef struct lista_biblioteca{
    char author[MAX_LEN]; // Autore
    char title[MAX_LEN]; // Titolo del libro
    int copies; // Copie totali
    int lent_copies; // Copie prestate
    int isbn; // ISBN
    struct lista_biblioteca *next; // Puntatore al prossimo elemento
} list;

list* insTesta(list*, char *, char *, int);
int insert(list *, int);
void printlist(list *);
int compare(const void *, const void *); // Funzione compare per il qsort
void from_list_to_ordered_array(list *, list *); // Uso un array ordinato per la funzione "Stampa catalogo"
void printarray(list *);
void search_menu(list *); // Richiesta del nome dell'autore e del titolo del libro
void search_in_list(list*, char *, char *);
void get_lending_isbn(list *); // Richiede l'ISBN del libro per il prestito
void lending(list*, int);
void get_return_isbn(list *); // Richiede l'ISBN del libro per la restituzione
void return_book(list*, int);



int number_of_elements; // Variabile globale che conta le new entries nella lista

int main()
{
    number_of_elements = 0; // Inizializzo la variabile globale
    list *lista = NULL; // Catalogo

    //  Stringhe per la tokenizzazione
    char *token = (char *)malloc(MAX_LEN*sizeof(char)); // Stringa per la tokenizzazione
    char line[MAX_LEN]; // Stringa dove salvo l'input
    memset(token, 0, MAX_LEN);
    memset(line, 0, MAX_LEN);
    

    // Attributi dei Libri
    char author[MAX_LEN];
    char title[MAX_LEN]; 
    char isbn_str[MAX_LEN];
    int  isbn = -1; 

    do{
        fgets(line, MAX_LEN-1, stdin);
        line[strcspn(line, "\n")] = 0;

        //  Tokenizzazione
        strcpy(token, strtok(line, DELIMITER));
        strcpy(isbn_str, token); // Token ISBN
        strcpy(token, strtok(NULL, DELIMITER));
        strcpy(title, token); // Token titolo
        strcpy(token, strtok(NULL, DELIMITER));
        strcpy(author, token); // Token Autore
        
        isbn = atoi(isbn_str); // Converto l'isbn in un intero

        if (insert(lista, isbn) == 1 && isbn != 0) 
            lista = insTesta(lista, author, title, isbn);

        
    } while(isbn != 0);
    
    //  Menu'
    
    void (*f[4])(list *) = {printarray, search_menu, get_lending_isbn, get_return_isbn}; // Array di puntatori alle funzioni
    
    int choice = -1; // Scelta del Menu'
    do{
        puts("Scegli un opzione:\n1) Stampa catalogo.\n2) Cerca.\n3) Prestito.\n4) Restituzione.\n5) Esci.");
        printf("Scelta:  ");
        while(scanf("%d", &choice) != 1 || choice > 5){
            printf("Errore. Scelta non valida.\n");
            while(getchar() != '\n');
        }
        if(choice < 5)
            (*f[choice-1])(lista); // Eseguo la funzione scelta
    }while(choice != 5);
    

    // Free Memory
   while(lista != NULL) {
            list* tmp = lista;
            lista = lista->next;
            free(tmp);
        }
    free(token);
    puts("Bye");

    return 0;
}

list* insTesta(list *head, char *author, char *title, int isbn)
{
    list *new = (list *)malloc(sizeof(list));
    strcpy(new->author, author);  
    strcpy(new->title, title);
    new->copies = 1;
    new->isbn = isbn;
    new->lent_copies = 0;  
    new->next = head;
    number_of_elements++;
    return new;
}

int insert(list *head, int isbn) // Insert controlla che il libro non sia gia' nel catalogo. Se c'e', aggiorna il numero di copie
{
    while(head != NULL){
        if (head->isbn == isbn){
            head->copies++; // Se gli ISBN corrispondono, aumento di 1 il numero delle copie disponibili
            return 0;
        }
        head = head->next;
    }
    return 1;
}

int compare(const void *a, const void *b) 
{
   list a1 = *(list *)a, b1 = *(list *)b; 
   if((strcmp(a1.author, b1.author)) != 0) // Ordino per autore
    return strcmp(a1.author, b1.author);
    
   else
    return strcmp(a1.title, b1.title); // Ordino per titolo se l'autore è il medesimo
}

// Dato che dopo il primo inserimento, la dimensione del catalogo è costante,
// copio la lista all'interno di un array per ordinarlo efficientemente
void from_list_to_ordered_array(list *array, list *head)
{
    for (int i = 0; i < number_of_elements; i++){ 
        array[i].isbn = head->isbn;
        array[i].lent_copies = head->lent_copies;
        array[i].copies = head->copies;
        strcpy(array[i].author, head->author);
        strcpy(array[i].title, head->title);
        head = head->next;
    }
    qsort(array, number_of_elements, sizeof(list), compare);
    
    
}

void printarray(list *head)
{
    list *array = (list *)malloc(number_of_elements*sizeof(list)); // Alloco la memoria per l'array di struct per il print
    from_list_to_ordered_array(array, head); // Ordino
    for (int i = 0; i < number_of_elements; i++)
        printf("%d - %s - %s (%d/%d)\n", array[i].isbn, array[i].author, array[i].title, (array[i].copies - array[i].lent_copies), array[i].copies);
    free(array);
}

void search_menu(list* head)
{
    char *author = (char *)malloc(MAX_LEN*sizeof(char));
    char *title = (char *)malloc(MAX_LEN*sizeof(char));
    memset(author, 0, MAX_LEN);
    memset(title, 0, MAX_LEN);

    printf("Inserire nome autore: ");
    while(getchar() != '\n');
    fgets(author, MAX_LEN-1, stdin);
    author[strcspn(author, "\n")] = 0;
    printf("Inserire titolo: ");
    fgets(title, MAX_LEN-1, stdin);
    title[strcspn(title, "\n")] = 0;
    search_in_list(head, author, title);
    free(author);
    free(title);
}

void search_in_list(list* head, char *author, char *title)
{
    if (head == NULL){
        puts("Libro non trovato.");
        return;
    }
    if (strcmp(head->author, author) == 0 && strcmp(head->title, title) == 0){
        if ((head->copies - head->lent_copies) != 0)
            printf("%d copie disponibili.\n", (head->copies - head->lent_copies));
        if ((head->copies - head->lent_copies) == 0)
            puts("Non ci sono copie disponibili del libro richiesto.");
        return;
    } 
    search_in_list(head->next, author, title);  

}

void get_lending_isbn(list *head)
{
    int isbn;
    printf("ISBN: ");
    scanf("%d", &isbn);
    lending(head, isbn);
}

void lending(list *head, int isbn)
{
    if (head == NULL){
        puts("Libro non trovato.");
        return;
    }
    if (head->isbn == isbn && (head->copies - head->lent_copies) > 0){
        head->lent_copies++;
        puts("Operazione completata.");
        return;
    }
    if (head->isbn == isbn && (head->copies - head->lent_copies) == 0){
        puts("Non ci sono copie disponibili del libro richiesto.");
        return;
    }
    lending(head->next, isbn);

}

void get_return_isbn(list *head)
{
    int isbn;
    printf("ISBN: ");
    scanf("%d", &isbn);
    return_book(head, isbn);
}

void return_book(list* head, int isbn)
{
    if (head == NULL){
        puts("Libro non trovato.");
        return;
    }
    if (head->isbn == isbn && head->lent_copies != 0){
        head->lent_copies--;
        puts("Operazione completata.");
        return;
    }
    if (head->isbn == isbn && head->lent_copies == 0){
        puts("Non risultano copie in prestito.");
        return;
    }
    return_book(head->next, isbn);

}