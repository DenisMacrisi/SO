#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <dirent.h>

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
} BITMAP_HEADER;

char print[1024];

void prelucrateFile(char *filename)
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

    int stats_fd = open("statistica.txt", O_CREAT | O_WRONLY | O_APPEND, S_IRWXU | S_IRWXG | S_IRWXO);

    if (stats_fd < 0)
    {
        handleError("Eroare la accesarea fisierului de statistica");
    }

    if (S_ISREG(file_stat.st_mode))
    {
        if (strstr(filename, ".bmp"))
        {
            if (read(input_fd, &h, sizeof(h)) != sizeof(BITMAP_HEADER))
            {
                handleError("Eroare la citirea antetului");
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

void traverseDirectory(char *dir_path)
{
DIR *dir = opendir(dir_path);
    if (!dir) {
        handleError("Directorul nu s-a putut deschide");
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        // Ignorăm directoarele /.. și /.
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char *path = malloc(strlen(dir_path) + strlen(entry->d_name) + 2); // 2 pentru '/' și '\0'
        if (path == NULL) {
            handleError("Eroare la alocarea memoriei pentru path");
        }
        snprintf(path, strlen(dir_path) + strlen(entry->d_name) + 2, "%s/%s", dir_path, entry->d_name);

        if (lstat(path, &file_stat) == -1) {
            handleError("Eroare la obtinerea informatiilor despre fisier");
        }

        if (S_ISREG(file_stat.st_mode) || S_ISDIR(file_stat.st_mode) || S_ISLNK(file_stat.st_mode)) {
            prelucrateFile(path);
        }

        // Dacă este un director, apelăm recursiv funcția pentru a-l procesa
        if (S_ISDIR(file_stat.st_mode)) {
            traverseDirectory(path);
        }

        free(path);
    }

    closedir(dir);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        handleError("Programul trebuie apelat cu un parametru ce reprezinta un director");
    }

    traverseDirectory(argv[1]);


    return 0;
}