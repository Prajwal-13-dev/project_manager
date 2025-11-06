#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <float.h> 
#include "project_structs.h"
#include "project.h"


int assign_project_to_person(ProjectStore* store, Person* person_db, int person_count) {
    
    //  Pre-Checks 
    if (person_count == 0) {
        printf("\nError: No people in the system to assign to.\n");
        return -1;
    }
    if (store->size == 0 || store->pq_head == NULL) {
        printf("\nNo pending projects in the priority queue.\n");
        return 0; 
    }


    float min_total_hours = FLT_MAX;
    int least_loaded_index = -1; 

    for (int i = 0; i < person_count; i++) {
        if (least_loaded_index == -1) { 
            
            least_loaded_index = i;
            min_total_hours = person_db[i].total_hours;
        } else {
            // Compare person[i] with the current best (person[least_loaded_index])
            
            if (person_db[i].total_hours < min_total_hours) {

                min_total_hours = person_db[i].total_hours;
                least_loaded_index = i;
            } else if (person_db[i].total_hours == min_total_hours) {
                
                if (person_db[i].workload < person_db[least_loaded_index].workload) {
                    
                    least_loaded_index = i;
                    
                }
            }
        }
    }
   

    // Dequeue the highest-priority project
    Project* project_to_assign = pq_dequeue(&store->pq_head);
    if (project_to_assign == NULL) {
        printf("\nNo pending projects in the priority queue.\n");
        return 0; 
    }

    Person* assigned_person = &person_db[least_loaded_index];

    // Check if the person's project list is full
    if (assigned_person->workload >= MAX_PROJECTS) {
        printf("\nError: %s's project list is full (max %d). Cannot assign.\n", 
               assigned_person->name, MAX_PROJECTS);
        
        //  Put the project back in the queue!
        pq_enqueue(&store->pq_head, project_to_assign); 
        return -1;
    }

    // Assign project to person
    assigned_person->assigned_projects[assigned_person->workload] = project_to_assign->id;
    assigned_person->workload++; 

    // Assign person to project
    project_to_assign->assigned_person_id = assigned_person->id;
    strncpy(project_to_assign->status, "In Progress", 19); 
    project_to_assign->status[19] = '\0';

    printf("\nSUCCESS: Project %d ('%s') assigned to %s (Workload: %d projects, %.1f hours).\n",
           project_to_assign->id, project_to_assign->name, 
           assigned_person->name, assigned_person->workload, assigned_person->total_hours);
    
    return 0;
}


int log_hours_to_project(Project* project, Person* logging_person, float hours) {
    
    if (logging_person == NULL) {
        printf("Error: Invalid person for logging.\n");
        return -1;
    }
    if (hours <= 0) {
        printf("Info: Cannot log zero or negative hours.\n");
        return 0;
    }

    int person_id = logging_person->id; // Get ID for the project's log

    if (strcmp(project->status, "Completed") == 0) {
        printf("Info: Project %d is already Completed. No hours logged.\n", project->id);
        return 0;
    }

    if (strcmp(project->status, "Pending") == 0) {
       strncpy(project->status, "In Progress", 19); 
       project->status[19] = '\0';
    }

    bool found = false;
    for (int i = 0; i < project->log_count; i++) {
        if (project->hours_logged[i].person_id == person_id) {
            project->hours_logged[i].hours += hours;
            found = true;
            printf("Log updated for person %d (%s).\n", person_id, logging_person->name);
            break;
        }
    }

    if (!found) {
        if (project->log_count >= 10) { 
            printf("Error: Project %d log book is full. Cannot add new person.\n", project->id);
            return -1; 
        }
        
        project->hours_logged[project->log_count].person_id = person_id;
        project->hours_logged[project->log_count].hours = hours;
        project->log_count++;
        printf("New log created for person %d (%s).\n", person_id, logging_person->name);
    }


    logging_person->total_hours += hours;
    printf("Added %.1f hours to %s's total (New Total: %.1f hours).\n",
           hours, logging_person->name, logging_person->total_hours);

    char complete_choice;
    printf("Hours logged. Is project %d complete? (y/n): ", project->id);
    clear_input_buffer(); 
    while (scanf(" %c", &complete_choice) != 1 || (complete_choice!='y' && complete_choice!='Y' && complete_choice!='n' && complete_choice!='N')) {
        printf("Invalid (y/n): ");
        clear_input_buffer();
    } 

    if (complete_choice == 'y' || complete_choice == 'Y') {
        strncpy(project->status, "Completed", 19);
        project->status[19] = '\0';
        printf("Project %d marked as Completed.\n", project->id);
    }

    return 0;
}