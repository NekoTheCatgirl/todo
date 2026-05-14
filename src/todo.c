#include <stdlib.h>
#include <string.h>
#include <todo.h>

#define INITIAL_CAPACITY 4

struct [[maybe_unused]] TodoEntry {
	const char *name;
	const char *description;
	bool completed;
};

struct [[maybe_unused]] TodoList {
	todo_entry_t **entries;
	size_t capacity;
	size_t size;
};

todo_list_t* todo_list_ctor(void) {
	todo_list_t *list = malloc(sizeof(todo_list_t));
	list->entries = malloc(INITIAL_CAPACITY * sizeof(todo_entry_t *));
	list->capacity = INITIAL_CAPACITY;
	list->size = 0;
	return list;
}

size_t todo_list_size(const todo_list_t* list) {
	return list->size;
}

todo_entry_t* todo_list_index(const todo_list_t* list, const size_t index) {
	if (index >= list->size) return nullptr;
	return list->entries[index];
}

void todo_list_add(todo_list_t* list, const char* name, const char* description) {
	if (list->size == list->capacity) {
		const size_t new_cap = list->capacity ? list->capacity * 2 : INITIAL_CAPACITY;
		const auto new_list = realloc(list->entries, new_cap * sizeof(todo_entry_t *));
		if (new_list == nullptr) return;
		list->entries = new_list;
		list->capacity = new_cap;
	}
	todo_entry_t *entry = malloc(sizeof(todo_entry_t));
	if (entry == nullptr) return;
	entry->name = strdup(name);
	entry->description = strdup(description);
	if (!entry->name || !entry->description) {
		free((char *)entry->name);
		free((char *)entry->description);
		free(entry);
		return;
	}

	entry->completed = false;
	list->entries[list->size++] = entry;
}

void todo_list_remove(todo_list_t* list, const size_t index) {
	if (index >= list->size) return;

	free((char *)list->entries[index]->name);
	free((char *)list->entries[index]->description);
	free(list->entries[index]);

	memmove(
		&list->entries[index],
		&list->entries[index + 1],
		(list->size - index - 1) * sizeof(todo_entry_t *)
	);
	list->size--;

	if (list->size > 0 && list->size <= list->capacity / 4 && list->capacity > INITIAL_CAPACITY) {
		const size_t new_cap = list->capacity / 2;
		const auto new_list = realloc(list->entries, new_cap * sizeof(todo_entry_t *));
		if (new_list == nullptr) return;
		list->entries = new_list;
		list->capacity = new_cap;
	}
}

void todo_list_clear(todo_list_t* list) {
	for (size_t i = 0; i < list->size; i++) {
		free((char *)list->entries[i]->name);
		free((char *)list->entries[i]->description);
		free(list->entries[i]);
	}
	list->size = 0;
}

void todo_list_toggle(const todo_list_t* list, const size_t index) {
	if (index >= list->size) return;
	list->entries[index]->completed = !list->entries[index]->completed;
}

void todo_list_dtor(todo_list_t* list) {
	todo_list_clear(list);
	free(list->entries);
	free(list);
}

const char* todo_entry_name(const todo_entry_t* entry) {
	return entry->name;
}

const char* todo_entry_description(const todo_entry_t* entry) {
	return entry->description;
}

bool todo_entry_completed(const todo_entry_t* entry) {
	return entry->completed;
}