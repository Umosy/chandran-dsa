#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME 50
#define MAX_BRANCH 5   

typedef enum { CEO, MANAGER, EMPLOYEE } PositionType;

typedef struct Employee {
    char name[MAX_NAME];
    int id;
    float salary;
    int experience;
    PositionType position;
    int branchNumber; 
    struct Employee* next;
} Employee;


Employee* ceo = NULL;
Employee* branchManagers[MAX_BRANCH] = {NULL}; 


void loadFromFile();
void saveToFile();
void addEmployee();
void removeEmployee();
void viewEmployeeDetails();
void traverseEmployees();
void traverseManagers();
void traverseBranchEmployees(int);
Employee* findEmployee(int);
void menu();

int main() {
    loadFromFile();

    int choice;
    do {
        menu();
        printf("Enter your choice: ");
        scanf("%d", &choice);
        switch(choice) {
            case 1: 
                addEmployee(); 
                break;
            case 2: 
                removeEmployee(); 
                break;
            case 3: 
                viewEmployeeDetails(); 
                break;
            case 4: 
                traverseEmployees(); 
                break;
            case 5: 
                traverseManagers(); 
                break;
            case 6: {
                int branch;
                printf("Enter branch number (1-%d): ", MAX_BRANCH);
                scanf("%d", &branch);
                if(branch >= 1 && branch <= MAX_BRANCH)
                    traverseBranchEmployees(branch);
                else
                    printf("Invalid branch number.\n");
                break;
            }
            case 7: 
                saveToFile(); 
                printf("Data saved.\n"); 
                break;
            case 8: 
                printf("Exiting...\n"); 
                saveToFile(); 
                exit(0);
            default: printf("Invalid choice! Try again.\n");
        }
    } while(1);

    return 0;
}

void menu() {
    printf("\n");
    printf("--- Employee Hierarchy Management ---\n");
    printf("1. Add Employee/Manager\n");
    printf("2. Remove Employee\n");
    printf("3. View All Details\n");
    printf("4. View Only Employees\n");
    printf("5. View Only Managers\n");
    printf("6. View Employees of a Branch\n");
    printf("7. Save Data\n");
    printf("8. Exit\n");
}


void loadFromFile() {
    FILE *fp = fopen("employee.txt", "r");
    if (!fp) return;

    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        char typeStr[20];
        Employee* emp = (Employee*)malloc(sizeof(Employee));
        emp->next = NULL;
        sscanf(line, "%[^,],%d,%f,%d,%[^,],%d",typeStr,&emp->id,&emp->salary,&emp->experience,emp->name,&emp->branchNumber);

        if (strcmp(typeStr, "CEO") == 0) {
            emp->position = CEO;
            ceo = emp;
        } else if (strcmp(typeStr, "MANAGER") == 0) {
            emp->position = MANAGER;
            int branch = emp->branchNumber;
            if (branch >=1 && branch <= MAX_BRANCH) {
                emp->next = branchManagers[branch - 1];
                branchManagers[branch - 1] = emp;
            }
        } else if (strcmp(typeStr, "EMPLOYEE") == 0) {
            emp->position = EMPLOYEE;
            int branch = emp->branchNumber;
            if (branch >=1 && branch <= MAX_BRANCH) {
                Employee* tmp = branchManagers[branch - 1];
                while (tmp->next) tmp = tmp->next;
                tmp->next = emp;
            }
        }
    }
    fclose(fp);
}


void saveToFile() {
    FILE *fp = fopen("employee.txt", "w");
    
    if (ceo) {
        fprintf(fp, "CEO,%d,%f,%d,%s,0\n", ceo->id, ceo->salary, ceo->experience, ceo->name);
    }

    
    for (int i=0; i<MAX_BRANCH; i++) {
        Employee* mgr = branchManagers[i];
        while (mgr) {
            if (mgr->position == MANAGER)
                fprintf(fp, "MANAGER,%d,%f,%d,%s,%d\n", mgr->id, mgr->salary, mgr->experience, mgr->name, i+1);
            mgr = mgr->next;
        }
    }

    
    for (int i=0; i<MAX_BRANCH; i++) {
        Employee* mgr = branchManagers[i];
        while (mgr) {
            if (mgr->position == EMPLOYEE)
                fprintf(fp, "EMPLOYEE,%d,%.2f,%d,%s,%d\n", mgr->id, mgr->salary, mgr->experience, mgr->name, i+1);
            mgr = mgr->next;
        }
    }
    fclose(fp);
}


void addEmployee() {
    int choice;
    printf("Add:\n1. CEO\n2. Branch Manager\n3. Employee\nChoice: ");
    scanf("%d", &choice);
    if (choice == 1) {
        if (ceo != NULL) {
            printf("CEO already exists.\n");
            return;
        }
        Employee* newCEO = (Employee*)malloc(sizeof(Employee));
        printf("Enter ID: "); scanf("%d", &newCEO->id);
        printf("Enter Name: "); scanf("%s", newCEO->name);
        printf("Enter Salary: "); scanf("%f", &newCEO->salary);
        printf("Enter Experience (years): "); scanf("%d", &newCEO->experience);
        newCEO->position = CEO;
        newCEO->branchNumber = 0;
        newCEO->next = NULL;
        ceo = newCEO;
        printf("CEO added.\n");
    } else if (choice == 2) {
        int branch;
        printf("Enter Branch Number (1-%d): ", MAX_BRANCH);
        scanf("%d", &branch);
        if (branch < 1 || branch > MAX_BRANCH) {
            printf("Invalid branch.\n");
            return;
        }
        
        Employee* temp = branchManagers[branch-1];
        while (temp) {
            if (temp->position == MANAGER) {
                printf("Manager for this branch already exists.\n");
                return;
            }
            temp = temp->next;
        }
        Employee* mgr = (Employee*)malloc(sizeof(Employee));
        printf("Enter Manager ID: ");
        scanf("%d", &mgr->id);
        printf("Enter Name: "); 
        scanf("%s", mgr->name);
        printf("Enter Salary: "); 
        scanf("%f", &mgr->salary);
        printf("Enter Experience: "); 
        scanf("%d", &mgr->experience);
        mgr->position = MANAGER;
        mgr->branchNumber = branch;
        mgr->next = branchManagers[branch-1];
        branchManagers[branch-1] = mgr;
        printf("Manager added.\n");
    } else if (choice == 3) {
        int branch;
        printf("Enter Branch Number (1-%d): ", MAX_BRANCH);
        scanf("%d", &branch);
        if (branch < 1 || branch > MAX_BRANCH) {
            printf("Invalid branch.\n");
            return;
        }
        Employee* mgr = branchManagers[branch-1];
        if (!mgr || mgr->position != MANAGER) {
            printf("No manager for this branch. Add manager first.\n");
            return;
        }
        Employee* emp = (Employee*)malloc(sizeof(Employee));
        printf("Enter Employee ID: "); scanf("%d", &emp->id);
        printf("Enter Name: "); scanf("%s", emp->name);
        printf("Enter Salary: "); scanf("%f", &emp->salary);
        printf("Enter Experience: "); scanf("%d", &emp->experience);
        emp->position = EMPLOYEE;
        emp->branchNumber = branch;
        emp->next = NULL;

        
        Employee* temp = mgr;
        while (temp->next) temp = temp->next;
        temp->next = emp;
        printf("Employee added.\n");
    } else {
        printf("Invalid choice.\n");
    }
}


void removeEmployee() {
    int id;
    printf("Enter Employee ID to remove: ");
    scanf("%d", &id);

    
    if (ceo && ceo->id == id) {
        free(ceo);
        ceo = NULL;
        printf("CEO removed.\n");
        return;
    }

    
    for (int i=0; i<MAX_BRANCH; i++) {
        Employee* curr = branchManagers[i];
        Employee* prev = NULL;
        while (curr) {
            if (curr->id == id) {
                if (prev == NULL)
                    branchManagers[i] = curr->next;
                else
                    prev->next = curr->next;
                free(curr);
                printf("Removed employee with ID %d.\n", id);
                return;
            }
            prev = curr;
            curr = curr->next;
        }
    }
    printf("Employee ID not found.\n");
}


void viewEmployeeDetails() {
    printf("\n--- CEO ---\n");
    if (ceo)
        printf("\nID:%d, Name:%s, Salary:%.2f, Exp:%d\n", ceo->id, ceo->name, ceo->salary, ceo->experience);
    else
        printf("No CEO assigned.\n");

    printf("\n--- Managers and Employees ---\n");
    for (int i=0; i<MAX_BRANCH; i++) {
        Employee* mgr = branchManagers[i];
        while (mgr) {
            if (mgr->position == MANAGER)
                printf("\nManager (Branch %d): ID:%d, Name:%s, Salary:%.2f, Exp:%d\n", i+1, mgr->id, mgr->name, mgr->salary, mgr->experience);
            else if (mgr->position == EMPLOYEE)
                printf("\nEmployee (Branch %d): ID:%d, Name:%s, Salary:%.2f, Exp:%d\n", i+1, mgr->id, mgr->name, mgr->salary, mgr->experience);
            mgr = mgr->next;
        }
    }
}

void traverseEmployees() {
    printf("\n--- Employees ---\n");
    for (int i=0; i<MAX_BRANCH; i++) {
        Employee* mgr = branchManagers[i];
        while (mgr) {
            if (mgr->position == EMPLOYEE)
                printf("ID:%d, Name:%s, Branch:%d\n", mgr->id, mgr->name, i+1);
            mgr = mgr->next;
        }
    }
}


void traverseManagers() {
    printf("\n--- Managers ---\n");
    for (int i=0; i<MAX_BRANCH; i++) {
        Employee* mgr = branchManagers[i];
        while (mgr) {
            if (mgr->position == MANAGER)
                printf("\nID:%d, Name:%s, Branch:%d\n", mgr->id, mgr->name, i+1);
            mgr = mgr->next;
        }
    }
}

void traverseBranchEmployees(int branch) {
    printf("\n--- Employees in Branch %d ---\n", branch);
    Employee* mgr = branchManagers[branch-1];
    if (mgr == NULL) return;
    if ( mgr->position != MANAGER) {
        printf("No manager for this branch.\n");
        return;
    }
    Employee* emp = mgr->next;
    while (emp) {
        if (emp->position == EMPLOYEE)
            printf("ID:%d, Name:%s\n", emp->id, emp->name);
        emp = emp->next;
    }
}