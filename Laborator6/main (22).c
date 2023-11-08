#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>


int low_count = 0;
int upp_count = 0;
int digit_count = 0;
int ch_count = 0;    
struct stat file_stat;


int main(int argc, char *argv[]) {
	if (argc != 4) {
		printf("Apel incorect");
		return -1;
	}

	char ch = argv[3][0];

	int input_fd = open(argv[1], O_RDONLY);
	if (input_fd == -1) {
		printf("Nu s-a putut deschide fisierul de intrare");
		return -1;
	}


	int stat_fd = open(argv[2], O_WRONLY);
	if (stat_fd == -1) {
		printf("Nu s-a putut deschide fisierul de statistica");
		close(input_fd);
		return -1;
	}


	if (fstat(input_fd, &file_stat) == -1) {
		printf("Eroare la obținerea informațiilor despre fisier");
		close(input_fd);
		close(stat_fd);
		return -1;
	}

	char buffer[1024];
	ssize_t bytes_read;

	while ((bytes_read = read(input_fd, buffer, sizeof(buffer))) > 0) 
	{
		//printf("%s\n", buffer);
		for (ssize_t i = 0; i < bytes_read; i++) 
		{
			char current_char = buffer[i];

			if (97 <= current_char && current_char <= 122) 
			{
				low_count++;
			} else if (65 <= current_char && current_char <= 90) 
			{
				upp_count++;
			} else if (48 <= current_char && current_char <= 57) 
			{
				digit_count++;
			}

			if (current_char == ch) {
				ch_count++;
			}
		}
	}


	char print[255];
	
	snprintf(print, sizeof(print), "numar litere mici: %d\n", low_count);
	write(stat_fd, print, strlen(print));
	snprintf(print, sizeof(print), "numar litere mari: %d\n", upp_count);
	write(stat_fd, print, strlen(print));
	snprintf(print, sizeof(print), "numar cifre: %d\n", digit_count);
	write(stat_fd, print, strlen(print));
	snprintf(print, sizeof(print), "numar aparitii caracter: %d\n", ch_count);
	write(stat_fd, print, strlen(print));
	snprintf(print, sizeof(print), "dimensiune fisier: %lld\n", (long long)file_stat.st_size);
	write(stat_fd, print, strlen(print));


	close(input_fd);
	close(stat_fd);

	return 0;
}

