#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "project_structs.h"
#include "project.h"


static const char* get_person_name(int person_id, Person* person_db, int person_count) {
    for (int i = 0; i < person_count; i++) {
        if (person_db[i].id == person_id) {
            return person_db[i].name;
        }
    }
    return "Unknown";
}

void display_project_invoice(ProjectStore* store, int project_id, Person* person_db, int person_count) {
    
    Project* project = bst_find(store->root, project_id);

    if (project == NULL) {
        printf("\n>> Error: Project with ID %d not found.\n", project_id);
        return;
    }

    float total_project_hours = 0.0;
    float total_cost = 0.0;

    printf("\n--- INVOICE ---\n");
    printf("Project ID:   %d\n", project->id);
    printf("Project Name: %s\n", project->name);
    printf("Client:       %s\n", project->client_name);
    printf("Project Status: %s\n", project->status);
    printf("Rate:         $%.2f / hour\n", project->billing_rate);
    printf("-------------------------\n");
    printf("Line Items (Hours Logged by Person):\n");
    
    
    for (int i = 0; i < project->log_count; i++) {
        int person_id = project->hours_logged[i].person_id;
        float hours_logged = project->hours_logged[i].hours;
        
        const char* person_name = get_person_name(person_id, person_db, person_count);
        
        printf(" -- %s %0.2f hours\n", person_name, hours_logged);
        
        total_project_hours += hours_logged;
    }

    total_cost = total_project_hours * project->billing_rate;
    
    printf("------------\n");
    printf("Total Hours:  %.2f\n", total_project_hours);
    printf("\nTOTAL COST:   $%.2f\n", total_cost);
    printf("------------\n");

    printf("Sub-Task Summary:\n");
    if (project->task_count == 0) {
        printf("  (No tasks defined for this project.)\n");
    } else {
        for (int i = 0; i < project->task_count; i++) {
            Task* task = &project->tasks[i];

            printf("  > Task %-3d | Status: %-12s | Assigned: %-15s | Name: %s\n", 
                   task->id, task->status, task->name);
        }
    }
    printf("----------------------\n");
}


// MODIFIED: Logic now prints task details under the project.
void bst_print_all_recursive(ProjectBSTNode* node) {
    // Base case: if the node is NULL
    if (node == NULL) {
        return;
    }

    bst_print_all_recursive(node->left);
    
    // --- Print Project Details ---
    printf("\n[PROJECT] ID: %-5d | Priority: %-3d | Status: %-12s | Name: %s\n", 
           node->project->id, 
           node->project->priority,
           node->project->status, 
           node->project->name);
           
    if (node->project->task_count > 0) {
        printf("  -> Tasks (%d):\n", node->project->task_count);
        for (int i = 0; i < node->project->task_count; i++) {
            Task* task = &node->project->tasks[i];
            const char* assigned_name = (task->assigned_person_id == -1) 
                                        ? "N/A" 
                                        : get_person_name(task->assigned_person_id, NULL, 0); // Need person_db for real name

            printf("    - Task %-3d | Status: %-12s | Assigned ID: %-3d | Name: %s\n", 
                   task->id, task->status, task->assigned_person_id, task->name);
        }
    } else {
        printf("  -> (No tasks defined)\n");
    }
    
    bst_print_all_recursive(node->right);
}