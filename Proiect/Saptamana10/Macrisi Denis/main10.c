#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <dirent.h>
#include <sys/wait.h>

struct stat file_stat;

void handleError(char *message)
{
    perror(message);
    exit(EXIT_FAILURE);
}

typedef struct __attribute__((packed))
{
    char signature[2];
    unsigned int file_size;
    int garbage_1;
    int garbage_2;
    int garbage_3;
    unsigned int width;
    unsigned int height;
    unsigned int garbage_4[8];
    // From here start to read pixels
} BITMAP_HEADER;

char print[1024];

void rights(const char *name, int fd)
{
    struct stat st;
    char print[255];

    stat(name, &st);
    sprintf(print, "Drepturi de acces user: %c%c%c\n", file_stat.st_mode & S_IRUSR ? 'R' : '-', file_stat.st_mode & S_IWUSR ? 'W' : '-', file_stat.st_mode & S_IXUSR ? 'X' : '-');
    write(fd, print, strlen(print));
    sprintf(print, "Drepturi de acces grup: %c%c%c\n", file_stat.st_mode & S_IRGRP ? 'R' : '-', file_stat.st_mode & S_IWGRP ? 'W' : '-', file_stat.st_mode & S_IXGRP ? 'X' : '-');
    write(fd, print, strlen(print));
    sprintf(print, "Drepturi de acces altii: %c%c%c\n", file_stat.st_mode & S_IROTH ? 'R' : '-', file_stat.st_mode & S_IWOTH ? 'W' : '-', file_stat.st_mode & S_IXOTH ? 'X' : '-');
    write(fd, print, strlen(print));
}

void prelucrateFile(char *filename, char *output_dir, char ch)
{
    BITMAP_HEADER h;

    int input_fd = open(filename, O_RDONLY);
    if (input_fd == -1)
    {
        handleError("Fisierul sursa nu s-a putut deschide");
    }

    if (fstat(input_fd, &file_stat) == -1)
    {
        close(input_fd);
        handleError("Eroare la obtinerea informatiilor despre fisier");
    }

    char statistics_filename[256];
    char *trunc_filename = strrchr(filename, '/');
    if (trunc_filename != NULL)
    {
        trunc_filename++;
    }
    else
    {
        trunc_filename = filename;
    }

    sprintf(statistics_filename, "%s/%s_statistica.txt", output_dir, trunc_filename);

    int stats_fd = open(statistics_filename, O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);
    // printf(statistics_filename);

    if (stats_fd < 0)
    {
        handleError("Eroare la accesarea fisierului de statistica");
    }

    if (S_ISREG(file_stat.st_mode))
    {
        if (strstr(filename, ".bmp"))
        {
            input_fd = open(filename, O_RDWR);

            if (read(input_fd, &h, sizeof(h)) != sizeof(BITMAP_HEADER))
            {
                handleError("Eroare la citirea antetului");
            }

            pid_t pid2 = fork();

            if (pid2 == -1)
            {
                handleError("Eroare la fork pentru prelucrare fisier bmp");
            }
            else if (pid2 == 0)
            {
                //printf("heigth = %d, width = %d \n", h.height, h.width);
                int size = h.height * h.width;
                unsigned char *photo = malloc(sizeof(unsigned char) * size * 3);
                if (photo == NULL)
                {
                    handleError("Problema la alocarea memorie pentru imagine");
                }
                if (read(input_fd, photo, sizeof(unsigned char) * size * 3) == -1)
                {
                    handleError("Eroare la citirea informatiilor din fisierul BMP");
                }

                for (int i = 0; i < size * 3; i = i + 3)
                {
                    short int red = photo[i];
                    short int green = photo[i + 1];
                    short int bleu = photo[i + 2];

                    short int p_gray = (short int)(0.299 * red + 0.587 * green + 0.114 * bleu);

                    photo[i] = p_gray;
                    photo[i + 1] = p_gray;
                    photo[i + 2] = p_gray;
                }

                if (lseek(input_fd, 54, SEEK_SET) == -1)
                {
                    handleError("Eroare la deschiderea imaginii pentru scriere");
                }
                if (write(input_fd, photo, sizeof(unsigned char) * size * 3) == -1)
                {
                    handleError("Eroare la scriere informatiilor prelucrate in fisierul BMP");
                }

                printf("Procesul fiu a prelucrat imaginea BMP: %s\n", filename);
                exit(EXIT_SUCCESS);
            }
            else
            {
                int status;
                waitpid(pid2, &status, 0);
                if (WIFEXITED(status))
                {
                    printf("S-a încheiat procesul cu pid-ul %d și codul %d\n", pid2, WEXITSTATUS(status));
                }
                else
                {
                    printf("Procesul cu pid-ul %d s-a încheiat anormal\n", pid2);
                }
            }

            sprintf(print, "nume fisier: %s \n", filename);
            write(stats_fd, print, strlen(print));

            sprintf(print, "inaltime: %d \n", h.height);
            write(stats_fd, print, strlen(print));

            sprintf(print, "lungime: %d \n", h.width);
            write(stats_fd, print, strlen(print));

            sprintf(print, "dimensiune: %ld \n", file_stat.st_size);
            write(stats_fd, print, strlen(print));

            sprintf(print, "identificatorul utilizatorului: %u \n", file_stat.st_uid);
            write(stats_fd, print, strlen(print));

            time_t lastModified = file_stat.st_mtime;
            struct tm *timeinfo = localtime(&lastModified);
            sprintf(print, "Ultima modificare a lui: %d %d %d\n", timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900);
            write(stats_fd, print, strlen(print));

            sprintf(print, "contorul de legaturi: %ld \n", file_stat.st_nlink);
            write(stats_fd, print, strlen(print));

            rights(filename, stats_fd);
        }
        else
        {
            /*
            int myPipe;
            int pf1f2[2];
            int pf2f1[2];
            */
           
            pid_t pid3 = fork();

            if (pid3 == -1)
            {
                handleError("Eroare la fork pentru prelucrare fisier bmp");
            }
            else if (pid3 == 0)
            {
                sprintf(print, "nume fisier: %s \n", filename);
                write(stats_fd, print, strlen(print));

                sprintf(print, "dimensiune: %ld \n", file_stat.st_size);
                write(stats_fd, print, strlen(print));

                sprintf(print, "identificatorul utilizatorului: %u \n", file_stat.st_uid);
                write(stats_fd, print, strlen(print));

                time_t lastModified = file_stat.st_mtime;
                struct tm *timeinfo = localtime(&lastModified);
                sprintf(print, "Ultima modificare a lui: %d %d %d\n", timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900);
                write(stats_fd, print, strlen(print));

                sprintf(print, "contorul de legaturi: %ld \n", file_stat.st_nlink);
                write(stats_fd, print, strlen(print));

                rights(filename, stats_fd);

                /*

                ///Aici are loc verificarea liniilor fisierului de statistica ///

            
                if (pipe(pf1f2) == -1)
                {
                    handleError("Eroare la pipe");
                }

                myPipe = fork();

                if (myPipe == -1)
                {
                    handleError("Eroare la fork pentru prelucrare fisier bmp");
                }

                if (myPipe == 0)
                {
                    FILE *output;
                    char buff[128];

                    output = popen("./script.sh", "w");
                    if (output == NULL)
                    {
                        handleError("Eroare la deschiderea script");
                    }

                    while (fgets(buff, sizeof(buff), output) != NULL)
                    {
                        int nr_linii = atoi(buff);
                        printf("Au fost identificate in total %d linii ce contin caracterul cautat", nr_linii);
                    }

                    if (pclose(output) == -1)
                    {
                        handleError("Eroare la inchiderea scriptului");
                    }
                }
                */

                printf("Procesul fiu a prelucrat fisierul statistica aferent fisierului: %s\n", filename);
                exit(EXIT_SUCCESS);
            }
            else
            {
                int status;
                waitpid(pid3, &status, 0);
                if (WIFEXITED(status))
                {
                    printf("S-a încheiat procesul cu pid-ul %d și codul %d\n", pid3, WEXITSTATUS(status));
                }
                else
                {
                    printf("Procesul cu pid-ul %d s-a încheiat anormal\n", pid3);
                }
            }
        }
    }
    else if (S_ISDIR(file_stat.st_mode))
    {
        if (fstat(input_fd, &file_stat) == 0)
        {
            sprintf(print, "nume director: %s \n", filename);
            write(stats_fd, print, strlen(print));

            sprintf(print, "identificatorul utilizatorului: %u \n", file_stat.st_uid);
            write(stats_fd, print, strlen(print));

            rights(filename, stats_fd);
        }
        else
        {
            sprintf(print, "Eroare la obtinerea informatiilor despre directorul %s", filename);
            handleError(print);
        }
    }
    else if (S_ISLNK(file_stat.st_mode))
    {
        char path[256];
        ssize_t link_size = readlink(filename, path, sizeof(path) - 1);
        if (link_size != -1)
        {
            path[link_size] = '\0';

            if (fstat(input_fd, &file_stat) == 0)
            {
                sprintf(print, "nume legatura: %s \n", filename);
                write(stats_fd, print, strlen(print));

                sprintf(print, "dimensiune: %ld \n", link_size);
                write(stats_fd, print, strlen(print));

                sprintf(print, "dimensiune fisier: %ld \n", file_stat.st_size);
                write(stats_fd, print, strlen(print));

                rights(filename, stats_fd);
            }
            else
            {
                handleError("Eroare la obtinerea informatiilor despre legatura simbolica");
            }
        }
        else
        {
            handleError("Eroare la citirea legaturii simbolice");
        }
    }

    close(stats_fd);
    close(input_fd);
}

void traverseDirectory(char *dir_path, char *dir_path_out, char ch)
{
    DIR *dir = opendir(dir_path);
    if (!dir)
    {
        handleError("Directorul nu s-a putut deschide");
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {

        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue; //
        }

        char *path = malloc(strlen(dir_path) + strlen(entry->d_name) + 2);
        if (path == NULL)
        {
            handleError("Eroare la alocarea memoriei pentru path");
        }
        snprintf(path, strlen(dir_path) + strlen(entry->d_name) + 2, "%s/%s", dir_path, entry->d_name);

        if (lstat(path, &file_stat) == -1)
        {
            handleError("Eroare la obtinerea informatiilor despre fisier");
        }

        if (S_ISREG(file_stat.st_mode) || S_ISDIR(file_stat.st_mode) || S_ISLNK(file_stat.st_mode))
        {

            pid_t child_pid = fork();

            if (child_pid == -1)
            {
                handleError("Eroare la fork");
            }
            else if (child_pid == 0)
            {
                // Procesul copil
                prelucrateFile(path, dir_path_out,ch);
                free(path);
                exit(EXIT_SUCCESS);
            }
            else
            {
                // Procesul părinte
                int status;
                waitpid(child_pid, &status, 0);

                if (WIFEXITED(status))
                {
                    printf("S-a încheiat procesul cu pid-ul %d și codul %d\n", child_pid, WEXITSTATUS(status));
                }
                else
                {
                    printf("Procesul cu pid-ul %d s-a încheiat anormal\n", child_pid);
                }
            }
        }

        if (S_ISDIR(file_stat.st_mode))
        {
            traverseDirectory(path, dir_path_out, ch);
        }

        free(path);
    }

    closedir(dir);
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        handleError("Programul trebuie apelat sub forma: Program <DirectorIntrare> <DirectorIesire> <caracter>");
    }

    traverseDirectory(argv[1], argv[2], argv[3][0]);

    return 0;
}