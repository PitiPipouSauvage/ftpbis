#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "./dynamic.h"

#define BUFF_SIZE 1024


void read_file(int fd, char* s, size_t size) {
	int i = 0;
	int nbread; 
	
	if (lseek(fd, 0, SEEK_END) > size) {
		printf("Buffer overflow !\n");
	}
	lseek(fd, 0, SEEK_SET);
	do {
		nbread = read(fd, s + (i * BUFF_SIZE), BUFF_SIZE);
		i ++;
	} while (nbread > 0);
}


void list_files_recursive(const char* base_path, struct dynamic_string_array* list) {
    struct dirent *entry;
    struct stat info;
    char* path = malloc(BUFF_SIZE);

    if (stat(base_path, &info) != 0) {
        perror("stat");
        return;
    }

    if (S_ISREG(info.st_mode)) {
        array_add(list, (char *)base_path);
        return;
    }

    
    if (S_ISDIR(info.st_mode)) {
        DIR *dir = opendir(base_path);
        if (!dir) {
            perror("opendir");
            return;
        }

        size_t base_path_len = strlen(base_path);
        size_t path_len = base_path_len + BUFF_SIZE; 
        path = malloc(path_len);

        while ((entry = readdir(dir)) != NULL) {
            // Ignorer "." et ".."
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;

            snprintf(path, path_len, "%s/%s", base_path, entry->d_name);
            list_files_recursive(path, list);
        }

        closedir(dir);
        free(path);
    }
}


/*
argv[*] => files link
argv[argc - 1] => server ip

*/

int main(int argc, char* argv[]) {
	struct stat buf;

	struct dynamic_string_array list;

	list.list = malloc(sizeof(char*) * 1);
	list.size = 0;
	list.r_size = 1;

	for (int i = 1; i < argc - 1; i ++) { 
		list_files_recursive(argv[i], &list);
	}
	/*
	read_file(fd, buffer + 64, buf.st_size);
	close(fd);
	*/
	// printl(&list);

	int fd;
	struct sockaddr_in s;
	s.sin_family = AF_INET;
	s.sin_addr.s_addr = inet_addr(argv[argc - 1]);
	s.sin_port = htons(8080);

	char** file_contents = malloc(sizeof(char*) * list.size);
	char* buffer;

	for (int i = 0; i < list.size - 1; i ++) {
		stat(get(list, i), &buf);

		buffer = malloc(buf.st_size + BUFF_SIZE);
		fd = open(get(list, i), O_RDONLY);
		read_file(fd, buffer + BUFF_SIZE, buf.st_size + BUFF_SIZE);

		int sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd < 0) {
			perror("sockfd");
			exit(1);
		}
		int connection = connect(sockfd, (struct sockaddr *)&s, sizeof(s));
		if (connection < 0) {
			perror("sockfd");
			exit(2);
		}

		sprintf(buffer, "%s\n%s", get(list, i), buffer + BUFF_SIZE);
		send(sockfd, buffer, buf.st_size + BUFF_SIZE, 0);
		if (send < 0) {
			perror("sockfd");
			exit(3);
		}
		close(sockfd);
		close(fd);
	}
	free(buffer);
}














