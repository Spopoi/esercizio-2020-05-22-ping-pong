/*
 * PingPongPipe.c
 *
 *  Created on: 28 mag 2020
 *      Author: davide
 *
 *      https://github.com/marcotessarotto/esercizio-2020-05-22-ping-pong
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/mman.h>

#define CHECK_ERR_MMAP(a,msg) {if ((a) == MAP_FAILED) { perror((msg)); exit(EXIT_FAILURE); } }

#define MAX_VALUE 1000000

int parent_pipe_fd[2];
int child_pipe_fd[2];
int buf; //buf utilizzato da entrambi i processi

void child_process() {

	close(parent_pipe_fd[1]); // chiudiamo l'estremità di scrittura della pipe, non ci serve
	int byteRead;

	while (1) {

		byteRead = read(parent_pipe_fd[0], &buf, sizeof(int));
		if (byteRead == -1) {
			perror("errore in read");
			exit(EXIT_FAILURE);
		}
		if (buf > MAX_VALUE - 1) {
			printf("Finished! value = %d\n", buf);
			write(child_pipe_fd[1], &buf, sizeof(int));
			exit(EXIT_SUCCESS);
		}
		buf++;
		write(child_pipe_fd[1], &buf, sizeof(int));

	}

}

int main(int argc, char *argv[]) {

	if (pipe(parent_pipe_fd) == -1) {
		perror("problema con pipe");

		exit(EXIT_FAILURE);
	}

	if (pipe(child_pipe_fd) == -1) {
		perror("problema con pipe");

		exit(EXIT_FAILURE);
	}

	switch (fork()) {
	case -1:
		perror("problema con fork");

		exit(EXIT_FAILURE);

	case 0: // processo FIGLIO: leggerà dalla PIPE

		child_process();
	}

	close(child_pipe_fd[1]);

	write(parent_pipe_fd[1], &buf, sizeof(int));

	int byteRead;

	while (1) {

		byteRead = read(child_pipe_fd[0], &buf, sizeof(int));
		if (byteRead == -1) {
			perror("errore in read");
			exit(EXIT_FAILURE);
		}
		if (buf > MAX_VALUE - 1) {
			printf("Finished! value = %d\n", buf);
			write(parent_pipe_fd[1], &buf, sizeof(int));
			exit(EXIT_SUCCESS);
		}
		buf++;
		write(parent_pipe_fd[1], &buf, sizeof(int));
	}
}
