#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct dynamic_string_array {
	char** list;
	size_t size;
	size_t r_size;
};


void array_add(struct dynamic_string_array* array, char* value) {
	while (array->size >= array->r_size) {
		array->r_size *= 2;
		array->list = realloc(array->list, sizeof(char*) * array->r_size);
	} 
	array->size ++;
	array->list[array->size - 1] = malloc(strlen(value) + 1);
	strcpy(array->list[array->size - 1], value);
}

int len(struct dynamic_string_array array) {
	return array.size;
}

void pop(struct dynamic_string_array* array) {
	free(array->list[array->size - 1]);
	array->size --;
}

void printl(struct dynamic_string_array* array) {
	if (array == NULL || array->size == 0) {
        printf("[]\n");
        return;
    }

	printf("['%s'", array->list[0]);
	for (int i = 1; i < array->size - 1; i ++) {
		printf(", \n'%s'", array->list[i]);
	}
	printf("]\n");
}

char* get(struct dynamic_string_array array, int i) {
	return array.list[i];
}