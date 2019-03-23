/**
 * TODO: validate all input
 * - All names must contain english letters only
 * - Student's id must contain 5 chars
 * - TA must have exactly 1 trained_for course
 * - Prof has at least 1 trained_for course
 * - Student has at least 1 required course
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define MAXN 50
#define MAXN_LENGTH 2
//"DaniilManakovskiyOutput<MAXN_LENGTH chars for number>.txt\0 -
#define FILENAME_SIZE (28 + MAXN_LENGTH)
#define MAX_ENTRY_SIZE 32
#define MAX_ENTRY_COUNT 32
//#define MAX_BUFFER_SIZE (MAX_ENTRY_SIZE * MAX_ENTRY_COUNT)
#define ID_SIZE 5

size_t MAX_BUFFER_SIZE = MAX_ENTRY_SIZE * MAX_ENTRY_COUNT;

// _Generic keyword is a part of C11 !
//#define print(X) _Generic((X), \
//    int: printInt,\
//    float: printDouble,\
//    double: printDouble\
//)(X)

typedef enum Status {
    PROFESSOR, TA, STUDENT
} status;

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


//TA and Prof are both faculty, but TA must have only 1 "trained for"
typedef struct faculty {
    char *name;
    int trained_for_count;
    Node *trained_for; // array for strings - names of courses
//    char **trained_for;
} Faculty;

typedef struct student {
    char *name;
    char *ID;
    int required_courses_count;
    Node *required_courses;
//    char **required_courses;
} Student;


///UTIL FUNCTIONS///
//erase the whole block of memory
void empty(char *str, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        str[i] = 0;
    }
}

///Operations on structs//
//Creation procedures:
//All of them fills the fields of stuct, given pre-allocated pointer to the struct
void new_course(Course *new, char *name, int labs, int students) {
    if (new == NULL)
        return;
    new->name = malloc(strlen(name) + 1);
    strcpy(new->name, name);
    new->required_labs = labs;
    new->allowed_students = students;
}

void new_faculty(Faculty *new, char *first_name, char *last_name) {
    if (new == NULL)
        return;
    // full_name ="<first_name> <last_name><\0>"
    size_t name_length = strlen(first_name) + strlen(last_name) + 2;
    char *full_name = (char *) malloc(name_length);
    empty(full_name, name_length);
    char *to_append = strcat(full_name, first_name);
    to_append = strcat(to_append, " ");
    strcat(to_append, last_name);


    new->name = malloc(strlen(full_name) + 1);
    strcpy(new->name, full_name);
    new->trained_for_count = 0;
    new->trained_for = NULL;

    free(full_name);
}

void new_student(Student *new, char *first_name, char *last_name, char *ID) {
    if (new == NULL)
        return;
    size_t name_length = strlen(first_name) + strlen(last_name) + 2;
    char *full_name = (char *) malloc(name_length);
    empty(full_name, name_length);
    char *to_append = strcat(full_name, first_name);
    to_append = strcat(to_append, " ");
    strcat(to_append, last_name);

//    printf("Got fullname:|%s|\n", full_name);

    new->name = malloc(strlen(full_name) + 1);
    strcpy(new->name, full_name);
    new->ID = malloc(ID_SIZE + 1); // 5 chars + \0
    strcpy(new->ID, ID);
    new->required_courses_count = 0;
    new->required_courses = NULL;

    free(full_name);

}

///LIST OPERATIONS///
void printList(Node *node, void (*print_function)(void *));

// functions for different representation of void* in list nodes
void printInt(void *n);

void printDouble(void *f);

void printString(void *s);

void printCourse(void *c);

void printFaculty(void *f);

void printStudent(void *s);

void pushFront(Node **head, void *data, size_t size);


///TASK FUNCTIONS///
void printEmail(); //TODO don't forget to call it

void solve();

int parseInput();

///GLOBAL VARIABLES///
//char global_buffer[MAX_BUFFER_SIZE];

char *global_buffer;

// removes \n from global_buffer
void trim() {
    if (global_buffer[strlen(global_buffer) - 1] == '\n') global_buffer[strlen(global_buffer) - 1] = '\0';
}


int main() {
    global_buffer = (char *) malloc(MAX_BUFFER_SIZE);
//    test();
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

void printString(void *s) {
    printf(" %s", (char *) s);
}

void printCourse(void *c) {
    Course *course = (Course *) c;
    printf("Course: %s\nLabs: %d\nStudents: %d\n\n", course->name, course->required_labs, course->allowed_students);
}

void printFaculty(void *f) {
    Faculty *faculty = (Faculty *) f;
    printf("Faculty: %s\n", faculty->name);
    printList(faculty->trained_for, printString);
    printf("\n\n");
}

void printStudent(void *s) {
    Student *student = (Student *) s;
    printf("Student: %s\nID: %s\nRequirements:", student->name, student->ID);
    printList(student->required_courses, printString);
    printf("\n\n");
}

//TODO: for some reason valgrind says, that pushFront leaks
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
    for (int i = 1; i <= MAXN; ++i) {
        char num[FILENAME_SIZE];
        //writes to the buffer filename of the current input file
        snprintf(num, FILENAME_SIZE, "input%d.txt", i);
        // if input file exist, solve the problem
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

            if (parseInput(&courses, &profs, &TAs, &students) != 0) {
                printf("Invalid input.");
                continue;
            }

            printf("\n------------\n");
            printList(courses, printCourse);
            printf("\n------------\n");
            printList(profs, printFaculty);
            printf("\n------------\n");
            printList(TAs, printFaculty);
            printf("\n------------\n");
            printList(students, printStudent);

        }
    }

}

int parseInput(Node **courses, Node **profs, Node **TAs, Node **students) {
    //reading courses, until "P" is met
    while (getline(&global_buffer, &MAX_BUFFER_SIZE, stdin) != -1 && strcmp(global_buffer, "P\n") != 0) {

        trim();

        char *course_name = (char *) malloc(MAX_ENTRY_SIZE);
        int labs_required, students_allowed;

        sscanf(global_buffer, "%s %d %d", course_name, &labs_required, &students_allowed);
        Course *n = (Course *) malloc(sizeof(Course));
        new_course(n, course_name, labs_required, students_allowed);
        pushFront(courses, n, sizeof(Course));

        free(course_name);
        free(n);
    }
    //EOF right after course block is unacceptable
    if (strcmp(global_buffer, "P\n") != 0) return 1;

    int status = PROFESSOR; // what group is being read

    while (getline(&global_buffer, &MAX_BUFFER_SIZE, stdin) != -1) {
        if (strcmp(global_buffer, "T\n") == 0) {
            status = TA;
            continue;
        }
        if (strcmp(global_buffer, "S\n") == 0) {
            status = STUDENT;
            continue;
        }
        trim();
        // tokenize given string by spaces
        // the format is always like {name} {surname} <{ID}> {List of courses}
        char *token = strtok(global_buffer, " ");
        char *name = malloc(strlen(token) + 1);
        strcpy(name, token);
        token = strtok(NULL, " ");
        char *surname = malloc(strlen(token) + 1);
        strcpy(surname, token);
        token = strtok(NULL, " ");
        Node *head = NULL;
        int len = 0;

        Node *student_id_ancestor = NULL;
        char *student_id = NULL;

        while (token != NULL) {
            pushFront(&head, token, strlen(token));
            len++;
            // for student his ID will be in the end of the list, so catch node that will be before ID and later retrieve the id
            if (status == STUDENT && len == 2) {
                student_id_ancestor = head;
            }
            token = strtok(NULL, " ");
        }

        // for student retrieve his ID
        if (status == STUDENT && student_id_ancestor != NULL) {
            if (strlen(student_id_ancestor->next->data) != 5) {
                return 1;
            } else {
                student_id = (char *) malloc(ID_SIZE + 1);
                strcpy(student_id, student_id_ancestor->next->data);
                free(student_id_ancestor->next->data);
                free(student_id_ancestor->next);
                student_id_ancestor->next = NULL;
            }
        }

        void *n;
        if (status != STUDENT) {
            n = (Faculty *) malloc(sizeof(Faculty));
            new_faculty(n, name, surname);
            ((Faculty *) n)->trained_for_count = len;
            ((Faculty *) n)->trained_for = head;
        } else {
            n = (Student *) malloc(sizeof(Student));
            new_student(n, name, surname, student_id);
            ((Student *) n)->required_courses_count = len;
            ((Student *) n)->required_courses = head;
            free(student_id);
        }


        switch (status) {
            case PROFESSOR: {
                pushFront(profs, n, sizeof(Faculty));
                break;
            }
            case TA: {
                pushFront(TAs, n, sizeof(Faculty));
                break;
            }
            case STUDENT: {
                pushFront(students, n, sizeof(Student));
            }
        }


        free(name);
        free(surname);
//        free(head);
        free(token);
        free(n);
    }

    //
    if (status != STUDENT) return 1;
    //TODO: check if at least 1 student was introduced



    return 0;
}
