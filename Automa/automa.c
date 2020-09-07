#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_LEN 51

typedef struct _state{
    int state; // Stato della FSM
    struct _state *next;
} state;

state* move_to_0(state *stato, int *lastState)
{
    if (stato == NULL){
        state *new = (state *)malloc(sizeof(state));
        new->state = 0;
        new->next = NULL;
        return new;
    }
    state *scanner;
    scanner = stato;
    while(scanner->next != NULL)
        scanner = scanner->next;
    
    scanner->next = (state *)malloc(sizeof(state));
    scanner->next->state = 0;
    *lastState = 0;
    scanner->next->next = NULL;
    return stato;
}

state* move_to_1(state *stato, int *lastState)
{
    if (stato == NULL){
        state *new = (state *)malloc(sizeof(state));
        new->state = 1;
        new->next = NULL;
        return new;
    }
    state *scanner;
    scanner = stato;
    while(scanner->next != NULL)
        scanner = scanner->next;
    
    scanner->next = (state *)malloc(sizeof(state));
    scanner->next->state = 1;
    *lastState = 1;
    scanner->next->next = NULL;
    return stato;
}

state* move_to_2(state *stato, int *lastState)
{
    if (stato == NULL){
        state *new = (state *)malloc(sizeof(state));
        new->state = 2;
        new->next = NULL;
        return new;
    }
    state *scanner;
    scanner = stato;
    while(scanner->next != NULL)
        scanner = scanner->next;
    
    scanner->next = (state *)malloc(sizeof(state));
    scanner->next->state = 2;
    *lastState = 2;
    scanner->next->next = NULL;
    return stato;
}

int isFinal(state *stato)
{
    int counter_1 = 0;
    int counter_2 = 0;
    while (stato != NULL){
        if (stato->state == 2)
            counter_2++;
        
        if (stato->state == 1)
            counter_1++;
        stato = stato->next;
    }
    if (counter_2 >= 3 && counter_1 == 1) 
        return 1; 
    
    return 0;
}

void fsm(char* string, const int states[][3], state *stato){
    int i = 0;
    int currentState = 0; // Stato corrente in cui è la FSM
    int *lastState = &currentState; // Puntatore per aggiornare currentState quando eseguo move_to_n con n = 0,1,2
    int choice = -1;
    state*(*f[3])(state*, int*) = {move_to_0, move_to_1, move_to_2}; // Array di puntatori a funzioni

    if (stato == NULL){ // Inizializzo la fsm

        if (string[i] >= 'a' && string[i] <= 'z'){
            puts("stringa non appartenente al linguaggio");
            while(stato != NULL) { // libero la memoria prima di uscire
                    state* tmp = stato;
                    stato = stato->next;
                    free(tmp);
            }  
            return; 
        }
        // Finché i caratteri appartengono [A-Z], rimango nello stato 0
        if (string[i] >= 'A' && string[i] <= 'Z')
            choice = 0;
        // Dallo stato 0, se il carattere è '0', passo allo stato 1
        else if (string[i] == '0')
            choice = 1;
        /*  Dal momento che le occorrenze di '1' dopo lo '0', devono essere almeno 0, 
            un qualsiasi carattere [0-9] mi fa passare allo stato 2
        */
        else if (string[i] >= '0' && string[i] <= '9')
            choice = 2;
        
        currentState = choice;
        /*  Se il valore della matrice in [currState, choice] è -1, vuol dire che la transizione 
            non è possibile pertanto la stringa non appartiene al linguaggio */
        if (choice < 0 || states[currentState][choice] == -1){  
            puts("stringa non appartenente al linguaggio");
            while(stato != NULL) {
            state* tmp = stato;
            stato = stato->next;
            free(tmp);
            }
            return;
        }
        
        else
            stato = (*f[choice])(stato, lastState);
        
        i++;
    }
    
    for (;i < strlen(string);i++){

        if (string[i] >= 'a' && string[i] <= 'z'){
            puts("stringa non appartenente al linguaggio");
            while(stato != NULL) { // libero la memoria prima di uscire
                    state* tmp = stato;
                    stato = stato->next;
                    free(tmp);
            } 
            return;  
        }
        // Finché i caratteri appartengono [A-Z], rimango nello stato 0
        if (string[i] >= 'A' && string[i] <= 'Z')
            choice = 0;
        // Dallo stato 0, se il carattere è '0', passo allo stato 1
        else if (string[i] == '0')
            choice = 1;
        /*  Dal momento che le occorrenze di '1' dopo lo '0', devono essere almeno 0, 
            un qualsiasi carattere [0-9] mi fa passare allo stato 2
        */
        else if (string[i] >= '0' && string[i] <= '9')
            choice = 2;

        if (choice < 0 ||states[currentState][choice] == -1){  
            puts("stringa non appartenente al linguaggio");
            while(stato != NULL) {
            state* tmp = stato;
            stato = stato->next;
            free(tmp);
            }
            return;
        }
        
        else
            stato = (*f[choice])(stato, lastState); 
    }
    // Se raggiungo uno stato finale, la stringa appartiene al linguaggio
    if (isFinal(stato)){
        puts("stringa appartenente al linguaggio");
        while(stato != NULL) { // libero la memoria prima di uscire
            state* tmp = stato;
            stato = stato->next;
            free(tmp);
        }
        return;
    }

    puts("stringa non appartenente al linguaggio");
    while(stato != NULL) { // libero la memoria prima di uscire
            state* tmp = stato;
            stato = stato->next;
            free(tmp);
    }
    return;
}



int main()
{   /*  "states" è la matrice contenente le transizioni possibili per ogni stato (se mi trovo nello stato 0, cioè nella riga 0,
        posso accedere allo stato 0, 1, e così via).
        Ho usato una matrice invece che una lista dato che gli stati possibili sono solamente 3,
        si conoscono a priori e l'accesso è molto più efficiente rispetto ad una lista.
        I possibili stati sono: 0. carattere [A-Z] 1. carattere '0' 2. carattere [0-9] -1. stato non possibile */
    const int states[][3] = {
        {0, 1, -1}, 
        {-1, 1, 2}, 
        {-1, -1, 2} 
    };
    char string[MAX_LEN]; // Stringa di input
    memset(string, 0, MAX_LEN);
    // Creo e inizializzo una lista contenente gli stati che assume la FSM
    state *stato = NULL;
    fgets(string, MAX_LEN-1, stdin);
    string[strcspn(string, "\n")] = 0;
    fsm(string, states, stato);

    
    return 0;
}