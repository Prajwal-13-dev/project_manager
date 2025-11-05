

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "project_structs.h" 
#include "project.h"     
/*
CONSTRAINTS:
1.One project assigned to one person only.
2.A person can have multiple projects assigned, up to MAX_PROJECTS.
3.Priority (lower number = higher priority).
4.Employees must be registered before projects can be assigned.
5.Empty String inputs are not allowed for names and client names.
*/

void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}


int get_int_input(const char* prompt) {
    int value;
    printf("%s", prompt);
    while (scanf(" %d", &value) != 1) {
        printf("Invalid input. Please enter a number: ");
        clear_input_buffer();
    }
    return value;
}

float get_float_input(const char* prompt) {
    float value;
    printf("%s", prompt);
    while (scanf(" %f", &value) != 1) {
        printf("Invalid input. Please enter a number (e.g., 150.75): ");
        clear_input_buffer();
    }
    return value;
}


void get_string_input(const char* prompt, char* strin) { 
    printf("%s", prompt);
    scanf(" %s", strin);
}


void initialize_employees(Person* person_db, int* person_count) {
    printf("===== Employee Setup =====\n");
    
    int n = 0;
    while (n <= 0 || n > MAX_PERSONS) {
        n = get_int_input("Enter the number of employees to register: ");
        if (n <= 0 || n > MAX_PERSONS) {
            printf("Please enter a number between 1 and %d.\n", MAX_PERSONS);
        }
    } 

    while (*person_count < n) {
        printf("Employee :%d    \n", *person_count + 1);
        
        int new_id;
        bool duplicate;
        
        while (true) { 
            new_id = get_int_input("Enter Employee ID: ");
            duplicate = false;
            
            for (int i = 0; i < *person_count; i++) {
                if (person_db[i].id == new_id) {
                    printf("Error: Employee ID %d already exists. Please enter a different ID.\n", new_id);
                    duplicate = true;
                    break;
                }
            }
            
            if (!duplicate) {
                break; 
            }
        }

        char name_buffer[MAX_NAME_LEN];
        get_string_input("Enter Employee Name: ", name_buffer);

        person_db[*person_count].id = new_id;
        strncpy(person_db[*person_count].name, name_buffer, MAX_NAME_LEN - 1);
        person_db[*person_count].name[MAX_NAME_LEN - 1] = '\0';
        person_db[*person_count].workload = 0; 
        
        (*person_count)++; 
    }
    
    printf(" Employee setup complete.\n");
}


void do_create_project(ProjectStore* store) {
    int id = get_int_input("Enter Project ID: ");
    
    if (bst_find(store->root, id) != NULL) {
        printf(">> Error: Project with ID %d already exists.\n", id);
        return;
    }
    
    char name[MAX_NAME_LEN];
    char client[MAX_NAME_LEN];
    get_string_input("Enter Project Name: ", name);
    get_string_input("Enter Client Name: ", client);
    float rate = get_float_input("Enter Billing Rate (e.g., 100.50): ");
    float hours = get_float_input("Enter Estimated Hours: ");
    int priority = get_int_input("Enter Priority (1-10, 1 is highest): ");

    if (store_create_project(store, id, name, client, rate, hours, priority) == 0) {
        printf("Success: Project '%s' (ID: %d) created and added to queue.\n", name, id);
    } else {
        printf(">> Error: Failed to create project (memory issue or duplicate ID).\n");
    }
}

void modify_project(ProjectStore* store) {
    int id = get_int_input("Enter Project ID to modify: ");
    Project* proj = bst_find(store->root, id);

    if (proj == NULL) {
        printf(">> Error: Project with ID %d not found.\n", id);
        return;
    }

    printf("Current Name: %s\n", proj->name);
    char name[MAX_NAME_LEN];
    get_string_input("Enter new Project Name: ", name);

    printf("Current Client: %s\n", proj->client_name);
    char client[MAX_NAME_LEN];
    get_string_input("Enter new Client Name: ", client);

    printf("Current Rate: %.2f\n", proj->billing_rate);
    float rate = get_float_input("Enter new Billing Rate: ");

    if (modify_project_details(proj, name, client, rate) == 0) {
        printf("Success: Project %d updated.\n", id);
    } else {
        printf(">> Error: Failed to update project.\n");
    }
}


void assign_project(ProjectStore* store, Person* person_db, int person_count) {
    printf("Assigning Next Pending Project \n");

    if (assign_project_to_person(store, person_db, person_count) != 0) {
        printf(">> Assignment failed.\n");
    }
}

void delete_project(ProjectStore* store, Person* person_db, int person_count) {
    int id = get_int_input("Enter Project ID to delete: ");
    
    if (bst_find(store->root, id) == NULL) {
        printf(">> Error: Project with ID %d not found.\n", id);
        return;
    }
    
    if (store_delete_project_and_unassign(store, id, person_db, person_count) == 0) {
        printf("Success: Project %d deleted and unassigned if necessary.\n", id);
    } else {
        printf(">> Error: Failed to delete project %d.\n", id);
    }
}

void generate_inv(ProjectStore* store, Person* person_db, int person_count) {
    printf("\n-- Generate Project Invoice --\n");
    int id = get_int_input("Enter Project ID to generate invoice for: ");
    display_project_invoice(store, id, person_db, person_count);
}

void view_projects(ProjectStore* store) {
    printf("\n-- All Projects (Sorted by ID) --\n");
    bst_print_all_recursive(store->root);

}

void handle_manager_menu(ProjectStore* store, Person* person_db, int person_count) {
    int choice = 0;
    while (choice != 7) {
        printf("\n:::Manager Menu:::\n");
        printf(" 1. Create Project\n");
        printf(" 2. Modify Project\n");
        printf(" 3. Assign Next Pending Project (by Priority)\n");
        printf(" 4. Delete Project\n");
        printf(" 5. Generate Project Invoice\n");
        printf(" 6. View All Projects\n");
        printf(" 7. Exit (Back to Role Selection)\n");
        choice = get_int_input("Enter choice: ");
        printf("\nChoice selected: %d\n", choice);
        switch (choice) {
            case 1:

                do_create_project(store);
                break;
            case 2:
                modify_project(store);
                break;
            case 3:
                assign_project(store, person_db, person_count);
                break;
            case 4:
                delete_project(store, person_db, person_count);
                break;
            case 5:
                generate_inv(store, person_db, person_count);
                break;
            case 6:
                view_projects(store);
                break;
            case 7:
                printf("Returning to role selection...\n");
                return; 
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }
}

void do_log_hours(ProjectStore* store, Person* person_db, int person_count) {
    printf("Log Hours to Project\n");
    
    int person_id = get_int_input("Enter your Employee ID: ");
    Person* person = find_person(person_db, person_count, person_id);
    if (person == NULL) {
        printf(">> Error: Employee ID %d not found.\n", person_id);
        return;
    }
    printf("Welcome, %s.\n", person->name);

    if (person->workload == 0) {
        printf(">> You have no projects assigned.\n");
        return;
    }

    printf("Your assigned projects:\n");
    for (int i = 0; i < person->workload; i++) {
        int proj_id = person->assigned_projects[i];
        Project* proj = bst_find(store->root, proj_id);
        if (proj) {
            printf(" ID: %d | Name: %s | Status: %s\n", proj->id, proj->name, proj->status);
        }
    }

    int project_id = get_int_input("Enter Project ID to log hours for: ");
    Project* project_to_log = bst_find(store->root, project_id);

    if (project_to_log == NULL) {
        printf(">> Error: Project ID %d not found.\n", project_id);
        return;
    }
    if (project_to_log->assigned_person_id != person->id) {
         printf(">> Error: Project %d is not assigned to you.\n", project_id);
         return;
    }

    float hours = get_float_input("Enter hours to log: ");
    if (hours <= 0) {
        printf(">> Hours must be positive. No hours logged.\n");
        return;
    }

    if (log_hours_to_project(project_to_log, person_id, hours) == 0) {
        printf("Success: Hours logged to project %d.\n", project_id);
    } else {
        printf(">> Error: Failed to log hours.\n");
    }
}

void handle_employee_menu(ProjectStore* store, Person* person_db, int person_count) {
    int choice = 0;
    while (choice != 2) {
        printf("\n::: Employee Menu:::\n");
        printf(" 1. Log Hours to Project\n");
        printf(" 2. Exit (Back to Role Selection)\n");
        choice = get_int_input("Enter choice: ");

        switch (choice) {
            case 1:
                do_log_hours(store, person_db, person_count);
                break;
            case 2:
                printf("Returning to role selection...\n");
                return; 
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }
}

// Memory Cleanup

static void bst_free_recursive(ProjectBSTNode* node) {
    if (node == NULL) {
        return;
    }
    bst_free_recursive(node->left);
    bst_free_recursive(node->right);
    
    if (node->project) {
        free(node->project);
    }
    free(node);
}

static void pr_free(PriorityQueue* node) {
    if (node == NULL) {
        return;
    }
    pr_free(node->next);
    free(node);
}

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
        clear_input_buffer(); 

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