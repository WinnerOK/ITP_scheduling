#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define MAXN 50
#define MAXN_LENGTH 2
//"DaniilManakovskiyOutput<MAXN_LENGTH chars for number>.txt\0 -
#define FILENAME_SIZE (28 + MAXN_LENGTH)
#define MAX_ENTRY_SIZE 32
#define MAX_ENTRY_COUNT 32
#define MAX_BUFFER_SIZE MAX_ENTRY_SIZE * MAX_ENTRY_COUNT

// _Generic keyword is a part of C11 !
//#define print(X) _Generic((X), \
//    int: printInt,\
//    float: printDouble,\
//    double: printDouble\
//)(X)

///BASIC DATA STRUCTURES///
typedef struct node {
    void *data;
    struct node *next;
} Node;

typedef struct course {
    char *name;
    int required_labs;
    int allowed_students;
} Course;

typedef struct faculty {
    char *name;
    int trained_for_count;
    char **trained_for; // array for strings - names of courses
} Faculty;

typedef struct student {
    char *name;
    char *ID;
    int required_courses_count;
    char **required_courses;
} Student;

///LIST OPERATIONS///
void pushFront(Node **head, void *data, size_t size);

void printList(Node *node, void (*print_function)(void *));

void printInt(void *n);

void printDouble(void *f);

///TASK FUNCTIONS///
void printEmail(); //TODO don't forget to call it

void solve();

int parseInput();

///GLOBAL VARIABLES///
char global_buffer[MAX_BUFFER_SIZE];

int main() {
    solve();
//    Node *start = NULL;
//
//    // Create and print an int linked list
//    unsigned int_size = sizeof(int);
//    int arr[] = {10, 20, 30, 40, 50}, i;
//    for (i = 4; i >= 0; i--)
//        pushFront(&start, &arr[i], int_size);
//    printf("Created integer linked list is \n");
//    printList(start, printInt);
//
//    // Create and print a float linked list
//    unsigned float_size = sizeof(double);
//    start = NULL;
//    double arr2[] = {10.1, 20.2, 30.3, 40.4, 50.5};
//    for (i = 4; i >= 0; i--)
//        pushFront(&start, &arr2[i], float_size);
//    printf("\n\nCreated double linked list is \n");
//    printList(start, printDouble);

    return 0;
}


void printInt(void *n) {
    printf(" %d", *(int *) n);
}


void printDouble(void *f) {
    printf(" %f", *(double *) f);
}

void pushFront(Node **head, void *data, size_t size) {
    Node *new_node = (Node *) malloc(sizeof(Node));
    if (new_node == NULL) {
        perror("Memory for new node was not allocated");
        exit(1);
    }

    new_node->data = malloc(size);
    new_node->next = (*head);

    //copy value of data
    memcpy(new_node->data, data, size);


    (*head) = new_node;
}

void printList(Node *node, void (*print_function)(void *)) {
    while (node != NULL) {
        (*print_function)(node->data);
        node = node->next;

    }
}

void printEmail() {
    FILE *email = fopen("DaniilManakovskiyEmail.txt", "w");
    fprintf(email, "d.manakovskiy@innopolis.university");
    fclose(email);
}

void solve() {
    //TODO don't forget to call printEmail
    for (int i = 1; i <= MAXN; ++i) {
        char num[FILENAME_SIZE];
        snprintf(num, FILENAME_SIZE, "input%d.txt", i);
        // if input file exists
        if (freopen(num, "r", stdin) != NULL) {
            snprintf(num, FILENAME_SIZE, "output%d.txt", i);
//            freopen(num, "w", stdout);

            //solve task for a particular input
            Node *courses = NULL;
            Node *profs = NULL;
            Node *TAs = NULL;
            Node *students = NULL;

            int courses_count = 0;
            int profs_count = 0;
            int TAs_count = 0;
            int students_count = 0;

            if (parseInput() != 0) {
                printf("Invalid input.");
                continue;
            }

        }
    }

}

int parseInput() {
    char course_name[MAX_ENTRY_SIZE];
    int labs_required, students_allowed;

    // if no characters were read, but expected to
    if (fgets(global_buffer, MAX_BUFFER_SIZE, stdin) == NULL){
        return 1;
    }
    //todo maybe use sscanf_s from global buffer to parse args
    sscanf(global_buffer, "%s %d %d", course_name, &labs_required, &students_allowed);

    printf("%s|%d|%d\n", course_name, labs_required, students_allowed);

    return 0;
}




