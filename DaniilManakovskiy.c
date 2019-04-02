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
#include <limits.h>

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
} Node;

void printList(Node *node, void (*print_function)(void *));

void freeList(Node *node, void (*destructor)(void *));

// functions for different representation of void* in list nodes
void printInt(void *n);

void printDouble(void *f);

void printString(void *s);

void printSubject(void *c);

void printFaculty(void *f);

void printStudent(void *s);

void pushFront(Node **head, void *data, size_t size);

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
    Node *trained_for; // array for strings - names of courses
} Faculty;

typedef struct student {
    char *name;
    char *ID;
    int required_courses_count;
    Node *required_courses;
} Student;


///Genetic data structures///
typedef struct prof {
    Faculty *professor;
    char /*Bool*/ isDoingWrongSubject;
    int courses_teaching_count;
    Node *courses_teaching; // what courses prof teaches
} ProfessorGenetic;

typedef struct ta {
    Faculty *TA;
    int courses_teaching_count;
    Node *courses_teaching; // what courses prof teaches
} TAGenetic;

typedef struct course {
    Subject *subject;
    ProfessorGenetic *prof;
    int TA_assigned;
    Node *TAs;
} CourseGenetic;

typedef struct individ {
    Node *schedule; //list of courses
    Node *professors;
    Node *TAs;
} Individual;

/////////////////////////////////Utility functions and constructors/////////////////////////////////
///Constructors//
//All of them fills the fields of stuct, given pre-allocated pointer to the struct
void new_subject(Subject *new, char *name, int labs, int students);

void new_faculty(Faculty *new, char *first_name, char *last_name);

void new_student(Student *new, char *first_name, char *last_name, char *ID);


///Destructors//
void del_subject(Subject *subj);

void del_faculty(Faculty *f);

void del_student(Student *s);

///ProfessorGenetic///
char /*Bool*/ isAvailableProf(ProfessorGenetic *prof, CourseGenetic *course);

char /*Bool*/ isBusy(ProfessorGenetic *prof);

void AssignToProf(ProfessorGenetic *prof, CourseGenetic *course);

char /*Bool*/ CanTeachSubject(ProfessorGenetic *prof, Subject *subj);

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

int parseInput();

/////////////////////////////////Generic macroses/////////////////////////////////

#define isAvailable(X, Y) _Generic((X), \
    ProfessorGenetic*: isAvailableProf \
    TAGenetic*: isAvailableTA\
)(X, Y)

#define AssignTo(X, Y) _Generic((X), \
    ProfessorGenetic*: AssignToProf\
    TAGenetic*: AssignToTA\
)(X, Y)

#define error(X) _Generic((X), \
    ProfessorGenetic*: errorProf\
    TAGenetic*: errorTA\
    CourseGenetic*: errorCourse\
    Individual*: errorIndividual\
)(X)

/////////////////////////////////Global variables/////////////////////////////////
char *global_buffer;
HashTable *subjectByName = NULL;
int subjects_count = 0;
int profs_count = 0;
int TAs_count = 0;
int students_count = 0;

ProfessorGenetic *standardProfessorPool;
TAGenetic *standardTAPool;
CourseGenetic *standardCoursePool;


///UTIL FUNCTIONS///
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

int main() {
    global_buffer = (char *) malloc(MAX_BUFFER_SIZE);
//    printEmail() //TODO don't forget to call it
//    test();
    solve();

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
            Node *subjects = NULL;
            Node *profs = NULL;
            Node *TAs = NULL;
            Node *students = NULL;


            subjectByName = (HashTable *) malloc(sizeof(HashTable));
            hash_table_init(subjectByName, 5, MAX_ENTRY_SIZE, sizeof(Node *), cmpStr);

            if (parseInput(&subjects, &profs, &TAs, &students) != 0) {
                printf("Invalid input.");
                continue;
            }


            printf("\n------------\n");
            printList(subjects, printSubject);
            printf("\n------------\n");
            printList(profs, printFaculty);
            printf("\n------------\n");
            printList(TAs, printFaculty);
//            printf("\n------------\n");
//            printList(students, printStudent);

            printf("Done with input\n");

            standardProfessorPool = (ProfessorGenetic *) malloc(profs_count * sizeof(ProfessorGenetic));
            standardTAPool = (TAGenetic *) malloc(TAs_count * sizeof(TAGenetic));
            standardCoursePool = (CourseGenetic *) malloc(subjects_count * sizeof(CourseGenetic));

            Node *iterator = profs;
            for (int i = 0; i < profs_count; ++i) {
                standardProfessorPool[i].professor = iterator->data;
                standardProfessorPool[i].courses_teaching_count = 0;
                standardProfessorPool[i].isDoingWrongSubject = False;
                standardProfessorPool[i].courses_teaching = NULL;
                iterator = iterator->next;
            }

            iterator = TAs;
            for (int i = 0; i < TAs_count; ++i) {
                standardTAPool[i].TA = iterator->data;
                standardTAPool[i].courses_teaching_count = 0;
                standardTAPool[i].courses_teaching = NULL;
                iterator = iterator->next;
            }

            iterator = subjects;
            for (int i = 0; i < subjects_count; ++i) {
                standardCoursePool[i].subject = iterator->data;
                standardCoursePool[i].prof = NULL;
                standardCoursePool[i].TAs = NULL;
                standardCoursePool[i].TA_assigned = 0;
                iterator = iterator->next;
            }

            Individual *population = (Individual *) malloc(POPULATION_SIZE * sizeof(Individual));
            // generate random population
            for (int i = 0; i < POPULATION_SIZE; ++i) {

            }


            freeList(subjects, (void (*)(void *)) del_subject);
            freeList(profs, (void (*)(void *)) del_faculty);
            freeList(TAs, (void (*)(void *)) del_faculty);
            freeList(students, (void (*)(void *)) del_student);
            free(standardProfessorPool);
            free(standardTAPool);
            free(standardCoursePool);
        }
    }

}

int parseInput(Node **subjects, Node **profs, Node **TAs, Node **students) {
    //reading subjects, until "P" is met
    while (getline(&global_buffer, &MAX_BUFFER_SIZE, stdin) != -1 && strcmp(global_buffer, "P\n") != 0) {

        trim();

        char *course_name = (char *) malloc(MAX_ENTRY_SIZE);
        int labs_required, students_allowed;

        sscanf(global_buffer, "%s %d %d", course_name, &labs_required, &students_allowed);
        Subject *n = (Subject *) malloc(sizeof(Subject));
        new_subject(n, course_name, labs_required, students_allowed);
        pushFront(subjects, n, sizeof(Subject));
        insert(subjectByName, course_name, *subjects);
        subjects_count++;
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
        // the format is always like {fullname} {surname} <{ID}> {List of subjects}
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
            // if list of required courses for student started
            if (len >= 2 && status == STUDENT) {
                void *subj = get_el(subjectByName, token);
                if (subj == NULL) {
                    return 1; // student requires the course that doesn't exists
                }
                Subject *t = (Subject *) (((Node *) (subj))->data);
                t->selectedCount++;
                free(subj);
            }
            token = strtok(NULL, " ");
        }

        // for student retrieve his ID
        if (status == STUDENT && student_id_ancestor != NULL) {
//            printList(student_id_ancestor, printString);
//            printf(" something\n");
            if (strlen(student_id_ancestor->next->data) != 5) {
                return 1; // ID's length is more than 5 chars
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
                ++profs_count;
                break;
            }
            case TA: {
                pushFront(TAs, n, sizeof(Faculty));
                ++TAs_count;
                break;
            }
            case STUDENT: {
                pushFront(students, n, sizeof(Student));
                ++students_count;
                break;
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


/////////////////////////////////Data structures/////////////////////////////////
///Linked List///
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

void pushFront(Node **head, void *data, size_t size) {
    Node *new_node = (Node *) malloc(sizeof(Node));
    if (new_node == NULL) {
        perror("Memory for new node was not allocated");
        exit(1);
    }

    new_node->data = malloc(size);
    new_node->next = (*head);
    if (*head != NULL)
        (*head)->prev = new_node;
    new_node->prev = NULL;

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

void freeList(Node *node, void (*destructor)(void *)) {
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
    item->value = malloc(table->value_size);
    item->next = NULL;
    item->prev = NULL;
    memcpy(item->key, key, table->key_size);
    memcpy(item->value, value, table->value_size);

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
                memcpy(node->value, value, table->value_size);
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
        void *result = malloc(table->value_size);
        memcpy(result, tmp->value, table->value_size);
        return result;
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


