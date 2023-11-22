#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void childProcess(FILE *file, char group) {
    char line[100];
    printf("Proces fiu ruleaza.\n");

    rewind(file);

    while (fgets(line, sizeof(line), file) != NULL) {
        char studentGroup;

        if (sscanf(line, "%*[^,], %c,", &studentGroup) == 1) {
            if (studentGroup == group) {
                printf("Student: %s", line);
                usleep(50000);
            }
        }
    }

    printf("Proces fiu se încheie.\n");
}

int main() {
    char group;
    FILE *file;
    pid_t pid;

    file = fopen("studenti.txt", "r");
    if (file == NULL) {
        perror("Eroare la deschiderea fisierului");
        exit(EXIT_FAILURE);
    }

    while (1) {
        printf("Introduceti litera corespunzatoare grupei ('X' pentru a iesi): ");
        scanf(" %c", &group);

        if (group == 'X') {
            break; 
        }

        pid = fork();

        if (pid == -1) {
            perror("Eroare la fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            childProcess(file, group);
            fclose(file);
            exit(EXIT_SUCCESS); 
        } else {
            wait(NULL);
          
        }
    }

    fclose(file);
    printf("Proces parinte se încheie.\n");

    return 0;
}
