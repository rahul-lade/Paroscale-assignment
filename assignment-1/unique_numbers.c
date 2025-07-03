#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define MAX_NUMBERS 1000000
#define THREAD_COUNT 4
#define MAX_LINE_LENGTH 100

int *file_numbers = NULL;
int total_numbers = 0;

int global_unique[MAX_NUMBERS];
int global_count = 0;

pthread_mutex_t mutex;

typedef struct {
    int start;
    int end;
} ThreadArgs;

int is_in_global(int num) {
    for (int i = 0; i < global_count; i++) {
        if (global_unique[i] == num) return 1;
    }
    return 0;
}

void add_to_global(int num) {
    pthread_mutex_lock(&mutex);
    if (!is_in_global(num)) {
        global_unique[global_count++] = num;
    }
    pthread_mutex_unlock(&mutex);
}

void* thread_work(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;
    for (int i = args->start; i < args->end; i++) {
        add_to_global(file_numbers[i]);
    }
    return NULL;
}

void load_file(const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        perror("File open failed");
        exit(1);
    }

    file_numbers = malloc(sizeof(int) * MAX_NUMBERS);
    char line[MAX_LINE_LENGTH];

    while (fgets(line, sizeof(line), fp)) {
        int value = atoi(line);
        file_numbers[total_numbers++] = value;
    }

    fclose(fp);
}

int main() {
    pthread_t threads[THREAD_COUNT];
    ThreadArgs args[THREAD_COUNT];

    pthread_mutex_init(&mutex, NULL);
    load_file("input.txt");

    int chunk = total_numbers / THREAD_COUNT;
    for (int i = 0; i < THREAD_COUNT; i++) {
        args[i].start = i * chunk;
        args[i].end = (i == THREAD_COUNT - 1) ? total_numbers : (i + 1) * chunk;
        pthread_create(&threads[i], NULL, thread_work, &args[i]);
    }

    for (int i = 0; i < THREAD_COUNT; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Unique Numbers:\n");
    for (int i = 0; i < global_count; i++) {
        printf("%d\n", global_unique[i]);
    }

    pthread_mutex_destroy(&mutex);
    free(file_numbers);
    return 0;
}
