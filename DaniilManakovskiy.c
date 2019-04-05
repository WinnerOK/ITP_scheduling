/**
 * TODO: validate all input
 * - All names must contain english letters only
 * - Student's id must contain 5 chars
 * - TA must have exactly 1 trained_for course
 * - Prof has at least 1 trained_for course
 * - Student has at least 1 required course
 */

// TODO migrate from Node* to List*
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>

/////////////////////////////////Constants, types/////////////////////////////////

typedef unsigned long long ull;

///Input Constants///
#define MAXN 50
#define MAXN_LENGTH 2
//"DaniilManakovskiyOutput<MAXN_LENGTH chars for number>.txt\0 -
#define FILENAME_SIZE (28 + MAXN_LENGTH)
#define MAX_ENTRY_SIZE 32
#define MAX_ENTRY_COUNT 32
size_t MAX_BUFFER_SIZE = MAX_ENTRY_SIZE * MAX_ENTRY_COUNT;
#define ID_SIZE 5

///Hash Table Constants///
#define MAX_LOAD_FACTOR 0.75
const ull z = 7;

///Evolution Constants///
#define POPULATION_SIZE 1000
#define EVOLUTION_STEPS 1000
#define BEST_FIT_PERCENTAGE 0.1
#define GOOD_FIT_PERCENTAGE 0.2
#define MUTATION_PROBABILITY 0.1
#define TAKE_SECOND_PARENT_GENE (1 - (MUTATION_PROBABILITY))
#define TAKE_FIRST_PARENT_GENE ((TAKE_SECOND_PARENT_GENE)/2)

///Error Penalties///
#define COURSE_NOT_RUN 20

#define PROFESSOR_UNASSIGNED 10
#define PROFESSOR_WRONG SUBJECT 5
#define PROFESSOR_LACKING_CLASS 5

#define TA_LACKING_CLASS 2

#define STUDENT_LACKING_CLASS 1

///Constraints///
#define TA_MAX_CLASS 4

///Utility Constants///
#define True 1
#define False 0


/////////////////////////////////Data structures/////////////////////////////////
///Linked List///
typedef struct node {
    void *data;
    struct node *next, *prev;
    size_t data_size;
} Node;

typedef struct list {
    size_t dataSize; // TODO: It is still unused, do you really need it?
    int cursor_position;
    int size;
    Node *head;
    Node *tail;
    Node *cursor;
} List;

Node *getNodeFromList(List *list, int ind);

void *getFromList(List *list, int ind);

void printList(List *list, void (*print_function)(void *));

void freeList(List *list, void (*destructor)(void *));

void listCopy(List *dest, List *src, size_t datasize); // list dest must be initialized, but empty!!!

// functions for different representation of void* in list nodes
void printInt(void *n);

void printDouble(void *f);

void printString(void *s);

void printSubject(void *c);

void printFaculty(void *f);

void printStudent(void *s);

void printProfessorGenetic(void *pg);

void printCourseGenetic(void *cg);

void printTAGenetic(void *taG);

void pushBack(List *list, void *data);

void initList(List *list, size_t dataSize);

char /*Bool*/ isInList(List *list, void *data, int (*key_cmp)(void *, void *));

void removeFromList(List *list, int index/*TODO maybe add destructor?*/);

///Hash Map///
typedef struct hash_node {
    void *key;
    void *value;
    struct hash_node *next;
    struct hash_node *prev;
} HashNode;

typedef struct {
    HashNode *head;
    HashNode *tail;
} HashList;

typedef struct {
    HashList *datalist;
    size_t size, capacity;
    size_t key_size, value_size;

    int (*key_comparator)(void *, void *);

    void (*value_destructor)(void *);

    void (*key_destructor)(void *);
    /* 1: key1 > key2
     * 0: key1 = key2 actually, only this need (!!!)
     * -1 key1 < key2
     */
} HashTable;

ull hash(const void *data, size_t size);

void free_hash_entry(HashNode *node);

void init_datalist(HashList *datalist, size_t capacity);

void hash_table_init(HashTable *table, size_t capacity, size_t key_size,
                     size_t value_size, int (*key_cmp)(void *, void *)/*,
                     void (*key_destructor)(void *), void (*value_destructor)(void *)*/);

void insert(HashTable *table, void *key, void *value);

void free_hash_table(HashTable *table);

void rehash(HashTable *table);

void *get_el(HashTable *table, void *key);

void remove_el(HashTable *table, void *key);

//WARNING! Returns a pointer to actual node in table
HashNode *find_node(HashTable *table, void *key);


/////////////////////////////////Data structures/////////////////////////////////
typedef enum Status {
    PROFESSOR, TA, STUDENT
} status;

///BASIC DATA STRUCTURES///

typedef struct subject {
    char *name;
    int required_labs;
    int allowed_students;
    int selectedCount; //how many student selected this course
} Subject;


//TA and Prof are both faculty, but TA can be assigned only to those courses that he/she can teach
typedef struct faculty {
    char *fullname;
    int trained_for_count;
    List *trained_for; // array for strings - names of courses
} Faculty;

typedef struct student {
    char *name;
    char *ID;
    int required_courses_count;
    List *required_courses;
} Student;


///Genetic data structures///
typedef struct prof {
    Faculty *professor;
    char /*Bool*/ isDoingWrongSubject;
    char /*Bool*/ isBusy;
    int courses_teaching_count;
    List *courses_teaching; // what courses prof teaches
} ProfessorGenetic;

typedef struct ta {
    Faculty *TA;
    int courses_teaching_count;
    List *courses_teaching; // what courses prof teaches
} TAGenetic;

typedef struct course {
    Subject *subject;
    ProfessorGenetic *prof;
    int TA_assigned;
    List *TAs;
} CourseGenetic;

typedef struct individ {
    CourseGenetic *schedule; //list of courses
    List *professors;
    List *TAs;
} Individual;

/////////////////////////////////functions on DTs and constructors/////////////////////////////////
///Constructors//
//All of them fills the fields of stuct, given pre-allocated pointer to the struct
void new_subject(Subject *new, char *name, int labs, int students);

void new_faculty(Faculty *new, char *first_name, char *last_name);

void new_student(Student *new, char *first_name, char *last_name, char *ID);


///Destructors//
void del_subject(Subject *subj);

void del_faculty(Faculty *f);

void del_student(Student *s);

void del_faculty_genetic(ProfessorGenetic *prof);

///ProfessorGenetic///
char /*Bool*/ isAvailableProf(ProfessorGenetic *prof, CourseGenetic *course);

void AssignToProf(ProfessorGenetic *prof, CourseGenetic *course);

int errorProf(ProfessorGenetic *prof);


///TAGenetic///
char /*Bool*/ isAvailableTA(TAGenetic *ta, CourseGenetic *course);

int errorTA(TAGenetic *ta);

void AssignToTA(TAGenetic *ta, CourseGenetic *course);

///CourseGenetic///
int errorCourse(CourseGenetic *course);

char /*Bool*/ willRun(CourseGenetic *course);

///Individual///
int errorIndividual(Individual *individual);

/////////////////////////////////Functions used for solving/////////////////////////////////
void printEmail();

void solve();

int parseInput(List *subjects, List *profs, List *TAs, List *students);

/////////////////////////////////Generic macroses/////////////////////////////////

#define isAvailable(X, Y) _Generic((X), \
    ProfessorGenetic*: isAvailableProf, \
    TAGenetic*: isAvailableTA\
)(X, Y)

#define AssignTo(X, Y) _Generic((X), \
    ProfessorGenetic*: AssignToProf,\
    TAGenetic*: AssignToTA\
)(X, Y)

#define error(X) _Generic((X), \
    ProfessorGenetic*: errorProf,\
    TAGenetic*: errorTA,\
    CourseGenetic*: errorCourse,\
    Individual*: errorIndividual\
)(X)

/////////////////////////////////Global variables/////////////////////////////////
char *global_buffer;
HashTable *subjectByName = NULL;
int subjects_count = 0;
int profs_count = 0;
int tas_count = 0;
int students_count = 0;


int ta = 1;

List *standardProfessorPool = NULL;
List *standardTAPool = NULL;
CourseGenetic *standardCoursePool;


/////////////////////////////////Util functions/////////////////////////////////
//erase the whole block of memory
void empty(char *str, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        str[i] = 0;
    }
}

// removes \n from global_buffer
void trim() {
    if (global_buffer[strlen(global_buffer) - 1] == '\n') global_buffer[strlen(global_buffer) - 1] = '\0';
}

int cmpStr(void *s1, void *s2) {
    return strcmp((char *) s1, (char *) s2);
}

void swap(CourseGenetic *a, CourseGenetic *b) {
    CourseGenetic temp = *a;
    *a = *b;
    *b = temp;
}

void shuffle(CourseGenetic arr[], int n) {
    for (int i = n - 1; i > 0; --i) {
        int j = rand() % (i + 1);

        swap(&arr[i], &arr[j]);
    }
}

void introduce() {
    printf("Sizeof(Node) = %ld\n", sizeof(Node));
    printf("Sizeof(List) = %ld\n", sizeof(List));
    printf("Sizeof(HashNode) = %ld\n", sizeof(HashNode));
    printf("Sizeof(HashList) = %ld\n", sizeof(HashList));
    printf("Sizeof(HashTable) = %ld\n", sizeof(HashTable));
    printf("Sizeof(Subject) = %ld\n", sizeof(Subject));
    printf("Sizeof(Faculty) = %ld\n", sizeof(Faculty));
    printf("Sizeof(Student) = %ld\n", sizeof(Student));
    printf("Sizeof(ProfessorGenetic) = %ld\n", sizeof(ProfessorGenetic));
    printf("Sizeof(TAGenetic) = %ld\n", sizeof(TAGenetic));
    printf("Sizeof(CourseGenetic) = %ld\n", sizeof(CourseGenetic));
    printf("Sizeof(Individual) = %ld\n", sizeof(Individual));
//    printf("Sizeof() = %lld", sizeof());
}

int main() {

//    introduce();

    srand((unsigned int) time(NULL));
    global_buffer = (char *) malloc(MAX_BUFFER_SIZE);
//    printEmail() //TODO don't forget to call it
    solve();

    free(global_buffer);
    return 0;
}

void printEmail() {
    FILE *email = fopen("DaniilManakovskiyEmail.txt", "w");
    fprintf(email, "d.manakovskiy@innopolis.university");
    fclose(email);
}

void solve() {
    for (int test = 1; test <= MAXN; ++test) {
        char num[FILENAME_SIZE];
        //writes to the buffer filename of the current input file
        snprintf(num, FILENAME_SIZE, "input%d.txt", test);
        // if input file exist, solve the problem
        if (freopen(num, "r", stdin) != NULL) {
            snprintf(num, FILENAME_SIZE, "output%d.txt", test);
//            freopen(num, "w", stdout);

            //solve task for a particular input
            List *subjects = malloc(sizeof(List));
            List *profs = malloc(sizeof(List));
            List *tas = malloc(sizeof(List));
            List *students = malloc(sizeof(List));


            initList(subjects, sizeof(Subject));
            initList(profs, sizeof(Faculty));
            initList(tas, sizeof(Faculty));
            initList(students, sizeof(Student));


            subjectByName = (HashTable *) malloc(sizeof(HashTable));
            hash_table_init(subjectByName, 5, MAX_ENTRY_SIZE, sizeof(Subject *), cmpStr);

            if (parseInput(subjects, profs, tas, students) != 0) {
                printf("Invalid input.");
                continue;
            }


//            printf("\n------------\n");
//            printList(subjects, printSubject);
//            printf("\n------------\n");
//            printList(profs, printFaculty);
//            printf("\n------------\n");
//            printList(tas, printFaculty);
//            printf("\n------------\n");
//            printList(students, printStudent);

            printf("\n------Done with input------\n");
//            input_finished = 1;

            standardProfessorPool = malloc(sizeof(List));
            initList(standardProfessorPool, sizeof(ProfessorGenetic *));

            standardTAPool = malloc(sizeof(List));
            initList(standardTAPool, sizeof(TAGenetic *));

            standardCoursePool = (CourseGenetic *) malloc(subjects_count * sizeof(CourseGenetic));

            for (int i = 0; i < profs_count; ++i) {
                ProfessorGenetic *new = malloc(sizeof(ProfessorGenetic));
                new->professor = (Faculty *) getFromList(profs, i);
                new->courses_teaching_count = 0;
                new->courses_teaching = malloc(sizeof(List));
                initList(new->courses_teaching, sizeof(CourseGenetic *)); //TODO really this datatype?
                new->isDoingWrongSubject = False;
                new->isBusy = False;
                pushBack(standardProfessorPool, new);
            }

//            printList(standardProfessorPool, printProfessorGenetic);


            for (int i = 0; i < tas_count; ++i) {
                TAGenetic *new = malloc(sizeof(TAGenetic));
                new->TA = (Faculty *) getFromList(tas, i);
                new->courses_teaching_count = 0;
                new->courses_teaching = malloc(sizeof(List));
                initList(new->courses_teaching, sizeof(CourseGenetic *)); //TODO really this datatype?
                pushBack(standardTAPool, new);
            }

            for (int i = 0; i < subjects_count; ++i) {
                standardCoursePool[i].subject = (Subject *) getFromList(subjects, i);
                standardCoursePool[i].prof = NULL;
                standardCoursePool[i].TAs = malloc(sizeof(List));
                initList(standardCoursePool[i].TAs, sizeof(TAGenetic *)); //TODO really this datatype?
                standardCoursePool[i].TA_assigned = 0;
            }


            Individual *population = (Individual *) malloc(POPULATION_SIZE * sizeof(Individual));

            /*TODO: Когда будешь чистить индивида, учти, что нужно удалить профессоров и ТА из пула и из курсов,
             * ибо они были тупо скопированы.
             * Сами курсы тоже нужно почистить изнутри
             *
             */
            // generate random population
            for (int i = 0; i < POPULATION_SIZE; ++i) {
                List *profPool = malloc(sizeof(List));
                initList(profPool, sizeof(ProfessorGenetic *));

                List *taPool = malloc(sizeof(List));
                initList(taPool, sizeof(TAGenetic *));

                listCopy(profPool, standardProfessorPool, sizeof(ProfessorGenetic));
                listCopy(taPool, standardTAPool, sizeof(TAGenetic));

                CourseGenetic *courses = (CourseGenetic *) malloc(subjects_count * sizeof(CourseGenetic));
                memcpy(courses, standardCoursePool, subjects_count * sizeof(CourseGenetic));

                int profPoolSize = profs_count;
                int taPoolSize = tas_count;


                shuffle(courses, subjects_count);
                printf("#%3d:\n", i);
                for (int j = 0; j < subjects_count; ++j) {
//                    printf("%s ", courses[j].subject->name);
                    if (profPoolSize == 0) {
                        break;
                    }

                    int randomInd = rand() % profPoolSize;
                    ProfessorGenetic *prof = getFromList(profPool, randomInd);

                    if (isAvailable(prof, &courses[j]) == False) {
                        continue;
                    }

                    AssignTo(prof, &courses[j]);

                    if (prof->isBusy == True) {
                        removeFromList(profPool, randomInd);
                        profPoolSize--;
                    }

                    // All professors assigned

                    population[i].schedule = courses;

//                    printf("\t%s - %s\n", population[i].schedule[j].subject->name, population[i].schedule[j].prof->professor->fullname);
                }
                printf("\n\n");
            }

//            for (int i = 0; i < POPULATION_SIZE; ++i) {
//                Node *profPool = NULL;
//                listCopy(&profPool, standardProfessorPool);
//                Node *taPool = NULL;
//                listCopy(&taPool, standardTAPool);
//                CourseGenetic *courses = (CourseGenetic *) malloc(subjects_count * sizeof(CourseGenetic));
//                memcpy(courses, standardCoursePool, subjects_count * sizeof(CourseGenetic));
//                // all values are copied to the individual
//
//                int profPoolSize = profs_count;
//                int taPoolSize = tas_count;
//
//
//                shuffle(courses, subjects_count);
//                printf("#%3d: ", i);
//                for (int j = 0; j < subjects_count; ++j) {
//                    if (profPoolSize == 0) {
//                        break;
//                    }
//
//                    Node *profNode = getFromList(profPool, rand() % profPoolSize);
//                    ProfessorGenetic *prof = profNode->data;
//
//                    if (isAvailable(prof, &courses[j]) == False) { //TODO implement
//                        continue;
//                    }
//
//                    AssignTo(prof, &courses[j]); //TODO implement
//
//                    if (isBusy(prof) == True) {
//                        //TODO: remove from professor pool
//                    }
//
//
//                }
//                printf("\n");
//                population[i].schedule = courses;
//
//
////                printList(profPool, printProfessorGenetic);
////                printList(taPool, printTAGenetic);
//
//
//                freeList(profPool, NULL);
//                freeList(taPool, NULL);
//            }
//
//
            freeList(subjects, (void (*)(void *)) del_subject);
            freeList(profs, (void (*)(void *)) del_faculty);
            freeList(tas, (void (*)(void *)) del_faculty);
            freeList(students, (void (*)(void *)) del_student);
            freeList(standardProfessorPool, (void (*)(void *)) del_faculty_genetic);
            freeList(standardTAPool, (void (*)(void *)) del_faculty_genetic);

            for (int i = 0; i < subjects_count; ++i) {
                free(standardCoursePool[i].TAs);
            }
            free(standardCoursePool);

            free(subjects);
            free(profs);
            free(tas);
            free(students);
            free(standardProfessorPool);
            free(standardTAPool);

            //TODO: free the population (Hope for the best)

        }
    }

}

int parseInput(List *subjects, List *profs, List *TAs, List *students) {
    //reading subjects, until "P" is met
    //TODO: continue migration (done?)
    while (getline(&global_buffer, &MAX_BUFFER_SIZE, stdin) != -1 && strcmp(global_buffer, "P\n") != 0) {

        trim();

        char *course_name = (char *) malloc(MAX_ENTRY_SIZE);
        int labs_required, students_allowed;

        sscanf(global_buffer, "%s %d %d", course_name, &labs_required, &students_allowed);
        Subject *n = (Subject *) malloc(sizeof(Subject));
        new_subject(n, course_name, labs_required, students_allowed);
        pushBack(subjects, n);
        insert(subjectByName, course_name, n);
        subjects_count++;
        free(course_name);
//        free(n);
    }
    //EOF right after course block is unacceptable
    if (strcmp(global_buffer, "P\n") != 0)
        return 1;

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
        // the format is always like {fullname} {surname} <{ID}> {List of subjects}
        char *token = strtok(global_buffer, " ");
        char *name = malloc(strlen(token) + 1);
        strcpy(name, token);
        token = strtok(NULL, " ");
        char *surname = malloc(strlen(token) + 1);
        strcpy(surname, token);
        token = strtok(NULL, " ");

        List *head = malloc(sizeof(List));
        initList(head, sizeof(char *));
        int len = 0;


        char *student_id;
        if (status == STUDENT) {
            student_id = malloc(strlen(token) + 1);
            strcpy(student_id, token);
            token = strtok(NULL, " ");
        }


        while (token != NULL) {
            char *tmp = malloc(strlen(token) + 1);
            strcpy(tmp, token);
            pushBack(head, tmp);
            len++;

            // if list of required courses for student started
            if (status == STUDENT) {
                Subject *subj = get_el(subjectByName, token);
                if (subj == NULL) {
                    return 1; // student requires the course that doesn't exists
                }
                subj->selectedCount++;
//                free(subj);
            }
            token = strtok(NULL, " ");
        }


        void *n;
        if (status == STUDENT) {
            n = (Student *) malloc(sizeof(Student));
            new_student(n, name, surname, student_id);
            ((Student *) n)->required_courses_count = len;
            ((Student *) n)->required_courses = head;
            free(student_id);


        } else {
            n = (Faculty *) malloc(sizeof(Faculty));
            new_faculty(n, name, surname);
            ((Faculty *) n)->trained_for_count = len;
            ((Faculty *) n)->trained_for = head;
        }


        switch (status) {
            case PROFESSOR: {
                pushBack(profs, n);
                ++profs_count;
                break;
            }
            case TA: {
                pushBack(TAs, n);
                ++tas_count;
                break;
            }
            case STUDENT: {
                pushBack(students, n);
                ++students_count;
                break;
            }
        }


        free(name);
        free(surname);
//        free(head);
        free(token);
//        free(n);
    }

    //
    if (status != STUDENT)
        return 1;
    //TODO: check if at least 1 student was introduced

    return 0;
}


/////////////////////////////////Data structures/////////////////////////////////
///Linked List///
void removeFromList(List *list, int index) {
    Node *tmp = getNodeFromList(list, index);
    if (tmp != NULL) {
        list->size--;

        if (list->size == 0) {
            list->head = NULL;
            list->tail = NULL;
            list->cursor = NULL;
            list->cursor_position = 0;
            free(tmp);
            return;
        }

        if (tmp == list->head) {
            list->head = tmp->next;
            list->head->prev = NULL;
            list->cursor = list->head;
            list->cursor_position = 0;
            free(tmp);
            return;
        }

        if (tmp == list->tail) {
            list->tail = tmp->prev;
            list->tail->next = NULL;
            list->cursor = list->tail;
            list->cursor_position = list->size - 1;
            free(tmp);
            return;
        }

        tmp->prev->next = tmp->next;
        tmp->next->prev = tmp->prev;
        free(tmp);
    }
}

Node *getNodeFromList(List *list, int ind) {
    while (ind < 0) {
        ind += list->size;
    }
    if (ind >= list->size) {
        return NULL;
    }

    int diff = ind - list->cursor_position; // if positive, go right, else - left

    if (ind < abs(diff)) {
        diff = ind;
        list->cursor = list->head;
    } else if (list->size - ind < abs(diff)) {
        diff = -ind;
        list->cursor = list->tail;
    }

    while (diff != 0) {
        if (diff < 0) {
            list->cursor = list->cursor->prev;
            ++diff;
        } else {
            list->cursor = list->cursor->next;
            --diff;
        }
    }
    list->cursor_position = ind;
    return list->cursor;
}

void *getFromList(List *list, int ind) {
    Node *res = getNodeFromList(list, ind);
    if (res != NULL) {
        return res->data;
    } else {
        return NULL;
    }
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

void printSubject(void *c) {
    Subject *subject = (Subject *) c;
    printf("Subject: %s\nLabs: %d\nStudents: %d\nRequired by %d students\n\n", subject->name, subject->required_labs,
           subject->allowed_students, subject->selectedCount);
}

void printFaculty(void *f) {
    Faculty *faculty = (Faculty *) f;
    printf("Faculty: %s\n%d) ", faculty->fullname, faculty->trained_for_count);
    printList(faculty->trained_for, printString);
    printf("\n\n");
}

void printStudent(void *s) {
    Student *student = (Student *) s;
    printf("Student: %s\nID: %s\nRequirements:", student->name, student->ID);
    printList(student->required_courses, printString);
    printf("\n\n");
}

void printProfessorGenetic(void *pg) {
    ProfessorGenetic *prof = (ProfessorGenetic *) pg;
    printf("ProfessorGenetic: %s\nIs doing wrong: %d\n%d) : ", prof->professor->fullname,
           prof->isDoingWrongSubject, prof->courses_teaching_count);
    printList(prof->courses_teaching, printCourseGenetic);
    printf("\n\n");
}

void printCourseGenetic(void *cg) {
    CourseGenetic *course = (CourseGenetic *) cg;
    printf("\tSubject: %s\n\tProf: %s\n\t#TA: %d\n\t", course->subject->name, course->prof->professor->fullname/*,
           course->TA_assigned*/);
}

void printTAGenetic(void *taG) {
    TAGenetic *ta = (TAGenetic *) taG;
    printf("TAGenetic: %s\n%d) ", ta->TA->fullname, ta->courses_teaching_count);
    printList(ta->courses_teaching, printCourseGenetic);
    printf("\n\n");

}

void pushBack(List *list, void *data) {
    Node *new_node = (Node *) malloc(sizeof(Node));

//    new_node->data = malloc(list->dataSize);
    //copy value of data
//    memcpy(new_node->data, data, list->dataSize);

    new_node->data = data;

    if (list->head == NULL) { // no elements were introduced
        list->head = new_node;
        list->tail = new_node;
        list->cursor = new_node;
        list->cursor_position = 0;
        new_node->prev = NULL;
        new_node->next = NULL;

    } else {
        list->tail->next = new_node;
        new_node->prev = list->tail;
        new_node->next = NULL;
        list->tail = new_node;
    }
    ++(list->size);

}

void initList(List *list, size_t dataSize) {
    list->head = NULL;
    list->tail = NULL;
    list->cursor = NULL;
    list->cursor_position = 0;
    list->size = 0;
    list->dataSize = dataSize;
}


void listCopy(List *dest, List *src, size_t datasize) {
    dest->dataSize = src->dataSize;
    if (dest->head != NULL) {
        perror("Destination list must be empty!\n");
        return;
    }

    void *tmp;

    for (int i = 0; i < src->size; ++i) {
        tmp = malloc(datasize);
        memcpy(tmp, getFromList(src, i), datasize);
        pushBack(dest, tmp);
    }
}

void printList(List *list, void (*print_function)(void *)) {
    Node *node = list->head;
    while (node != NULL) {
        (*print_function)(node->data);
        node = node->next;

    }
}

void freeList(List *list, void (*destructor)(void *)) {
    Node *node = list->head;
    Node *next;
    while (node != NULL) {
        next = node->next;
        if (destructor != NULL)
            destructor(node->data);
        free(node->data);
        node->prev = NULL;
        node->next = NULL;
        free(node);
        node = next;
    }
}

char /*Bool*/ isInList(List *list, void *data, int (*key_cmp)(void *, void *)) {
    for (int i = 0; i < list->size; ++i) {
        if (key_cmp(data, getFromList(list, i)) == 0) {
            return True;
        }
    }
    return False;
}

///Hash Map///

ull hash(const void *data, size_t size) {
    char *i = (char *) data;
    ull result = 0;
    ull mult = 1;
    for (int j = 0; j < size; j++, i++) {
        if (*i == '\0') // now map works only for string keys
            break;
        result += mult * (*i);
        result %= ULLONG_MAX; // just in case of overflow
        mult *= z;
    }
    return result;
}

void free_hash_entry(HashNode *node) {
    if (node != NULL) {
        if (node->key != NULL) free(node->key);
        if (node->value != NULL) free(node->value);
    }
}


void
hash_table_init(HashTable *table, size_t capacity, size_t key_size, size_t value_size, int (*key_cmp)(void *, void *)/*,
                void (*key_destructor)(void *), void (*value_destructor)(void *)*/) {
    table->key_size = key_size;
    table->value_size = value_size;
    table->capacity = capacity;
    table->datalist = (HashList *) malloc(capacity * sizeof(HashList));
    table->key_comparator = key_cmp;
    table->size = 0;
//    table->key_destructor = key_destructor;
//    table->value_destructor = value_destructor;
    init_datalist(table->datalist, capacity);
}

void insert(HashTable *table, void *key, void *value) {
    ull index = hash(key, table->key_size) % table->capacity;

//    printf("%s -> %d\n", (char*)key, index);

    HashNode *node = (table->datalist[index]).head;

    HashNode *item = (HashNode *) malloc(sizeof(HashNode));
    item->key = malloc(table->key_size);
//    item->value = malloc(table->value_size);
    item->next = NULL;
    item->prev = NULL;
    memcpy(item->key, key, table->key_size);
//    memcpy(item->value, value, table->value_size);
    item->value = value;

    if (node == NULL) {
        // definetely no item
        (table->datalist)[index].head = item;
        (table->datalist)[index].tail = item;
        table->size++;
    } else {
        // iterate over the list. If needed node is not present - add to the
        // list
        while (node != NULL) {
            if (table->key_comparator(node->key, key) == 0) {
//                memcpy(node->value, value, table->value_size);
                node->value = value;
                free_hash_entry(item);
                free(item);
                break;
            }
            node = node->next;
        }
        if (node == NULL) {
            (table->datalist)[index].tail->next = item;
            (table->datalist)[index].tail = item;
            table->size++;
        }

    }

    double load_factor = (1.0 * table->size) / table->capacity;
    if (load_factor >= MAX_LOAD_FACTOR) {
        rehash(table);
    }
}


void free_hash_table(HashTable *table) {
    if (table != NULL) {
        HashNode *node = NULL;
        for (int i = 0; i < table->capacity; ++i) {
            //start from tail
            node = table->datalist[i].tail;
            //if hash_list is empty - go next
            if (node == NULL) continue;
            //until node passed the head of a hash_list, free it, then move_back
            while (node != NULL) {
                if (node != table->datalist[i].tail) free(node->next);
                free_hash_entry(node);
                node = node->prev;
                table->size--;
            }
            if (table->size == 0) break;
        }
        free(table->datalist);
        free(table);

    }
}

void rehash(HashTable *table) {
    HashList *old = table->datalist;
    HashTable *temp = malloc(sizeof(HashTable));
    hash_table_init(temp, 2 * table->capacity, table->key_size, table->value_size, table->key_comparator);

    for (int i = 0; i < table->capacity; ++i) {
        HashNode *list = old[i].head;
        // if hash_list contains anything, insert it into a new ht
        while (list != NULL) {
            insert(temp, list->key, list->value);
            list = list->next;
        }
    }
    table->capacity *= 2;
    table->datalist = temp->datalist;
    printf("New size: %zu\n", table->capacity);
    free(temp);
}


void init_datalist(HashList *datalist, size_t capacity) {
    for (size_t i = 0; i < capacity; i++) {
        datalist[i].head = NULL;
        datalist[i].tail = NULL;
    }
}

//WARNING! Returns a pointer to actual node in table
HashNode *find_node(HashTable *table, void *key) {
//    printf("find(%s) -> %d\n", (char *) key, hash(key, table->key_size) % table->capacity);
    HashNode *node = table->datalist[hash(key, table->key_size) % table->capacity].head;
    while (node != NULL) {
        if (table->key_comparator(key, node->key) == 0) {
            return node;
        }
        node = node->next;
    }
    return NULL;
}

void *get_el(HashTable *table, void *key) {
    HashNode *tmp = find_node(table, key);
    if (tmp != NULL) {
//        void *result = malloc(table->value_size);
//        memcpy(result, tmp->value, table->value_size);
        return tmp->value;
    } else {
        return NULL;
    }
}

void remove_el(HashTable *table, void *key) {
    HashNode *tmp = find_node(table, key);
    if (tmp != NULL) {
        size_t index = hash(key, table->key_size) % table->capacity;
        if (tmp->next != NULL && tmp->prev != NULL) {
            tmp->prev->next = tmp->next;
            tmp->next->prev = tmp->prev;

        } else if (tmp->prev != NULL) {
            tmp->prev->next = NULL;
            table->datalist[index].tail = tmp->prev;
        } else if (tmp->next != NULL) {
            tmp->next->prev = NULL;
            table->datalist[index].head = tmp->next;
        } else {
            table->datalist[index].tail = NULL;
            table->datalist[index].head = NULL;
        }
        free(tmp->key);
        free(tmp->value);
        free(tmp);
    }
}


///Constructors//
void new_subject(Subject *new, char *name, int labs, int students) {
    if (new == NULL)
        return;
    new->name = malloc(strlen(name) + 1);
    strcpy(new->name, name);
    new->required_labs = labs;
    new->allowed_students = students;
    new->selectedCount = 0;
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


    new->fullname = malloc(strlen(full_name) + 1);
    strcpy(new->fullname, full_name);
//    new->trained_for_count = 0;

//    new->trained_for = malloc(sizeof(List));
//    initList(new->trained_for, MAX_ENTRY_SIZE);

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
//    new->required_courses_count = 0;
//    new->required_courses = malloc(sizeof(List));
//    initList(new->required_courses, MAX_ENTRY_SIZE);

    free(full_name);

}

///Destructors//
void del_subject(Subject *subj) {
    free(subj->name);
}

void del_faculty(Faculty *f) {
    free(f->fullname);
    freeList(f->trained_for, NULL);
}

void del_student(Student *s) {
    free(s->name);
    free(s->ID);
    freeList(s->required_courses, NULL);
}

void del_faculty_genetic(ProfessorGenetic *prof) {
    free(prof->courses_teaching);
}

///Functions on DT///
///ProfessorGenetic///
char /*Bool*/ isAvailableProf(ProfessorGenetic *prof, CourseGenetic *course) {
    if (prof->courses_teaching_count == 0) {
        return True;
    }

    if (prof->courses_teaching_count == 1) {
        return !prof->isDoingWrongSubject &&
               isInList(prof->professor->trained_for, course->subject->name, cmpStr) == True;
    }

    return False;
}

void AssignToProf(ProfessorGenetic *prof, CourseGenetic *course) {
    prof->courses_teaching_count++;
    pushBack(prof->courses_teaching, course);

    if (isInList(prof->professor->trained_for, course->subject->name, cmpStr) == False) {
        prof->isDoingWrongSubject = True;
    }

    course->prof = prof;

    if (prof->isDoingWrongSubject == True || prof->courses_teaching_count == 2) {
        prof->isBusy = True;
    }
}
