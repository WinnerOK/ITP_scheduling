/**
 * TODO: validate all input
 * - TA must have exactly 1 trained_for course
 * - Prof has at least 1 trained_for course
 * - Student has at least 1 required course
 *
 *
 * - ПРОВЕРЬ НА ПРОБЕЛЫ В КОНЦЕ СТРОКИ И ПУСТУЮ СТРОКУ В КОНЦЕ - ИХ БЫТЬ НЕ ДОЛЖНО
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include <errno.h>
#include <ctype.h>
#include <stdarg.h>


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
char *sep = " "; // separator between first and last names

///Evolution Constants///
#define POPULATION_SIZE 1000
#define EVOLUTION_STEPS 1000
#define BEST_FIT_PERCENTAGE 0.1
#define GOOD_FIT_PERCENTAGE 0.2
#define MUTATION_PROBABILITY 0.1
#define TAKE_SECOND_PARENT_GENE (1 - (MUTATION_PROBABILITY))
#define TAKE_FIRST_PARENT_GENE ((TAKE_SECOND_PARENT_GENE)/2)
#define BEST_COUNT (int)(POPULATION_SIZE*BEST_FIT_PERCENTAGE)
#define CROSSING_PARENTS_COUNT (int)(POPULATION_SIZE * GOOD_FIT_PERCENTAGE)
#define MAX_STEPS_WITHOUT_IMPROVEMENT 25

///Error Penalties///
#define COURSE_NOT_RUN 20

#define PROFESSOR_UNASSIGNED 10
#define PROFESSOR_WRONG_SUBJECT 5
#define PROFESSOR_LACKING_CLASS 5

#define TA_LACKING_CLASS 2

#define STUDENT_LACKING_CLASS 1

///Constraints///
#define TA_MAX_CLASS 4
#define PROFESSOR_MAX_TRAINED_COURSES_TEACHING 2
//hardcoded that Professor can teach only 1 untrained course

///Utility Constants///
#define True 1
#define False 0

#define PROF_DELIMITER "P"
#define TA_DELIMITER "T"
#define STUDENT_DELIMITER "S"


/////////////////////////////////Data structures/////////////////////////////////
///Linked List///
typedef struct node {
    void *data;
    struct node *next, *prev;
} Node;

typedef struct list {
    int cursor_position;
    int size;
    Node *head;
    Node *tail;
    Node *cursor;
} List;


Node *getNodeFromList(List *list, int ind);

void *getFromList(List *list, int ind);

void printList(List *list, int n, void (*print_function)(void *));

void freeList(List *list, void (*destructor)(void *));

void freeListSaveData(List *list);

// functions for different representation of void* in list nodes

void printStudent(void *s);

void printTAGenetic(void *taG);

void pushBack(List *list, void *data);

void initList(List *list);

char /*Bool*/ isInList(List *list, void *data, int (*key_cmp)(void *, void *));

void removeFromList(List *list, int index);

int findIndex(List *list, void *key, int (*cmp)(void *, void *));

void removeByKey(List *list, void *key, int (*cmp)(void *, void *));

///Hash Map///
typedef struct hash_node {
    char *key;
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

    int (*key_comparator)(void *, void *);
    /* 1: key1 > key2
     * 0: key1 = key2 actually, only this is needed (!!!)
     * -1 key1 < key2
     */
} HashTable;

ull hash(const void *data);

void free_hash_entry(HashNode *node);

void init_datalist(HashList *datalist, size_t capacity);

void hash_table_init(HashTable *table, size_t capacity,
                     int (*key_cmp)(void *, void *));

void insert(HashTable *table, char *key, void *value);

void free_hash_table(HashTable *table);

void rehash(HashTable *table);

void *get_el(HashTable *table, char *key);

void remove_el(HashTable *table, char *key);

//WARNING! Returns a pointer to actual node in table
HashNode *find_node(HashTable *table, char *key);


/////////////////////////////////Data structures/////////////////////////////////
typedef enum Status {
    COURSE, PROFESSOR, TA, STUDENT
} status;

///BASIC DATA STRUCTURES///

typedef struct subject {
    char *name;
    int required_labs;
    int allowed_students;
    int selectedCount; //how many student selected this course
    List *required_by;
} Subject;


//TA and Prof are both faculty, but TA can be assigned only to those courses that he/she can teach
typedef struct faculty {
    char *fullname;
    List *trained_for; // array for strings - names of courses
} Faculty;

typedef struct student {
    char *name;
    char *ID;
    List *lacking_courses;
} Student;


///Genetic data structures///
typedef struct prof {
    Faculty *professor;
    char /*Bool*/ isDoingWrongSubject;
    char /*Bool*/ isBusy;
    List *courses_teaching; // what courses prof teaches
} ProfessorGenetic;

typedef struct ta {
    Faculty *TA;
    List *courses_teaching; // what courses prof teaches
} TAGenetic;

typedef struct course {
    Subject *subject;
    ProfessorGenetic *prof;
    List *TAs;
} CourseGenetic;

typedef struct individ {
    CourseGenetic *schedule; //list of courses
    List *allprofs;
    List *professors;
    List *allTAs;
    List *TAs;
    int error;
} Individual;

/////////////////////////////////functions on DTs and constructors/////////////////////////////////
///Constructors//
//All of them fills the fields of stuct, given pre-allocated pointer to the struct
void new_subject(Subject *new, char *name, int labs, int students);

void new_faculty(Faculty *new, char *fullname);

void new_student(Student *new, char *fullname, char *ID);

void new_professorGenetic(ProfessorGenetic *new, Faculty *prof);

void new_TAGenetic(TAGenetic *new, Faculty *TA);

void new_CourseGenetic(CourseGenetic *new, Subject *subject);


///Destructors//
void del_subject(Subject *subj);

void del_faculty(Faculty *f);

void del_student(Student *s);

void del_Professor_genetic(ProfessorGenetic *prof);

void del_TA_genetic(TAGenetic *ta);

///ProfessorGenetic///
char /*Bool*/ isAvailableProf(ProfessorGenetic *prof, CourseGenetic *course);

void AssignToProf(ProfessorGenetic *prof, CourseGenetic *course);

int errorProf(ProfessorGenetic *prof, char/*Bool*/ print);


///TAGenetic///
char /*Bool*/ isAvailableTA(TAGenetic *ta, CourseGenetic *course);

int errorTA(TAGenetic *ta, char/*Bool*/ print);

void AssignToTA(TAGenetic *ta, CourseGenetic *course);

///CourseGenetic///
int errorCourse(CourseGenetic *course, char/*Bool*/ print);

char /*Bool*/ willRun(CourseGenetic *course);

///Individual///
int errorIndividual(Individual *individual, char /*Bool*/ print);

int cmpIndividuals(Individual *i1, Individual *i2);

/////////////////////////////////Functions used for solving/////////////////////////////////
void printEmail();

void solve(int max_test);

int parseInput(List *subjects, List *profs, List *TAs, List *students);

int findMaximumInputFile();
/////////////////////////////////Input Validation/////////////////////////////////

char /*Bool*/ checkID(char *id);

char /*Bool*/ checkName(char *name);

int findAllNumbers(char *str, int max_ints, long found_numbers[]);

// Modified version of strtok from the standard library that does not skip the block
// between 2 consecutive delimiters.
// Retrieved from: https://stackoverflow.com/a/8706031
char *strtok_single(char *str, char const *delims);

/////////////////////////////////Generic macroses/////////////////////////////////

#define isAvailable(X, Y) _Generic((X), \
    ProfessorGenetic*: isAvailableProf, \
    TAGenetic*: isAvailableTA\
)(X, Y)

#define AssignTo(X, Y) _Generic((X), \
    ProfessorGenetic*: AssignToProf,\
    TAGenetic*: AssignToTA\
)(X, Y)

#define error(X, i) _Generic((X), \
    ProfessorGenetic*: errorProf,\
    TAGenetic*: errorTA,\
    CourseGenetic*: errorCourse,\
    Individual*: errorIndividual\
)(X, i)

/////////////////////////////////Global variables/////////////////////////////////
char *global_buffer;
void *new_entity = NULL;
HashTable *subjectByName = NULL;
HashTable *professorPresence = NULL;
HashTable *TAPresence = NULL;
List *subjects = NULL;
List *profs = NULL;
List *tas = NULL;
List *students = NULL;
int input_status;

/////////////////////////////////Util functions/////////////////////////////////
char /*Bool*/ isValid(const char *token) {
    return token != NULL && (*token) != '\0';
}

// removes \n from global_buffer
void trim() {
    if (global_buffer[strlen(global_buffer) - 1] == '\n') global_buffer[strlen(global_buffer) - 1] = '\0';
}

int cmpStr(void *s1, void *s2) {
    return strcmp((char *) s1, (char *) s2);
}

int cmpTA(TAGenetic *t1, TAGenetic *t2) {
    return strcmp(t1->TA->fullname, t2->TA->fullname);
}

int cmpProf(ProfessorGenetic *p1, ProfessorGenetic *p2) {
    return strcmp(p1->professor->fullname, p2->professor->fullname);
}

void swap(void **pa, void **pb) {
    void *pc;

    pc = *pa;
    *pa = *pb;
    *pb = pc;
}

CourseGenetic **shuffle(CourseGenetic *arr, int n) {
    CourseGenetic **result = (CourseGenetic **) malloc(subjects->size * sizeof(CourseGenetic *));

    for (int k = 0; k < n; ++k) {
        result[k] = &(arr[k]);
    }

    for (int i = n - 1; i > 0; --i) {
        int j = rand() % (i + 1);
        swap((void **) &(result[i]), (void **) &(result[j]));
    }

    return result;
}

double randDouble() {
    return (double) rand() / (double) RAND_MAX;
}

void getFullName(char *fullname, char *first_name, char *last_name) {
    strcpy(fullname, first_name);
    strcat(fullname, sep);
    strcat(fullname, last_name);
}

// Free all n pointers
void freeAll(int n, ...) {
    va_list ptrs;
    va_start(ptrs, n);

    while (n-- > 0) {
        void *p = va_arg(ptrs, void*);
        free(p);
    }
    va_end(ptrs);
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
    printEmail();
    solve(findMaximumInputFile());

    free(global_buffer);
    return 0;
}

void printEmail() {
    FILE *email = fopen("DaniilManakovskiyEmail.txt", "w");
    fprintf(email, "d.manakovskiy@innopolis.university");
    fclose(email);
}

int findMaximumInputFile() {
    char num[FILENAME_SIZE];
    for (int test = MAXN; test >= 1; --test) {
        //writes to the buffer filename of the current input file
        snprintf(num, FILENAME_SIZE, "input%d.txt", test);
        FILE *file;
        if ((file = fopen(num, "r"))) {
            fclose(file);
            return test;
        }
    }
    return 0;
}

void solve(int max_test) {
    char inputFile[FILENAME_SIZE];
    char outputFile[FILENAME_SIZE];
    for (int test = 1; test <= max_test; ++test) {
        //writes to the buffer filename of the current input file
        snprintf(inputFile, FILENAME_SIZE, "input%d.txt", test);
        snprintf(outputFile, FILENAME_SIZE, "DaniilManakovskiyOutput%d.txt", test);
        // if input file exist, solve the problem, otherwise print error
        freopen(outputFile, "w", stdout);
        if (freopen(inputFile, "r", stdin) != NULL) {

            int current_result = -1;
            int steps_without_improvement = 0;

            //solve task for a particular input
            subjects = malloc(sizeof(List));
            profs = malloc(sizeof(List));
            tas = malloc(sizeof(List));
            students = malloc(sizeof(List));


            initList(subjects);
            initList(profs);
            initList(tas);
            initList(students);

            subjectByName = (HashTable *) malloc(sizeof(HashTable));
            hash_table_init(subjectByName, 5, cmpStr);

            professorPresence = (HashTable *) malloc(sizeof(HashTable));
            hash_table_init(professorPresence, 5, cmpStr);

            TAPresence = (HashTable *) malloc(sizeof(HashTable));
            hash_table_init(TAPresence, 5, cmpStr);


            if (parseInput(subjects, profs, tas, students) != 0) {
                printf("Invalid input.");
                switch (input_status) {
                    case PROFESSOR:
                    case TA: {
                        del_faculty(new_entity);
                        break;
                    }
                    case STUDENT: {
                        del_student(new_entity);
                        break;
                    }
                    default: {
                    }
                }
                free(new_entity);
                freeList(subjects, (void (*)(void *)) del_subject);
                freeList(profs, (void (*)(void *)) del_faculty);
                freeList(tas, (void (*)(void *)) del_faculty);
                freeList(students, (void (*)(void *)) del_student);
                free_hash_table(TAPresence);
                free_hash_table(professorPresence);
                continue;
            }

            free_hash_table(TAPresence);
            free_hash_table(professorPresence);

//            printf("\n------Done with input------\n");
            Individual *population = (Individual *) malloc(POPULATION_SIZE * sizeof(Individual));

            // generate intial random population
            for (int i = 0; i < POPULATION_SIZE; ++i) {

                //List of all professors and TAs in order to perform easy memory cleaning
                List *allProfs = malloc(sizeof(List));
                initList(allProfs);

                List *allTA = malloc(sizeof(List));
                initList(allTA);

                //Pools contain objects of professor/ta that is available to any course
                List *profPool = malloc(sizeof(List));
                initList(profPool);

                List *taPool = malloc(sizeof(List));
                initList(taPool);

                for (int j = 0; j < profs->size; ++j) {
                    ProfessorGenetic *new = malloc(sizeof(ProfessorGenetic));
                    new_professorGenetic(new, (Faculty *) getFromList(profs, j));
                    pushBack(profPool, new);
                    pushBack(allProfs, new);
                }


                for (int j = 0; j < tas->size; ++j) {
                    TAGenetic *new = malloc(sizeof(TAGenetic));
                    new_TAGenetic(new, (Faculty *) getFromList(tas, j));
                    pushBack(allTA, new);
                    pushBack(taPool, new);
                }

                CourseGenetic *courses = (CourseGenetic *) malloc(subjects->size * sizeof(CourseGenetic));
                for (int j = 0; j < subjects->size; ++j) {
                    new_CourseGenetic(&(courses[j]), (Subject *) getFromList(subjects, j));
                }

                /* since all individuals have the same order of courses in schedule, we have to shuffle all courses to
                 * add some randomness into professor/ta distribution
                 */
                CourseGenetic **shuffled_courses = shuffle(courses, subjects->size);
                for (int j = 0; j < subjects->size; ++j) {
                    // if there is any available prof, try assign him/her to the course
                    if (profPool->size == 0) {
                        break;
                    }

                    int randomInd = rand() % profPool->size;

                    ProfessorGenetic *prof = getFromList(profPool, randomInd);

                    if (isAvailable(prof, shuffled_courses[j]) == False) {
                        continue;
                    }

                    AssignTo(prof, shuffled_courses[j]);
                    if (prof->isBusy) {
                        removeFromList(profPool, randomInd);
                    }
                }

                // All professors assigned, perform the same assignment for TAs
                for (int j = 0; j < subjects->size; ++j) {
                    //Construct a list of TAs that now can teach current course
                    List *availableTA = malloc(sizeof(List));
                    initList(availableTA);
                    for (int k = 0; k < taPool->size; ++k) {
                        TAGenetic *ta = getFromList(taPool, k);

                        if (isAvailable(ta, shuffled_courses[j])) {
                            pushBack(availableTA, ta);
                        }
                    }

                    //For each lab try to assign a TA
                    for (int l = 0; l < shuffled_courses[j]->subject->required_labs; ++l) {
                        if (availableTA->size == 0) {
                            break;
                        }

                        int randomInd = rand() % availableTA->size;
                        TAGenetic *ta = getFromList(availableTA, randomInd);

                        AssignTo(ta, shuffled_courses[j]);

                        if (ta->courses_teaching->size == TA_MAX_CLASS) {
                            removeFromList(availableTA, randomInd);
                            removeByKey(taPool, ta, (int (*)(void *, void *)) cmpTA);
                        }

                    }
                    freeListSaveData(availableTA);
                }

                population[i].allprofs = allProfs;
                population[i].schedule = courses;
                population[i].professors = profPool;
                population[i].allTAs = allTA;
                population[i].TAs = taPool;
                population[i].error = error(&(population[i]), False);
                free(shuffled_courses);
            }
            // Random population created

            //Sort all individuals by badness
            qsort(population, POPULATION_SIZE, sizeof(Individual), (__compar_fn_t) cmpIndividuals);

            for (int step = 0; step < EVOLUTION_STEPS; ++step) {
                // At each evolution step we should create new population, based on good individual of the current one
                Individual *new_population = (Individual *) malloc(POPULATION_SIZE * sizeof(Individual));

                //Some percentage of best individuals go to the next generation directly
                for (int i = 0; i < BEST_COUNT; ++i) {
                    memcpy(&(new_population[i]), &(population[i]), sizeof(Individual));
                }

                //Other part of population must be reproducted from CROSSING_PARENTS_COUNT best individuals

                for (int child_no = 0; child_no < POPULATION_SIZE - BEST_COUNT; ++child_no) {
                    Individual first_parent = population[rand() % CROSSING_PARENTS_COUNT];
                    Individual second_parent = population[rand() % CROSSING_PARENTS_COUNT];

                    // Maps TA's Fullname -> object into child
                    HashTable *TAbyName = malloc(sizeof(HashTable));
                    hash_table_init(TAbyName, (int) (tas->size * 1.5), cmpStr);

                    // Maps Professor's Fullname -> object into child
                    HashTable *ProfbyName = malloc(sizeof(HashTable));
                    hash_table_init(ProfbyName, (int) (profs->size * 1.5), cmpStr);

                    Individual *child = malloc(sizeof(Individual));

                    child->TAs = malloc(sizeof(List));
                    initList(child->TAs);

                    child->allTAs = malloc(sizeof(List));
                    initList(child->allTAs);

                    child->professors = malloc(sizeof(List));
                    initList(child->professors);

                    child->allprofs = malloc(sizeof(List));
                    initList(child->allprofs);

                    CourseGenetic *childSchedule = (CourseGenetic *) malloc(subjects->size * sizeof(CourseGenetic));
                    for (int j = 0; j < subjects->size; ++j) {
                        new_CourseGenetic(&(childSchedule[j]), (Subject *) getFromList(subjects, j));
                    }

                    for (int j = 0; j < tas->size; ++j) {
                        TAGenetic *new = malloc(sizeof(TAGenetic));
                        new_TAGenetic(new, (Faculty *) getFromList(tas, j));
                        pushBack(child->allTAs, new);
                        pushBack(child->TAs, new);
                        insert(TAbyName, new->TA->fullname, new);
                    }

                    for (int j = 0; j < profs->size; ++j) {
                        ProfessorGenetic *new = malloc(sizeof(ProfessorGenetic));

                        new_professorGenetic(new, (Faculty *) getFromList(profs, j));

                        pushBack(child->professors, new);
                        pushBack(child->allprofs, new);
                        insert(ProfbyName, new->professor->fullname, new);
                    }

                    child->schedule = childSchedule;

                    // after 2 parents was chosen and child was initialized, the crossing process starts
                    for (int i = 0; i < subjects->size; ++i) {
                        CourseGenetic *course = &(childSchedule[i]);

                        List *sourceAssistants = first_parent.schedule[i].TAs;

                        List *availableAssistants = malloc(sizeof(List));
                        initList(availableAssistants);

                        for (int k = 0; k < sourceAssistants->size; ++k) {
                            TAGenetic *ta = get_el(TAbyName,
                                                   ((TAGenetic *) (getFromList(sourceAssistants,
                                                                               k)))->TA->fullname);
                            if (ta != NULL && isAvailable(ta, course)) {
                                pushBack(availableAssistants, ta);
                            }
                        }

                        for (int j = 0; j < course->subject->required_labs; ++j) {
                            // try to find an available ta from the first parent

                            if (availableAssistants->size != 0) {
                                int randomInd = rand() % availableAssistants->size;
                                TAGenetic *ta = getFromList(availableAssistants, randomInd);

                                AssignTo(ta, course);

                                removeByKey(availableAssistants, ta, (int (*)(void *, void *)) cmpTA);
                                if (ta->courses_teaching->size == TA_MAX_CLASS) {
                                    removeByKey(child->TAs, ta, (int (*)(void *, void *)) cmpTA);
                                    remove_el(TAbyName,
                                              ta->TA->fullname); // раньше удаляло целиком объект с ключом - ТА
                                }
                                continue;
                            }

                            //try to find an available TA from the second parent
                            freeListSaveData(availableAssistants);
                            availableAssistants = malloc(sizeof(List));
                            initList(availableAssistants);
                            sourceAssistants = second_parent.schedule[i].TAs;

                            for (int k = 0; k < sourceAssistants->size; ++k) {
                                TAGenetic *ta = get_el(TAbyName,
                                                       ((TAGenetic *) (getFromList(sourceAssistants,
                                                                                   k)))->TA->fullname);
                                if (ta != NULL && isAvailable(ta, course)) {
                                    pushBack(availableAssistants, ta);
                                }
                            }

                            if (availableAssistants->size != 0) {
                                int randomInd = rand() % availableAssistants->size;
                                TAGenetic *ta = getFromList(availableAssistants, randomInd);

                                AssignTo(ta, course);

                                removeByKey(availableAssistants, ta, (int (*)(void *, void *)) cmpTA);
                                if (ta->courses_teaching->size == TA_MAX_CLASS) {
                                    removeByKey(child->TAs, ta, (int (*)(void *, void *)) cmpTA);
                                    remove_el(TAbyName, ta->TA->fullname);
                                }
                                continue;
                            }

                            //If there is any TA, pick random
                            if (child->TAs->size == 0)
                                continue;
                            int randomInd = rand() % child->TAs->size;
                            TAGenetic *ta = getFromList(child->TAs, randomInd);
                            if (isAvailable(ta, course)) {
                                AssignTo(ta, course);

                                if (ta->courses_teaching->size == TA_MAX_CLASS) {
                                    removeByKey(child->TAs, ta, (int (*)(void *, void *)) cmpTA);
                                    remove_el(TAbyName, ta->TA->fullname);
                                }
                            }
                        }
                        freeListSaveData(availableAssistants);

                        //TA assigned; Assign professor:
                        double probability = randDouble();
                        Individual *emitter = NULL;
                        if (probability < TAKE_FIRST_PARENT_GENE) {
                            emitter = &first_parent;
                        } else if (probability < TAKE_SECOND_PARENT_GENE) {
                            emitter = &second_parent;
                        }

                        if (child->professors->size == 0) continue;

                        if (emitter == NULL) {
                            int randomInd = rand() % child->professors->size;
                            ProfessorGenetic *prof = getFromList(child->professors, randomInd);
                            AssignTo(prof, course);
                            removeByKey(child->professors, prof, (int (*)(void *, void *)) cmpProf);
                            remove_el(ProfbyName, prof->professor->fullname);
                            continue;
                        }


                        ProfessorGenetic *parentProf = emitter->schedule[i].prof;
                        if (parentProf == NULL) {
                            continue;
                        } else {
                            ProfessorGenetic *childProf = get_el(ProfbyName, parentProf->professor->fullname);
                            if (childProf == NULL) {
                                if (child->professors->size == 0) continue;
                                int randomInd = rand() % child->professors->size;
                                childProf = getFromList(child->professors, randomInd);

                                AssignTo(childProf, course);

                                if (childProf->isBusy) {
                                    removeFromList(child->professors, randomInd);
                                    remove_el(ProfbyName, childProf->professor->fullname);
                                }

                            } else {
                                AssignTo(childProf, course);
                                if (childProf->isBusy) {
                                    removeByKey(child->professors, childProf, (int (*)(void *, void *)) cmpProf);
                                    remove_el(ProfbyName, childProf->professor->fullname);
                                }
                            }
                        }


                    }

                    free_hash_table(ProfbyName);
                    free_hash_table(TAbyName);

//                    free(ProfbyName);
//                    free(TAbyName);

                    child->error = error(child, False);
                    new_population[BEST_COUNT + child_no] = *child;
                    free(child); // этот объект и объект выше - разные, но внутри ссылки одинаковые.
                }

                //free all population from the inside
                for (int l = BEST_COUNT; l < POPULATION_SIZE; ++l) {
                    freeListSaveData(population[l].TAs);
                    freeListSaveData(population[l].professors);

                    freeList(population[l].allprofs, (void (*)(void *)) del_Professor_genetic);
                    freeList(population[l].allTAs, (void (*)(void *)) del_TA_genetic);

                    for (int i = 0; i < subjects->size; ++i) {
                        freeListSaveData(population[l].schedule[i].TAs);
                    }
                    free(population[l].schedule);
                }
                free(population);


                population = new_population;
                qsort(population, POPULATION_SIZE, sizeof(Individual), (__compar_fn_t) cmpIndividuals);
//                fprintf(stderr,"%d - %d\n", step, population[0].error);
                if (population[0].error == current_result) {
                    ++steps_without_improvement;
                } else {
                    current_result = population[0].error;
                    steps_without_improvement = 0;
                }

                if (steps_without_improvement == MAX_STEPS_WITHOUT_IMPROVEMENT || current_result == 0) {
//                    printf("%d\n", current_result);
                    break;
                }
            }
            for (int subj = 0; subj < subjects->size; ++subj) {
                if (willRun(&(population[0].schedule[subj]))) {
                    printf("%s\n%s\n",
                           population[0].schedule[subj].subject->name,
                           population[0].schedule[subj].prof->professor->fullname
                    );
                    printList(population[0].schedule[subj].TAs, -1, printTAGenetic);
                    printList(population[0].schedule[subj].subject->required_by,
                              population[0].schedule[subj].subject->allowed_students,
                              printStudent);
                    printf("\n");
                }
            }
            error(&(population[0]), True);
            printf("Total score is %d.", population[0].error);


//Population free start
            for (int l = 0; l < POPULATION_SIZE; ++l) {
                freeListSaveData(population[l].TAs);

                freeListSaveData(population[l].professors);

                freeList(population[l].allprofs, (void (*)(void *)) del_Professor_genetic);
                freeList(population[l].allTAs, (void (*)(void *)) del_TA_genetic);

                for (int i = 0; i < subjects->size; ++i) {
                    freeListSaveData(population[l].schedule[i].TAs);
                }
                free(population[l].schedule);
            }
            free(population);
//population free end

            freeList(subjects, (void (*)(void *)) del_subject);
            freeList(profs, (void (*)(void *)) del_faculty);
            freeList(tas, (void (*)(void *)) del_faculty);
            freeList(students, (void (*)(void *)) del_student);
        } else {
            printf("Invalid input.");
        }
    }

}

int parseInput(List *subjects, List *profs, List *TAs, List *students) {

    //get last symbol of the file. If it is a newline - immediately break
    char last_char;
    fseek(stdin, -1, SEEK_END);
    scanf("%c", &last_char);
    if (last_char == *"\n")
        return 111;
    fseek(stdin, SEEK_SET, 0);


    input_status = COURSE;
    //reading subjects, until "P" is met
    while (getline(&global_buffer, &MAX_BUFFER_SIZE, stdin) != -1 && strcmp(global_buffer, "P\n") != 0) {

//        trim();

        char *course_name = (char *) calloc(MAX_ENTRY_SIZE, sizeof(char));
        int labs_required, students_allowed, course_name_end_pos = 0;
        char *cursor = global_buffer;

        //read letters as much as possible
        while (isalpha(global_buffer[course_name_end_pos])) {
            ++course_name_end_pos;
            ++cursor;
        }
        strncpy(course_name, global_buffer, course_name_end_pos);
        // if non-letter char met, it must be a space followed by a digit
        if (!(*cursor == ' ' && isdigit(*(cursor + 1)))) {
            free(course_name);
            return 10;
        }
        // If name does not satisfy given contraints
        if (checkName(course_name) == False) {
            free(course_name);
            return 20;
        }

        ++cursor;
        long nums[2];
        // if 2 number have not been found or they overflowed
        if (findAllNumbers(cursor, 2, nums) != 0) {
            free(course_name);
            return 30;
        }

        labs_required = nums[0];
        students_allowed = nums[1];
        // if information about course contains negative numbers (it must be handled above, but for any case)
        // or zeroes
        if (labs_required <= 0 || students_allowed <= 0) {
            free(course_name);
            return 35;
        }

        //If some course was introduced twice
        if (get_el(subjectByName, course_name) != NULL) {
            free(course_name);
            return 36;
        }

        Subject *n = (Subject *) malloc(sizeof(Subject));
        new_subject(n, course_name, labs_required, students_allowed);
        pushBack(subjects, n);
        insert(subjectByName, course_name, n);
        free(course_name);


    }
    //EOF right after course block is unacceptable
    if (strcmp(global_buffer, "P\n") != 0)
        return 40;

    input_status = PROFESSOR; // what group is being read

    while (getline(&global_buffer, &MAX_BUFFER_SIZE, stdin) != -1) {
        // Performs changing of group of people Profs -> TAs
        trim();
        if (strcmp(global_buffer, TA_DELIMITER) == 0) {
            if (input_status == PROFESSOR) {
                input_status = TA;
                continue;
            } else {
                return 41;
            }
        }
        // TAs -> Students
        if (strcmp(global_buffer, STUDENT_DELIMITER) == 0) {
            if (input_status == TA) {
                input_status = STUDENT;
                continue;
            } else {
                return 42;
            }
        }

        //If before the newline/EOF character there is a non-letter symbol - input is incorrect
        if (!isalpha(global_buffer[strlen(global_buffer) - 1])) {
            return 39;
        }

        // tokenize given string by spaces
        // the format is always like {fullname} {surname} <{ID}> {List of subjects}
        char *token = strtok_single(global_buffer, " ");
        if (!isValid(token)) {
            return 43;
        }

        char *name = strdup(token);
        token = strtok_single(NULL, " ");
        if (!isValid(token)) {
            free(name);
            return 44;
        }

        char *surname = strdup(token);
        token = strtok_single(NULL, " ");
        if (!isValid(token)) {
            freeAll(2, name, surname);
            return 45;
        }

        char *fullname = (char *) malloc((2 + strlen(name) + strlen(surname)) * sizeof(char));
        getFullName(fullname, name, surname);
        if (!checkName(fullname)) {
            freeAll(3, name, surname, fullname);
            return 446;
        }
        switch (input_status) {
            case PROFESSOR: {
                if (get_el(professorPresence, fullname) != NULL) {
                    freeAll(3, name, surname, fullname);
                    return 447;
                }
                break;
            }

            case TA: {
                if (get_el(TAPresence, fullname) != NULL) {
                    freeAll(3, name, surname, fullname);
                    return 448;
                }
                break;
            }

            default: {
            }
        }

        List *head = malloc(sizeof(List));
        initList(head);
        int len = 0;


        char *student_id = NULL;
        if (input_status == STUDENT) {

            if (!checkID(token)) {
                freeList(head, NULL);
                freeAll(3, name, surname, fullname);
                return 447;
            }

            student_id = malloc(strlen(token) + 1);
            strcpy(student_id, token);
            token = strtok_single(NULL, " ");
            if (!isValid(token)) {
                freeList(head, NULL);
                freeAll(4, name, surname, fullname, student_id);
                return 46;
            }
        }


        if (input_status == STUDENT) {
            new_entity = (Student *) malloc(sizeof(Student));
            new_student(new_entity, fullname, student_id);
        } else {
            new_entity = (Faculty *) malloc(sizeof(Faculty));
            new_faculty(new_entity, fullname);
        }


        while (token != NULL) {
            if (!isValid(token)) {
                freeList(head, NULL);
                freeAll(3, name, surname, fullname);
                if (input_status == STUDENT) free(student_id);
                return 47;
            }
            if (input_status != STUDENT) {
                char *tmp = malloc(strlen(token) + 1);
                strcpy(tmp, token);
                pushBack(head, tmp);
            }
            len++;

            Subject *subj = get_el(subjectByName, token);
            if (subj == NULL) {
                freeList(head, NULL);
                freeAll(3, name, surname, fullname);
                if (input_status == STUDENT) free(student_id);
                return 48; // Found a course_name that doesn't exists
            }

            // if list of required courses for student started
            if (input_status == STUDENT) {
                pushBack(subj->required_by, new_entity);
                subj->selectedCount++;
            }
            token = strtok_single(NULL, " ");

        }


        switch (input_status) {
            case PROFESSOR: {
                ((Faculty *) new_entity)->trained_for = head;
                pushBack(profs, new_entity);
                insert(professorPresence, fullname, new_entity);
                break;
            }
            case TA: {
                ((Faculty *) new_entity)->trained_for = head;
                pushBack(TAs, new_entity);
                insert(TAPresence, fullname, new_entity);
                break;
            }
            case STUDENT: {
                free(head);
                pushBack(students, new_entity);
                break;
            }
            default: {
            }
        }

        new_entity = NULL;
        freeAll(3, name, surname, token);
        if (input_status == STUDENT) free(student_id);

    }

    //
    if (input_status != STUDENT)
        return 50;

    return 0;
}

/////////////////////////////////Input Validation/////////////////////////////////
int findAllNumbers(char *str, int max_ints, long found_numbers[]) {
    if (isdigit(str[0]) == 0) {
        return 1;
    }
    if (str[0] == '0' && isdigit(str[1])) return 1;

    char *p = calloc(strlen(str) + 1, sizeof(char));
    char *cleaner = p;
    int found_number_count = 0;
    strcpy(p, str);
    char *end;

    // strtol returns long, cast to integer behaves unexpectedly with numbers X:
    // MAX_INT <= X <= MAX_LONG, but working fine with longs
    for (long i = strtol(p, &end, 10); p != end; i = strtol(p, &end, 10)) {
        found_number_count++;
        // strtol sets errno to ERANGE in case of overflow
        if (errno == ERANGE) {
            errno = 0;
            free(cleaner);
//            freeIfPossible(p);
            return 1;
        }
        if (i > INT_MAX) {
            free(cleaner);
            return 1;
        }
        if (found_number_count > max_ints) {
            // if at some moment it findes more numbers, than
            // expected, terminates
            free(cleaner);
            return 1;
        }

        found_numbers[found_number_count - 1] = i;
        p = end;
        // p is pointing to the first symbol after the last read number

        // if we did not read all number, we have to check
        // spaces and leading zeroes between them
        // If all numbers are read, then after the loop
        // last symbol will be checked
        if (found_number_count < max_ints) {
            // problem cases:
            // there is not a number after 1 delimiter
            if (!isdigit(p[1])) {
                free(cleaner);
                return 1;
            } else {
                // there is a leading zero
                if (p[1] == '0' && isdigit(p[2])) {
                    free(cleaner);
                    return 1;
                }
            }
            // The delimiter must be a space
            if (*p != ' ') {
                free(cleaner);
                return 1;
            }
        }

        // printf("%d %d %d\n", p[0], p[1],
        //        (found_number_count == max_ints) ? 1 : 0);
    }
    // p points to the first char after the last found number
    // if flag is_end set, then it supposed to be null terminator, otherwise
    // line feed
    // printf("String ended with: |%d|\n", *p);
    if (*p != '\n') {
        free(cleaner);
        return 1;
    }
    free(cleaner);
    return (found_number_count == max_ints) ? 0 : 1;
}


char /*Bool*/ checkID(char *id) {
    if (id != NULL) {
        if (strlen(id) != ID_SIZE)
            return False;
        for (int i = 0; i < ID_SIZE; ++i) {
            if (isalnum(id[i]) == 0) {
                return False;
            }
        }
        return True;
    }
    return False;
}

char /*Bool*/ checkName(char *name) {
    if (name != NULL) {
        for (int i = 0; i < strlen(name); ++i) {
            if (isalpha(name[i]) == 0 && name[i] != ' ') {
                return False;
            }
        }
        if (strcmp(name, PROF_DELIMITER) == 0 ||
            strcmp(name, STUDENT_DELIMITER) == 0 ||
            strcmp(name, TA_DELIMITER) == 0) {
            return False;
        }
        return True;
    }
    return False;
}

char *strtok_single(char *str, char const *delims) {
    static char *src = NULL;
    char *p, *ret = 0;

    if (str != NULL)
        src = str;

    if (src == NULL)
        return NULL;

    if ((p = strpbrk(src, delims)) != NULL) {
        *p = 0;
        ret = src;
        src = ++p;

    } else if (*src) {
        ret = src;
        src = NULL;
    }

    return ret;
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
        --list->cursor_position;
        list->cursor = list->cursor->prev;
        free(tmp);
    }
}

void removeByKey(List *list, void *key, int (*cmp)(void *, void *)) {
    int ind = findIndex(list, key, cmp);
    if (ind == -1)
        return;
    removeFromList(list, ind);
}

int findIndex(List *list, void *key, int (*cmp)(void *, void *)) {

    for (int i = 0; i < list->size; ++i) {
        if (cmp(key, getFromList(list, i)) == 0) {
            return i;
        }
    }

    return -1;
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
    } else if (list->size - ind - 1 < abs(diff)) {
        diff = ind - (list->size - 1);
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
    return (res != NULL) ? res->data : NULL;
}

void printStudent(void *s) {
    Student *st = (Student *) (s);
    printf("%s %s\n", st->name, st->ID);
}

void printTAGenetic(void *taG) {
    TAGenetic *ta = (TAGenetic *) taG;
    printf("%s\n", ta->TA->fullname);

}

void pushBack(List *list, void *data) {
    Node *new_node = (Node *) malloc(sizeof(Node));

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

void initList(List *list) {
    list->head = NULL;
    list->tail = NULL;
    list->cursor = NULL;
    list->cursor_position = 0;
    list->size = 0;
}

void printList(List *list, int n, void (*print_function)(void *)) {
    int printed = 0;
    Node *node = list->head;
    while (node != NULL && printed != n) {
        (*print_function)(node->data);
        node = node->next;
        ++printed;
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
    free(list);
}

void freeListSaveData(List *list) {
    Node *node = list->head;
    Node *next;
    while (node != NULL) {
        next = node->next;
        node->prev = NULL;
        node->next = NULL;
        free(node);
        node = next;
    }
    free(list);
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
//Source: http://www.cse.yorku.ca/~oz/hash.html
ull hash(const void *data) {
    ull result = 5381;
    char *i = (char *) data;
    int c;

    while ((c = *i++))
        result = ((result << 5U) + result) + c;

    return result;
}

void free_hash_entry(HashNode *node) {
    if (node != NULL) {
        free(node->key);
    }
}


void
hash_table_init(HashTable *table, size_t capacity, int (*key_cmp)(void *, void *)) {
    table->capacity = capacity;
    table->datalist = (HashList *) malloc(capacity * sizeof(HashList));
    table->key_comparator = key_cmp;
    table->size = 0;
    init_datalist(table->datalist, capacity);
}

void insert(HashTable *table, char *key, void *value) {
    ull index = hash(key) % table->capacity;

    HashNode *node = (table->datalist[index]).head;

    HashNode *item = (HashNode *) malloc(sizeof(HashNode));

    item->next = NULL;
    item->prev = NULL;
    item->key = strdup(key);
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
                node->value = value;
                free_hash_entry(item);
                free(item);
                break;
            }
            node = node->next;
        }
        if (node == NULL) {
            (table->datalist)[index].tail->next = item;
            item->prev = (table->datalist)[index].tail;
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
        HashNode *next = NULL;
        for (int i = 0; i < table->capacity; ++i) {
            node = table->datalist[i].head;
            while (node != NULL) {
                free(node->key);
                next = node->next;
                free(node);
                node = next;
            }
        }
        freeAll(2, table->datalist, table);
    }
}

void rehash(HashTable *table) {
//    fprintf(stderr, "REHASH!\n");
    HashList *old = table->datalist;
    HashTable *temp = malloc(sizeof(HashTable));
    hash_table_init(temp, 2 * table->capacity, table->key_comparator);

    HashNode *list = NULL;
    HashNode *next = NULL;

    for (int i = 0; i < table->capacity; ++i) {
        list = old[i].head;
        // if hash_list contains anything, insert it into a new ht
        while (list != NULL) {
            insert(temp, list->key, list->value);
            free(list->key);
            next = list->next;
            free(list);
            list = next;
        }
    }
    table->capacity *= 2;
    free(table->datalist);
    table->datalist = temp->datalist;
    free(temp);
}


void init_datalist(HashList *datalist, size_t capacity) {
    for (size_t i = 0; i < capacity; i++) {
        datalist[i].head = NULL;
        datalist[i].tail = NULL;
    }
}

//WARNING! Returns a pointer to actual node in table
HashNode *find_node(HashTable *table, char *key) {
    HashNode *node = table->datalist[hash(key) % table->capacity].head;
    while (node != NULL) {
        if (table->key_comparator(key, node->key) == 0) {
            return node;
        }
        node = node->next;
    }
    return NULL;
}

void *get_el(HashTable *table, char *key) {
    HashNode *tmp = find_node(table, key);
    return (tmp != NULL) ? tmp->value : NULL;
}

void remove_el(HashTable *table, char *key) {
    HashNode *tmp = find_node(table, key);
    if (tmp != NULL) {
        size_t index = hash(key) % table->capacity;
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
        freeAll(2, tmp->key, tmp);
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
    new->required_by = malloc(sizeof(List));
    initList(new->required_by);
}

void new_faculty(Faculty *new, char *fullname) {
    if (new == NULL)
        return;
    new->fullname = fullname;
}

void new_student(Student *new, char *fullname, char *ID) {
    if (new == NULL)
        return;
    new->name = fullname;
    new->ID = malloc(ID_SIZE + 1); // ID_SIZE chars + \0
    strcpy(new->ID, ID);
    new->lacking_courses = malloc(sizeof(List));
    initList(new->lacking_courses);
}

void new_professorGenetic(ProfessorGenetic *new, Faculty *prof) {
    new->professor = prof;
    new->isDoingWrongSubject = False;
    new->isBusy = False;
    new->courses_teaching = malloc(sizeof(List));
    initList(new->courses_teaching);

}

void new_TAGenetic(TAGenetic *new, Faculty *TA) {
    new->TA = TA;
    new->courses_teaching = malloc(sizeof(List));
    initList(new->courses_teaching);
}

void new_CourseGenetic(CourseGenetic *new, Subject *subject) {
    new->prof = NULL;
    new->subject = subject;
    new->TAs = malloc(sizeof(List));
    initList(new->TAs);
}


///Destructors//
void del_subject(Subject *subj) {
//    freeList(subj->required_by, (void (*)(void *)) del_student); // TODO: check Вроде, студенты отдельно чистятся
    freeListSaveData(subj->required_by);
    free(subj->name);
}

void del_faculty(Faculty *f) {
    freeList(f->trained_for, NULL);
    free(f->fullname);
}

void del_student(Student *s) {
    freeListSaveData(s->lacking_courses);
    freeAll(2, s->name, s->ID);
}

void del_Professor_genetic(ProfessorGenetic *prof) {
    freeListSaveData(prof->courses_teaching);
}

void del_TA_genetic(TAGenetic *ta) {
    freeListSaveData(ta->courses_teaching);
}

///Functions on DT///
///ProfessorGenetic///
char /*Bool*/ isAvailableProf(ProfessorGenetic *prof, CourseGenetic *course) {
    if (prof->courses_teaching->size == 0) {
        return True;
    }

    if (prof->courses_teaching->size == 1) {
        return !prof->isDoingWrongSubject &&
               isInList(prof->professor->trained_for, course->subject->name, cmpStr);
    }

    return False;
}

void AssignToProf(ProfessorGenetic *prof, CourseGenetic *course) {
    pushBack(prof->courses_teaching, course);

    if (isInList(prof->professor->trained_for, course->subject->name, cmpStr) == False) {
        prof->isDoingWrongSubject = True;
    }

    course->prof = prof;

    if (prof->isDoingWrongSubject || prof->courses_teaching->size == PROFESSOR_MAX_TRAINED_COURSES_TEACHING) {
        prof->isBusy = True;
    }
}

int errorProf(ProfessorGenetic *prof, char/*Bool*/ print) {
    int run_course_count = 0;
    for (int i = 0; i < prof->courses_teaching->size; ++i) {
        if (willRun(getFromList(prof->courses_teaching, i))) {
            ++run_course_count;
        }
    }

    if (run_course_count == 0) {
        if (print) {
            printf("%s is unassigned.\n", prof->professor->fullname);
        }
        return PROFESSOR_UNASSIGNED;
    }
    if (prof->isDoingWrongSubject == True) {
        if (print) {
            printf("%s is not trained for %s.\n", prof->professor->fullname,
                   ((CourseGenetic *) (getFromList(prof->courses_teaching, 0)))->subject->name);
        }
        return PROFESSOR_WRONG_SUBJECT;
    }
    if (run_course_count == 1) {
        if (print) {
            printf("%s is lacking class.\n", prof->professor->fullname);
        }
        return PROFESSOR_LACKING_CLASS;
    }

    return 0;
}

///TAGenetic///
char /*Bool*/ isAvailableTA(TAGenetic *ta, CourseGenetic *course) {
    return ta->courses_teaching->size < TA_MAX_CLASS && isInList(ta->TA->trained_for, course->subject->name, cmpStr);
}

void AssignToTA(TAGenetic *ta, CourseGenetic *course) {
    pushBack(course->TAs, ta);

    pushBack(ta->courses_teaching, course);
}

int errorTA(TAGenetic *ta, char/*Bool*/ print) {
    int run_course_count = 0;
    for (int i = 0; i < ta->courses_teaching->size; ++i) {
        if (willRun(getFromList(ta->courses_teaching, i))) {
            ++run_course_count;
        }
    }
    if (print && (TA_MAX_CLASS - run_course_count) > 0) {
        printf("%s is lacking %d lab(s).\n", ta->TA->fullname, TA_MAX_CLASS - run_course_count);
    }
    return (TA_MAX_CLASS - run_course_count) * TA_LACKING_CLASS;
}

///CourseGenetic///
int errorCourse(CourseGenetic *course, char/*Bool*/ print) {
    if (willRun(course)) {
        int lacking_students = course->subject->required_by->size - course->subject->allowed_students;
        if (print && lacking_students > 0) {
            for (int i = course->subject->allowed_students; i < course->subject->required_by->size; ++i) {
                Student *student = getFromList(course->subject->required_by, i);
                pushBack(student->lacking_courses, course);

//                printf("%s is lacking %s.\n",
//                       ((Student *) (getFromList(course->subject->required_by, i)))->name,
//                       course->subject->name);
            }
        }
        return (lacking_students > 0) ? lacking_students * STUDENT_LACKING_CLASS : 0;
    } else {
        if (print) {
            printf("%s cannot be run.\n", course->subject->name);
            for (int i = 0; i < course->subject->required_by->size; ++i) {
                Student *student = getFromList(course->subject->required_by, i);
                pushBack(student->lacking_courses, course);
//                printf("%s is lacking %s.\n",
//                       ((Student *) (getFromList(course->subject->required_by, i)))->name,
//                       course->subject->name);
            }
        }
        return course->subject->required_by->size * STUDENT_LACKING_CLASS + COURSE_NOT_RUN;
    }
}

char /*Bool*/ willRun(CourseGenetic *course) {
    return (course->prof != NULL && course->TAs->size == course->subject->required_labs) ?
           True : False;
}

///Individual///
int errorIndividual(Individual *individual, char /*Bool*/ print) {
    int overall_error = 0;
    int subj = 0;
    int prof = 0;
    int ta = 0;
    for (int i = 0; i < subjects->size; ++i) {
        subj += error(&(individual->schedule[i]), print);
    }

    for (int i = 0; i < individual->allprofs->size; ++i) {
        prof += error((ProfessorGenetic *) (getFromList(individual->allprofs, i)), print);
    }

    for (int i = 0; i < individual->allTAs->size; ++i) {
        ta += error((TAGenetic *) (getFromList(individual->allTAs, i)), print);
    }

    if (print) {
        for (int i = 0; i < students->size; ++i) {
            Student *student = getFromList(students, i);
            for (int j = 0; j < student->lacking_courses->size; ++j) {
                CourseGenetic *lacking = getFromList(student->lacking_courses, j);
                printf("%s is lacking %s.\n", student->name, lacking->subject->name);
            }
        }
    }

    overall_error = subj + prof + ta;
    return overall_error;
}

int cmpIndividuals(Individual *i1, Individual *i2) {
    return i1->error - i2->error;
}