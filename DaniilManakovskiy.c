/**
 * TODO: validate all input
 * - All names must contain english letters only
 * - Student's id must contain 5 chars
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

///Utility Constants///
#define True 1
#define False 0


/////////////////////////////////Data structures/////////////////////////////////
typedef struct stringPair {
    char *fullname;
    char *id;
} StringPair;
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

void printStringPair(void *s);

void printTAGenetic(void *taG);

void pushBack(List *list, void *data);

void initList(List *list);

char /*Bool*/ isInList(List *list, void *data, int (*key_cmp)(void *, void *));

void removeFromList(List *list, int index);

int findIndex(List *list, void *key, int (*cmp)(void *, void *));

void removeByKey(List *list, void *key, int (*cmp)(void *, void *));

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
    /* 1: key1 > key2
     * 0: key1 = key2 actually, only this is needed (!!!)
     * -1 key1 < key2
     */
} HashTable;

ull hash(const void *data, size_t size);

void free_hash_entry(HashNode *node);

void init_datalist(HashList *datalist, size_t capacity);

void hash_table_init(HashTable *table, size_t capacity, size_t key_size,
                     size_t value_size, int (*key_cmp)(void *, void *));

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
    List *required_courses;
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
void delStringPair(StringPair *str);

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

void solve();

int parseInput(List *subjects, List *profs, List *TAs, List *students);

int findAllNumbers(char *str, int max_ints, long found_numbers[]);

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
HashTable *subjectByName = NULL;
int subjects_count = 0;
int profs_count = 0;
int tas_count = 0;
int students_count = 0;

/////////////////////////////////Util functions/////////////////////////////////
int isDigit(char c) { return (c >= '0' && c <= '9') ? 1 : 0; }

void freeIfPossible(void *p) {
    if (p != NULL) free(p);
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

int k = 0;

CourseGenetic **shuffle(CourseGenetic *arr, int n) {
    ++k;
    CourseGenetic c = arr[0];
    CourseGenetic **result = (CourseGenetic **) malloc(subjects_count * sizeof(CourseGenetic *));

    for (int k = 0; k < n; ++k) {
        result[k] = &(arr[k]);
    }

    for (int i = n - 1; i > 0; --i) {
        int j = rand() % (i + 1);
        swap(&(result[i]), &(result[j]));
    }

    return result;
}

char checkID(char *id) {
    if (strlen(id) != ID_SIZE)
        return 1;
    for (int i = 0; i < ID_SIZE; ++i) {
        if (!isalnum(id[i]))
            return 1;
    }
    return 0;
}

double randDouble() {
    return (double) rand() / (double) RAND_MAX;
}

void getFullName(char *fullname, char *first_name, char *last_name) {
    strcpy(fullname, first_name);
    strcat(fullname, sep);
    strcat(fullname, last_name);
    fprintf(stderr, "|%s| - %d\n", fullname, strlen(fullname) );
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

            int current_result = -1;
            int steps_without_improvement = 0;

            //solve task for a particular input
            List *subjects = malloc(sizeof(List));
            List *profs = malloc(sizeof(List));
            List *tas = malloc(sizeof(List));
            List *students = malloc(sizeof(List));


            initList(subjects);
            initList(profs);
            initList(tas);
            initList(students);


            subjectByName = (HashTable *) malloc(sizeof(HashTable));
            hash_table_init(subjectByName, 5, MAX_ENTRY_SIZE, sizeof(Subject *), cmpStr);

            if (parseInput(subjects, profs, tas, students) != 0) {
                printf("Invalid input.");
                freeList(subjects, (void (*)(void *)) del_subject);
                freeList(profs, (void (*)(void *)) del_faculty);
                freeList(tas, (void (*)(void *)) del_faculty);
                freeList(students, (void (*)(void *)) del_student);

                free(subjects);
                free(profs);
                free(tas);
                free(students);
                continue;
            }


            printf("\n------Done with input------\n");
            Individual *population = (Individual *) malloc(POPULATION_SIZE * sizeof(Individual));

            // generate random population
            for (int i = 0; i < POPULATION_SIZE; ++i) {

                population[i].allprofs = malloc(sizeof(List));
                initList(population[i].allprofs);

                List *profPool = malloc(sizeof(List));
                initList(profPool);

                List *taPool = malloc(sizeof(List));
                initList(taPool);

                List *allTA = malloc(sizeof(List));
                initList(allTA);


                for (int j = 0; j < profs_count; ++j) {
                    ProfessorGenetic *new = malloc(sizeof(ProfessorGenetic));
                    new_professorGenetic(new, (Faculty *) getFromList(profs, j));

                    pushBack(profPool, new);
                    pushBack(population[i].allprofs, new);
                }


                for (int j = 0; j < tas_count; ++j) {
                    TAGenetic *new = malloc(sizeof(TAGenetic));
                    new_TAGenetic(new, (Faculty *) getFromList(tas, j));
                    pushBack(allTA, new);
                    pushBack(taPool, new);
                }

                CourseGenetic *courses = (CourseGenetic *) malloc(subjects_count * sizeof(CourseGenetic));
                for (int j = 0; j < subjects_count; ++j) {
                    new_CourseGenetic(&(courses[j]), (Subject *) getFromList(subjects, j));
                }

//                at this point all parents have the save order of courses
                CourseGenetic **shuffled_courses = shuffle(courses, subjects_count);
                for (int j = 0; j < subjects_count; ++j) {
                    if (profPool->size == 0) {
                        break;
                    }

                    int randomInd = rand() % profPool->size;

                    ProfessorGenetic *prof = getFromList(profPool, randomInd);

                    if (isAvailable(prof, shuffled_courses[j]) == False) {
                        continue;
                    }

                    AssignTo(prof, shuffled_courses[j]);
//                    shuffled_courses[j]
                    if (prof->isBusy == True) {
                        removeFromList(profPool, randomInd);
                    }
                }

                // All professors assigned


                for (int j = 0; j < subjects_count; ++j) {
                    List *availableTA = malloc(
                            sizeof(List));
                    initList(availableTA);
                    for (int k = 0; k < taPool->size; ++k) {
                        TAGenetic *ta = getFromList(taPool, k);

                        if (isAvailable(ta, shuffled_courses[j]) == True) {
                            pushBack(availableTA, ta);
                        }
                    }


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
                    free(availableTA);
                }

                population[i].schedule = courses;
                population[i].professors = profPool;
                population[i].allTAs = allTA;
                population[i].TAs = taPool;
                population[i].error = error(&(population[i]), False);
                free(shuffled_courses);
            }
            // Random population created

            qsort(population, POPULATION_SIZE, sizeof(Individual), (__compar_fn_t) cmpIndividuals);
//
//            for (int i = 0; i < POPULATION_SIZE; ++i) {
//                printf("#%3d:\n", i);
//                for (int j = 0; j < subjects_count; ++j) {
//                    printf("\t%s - %s, ", population[i].schedule[j].subject->name,
//                           (population[i].schedule[j].prof != NULL)
//                           ? population[i].schedule[j].prof->professor->fullname : "NULL");
//                    printf("%d TAs: ", population[i].schedule->TA_assigned);
//                    for (int l = 0; l < population[i].schedule[j].TA_assigned; ++l) {
//                        printf("%s ", ((TAGenetic *) getFromList(population[i].schedule[j].TAs, l))->TA->fullname);
//                    }
//                    printf("\n");
//                }
//
//                printf("Error: %d", population[i].error);
//                printf("\n\n");
//            }

            for (int step = 0; step < EVOLUTION_STEPS; ++step) {
                Individual *new_population = (Individual *) malloc(POPULATION_SIZE * sizeof(Individual));

                for (int i = 0; i < BEST_COUNT; ++i) {
                    memcpy(&(new_population[i]), &(population[i]), sizeof(Individual));
                }

                //Do reproduction with CROSSING_PARENTS_COUNT

                for (int child_no = 0; child_no < POPULATION_SIZE - BEST_COUNT; ++child_no) {
                    Individual first_parent = population[rand() % CROSSING_PARENTS_COUNT];
                    Individual second_parent = population[rand() % CROSSING_PARENTS_COUNT];

                    HashTable *TAbyName = malloc(sizeof(HashTable));
                    hash_table_init(TAbyName, 5, MAX_ENTRY_SIZE, sizeof(TAGenetic *), cmpStr);

                    HashTable *ProfbyName = malloc(sizeof(HashTable));
                    hash_table_init(ProfbyName, 5, MAX_ENTRY_SIZE, sizeof(ProfessorGenetic *), cmpStr);

                    Individual *child = malloc(sizeof(Individual));

                    child->TAs = malloc(sizeof(List));
                    initList(child->TAs);

                    child->allTAs = malloc(sizeof(List));
                    initList(child->allTAs);

                    child->professors = malloc(sizeof(List));
                    initList(child->professors);

                    child->allprofs = malloc(sizeof(List));
                    initList(child->allprofs);

                    CourseGenetic *newGenome = (CourseGenetic *) malloc(subjects_count * sizeof(CourseGenetic));
                    for (int j = 0; j < subjects_count; ++j) {
                        new_CourseGenetic(&(newGenome[j]), (Subject *) getFromList(subjects, j));
                    }

                    for (int j = 0; j < tas_count; ++j) {
                        TAGenetic *new = malloc(sizeof(TAGenetic));
                        new_TAGenetic(new, (Faculty *) getFromList(tas, j));
                        pushBack(child->allTAs, new);
                        pushBack(child->TAs, new);
                        insert(TAbyName, new->TA->fullname, new);
                    }

                    for (int j = 0; j < profs_count; ++j) {
                        ProfessorGenetic *new = malloc(sizeof(ProfessorGenetic));

                        new_professorGenetic(new, (Faculty *) getFromList(profs, j));

                        pushBack(child->professors, new);
                        pushBack(child->allprofs, new);
                        insert(ProfbyName, new->professor->fullname, new);
                    }


                    child->schedule = newGenome;

                    for (int i = 0; i < subjects_count; ++i) {
                        CourseGenetic *course = &(newGenome[i]);

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
                            //find try to find an available ta from the first parent


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
                        free(availableAssistants);

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

                                if (childProf->isBusy == True) {
                                    removeFromList(child->professors, randomInd);
                                    remove_el(ProfbyName, childProf->professor->fullname);
                                }

                            } else {
                                AssignTo(childProf, course);
                                if (childProf->isBusy == True) {
                                    removeByKey(child->professors, childProf, (int (*)(void *, void *)) cmpProf);
                                    remove_el(ProfbyName, childProf->professor->fullname);
                                }
                            }
                        }


                    }

                    //TODO: fix a big memory leak
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
                    free(population[l].TAs);

                    freeListSaveData(population[l].professors);
                    free(population[l].professors);

                    freeList(population[l].allprofs, (void (*)(void *)) del_Professor_genetic);
                    freeList(population[l].allTAs, (void (*)(void *)) del_TA_genetic);

                    free(population[l].allTAs);
                    free(population[l].allprofs);

                    for (int i = 0; i < subjects_count; ++i) {
                        freeListSaveData(population[l].schedule[i].TAs);
                        free(population[l].schedule[i].TAs);
                    }
                    free(population[l].schedule);
                }
                free(population);


                population = new_population;
                qsort(population, POPULATION_SIZE, sizeof(Individual), (__compar_fn_t) cmpIndividuals);

                printf("%d) best - %d\n", step, population[0].error);
//                printf("%d) %d %d %d %d %d\n", step, population[0].error, population[249].error, population[499].error,
//                       population[749].error, population[999].error);

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
            for (int subj = 0; subj < subjects_count; ++subj) {
                if (willRun(&(population[0].schedule[subj]))) {
                    printf("%s\n%s\n",
                           population[0].schedule[subj].subject->name,
                           population[0].schedule[subj].prof->professor->fullname
                    );
                    printList(population[0].schedule[subj].TAs, -1, printTAGenetic);
                    printList(population[0].schedule[subj].subject->required_by,
                              population[0].schedule[subj].subject->allowed_students,
                              printStringPair);
                    printf("\n");
                }
            }
            error(&(population[0]), True);


//Population free start
            for (int l = 0; l < POPULATION_SIZE; ++l) {
                freeListSaveData(population[l].TAs);
                free(population[l].TAs);

                freeListSaveData(population[l].professors);
                free(population[l].professors);

                freeList(population[l].allprofs, (void (*)(void *)) del_Professor_genetic);
                freeList(population[l].allTAs, (void (*)(void *)) del_TA_genetic);

                free(population[l].allTAs);
                free(population[l].allprofs);

                for (int i = 0; i < subjects_count; ++i) {
                    freeListSaveData(population[l].schedule[i].TAs);
                    free(population[l].schedule[i].TAs);
                }
                free(population[l].schedule);
            }
            free(population);
//population free end



            freeList(subjects, (void (*)(void *)) del_subject);
            freeList(profs, (void (*)(void *)) del_faculty);
            freeList(tas, (void (*)(void *)) del_faculty);
            freeList(students, (void (*)(void *)) del_student);

            free(subjects);
            free(profs);
            free(tas);
            free(students);
        } else {
            break;
        }
    }

}

int parseInput(List *subjects, List *profs, List *TAs, List *students) {
    //reading subjects, until "P" is met
    while (getline(&global_buffer, &MAX_BUFFER_SIZE, stdin) != -1 && strcmp(global_buffer, "P\n") != 0) {

//        trim();

        char *course_name = (char *) calloc(MAX_ENTRY_SIZE, sizeof(char));
        int labs_required, students_allowed, course_name_end_pos = 0;
        char *cursor = global_buffer;
//        sscanf(global_buffer, "%s %d %d", course_name, &labs_required, &students_allowed);
        //read letters as much as possible
        while (isalpha(global_buffer[course_name_end_pos])) {
            ++course_name_end_pos;
            ++cursor;
        }
        // if nonletter char met, it must be a space followed by a digit
        if (!(*cursor == ' ' && isdigit(*(cursor + 1)))) {
            return 1;
        }
        ++cursor;
        long nums[2];
        if (findAllNumbers(cursor, 2, nums) != 0) {
            return 2;
        }
        strncpy(course_name, global_buffer, course_name_end_pos);
        labs_required = nums[0];
        students_allowed = nums[1];
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
        return 3;

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
//        char *name = malloc(strlen(token) + 1);
//        strcpy(name, token);
        char *name = strdup(token);
        token = strtok(NULL, " ");
//        char *surname = malloc(strlen(token) + 1);
//        strcpy(surname, token);
        char* surname = strdup(token);
        token = strtok(NULL, " ");

        char *fullname = (char *) malloc((2 + strlen(name) + strlen(surname)) * sizeof(char));
        getFullName(fullname, name, surname);

        List *head = malloc(sizeof(List));
        initList(head);
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
                    return 4; // student requires the course that doesn't exists
                }
                StringPair *stringPair = malloc(sizeof(StringPair));
                stringPair->fullname = strdup(fullname);
                stringPair->id = strdup(student_id);
                pushBack(subj->required_by, stringPair);
                subj->selectedCount++;
//                free(subj);
            }
            token = strtok(NULL, " ");
        }


        void *n;
        if (status == STUDENT) {
            n = (Student *) malloc(sizeof(Student));
            new_student(n, fullname, student_id);
            ((Student *) n)->required_courses = head;


        } else {
            n = (Faculty *) malloc(sizeof(Faculty));
            new_faculty(n, fullname);
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
        free(token);
//        if (status == STUDENT ){
//            if (!checkID(student_id)){
//                free(student_id);
//                return 5;
//            }
//        }
        if (status == STUDENT) {
            free(student_id);
        }

    }

    //
    if (status != STUDENT || students_count == 0)
        return 6;

    return 0;
}

int findAllNumbers(char *str, int max_ints, long found_numbers[]) {
    if (!isdigit(str[0])) {
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
            freeIfPossible(cleaner);
//            freeIfPossible(p);
            return 1;
        }
        if (i > INT_MAX) {
            freeIfPossible(cleaner);
            return 1;
        }
        if (found_number_count > max_ints) {
            // if at some moment it findes more numbers, than
            // expected, terminates
            freeIfPossible(cleaner);
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
                freeIfPossible(cleaner);
                return 1;
            } else {
                // there is a leading zero
                if (p[1] == '0' && isdigit(p[2])) {
                    freeIfPossible(cleaner);
                    return 1;
                }
            }
            // The delimiter must be a space
            if (*p != ' ') {
                freeIfPossible(cleaner);
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
        freeIfPossible(cleaner);
        return 1;
    }
    freeIfPossible(cleaner);
    return (found_number_count == max_ints) ? 0 : 1;
}

/////////////////////////////////Data structures/////////////////////////////////
///Linked List///

void copyList(List *dest, List *src) {
    for (int i = 0; i < src->size; ++i) {
        pushBack(dest, getFromList(src, i));
    }
}

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
//    int start_position = list->cursor_position;
    int diff = ind - list->cursor_position; // if positive, go right, else - left
//    int start_diff = diff;

    if (ind < abs(diff)) {
        diff = ind;
        list->cursor = list->head;
//        start_position = 0;
//        start_diff = ind;
    } else if (list->size - ind - 1 < abs(diff)) {
        diff = ind - (list->size - 1);
        list->cursor = list->tail;
//        start_position = list->size - 1;
//        start_diff = diff;
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

void printStringPair(void *s) {
    StringPair *str = (StringPair *) (s);
    printf("%s %s\n", str->fullname, str->id);
}

void printTAGenetic(void *taG) {
    TAGenetic *ta = (TAGenetic *) taG;
//    printf("TAGenetic: %s\n%d) ", ta->TA->fullname, ta->courses_teaching_count);
//    printList(ta->courses_teaching, printCourseGenetic);
//    printf("\n\n");
    printf("%s\n", ta->TA->fullname);

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
//        if (node->value != NULL) free(node->value);
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

    item->next = NULL;
    item->prev = NULL;
    item->key = malloc(table->key_size);
//    item->value = malloc(table->value_size);
    memcpy(item->key,
           key,
           table->key_size);
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
        free(table->datalist);
        free(table);
    }
}

void rehash(HashTable *table) {
    fprintf(stderr, "rehash\n");
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
//    printf("New size: %zu\n", table->capacity);
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
//        free(tmp->value);
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
    new->ID = malloc(ID_SIZE + 1); // 5 chars + \0
    strcpy(new->ID, ID);

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

void delStringPair(StringPair *str) {
    free(str->id);
    free(str->fullname);
}

void del_subject(Subject *subj) {
    free(subj->name);
    freeList(subj->required_by, (void (*)(void *)) delStringPair);
    free(subj->required_by);
}

void del_faculty(Faculty *f) {
    free(f->fullname);
    freeList(f->trained_for, NULL);
    free(f->trained_for);
}

void del_student(Student *s) {
    free(s->name);
    free(s->ID);
    freeList(s->required_courses, NULL);
    free(s->required_courses);
}

void del_Professor_genetic(ProfessorGenetic *prof) {
    freeListSaveData(prof->courses_teaching);
    free(prof->courses_teaching);
}

void del_TA_genetic(TAGenetic *ta) {
    freeListSaveData(ta->courses_teaching);
    free(ta->courses_teaching);
}

///Functions on DT///
///ProfessorGenetic///
char /*Bool*/ isAvailableProf(ProfessorGenetic *prof, CourseGenetic *course) {
    if (prof->courses_teaching->size == 0) {
        return True;
    }

    if (prof->courses_teaching->size == 1) {
        return !prof->isDoingWrongSubject &&
               isInList(prof->professor->trained_for, course->subject->name, cmpStr) == True;
    }

    return False;
}

void AssignToProf(ProfessorGenetic *prof, CourseGenetic *course) {
    pushBack(prof->courses_teaching, course);

    if (isInList(prof->professor->trained_for, course->subject->name, cmpStr) == False) {
        prof->isDoingWrongSubject = True;
    }

    course->prof = prof;

    if (prof->isDoingWrongSubject == True || prof->courses_teaching->size == 2) {
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
    return (ta->courses_teaching->size < TA_MAX_CLASS && isInList(ta->TA->trained_for, course->subject->name, cmpStr)) ?
           True : False;
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
                printf("%s is lacking %s.\n",
                       ((StringPair *) (getFromList(course->subject->required_by, i)))->fullname,
                       course->subject->name);
            }
        }
        return (lacking_students > 0) ? lacking_students * STUDENT_LACKING_CLASS : 0;
    } else {
        if (print) {
            for (int i = 0; i < course->subject->required_by->size; ++i) {
                printf("%s is lacking %s.\n",
                       ((StringPair *) (getFromList(course->subject->required_by, i)))->fullname,
                       course->subject->name);
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

    if (print) {
        for (int i = 0; i < subjects_count; ++i) {
            if (!willRun(&(individual->schedule[i]))) {
                printf("%s cannot be run.\n", individual->schedule[i].subject->name);
            }
        }
    }

    for (int i = 0; i < individual->allprofs->size; ++i) {
        prof += error((ProfessorGenetic *) (getFromList(individual->allprofs, i)), print);
    }

    for (int i = 0; i < individual->allTAs->size; ++i) {
        ta += error((TAGenetic *) (getFromList(individual->allTAs, i)), print);
    }

    for (int i = 0; i < subjects_count; ++i) {
        subj += error(&(individual->schedule[i]), print);
    }
    overall_error = subj + prof + ta;
    return overall_error;
}

int cmpIndividuals(Individual *i1, Individual *i2) {
    return i1->error - i2->error;
}