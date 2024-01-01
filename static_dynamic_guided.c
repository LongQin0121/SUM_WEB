#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <omp.h>

#define NUM_FISH 30000000    // Number of fish
#define WEIGHT 5.0           // Initial weight of fish
#define W_MAX 10.0      // Maximum weight of fish
#define POND_SIZE 200   // Pond size
#define ROUND 10       // Number of rounds
#define TIMES 5         // number of experiments

typedef struct {
    double x;
    double y;
    double weight;
    double prevObjective;
    double currentObjective;
} Fish;

typedef struct {
    double time_start;
    double time_end;
    double time_duration;
} Timer;

Fish *school; //pointer to a dynamic array to store the fishes named school
Timer *performanceTimer;
double time_reduction = 0.0;

double *barycentre;
int currentRound;
unsigned int *rand_state;  // Thread local state for random number generation
//#pragma omp threadprivate(rand_state)

int rand_threadsafe() {
    int tid = omp_get_thread_num();
    rand_state[tid] = (rand_state[tid] * 11035u + 10086u) % ((unsigned)RAND_MAX + 1);
    return (rand_state[tid] % ((unsigned)RAND_MAX + 1));
}

// initialize the whole system
void systemInitialize(){

    srand(time(NULL));
    int num_threads = omp_get_max_threads(); // Get the number of threads
    rand_state = (unsigned int *)malloc(sizeof(unsigned int) * num_threads);

    if (!rand_state) {
        printf("Memory allocation for rand_state failed!\n");
        exit(1);
    }

    // Seed the random state for each thread
    #pragma omp parallel
    {
        int tid = omp_get_thread_num();
        rand_state[tid] = rand();
    }

    school = (Fish *)malloc(sizeof(Fish) * NUM_FISH);  //   10000num  of  fish  512 f   mish  atch    size  cache  60      static(4)
    performanceTimer = (Timer *)malloc(sizeof(Timer) * TIMES);
    barycentre = (double *)malloc(sizeof(double) * ROUND);
    currentRound = 0;

    if(!school || !performanceTimer || !barycentre) {
        printf("Memory allocation failed!\n");
        exit(1);
    }
}

// initialize the fish school in the pool
void initializeFish() {
    #pragma omp parallel for
    for(int i = 0; i < NUM_FISH; i++) {
        int tid = omp_get_thread_num();
        school[i].x = ((double)rand_threadsafe() / RAND_MAX - 0.5) * POND_SIZE;
        school[i].y = ((double)rand_threadsafe() / RAND_MAX - 0.5) * POND_SIZE;
        school[i].weight = WEIGHT + ((double)rand_threadsafe() / RAND_MAX - 0.5) * 0.1;
        school[i].currentObjective = sqrt(school[i].x * school[i].x + school[i].y * school[i].y);
    }
}

// move the fishes is each round: generate 2 random little numbers and add them in coordinates with both x and y of each fish and calculate the objective function currrently in this round.
void move() {
    #pragma omp parallel for
    for(int i = 0; i < NUM_FISH; i++) {
        int tid = omp_get_thread_num();
        school[i].x += (double)rand_threadsafe() / RAND_MAX * 0.2 - 0.1;
        school[i].y += (double)rand_threadsafe() / RAND_MAX * 0.2 - 0.1;
        school[i].prevObjective = school[i].currentObjective;
        school[i].currentObjective = sqrt(school[i].x * school[i].x + school[i].y * school[i].y);
    }
}

// each fish in fish school eat the food: calculate the current weight for each fish in fish school in this round.
void eat() {

    double startTime = omp_get_wtime();
    double maxDiff = -INFINITY;
    #pragma omp parallel for reduction(max: maxDiff)
    for(int i = 0; i < NUM_FISH; i++) {
        double diff = school[i].prevObjective - school[i].currentObjective;
        if(diff > maxDiff) {
            maxDiff = diff;
        }
    }
    double endTime = omp_get_wtime();
    time_reduction = endTime - startTime;

    #pragma omp parallel for
    for(int i = 0; i < NUM_FISH; i++) {
        double diff = school[i].prevObjective - school[i].currentObjective;
        school[i].weight += diff / maxDiff;
        if(school[i].weight > W_MAX) {
            school[i].weight = W_MAX;
        }
    }
}

// orient each fish to idea direction based on collective experience - barycentre: calculate the barycentre for each round for all fishes in the fish school
void collectiveExperience() {
    double barycentreNumerator = 0.0;
    double barycentreDenominator = 0.0;
    #pragma omp parallel for reduction(+:barycentreNumerator, barycentreDenominator)
    for(int i = 0; i < NUM_FISH; i++) {
        barycentreNumerator += school[i].currentObjective * school[i].weight;
        barycentreDenominator += school[i].currentObjective;
    }

    if (barycentreDenominator == 0.0) {
        printf("Calculation Error\n");
        exit(1);
    }

    barycentre[currentRound] = barycentreNumerator / barycentreDenominator;
}

// this is the function for optimizing the FSB: operating and start all system.
void optimization() {
    performanceTimer[0].time_start = omp_get_wtime();
    for(int i = 0; i < ROUND; i++) {
        move();
        eat();
        collectiveExperience();
        currentRound++;
    }
     performanceTimer[0].time_end = omp_get_wtime();
     performanceTimer[0].time_duration = performanceTimer[0].time_end - performanceTimer[0].time_start;
    printf("%.1lf\n",  performanceTimer[0].time_duration);
   // return  performanceTimer[0].time_duration;
}

// Free the dynamically allocated memory before exiting.
void freeAll(){
    free(school);
    free(performanceTimer);
    free(barycentre);
}

int main(int argc, char* argv[]) {
    //For different  combinations of threads and chunk size by static and dynamic
    int num_threads_array[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,18,20,24,30,36,48,64,72,96,128,200,256};
    int num_threads_length = sizeof(num_threads_array) / sizeof(num_threads_array[0]);

    int chunk_size_static_array[] = {1, 2, 4, 8, 16,32,64,128,256,512,1024,2048};
    int chunk_size_static_length = sizeof(chunk_size_static_array) / sizeof(chunk_size_static_array[0]);

    int chunk_size_dynamic_array[] = {1, 2, 4, 8, 16,32,64,128,256,512,1024,2048,4096,8192,10000,15000,20000};
    int chunk_size_dynamic_length= sizeof(chunk_size_dynamic_array) / sizeof( chunk_size_dynamic_array[0]);

    int chunk_size_guided_array[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048};
    int chunk_size_guided_length = sizeof(chunk_size_guided_array) / sizeof(chunk_size_guided_array[0]);

   // double total_Time[num_threads_length][chunk_size_length]; // A 2D array to store experiment times
     printf("Num_Threads\tChunk\tTotal_Time\n");
    for (int i = 0; i < num_threads_length; i++) {
        int num_threads = num_threads_array[i];
        omp_set_num_threads(num_threads);

        //for static experiments
        for (int j = 0; j < chunk_size_static_length; j++) {
            int chunk_size_static = chunk_size_static_array[j];

            // Set the OMP_SCHEDULE environment variable for static scheduling
            char omp_schedule_static_env[64];
            sprintf(omp_schedule_static_env, "OMP_SCHEDULE=static,%d", chunk_size_static);
            putenv(omp_schedule_static_env); // Use putenv instead of setenv

            printf("%d\t%d\t", num_threads, chunk_size_static);

            systemInitialize();
            initializeFish();
            optimization(); 
            free(school);
            free(performanceTimer);
            free(barycentre);
        }

        //for dynamic experiments
        for (int j = 0; j < chunk_size_dynamic_length; j++) {
            int chunk_size_dynamic = chunk_size_dynamic_array[j];

            // Set the OMP_SCHEDULE environment variable for dynamic scheduling
            char omp_schedule_dynamic_env[64];
            sprintf(omp_schedule_dynamic_env, "OMP_SCHEDULE=dynamic,%d", chunk_size_dynamic);
            putenv(omp_schedule_dynamic_env); // Use putenv instead of setenv

            printf("%d\t%d\t", num_threads, chunk_size_dynamic);

            systemInitialize();
            initializeFish();
            optimization(); 
            free(school);
            free(performanceTimer);
            free(barycentre);
        }

         //for guided experiments
        for (int j = 0; j < chunk_size_guided_length; j++) {
            int chunk_size_guided = chunk_size_guided_array[j];

            // Set the OMP_SCHEDULE environment variable for static scheduling
            char omp_schedule_guided_env[64];
            sprintf(omp_schedule_guided_env, "OMP_SCHEDULE=guided,%d", chunk_size_guided);
            putenv(omp_schedule_guided_env); // Use putenv instead of setenv

            printf("%d\t%d\t", num_threads, chunk_size_guided);

            systemInitialize();
            initializeFish();
            optimization(); 
            free(school);
            free(performanceTimer);
            free(barycentre);
        }
    }

    freeAll();
    return 0;
}
