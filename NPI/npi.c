#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#define DELIMITER " "
#define MAX_LEN 150

typedef struct _stack{ // Pila per la risoluzione delle espressioni
    int key;
    struct _stack *next;
}stack;

typedef struct _input{ // Lista contenente l'input
    char expression[MAX_LEN];
    struct _input *next;
} input;

typedef struct _toResolve{ // Struct di espressioni tokenizzate
    char element[MAX_LEN];
    struct _toResolve *next;
} toResolve;

typedef struct _result{ // Lista dei risultati
    int result;
    char overflow[10];
    int binary[32];
    struct _result *next;
    
}result;

int check_overflow_atoi(char* str, char *o_flow); // Ha la stessa funzione di atoi, ma controlla che il numero inserito in input sia minore di INT_MAX
input* insCoda_input(input *head, char *expression); // Inserisce le espressioni nella lista
toResolve* insCoda_resolve(toResolve *head, char* token); // Inserisce gli elementi delle espressioni tokenizzate in una lista
result* insResult(result *head, int res, char* o_flow);// Inserisce i risultati delle espressioni in una lista per poi poter essere stampati
void printresult(result *head); // Stampa l'output nel corretto formato
void safe_add(int a, int b, int *res, char* o_flow); // Addizione con controllo dell'overflow
void safe_sub(int a, int b, int *res, char* o_flow); // Sottrazione con controllo dell'overflow
void safe_mult(int a, int b, int *res, char* o_flow); // Moltiplicazione con controllo dell'overflow
int calculator(toResolve *resolve, char* o_flow); // Calcolatrice che prende in input un'espressione in NPI
int tokenize_and_resolve(input *expressions, char *o_flow); // Tokenizza le espressioni e le passa alla calcolatrice
stack* push(stack *head, int key); // Funzione di inserimento in una pila
int pop(stack **head); // Funzione di rimozione da una pila
void from_int_to_binary(int binary[], int num); // Converte un intero in binario


int main()
{
    input *expressions = NULL;
    result *res = NULL;
    int risultato = 0;
    char o_flow[10];
    memset(o_flow, 0, 10);
    char expression[MAX_LEN];
    memset(expression, 0, MAX_LEN);
    // Salvo l'input in una lista per poi processarlo
    while (1){
        fgets(expression, MAX_LEN-1, stdin);
        expression[strcspn(expression, "\n")] = 0;
        if (strcmp(expression, "fine") == 0)
            break;
        expressions = insCoda_input(expressions, expression);
    } 
    /*  Processo le espressioni e al termine le elimino dalla lista.
        Ogni funzione ha in input la stringa o_flow, che, in caso di overflow, diventa "Overflow!", così da
        poter marcare l'elemento della lista dei risultati con la dicitura "Overflow!" .
        La stringa verrà resettata a ogni iterazione, altrimenti dopo il primo caso di OF, tutti i
        risultati verranno marcati.
    */
    while (expressions != NULL){
        risultato = tokenize_and_resolve(expressions, o_flow);
        res = insResult(res, risultato, o_flow);
        memset(o_flow, 0, 10); 
        input* tmp_1 = expressions;
        free(tmp_1);
        expressions = expressions->next;
    } 

    printresult(res);
    
    while(res != NULL) {
            result* tmp_2 = res;
            res = res->next;
            free(tmp_2);
        } // Libero la memoria prima di uscire
    
    return 0;  
}

int check_overflow_atoi(char* str, char *o_flow) 
{  
    int converted = 0; 
    int segno = 1; 
    int i = 0;
    if (str[0] == '-') {
        segno = -1;
        i++;
    } 
    for (; str[i] != '\0'; i++){
        if (converted * 10 + str[i] - '0' <= 0 && str[i+1] != '\0')
            strcpy(o_flow, "Overflow!");
        
        converted = converted * 10 + str[i] - '0';
    } 
    return segno * converted; 
} 


stack* push(stack *head, int key)
{
    stack *new = (stack *)malloc(sizeof(stack));
    new->key = key;
    new->next = head;
    
    return new;
}

int pop(stack **head)
{
    int key = (*head)->key;
    stack *tmp = *head;
    *head = (*head)->next;

    free(tmp);
    return key;
}

input* insCoda_input(input *head, char *expression)
{
    if (head == NULL){
        input *new = (input *)malloc(sizeof(input));
        memset(new->expression, 0, MAX_LEN);
        strcpy(new->expression, expression);
        new->next = NULL;
        return new;
    }
    input *scanner;
    scanner = head;
    while(scanner->next != NULL)
        scanner = scanner->next;
    
    scanner->next = (input *)malloc(sizeof(input));
    memset(scanner->next->expression, 0, MAX_LEN);
    strcpy(scanner->next->expression, expression);
    scanner->next->next = NULL;

    return head;
}

toResolve* insCoda_resolve(toResolve *head, char *token)
{
    if (head == NULL){
        toResolve *new = (toResolve *)malloc(sizeof(toResolve));
        memset(new->element, 0, MAX_LEN);
        strcpy(new->element, token);
        new->next = NULL;
        return new;
    }
    toResolve *scanner;
    scanner = head;
    while(scanner->next != NULL)
        scanner = scanner->next;
    
    scanner->next = (toResolve *)malloc(sizeof(toResolve));
    memset(scanner->next->element, 0, MAX_LEN);
    strcpy(scanner->next->element, token);
    scanner->next->next = NULL;

    return head;
}

void printresult(result *head)
{
    while(head != NULL){
        if (strcmp(head->overflow, "Overflow!") == 0){
            puts("Overflow!");
            head = head->next;
            continue;
        }
        printf("%d in C2:", head->result);
        for (int i = 0; i < 32; i++){
            if (i%4 == 0)
                printf(" ");
            printf("%d", head->binary[i]);
        }
        printf(" \n");     
        head = head->next;
    }
}

void safe_add(int a, int b, int *res, char* o_flow) 
{
    if((b > 0 && a > (INT_MAX - b)) || (b < 0 && a < (INT_MIN - b))){
        strcpy(o_flow, "Overflow!");
        return;
    }
    *res = a + b;
}

void safe_sub(int a, int b, int *res, char* o_flow)
{
    if((b >= 0 && a <= INT_MIN + b) || (b < 0 && a > INT_MAX + b)){
        strcpy(o_flow, "Overflow!");
        return;
    } 
    *res = b - a;
}

void safe_mult(int a, int b, int *res, char* o_flow) 
{

    if ((b > 0 && a > 0 && b*a < 0) || (b < 0 && a < 0 && b*a < 0)) {
        strcpy(o_flow, "Overflow!");
        return;
    }
    
    *res = a*b;
}

result* insResult(result *head, int res, char *o_flow)
{
    if (head == NULL){
        result *new = (result *)malloc(sizeof(result));
        new->result = res;
        memset(new->overflow, 0, 10);
        memset(new->binary, 0, 32*sizeof(int));
        if (strcmp(o_flow, "Overflow!") == 0)
            strcpy(new->overflow, o_flow);
        else            
            from_int_to_binary(new->binary, res);
            
        new->next = NULL;
        return new;
        
    }
    result *scanner;
    scanner = head;
    while(scanner->next != NULL)
        scanner = scanner->next;

    scanner->next = (result *)malloc(sizeof(result));
    memset(scanner->next->binary, 0, 32*sizeof(int));
    memset(scanner->next->overflow, 0, 10);
    scanner->next->result = res;
    
    if (strcmp(o_flow, "Overflow!") == 0)
        strcpy(scanner->next->overflow, o_flow);
    
            
    else
        from_int_to_binary(scanner->next->binary, res);
           
    scanner->next->next = NULL;
    return head;

}

/*
    La calcolatrice esegue i seguenti passi:
        1. insersce i primi due numeri nella pila
        2. recupera i due numeri dalla pila ed esegue l'operazione
        3. reinserisce il risultato nella pila 
        4. inserisce nuovi numeri all'interno della pila finché non trova '+', '-' o '*'
        5. esegue l'operazione sui primi due numeri che tira fuori nella lista 

    La funzione termina quando la lista toResolve è vuota, pertanto quando ci sarà solo un elemento nella pila, 
    che è il risultato.
    La calcolatrice ritorna il valore -1 in caso di overflow

*/

int calculator(toResolve *resolve, char *o_flow)
{
    int a, b, c, result;
    a = b = c = result = 0;
    char operation = 0;
    stack *pila = NULL;
    while (resolve != NULL){
        if(strcmp(resolve->element, "+") != 0 && strcmp(resolve->element, "*") != 0 && strcmp(resolve->element, "-") != 0){
            a = check_overflow_atoi(resolve->element, o_flow);
            if (strcmp(o_flow, "Overflow!") == 0)  // Check dell'overflow in input
                return -1;
            pila = push(pila, a);
            resolve = resolve->next;
            if(strcmp(resolve->element, "+") != 0 && strcmp(resolve->element, "*") != 0 && strcmp(resolve->element, "-") != 0){
                b = check_overflow_atoi(resolve->element, o_flow);
                if (strcmp(o_flow, "Overflow!") == 0) // Check dell'overflow in input
                    return -1;
                pila = push(pila, b);
                resolve = resolve->next;
            }
        }
        operation = resolve->element[0];
        resolve = resolve->next;
        a = pop(&pila);
        b = pop(&pila);
        // Dopo ogni operazione controllo se ho un OF
        switch (operation)
        {
        case '+':
            safe_add(a, b, &c, o_flow);
            if (strcmp(o_flow, "Overflow!") == 0)
                return -1;
            break;
        
        case '-':
            safe_sub(a, b, &c, o_flow);
            if (strcmp(o_flow, "Overflow!") == 0)
                return -1;
            break;

        case '*':
            safe_mult(a, b, &c, o_flow);
            if (strcmp(o_flow, "Overflow!") == 0)
                return -1;
            break;
        
        default:
            break;
        }
        pila = push(pila, c);
    }

    result = pop(&pila);
    free(pila);
    return result;
}

int tokenize_and_resolve(input *expressions, char *o_flow)
{
    toResolve *resolve = NULL;
    int res = 0;
    char *token = (char *)malloc(MAX_LEN*sizeof(char));
    memset(token, 0, MAX_LEN);
    token = strtok(expressions->expression, DELIMITER);     
    resolve = insCoda_resolve(resolve, token);
    while (1){
        token = strtok(NULL, DELIMITER);
        if (token == NULL)
            break;
        resolve = insCoda_resolve(resolve, token);
    }
    
    res = calculator(resolve, o_flow);   
    while(resolve != NULL) {
            toResolve* tmp = resolve;
            resolve = resolve->next;
            free(tmp);
        }
    free(token);
    return res;
    
}

void from_int_to_binary(int binary[], int num)
{
    int module, div, a, check;
    module = a = check = 0;
    div = 2;
    int i = 30;
    int j = 1;
    if(num == 0)
        return;
    
    else
        binary[31] = num % 2;

    if (num < 0){
        num *= -1;
        check = 1;
    }
    while(div / 2 > 0){
        a = pow(2, j);
        div = num / a;
        module = (div % 2);
        binary[i] = module;
        i--;
        j++;
    }
    if(check){
        for (int j = 0; j < 32; j++){
            if (binary[j] == 1)
                binary[j] -= 1;
            else
                binary[j] += 1;
        }
        for (int k = 31; k != 0; k--){
            if (binary[k] == 1){
                binary[k] = 0;
            }    
            else{
                binary[k] = 1;
                return;
            }
        }  
    }
}


