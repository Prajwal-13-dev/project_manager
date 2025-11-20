

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "project_structs.h" 
#include "project.h"     
/*
1. A person can have multiple projects assigned, up to MAX_PROJECTS. (Now: A person can have up to MAX_PROJECTS tasks assigned.)
2. Priority (lower number = higher priority).
3. Employees must be registered before projects can be assigned.
4. Empty String inputs are not allowed for names and client names.
*/




int main() {
    ProjectStore store;
    store.root = NULL;
    store.pq_head = NULL;
    store.size = 0;
    
    Person person_db[MAX_PERSONS];
    int person_count = 0;

    printf("The Project Management System\n");
    
    initialize_employees(person_db, &person_count);

    char role_choice = ' ';
    while (role_choice != 'q') {
        printf("\n=====Role Selection =====\n");
        printf(" (m) Manager\n");
        printf(" (e) Employee\n");
        printf(" (q) Quit\n");
        printf("Enter your role: ");
        
        if (scanf(" %c", &role_choice) != 1) {
             role_choice = ' '; 
        }
        

        switch (role_choice) {
            case 'm':
            case 'M':
                handle_manager_menu(&store, person_db, person_count);
                break;
            case 'e':
            case 'E':
                handle_employee_menu(&store, person_db, person_count);
                break;
            case 'q':
            case 'Q':
                printf("Quitting program...\n");
                break; 
            default:
                printf("Invalid role. Please select 'm', 'e', or 'q'.\n");
        }
    }

    bst_free_recursive(store.root);
    pr_free(store.pq_head);
    return 0;
}