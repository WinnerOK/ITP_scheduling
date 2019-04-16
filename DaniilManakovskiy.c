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

#define DEBUG 1

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
#define BEST_COUNT (int)(POPULATION_SIZE*BEST_FIT_PERCENTAGE)
#define CROSSING_PARENTS_COUNT (int)(POPULATION_SIZE * GOOD_FIT_PERCENTAGE)
#define MAX_STEPS_WITHOUT_IMPROVEMENT 100

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


int flag = 0;

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

void copyList(List *dest, List *src);

Node *getNodeFromList(List *list, int ind);

void *getFromList(List *list, int ind);

void printList(List *list, void (*print_function)(void *));

void freeList(List *list, void (*destructor)(void *));

void freeListSaveData(List *list);

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

void new_faculty(Faculty *new, char *first_name, char *last_name);

void new_student(Student *new, char *first_name, char *last_name, char *ID);

void new_professorGenetic(ProfessorGenetic* new, Faculty* prof);

void new_TAGenetic(TAGenetic *new, Faculty* TA);

void new_CourseGenetic(CourseGenetic* new, Subject* subject);

///Copy methods//
void copyProfessorGenetic(ProfessorGenetic* dest, ProfessorGenetic* src);


///Destructors//
void del_subject(Subject *subj);

void del_faculty(Faculty *f);

void del_student(Student *s);

void del_Professor_genetic(ProfessorGenetic *prof);

void del_TA_genetic(TAGenetic* ta);

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

int cmpIndividuals(Individual *i1, Individual *i2);

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
#ifdef DEBUG
    if (strcmp(c.subject->name, arr[0].subject->name) != 0) {
        fprintf(stderr, "%d) 0st elements are different\n", k);
    }

#endif

    return result;
}

double randDouble() {
    return (double) rand() / (double) RAND_MAX;
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
            Individual *population = (Individual *) malloc(POPULATION_SIZE * sizeof(Individual));

            // generate random population
            for (int i = 0; i < POPULATION_SIZE; ++i) {

                population[i].allprofs = malloc(sizeof(List));
                initList(population[i].allprofs, sizeof(ProfessorGenetic *));

                List *profPool = malloc(sizeof(List));
                initList(profPool, sizeof(ProfessorGenetic *));

                List *taPool = malloc(sizeof(List));
                initList(taPool, sizeof(TAGenetic *));

                List *allTA = malloc(sizeof(List));
                initList(allTA, sizeof(TAGenetic *));


                for (int j = 0; j < profs_count; ++j) {
                    ProfessorGenetic *new = malloc(sizeof(ProfessorGenetic));
                    new_professorGenetic(new, (Faculty*)getFromList(profs, j));

                    pushBack(profPool, new);
                    pushBack(population[i].allprofs, new);
                }

//            printList(standardProfessorPool, printProfessorGenetic);


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
//                printf("#%3d:\n", i);
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
                    initList(availableTA, sizeof(TAGenetic *));
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

                        if (ta->courses_teaching_count == TA_MAX_CLASS) {
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
                population[i].error = error(&(population[i]));
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
            flag = 2;

            for (int step = 0; step < EVOLUTION_STEPS; ++step) {
                Individual *new_population = (Individual *) malloc(POPULATION_SIZE * sizeof(Individual));

                for (int i = 0; i < BEST_COUNT; ++i) {// TODO make full copy of an individual
                    memcpy(&(new_population[i]), &(population[i]), sizeof(Individual));
                }
                // TODO: don't free [0; BEST_COUNT) individuals

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
                    initList(child->TAs, sizeof(TAGenetic *));

                    child->allTAs = malloc(sizeof(List));
                    initList(child->allTAs, sizeof(TAGenetic *));

                    child->professors = malloc(sizeof(List));
                    initList(child->professors, sizeof(ProfessorGenetic *));

                    child->allprofs = malloc(sizeof(List));
                    initList(child->allprofs, sizeof(ProfessorGenetic *));

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
                        initList(availableAssistants, sizeof(TAGenetic *));

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
                                if (ta->courses_teaching_count == TA_MAX_CLASS) {
                                    removeByKey(child->TAs, ta, (int (*)(void *, void *)) cmpTA);
                                    remove_el(TAbyName,
                                              ta->TA->fullname); // раньше удаляло целиком объект с ключом - ТА
                                }
                                continue;
                            }

                            //try to find an available TA from the second parent
                            freeListSaveData(availableAssistants);
                            initList(availableAssistants, sizeof(TAGenetic *));
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
                                if (ta->courses_teaching_count == TA_MAX_CLASS) {
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

                                if (ta->courses_teaching_count == TA_MAX_CLASS) {
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

                    free_hash_table(ProfbyName);
                    free_hash_table(TAbyName);

//                    free(ProfbyName);
//                    free(TAbyName);

                    child->error = error(child);
                    new_population[BEST_COUNT + child_no] = *child;
                    free(child); // этот объект и объект выше - разные, но внутри ссылки одинаковые.
                }
                //TODO: fix a lot of memory leaks

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
//                printf("%d) best - %d\n", step, population[0].error);
//                printf("%d) %d %d %d %d %d\n", step, population[0].error, population[249].error, population[499].error,
//                       population[749].error, population[999].error);
            }


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
//TODO: поставь это на нужное место


            freeList(subjects, (void (*)(void *)) del_subject);
            freeList(profs, (void (*)(void *)) del_faculty);
            freeList(tas, (void (*)(void *)) del_faculty);
            freeList(students, (void (*)(void *)) del_student);

            free(subjects);
            free(profs);
            free(tas);
            free(students);
            free(standardProfessorPool);
            free(standardTAPool);
        }
    }

}

int parseInput(List *subjects, List *profs, List *TAs, List *students) {
    //reading subjects, until "P" is met
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
                free(node); // todo: do you need it?
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

void new_professorGenetic(ProfessorGenetic* new, Faculty* prof){
    new->professor = prof;
    new->courses_teaching_count = 0;
    new->isDoingWrongSubject = False;
    new->isBusy = False;
    new->courses_teaching = malloc(sizeof(List));
    initList(new->courses_teaching, sizeof(CourseGenetic *)); //TODO really this datatype?

}

void new_TAGenetic(TAGenetic *new, Faculty* TA){
    new->TA = TA;
    new->courses_teaching_count = 0;
    new->courses_teaching = malloc(sizeof(List));
    initList(new->courses_teaching, sizeof(CourseGenetic *)); //TODO really this datatype?
}

void new_CourseGenetic(CourseGenetic* new, Subject* subject){
    new->prof = NULL;
    new->subject = subject;
    new->TA_assigned = 0;
    new->TAs = malloc(sizeof(List));
    initList(new->TAs, sizeof(TAGenetic*)); //TODO really this datatype?
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

void del_Professor_genetic(ProfessorGenetic *prof) {
    freeListSaveData(prof->courses_teaching);
    free(prof->courses_teaching);
}

void del_TA_genetic(TAGenetic* ta){
    freeListSaveData(ta->courses_teaching);
    free(ta->courses_teaching);
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

int errorProf(ProfessorGenetic *prof) {
    int run_course_count = 0;
    for (int i = 0; i < prof->courses_teaching->size; ++i) {
        if (willRun(getFromList(prof->courses_teaching, i))) {
            ++run_course_count;
        }
    }

    if (run_course_count == 0)
        return PROFESSOR_UNASSIGNED;
    if (prof->isDoingWrongSubject == True)
        return PROFESSOR_WRONG_SUBJECT;
    if (run_course_count == 1)
        return PROFESSOR_LACKING_CLASS;

    return 0;
}

///TAGenetic///
char /*Bool*/ isAvailableTA(TAGenetic *ta, CourseGenetic *course) {
    return (ta->courses_teaching_count < TA_MAX_CLASS && isInList(ta->TA->trained_for, course->subject->name, cmpStr)) ?
           True : False;
}

void AssignToTA(TAGenetic *ta, CourseGenetic *course) {
    pushBack(course->TAs, ta);
    ++course->TA_assigned;

    pushBack(ta->courses_teaching, course);
    ++ta->courses_teaching_count;
}

int errorTA(TAGenetic *ta) {
    int run_course_count = 0;
    for (int i = 0; i < ta->courses_teaching->size; ++i) {
        if (willRun(getFromList(ta->courses_teaching, i))) { //TODO do TA really have one course in list multiple times?
            ++run_course_count;
        }
    }

    return (TA_MAX_CLASS - run_course_count) * TA_LACKING_CLASS;
}

///CourseGenetic///
int errorCourse(CourseGenetic *course) {
    if (willRun(course)) {
        int lacking_students = course->subject->selectedCount - course->subject->allowed_students;
        return (lacking_students > 0) ? lacking_students * STUDENT_LACKING_CLASS : 0;
    } else {
        return course->subject->selectedCount * STUDENT_LACKING_CLASS + COURSE_NOT_RUN;
    }
}

char /*Bool*/ willRun(CourseGenetic *course) {
    return (course->prof != NULL && course->TAs->size == course->subject->required_labs) ?
           True : False;
}

///Individual///
int errorIndividual(Individual *individual) {
    int overall_error = 0;
    int subj = 0;
    int prof = 0;
    int ta = 0;
    for (int i = 0; i < subjects_count; ++i) {
        subj += error(&(individual->schedule[i]));
//        overall_error += error(&(individual->schedule[i]));
    }

    for (int i = 0; i < individual->allprofs->size; ++i) {
        prof += error((ProfessorGenetic *) (getFromList(individual->allprofs, i)));
//        overall_error += error((ProfessorGenetic *) (getFromList(individual->allprofs, i)));
    }

    for (int i = 0; i < individual->allTAs->size; ++i) {
        ta += error((TAGenetic *) (getFromList(individual->allTAs, i)));
//        overall_error += error((TAGenetic *) (getFromList(individual->allTAs, i)));
    }

    overall_error = subj + prof + ta;
    return overall_error;
}

int cmpIndividuals(Individual *i1, Individual *i2) {
    return i1->error - i2->error;
}