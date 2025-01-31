#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <libgen.h>

#define BUFF_SIZE 1024


int main() {
	char* buffer = malloc(sizeof(char) * 800 * BUFF_SIZE);

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in s;
	s.sin_family = AF_INET;
	s.sin_addr.s_addr = inet_addr("127.0.0.1");
	s.sin_port = htons(8080);

	unsigned int s_size = sizeof(s);

	bind(sockfd, (struct sockaddr *)&s, s_size);
	listen(sockfd, 5);
	printf("[NETWORK] Listening on port 8080\n");

	int csockfd;
	int nbread;
	char* file_name = malloc(sizeof(char) * 1024);
	int fd;

	while (1) {
		csockfd = accept(sockfd, (struct sockaddr *)&s, &s_size);
		printf("[NETWORK] Connection established\n");
		nbread = read(csockfd, buffer, 800 * BUFF_SIZE);
		for (int i = 0; i < 1024 && buffer[i] != '\n'; i++) {
			file_name[i] = buffer[i];
		}
		printf("[NETWORK] File %s received (%d bytes)\n", basename(file_name), nbread);
		int done;
		do {
			done = 1;
			fd = open(file_name, O_WRONLY | O_CREAT | O_TRUNC);
			if (errno == ENOENT) {
				printf("[SYSTEM] Creating folder...\n");
				char* folder_name = dirname(file_name);
				mkdir(folder_name, S_IRWXU | S_IRGRP | S_IWGRP | S_IROTH);
			}
		} while (done != 1);
		write(fd, buffer + 1024, nbread - 1024);
		close(fd);
		printf("[NETWORK] Connection closed\n");
		close(csockfd);

	}

	return 0;
}
