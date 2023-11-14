#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>

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


int main(int argc, char *argv[])
{
	BITMAP_HEADER h;

	if (argc != 2)
	{
		handleError("Programul trebui apelat cu un parametru ce reprezinta o poza in format bmp");
	}

	int input_fd = open(argv[1], O_RDONLY);
	if (input_fd == -1)
	{
		handleError("Fisierul sursa nu s-a putut deschide");
	}

	if (read(input_fd, &h, sizeof(h)) != sizeof(BITMAP_HEADER))
	{
		handleError("Eroare la citirea antetului");
	}

	printf("%c%c \n",h.signature[0],h.signature[1]);

	int stats_fd = open("statistica.txt", O_CREAT | O_WRONLY | O_APPEND, S_IRWXU | S_IRWXG | S_IRWXO);


	if (stats_fd < 0)
	{
		handleError("Eroare la accesarea fisierului de statistica");
	}

	if (fstat(input_fd, &file_stat) == -1)
	{
		close(input_fd);
		handleError("Eroare la obtinerea informatiilor despre fisier");
	}

	char print[1024];
	sprintf(print, "nume fisier: %s \n", argv[1]);
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
	sprintf(print,"Ultima modificare a lui: %d %d %d\n",timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900 );
	write(stats_fd, print, strlen(print));
	sprintf(print, "contorul de legaturi: %ld \n", file_stat.st_nlink);
	write(stats_fd, print, strlen(print));
	sprintf(print,       "Drepturi de acces user: %c%c%c\n"
                         "Drepturi de acces grup: %c%c%c\n"
                         "Drepturi de acces altii: %c%c%c\n",
           file_stat.st_mode & S_IRUSR ? 'R' : '-', file_stat.st_mode & S_IWUSR ? 'W' : '-', file_stat.st_mode & S_IXUSR ? 'X' : '-',
           file_stat.st_mode & S_IRGRP ? 'R' : '-', file_stat.st_mode & S_IWGRP ? 'W' : '-', file_stat.st_mode & S_IXGRP ? 'X' : '-',
           file_stat.st_mode & S_IROTH ? 'R' : '-', file_stat.st_mode & S_IWOTH ? 'W' : '-', file_stat.st_mode & S_IXOTH ? 'X' : '-');
	write(stats_fd, print, strlen(print));

	printf("Dimensiune: %ld\n", file_stat.st_size);
	printf("Identificatorul utilizatorului: %u\n", file_stat.st_uid);


	close(input_fd);
	close(stats_fd);

	return 0;
}