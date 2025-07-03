#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

#define MAX_CACHE_SIZE 5

typedef struct Node {
    char filepath[256];
    time_t opened_time;
    off_t size;
    ino_t inode;
    struct Node* prev;
    struct Node* next;
} Node;

Node* head = NULL;
Node* tail = NULL;
int current_size = 0;

Node* find_node(const char* path) {
    Node* temp = head;
    while (temp) {
        if (strcmp(temp->filepath, path) == 0)
            return temp;
        temp = temp->next;
    }
    return NULL;
}

void move_to_front(Node* node) {
    if (node == head) return;

    if (node->prev) node->prev->next = node->next;
    if (node->next) node->next->prev = node->prev;

    if (node == tail) tail = node->prev;

    node->prev = NULL;
    node->next = head;
    if (head) head->prev = node;
    head = node;
}

void remove_last() {
    if (!tail) return;

    Node* temp = tail;
    if (tail->prev) tail->prev->next = NULL;
    tail = tail->prev;
    if (!tail) head = NULL;

    free(temp);
    current_size--;
}

void add_entry(const char* path) {
    Node* existing = find_node(path);
    if (existing) {
        existing->opened_time = time(NULL);
        move_to_front(existing);
        return;
    }

    struct stat st;
    if (stat(path, &st) != 0) {
        perror("Stat failed");
        return;
    }

    Node* new_node = malloc(sizeof(Node));
    strcpy(new_node->filepath, path);
    new_node->opened_time = time(NULL);
    new_node->size = st.st_size;
    new_node->inode = st.st_ino;
    new_node->prev = NULL;
    new_node->next = head;

    if (head) head->prev = new_node;
    head = new_node;
    if (!tail) tail = new_node;

    current_size++;
    if (current_size > MAX_CACHE_SIZE) {
        remove_last();
    }
}

void print_cache() {
    printf("Current LRU Cache:\n");
    Node* temp = head;
    while (temp) {
        printf("Path: %s | Size: %ld | Inode: %ld | Opened: %s",
            temp->filepath, temp->size, temp->inode, ctime(&temp->opened_time));
        temp = temp->next;
    }
}

int main() {
    add_entry("file1.txt");
    add_entry("file2.txt");
    add_entry("file3.txt");
    add_entry("file4.txt");
    add_entry("file5.txt");
    add_entry("file1.txt"); // access again to update order
    add_entry("file6.txt"); // triggers LRU eviction

    print_cache();
    return 0;
}
