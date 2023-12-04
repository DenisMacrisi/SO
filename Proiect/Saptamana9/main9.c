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

void prelucrateFile(char *filename, char *output_dir)
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
    char *base_filename = strrchr(filename, '/');
    if (base_filename != NULL)
    {
        base_filename++;
    }
    else
    {
        base_filename = filename;
    }

    sprintf(statistics_filename, "%s/%s_statistica.txt", output_dir, base_filename);

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
                printf("heigth = %d, width = %d \n", h.height, h.width);
                int size = h.height * h.width;
                unsigned char *photo = malloc(sizeof(unsigned char) * size * 3);
                if (photo == NULL)
                {
                    handleError("Problema la alocarea memorie pentru imagine");
                }
                if (read(input_fd, photo, sizeof(unsigned char) * size * 3) == -1)
                {
                    free(photo);
                    handleError("Eroare la citirea informatiilor din fisierul BMP");
                }

                for (int i = 0; i < size * 3; i = i + 3)
                {
                    __uint8_t r = photo[i];
                    __uint8_t g = photo[i + 1];
                    __uint8_t b = photo[i + 2];

                    __uint8_t p_gri = (__uint8_t)(0.299 * r + 0.587 * g + 0.114 * b);

                    photo[i] = p_gri;
                    photo[i + 1] = p_gri;
                    photo[i + 2] = p_gri;
                }

                if (lseek(input_fd, 54, SEEK_SET) == -1)
                {
                    free(photo);
                    handleError("Eroare la deschiderea imaginii pentru scriere");
                }
                if (write(input_fd, photo, sizeof(unsigned char) * size * 3) == -1)
                {
                    free(photo);
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
            sprintf(print, "Drepturi de acces user: %c%c%c\n"
                           "Drepturi de acces grup: %c%c%c\n"
                           "Drepturi de acces altii: %c%c%c\n",
                    file_stat.st_mode & S_IRUSR ? 'R' : '-', file_stat.st_mode & S_IWUSR ? 'W' : '-', file_stat.st_mode & S_IXUSR ? 'X' : '-',
                    file_stat.st_mode & S_IRGRP ? 'R' : '-', file_stat.st_mode & S_IWGRP ? 'W' : '-', file_stat.st_mode & S_IXGRP ? 'X' : '-',
                    file_stat.st_mode & S_IROTH ? 'R' : '-', file_stat.st_mode & S_IWOTH ? 'W' : '-', file_stat.st_mode & S_IXOTH ? 'X' : '-');
            write(stats_fd, print, strlen(print));

        }
        else
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
            sprintf(print, "Drepturi de acces user: %c%c%c\n"
                           "Drepturi de acces grup: %c%c%c\n"
                           "Drepturi de acces altii: %c%c%c\n",
                    file_stat.st_mode & S_IRUSR ? 'R' : '-', file_stat.st_mode & S_IWUSR ? 'W' : '-', file_stat.st_mode & S_IXUSR ? 'X' : '-',
                    file_stat.st_mode & S_IRGRP ? 'R' : '-', file_stat.st_mode & S_IWGRP ? 'W' : '-', file_stat.st_mode & S_IXGRP ? 'X' : '-',
                    file_stat.st_mode & S_IROTH ? 'R' : '-', file_stat.st_mode & S_IWOTH ? 'W' : '-', file_stat.st_mode & S_IXOTH ? 'X' : '-');
            write(stats_fd, print, strlen(print));
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

            sprintf(print, "Drepturi de acces user: %c%c%c\n"
                           "Drepturi de acces grup: %c%c%c\n"
                           "Drepturi de acces altii: %c%c%c\n",
                    file_stat.st_mode & S_IRUSR ? 'R' : '-', file_stat.st_mode & S_IWUSR ? 'W' : '-', file_stat.st_mode & S_IXUSR ? 'X' : '-',
                    file_stat.st_mode & S_IRGRP ? 'R' : '-', file_stat.st_mode & S_IWGRP ? 'W' : '-', file_stat.st_mode & S_IXGRP ? 'X' : '-',
                    file_stat.st_mode & S_IROTH ? 'R' : '-', file_stat.st_mode & S_IWOTH ? 'W' : '-', file_stat.st_mode & S_IXOTH ? 'X' : '-');
            write(stats_fd, print, strlen(print));
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
                sprintf(print, "Drepturi de acces user: %c%c%c\n"
                               "Drepturi de acces grup: %c%c%c\n"
                               "Drepturi de acces altii: %c%c%c\n",
                        file_stat.st_mode & S_IRUSR ? 'R' : '-', file_stat.st_mode & S_IWUSR ? 'W' : '-', file_stat.st_mode & S_IXUSR ? 'X' : '-',
                        file_stat.st_mode & S_IRGRP ? 'R' : '-', file_stat.st_mode & S_IWGRP ? 'W' : '-', file_stat.st_mode & S_IXGRP ? 'X' : '-',
                        file_stat.st_mode & S_IROTH ? 'R' : '-', file_stat.st_mode & S_IWOTH ? 'W' : '-', file_stat.st_mode & S_IXOTH ? 'X' : '-');
                write(stats_fd, print, strlen(print));
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

void traverseDirectory(char *dir_path, char *dir_path_out)
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
            continue;
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
                prelucrateFile(path, dir_path_out);
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
            traverseDirectory(path, dir_path_out);
        }

        free(path);
    }

    closedir(dir);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        handleError("Programul trebuie apelat sub forma: Program <DirectorIntrare> <DirectorIesire>");
    }

    traverseDirectory(argv[1], argv[2]);

    return 0;
}