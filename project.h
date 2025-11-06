#ifndef PROJECT_H
#define PROJECT_H

#include "project_structs.h"
#include <stdbool.h>


void clear_input_buffer();


Project* bst_find(ProjectBSTNode* root, int project_id);
void bst_print_all_recursive(ProjectBSTNode* node);


int pq_enqueue(PriorityQueue** pq_head, Project* project);
Project* pq_dequeue(PriorityQueue** pq_head);

int store_create_project(ProjectStore* store, int id, const char* name, const char* client, float rate, float est_hours, int priority);
int store_delete_project_and_unassign(ProjectStore* store, int project_id, Person* person_db, int person_count);
int modify_project_details(Project* project, const char* new_name, const char* new_client_name, float new_rate);

Person* find_person(Person* person_db, int person_count, int person_id);
int assign_project_to_person(ProjectStore* store, Person* person_db, int person_count);
int log_hours_to_project(Project* project, Person* logging_person, float hours);


void display_project_invoice(ProjectStore* store, int project_id, Person* person_db, int person_count);

#endif