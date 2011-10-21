#include <aio.h>
#include <netinet/in.h>
#include <pthread.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* Misordered thanks to libc */
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <unistd.h>
#include <arpa/inet.h>

/* Constants */
#define BUFF_SIZE 4096
#define SERVER_PORT 1340
#define MAX_EVENTS 5000
#define MAX_FD 5000

/* Macros */
#define DEBUGP(x, y...) \
		printf("%s@%d:%s_LOG_" x "\n", __FILE__, \
		__LINE__, __func__, ##y)

/* Structures */
typedef struct {
	void *arg;
	void (*func)(int, void *);
	void (*write_func)(void *);
} interest;

typedef struct {
	char *buf;
	size_t actual_size;
	size_t alloc_size;
	size_t sent_size;
} track;

typedef struct {
	int file;
	int socket;
	void *buf;
	int buf_offset;
	int file_size; 
	int file_offset; 
	int nbytes;
} aio_transfer;

/* Global Variables */
// tracking
int conns = 0;
//eventing
int kernel_queue;
int nchanges = 0;
int nevents = 0;
struct kevent event_list[MAX_EVENTS];
struct kevent change_list[MAX_EVENTS];
interest interest_array[MAX_FD];
// TCPing
int listen_sd;
int listen_usd;

/* Functions */
static void write_conn_pre_aio_helper(void *arg);

/* MEMORY AND ERROR FUNCTIONS */
static void
perror_exit(const char *str)
{
	perror(str);
	exit(-1);
}

/* REGISTERING INTEREST */
static void 
register_interest(int fd, void (*func)(int, void*), void *arg, 
					uint16_t flags, int16_t filter)
{
	EV_SET(&change_list[nchanges], fd, filter, flags, 0, 0, 0);
	interest_array[fd].func = func;
	interest_array[fd].arg = arg;
	nchanges++;
}

static void 
register_read_interest(int fd, void (*func)(int, void *), void *arg)
{
	register_interest(fd, func, arg, (EV_ADD | EV_ONESHOT), EVFILT_READ);
}	

static void 
register_write_interest(int fd, void (*func)(void *), void *arg)
{
	EV_SET(&change_list[nchanges], fd, EVFILT_WRITE, (EV_ADD | EV_ONESHOT),  
			0, 0, 0);
	interest_array[fd].write_func = func; //?terrible hack
	interest_array[fd].arg = arg;
	nchanges++;
}

static void
register_aio_read_interest(int fd, void *arg, int offset, int cbfd, void (*cbfunc)(void *), 
							void *cbarg, int nbytes) 
{
	struct aiocb *aioc;
	if (NULL == (aioc = malloc(sizeof(struct aiocb)))) 
		perror_exit("aciob malloc");
	aioc->aio_fildes = fd;     
	aioc->aio_buf = arg; //not right
	aioc->aio_nbytes = nbytes; //?this shouldn't be here
    aioc->aio_offset = offset;     
	aioc->aio_sigevent.sigev_value.sigval_ptr = cbarg;
    aioc->aio_sigevent.sigev_notify_kqueue = kernel_queue;
	aioc->aio_sigevent.sigev_notify = SIGEV_KEVENT;	
	aio_read(aioc);
}

/* HTTP LAYER */
static char * 
interpret_buf(char *read_buf)
{
	/* 
	I realize this is 
	1) a terrible implementation. 
	2) not testing for anything other 
	then an expected GET request.
	
	However I lost my buf interpretation code pre-setting up github, 
	so I rewrote this quickly.
	*/
	int bool = 0;
	char *line = NULL;
	char *last_line = NULL;
	char *word = NULL;
	char *last_word = NULL;
	line = strtok_r(read_buf, "\n", &last_line);
	while (line != NULL) {
		word = strtok_r(line, " ", &last_word); 
		while (word != NULL) {
			if (0 == strcmp(word, "GET"))
				bool = 1; 
			if (bool && word[0] == '/') {
				return word + 1;
			}
			word = strtok_r(NULL, " ", &last_word);
		} 
		bool = 0;
		line = strtok_r(NULL, "\n", &last_line);
	} 
	return "UNSUPPORTED METHOD"; 
}

static void
aio_read_buf(void *arg)
{
	aio_transfer *data = (aio_transfer *) arg;
	data->file_offset += data->nbytes; //?here or in event loop
	register_write_interest(data->socket, &write_conn_pre_aio_helper, data);
}
	
static void 
write_conn_pre_aio_helper(void *arg)
{	
	aio_transfer *data = (aio_transfer *)arg;
	int n;		
	if (0 >= (data->file_size - (data->file_offset + data->nbytes))) { //file read
		DEBUGP("FILE READ");
		while (0 > (n = send(data->socket, data->buf+data->buf_offset, data->nbytes - data->buf_offset, 0))) {
			data->buf_offset += n;	
		} //?this is blocking
		close(data->socket);
		close(data->file);
		free(data->buf);
		free(data);
	} else { //chunks
		while (0 > (n = send(data->socket, data->buf+data->buf_offset, data->nbytes - data->buf_offset, 0))) {
			data->buf_offset += n;	
		} //?this is blocking
	    data->file_offset += data->buf_offset;
		register_aio_read_interest(data->file, data->buf, data->file_offset, data->socket, aio_read_buf, 
									data, data->nbytes);
	}	
}

static void 
read_conn(int fd, void *arg) 
{
	int n;
	track *tracker = malloc(sizeof(track));
	tracker->actual_size = 0;
	tracker->alloc_size = BUFF_SIZE;
	tracker->buf = malloc(BUFF_SIZE);
	while (0 < (n = recv(fd, tracker->buf + tracker->actual_size, 
							tracker->alloc_size - tracker->actual_size, 0))) { /* Remember you changed this */
		tracker->actual_size += n;
		if (tracker->actual_size >= tracker->alloc_size) {
			tracker->alloc_size += BUFF_SIZE;
			void *temp = realloc(tracker->buf, tracker->alloc_size);
			if (temp == NULL)
				perror_exit("REALLOC BUG");
			tracker->buf = (char *)temp;
		}
	} //? blocking
	char *temp = interpret_buf(tracker->buf);
	// 	if (0 == n)  
// 		perror_exit("peer dc hmm");
// 	else if (EAGAIN == errno)
// 		register_read_interest(fd, read_conn, arg);
	if (!(0 == strcmp(temp, "UNSUPPORTED METHOD"))) {
		aio_transfer *data = malloc(sizeof(aio_transfer));
		data->socket = fd; 
		data->nbytes = BUFF_SIZE;
		data->buf_offset = 0;
		data->buf = malloc(BUFF_SIZE);
		if (0 > (data->file = open(temp, O_RDONLY)))
			perror_exit("open() file failed");
		struct stat *stats = malloc(sizeof(struct stat));
		if (0 > fstat(data->file, stats))
			perror_exit("stats() failed");
		data->file_size = (int) stats->st_size; //?
		data->file_offset = 0;
		register_aio_read_interest(data->file, data->buf, data->file_offset, data->socket, aio_read_buf, 
									data, data->nbytes);
	} else perror_exit("recv() failed");
}

static void
accept_conn(int fd, void *arg) 
{
	int accept_sd;
	if (0 > (accept_sd = accept(fd, NULL, NULL)) && EAGAIN != errno)
		perror_exit("accept() failed");
	else if (0 < accept_sd) {
		register_read_interest(accept_sd, read_conn, arg);
		conns++;	
	}
	register_read_interest(fd, accept_conn, arg);
}

/* EVENT LAYER */
static void  
event_loop(void)
{
	struct kevent kernel_event;
	struct timespec time = {0, 0};
	int n;		
	if (0 > (kernel_queue = kqueue())) 
		perror_exit("kqueue() failed");	
	void *arg = NULL;
	register_read_interest(listen_sd, accept_conn, arg);
	while (1) {
		struct kevent *event;
		if (0 > (n = kevent(kernel_queue, change_list, nchanges, 
										event_list, MAX_EVENTS, &time)))
			perror_exit("kevent() failed");
		else {
			nchanges = 0;
			for (event = event_list; event < event_list + n; event++) {
				if (EVFILT_READ == event->filter) {
					(interest_array[event->ident]).func(event->ident, 
														event->udata);
				} else if (EVFILT_WRITE == event->filter) {
					(interest_array[event->ident]).write_func(interest_array[event->ident].arg);
				} else if (EVFILT_AIO == event->filter) {
					struct aiocb *aioc = (struct aiocb *) event->ident;
					int nbytes;
					if (0 > (nbytes = aio_return(aioc)))
						perror_exit("aio_return() failed: ");
					aio_read_buf(aioc->aio_sigevent.sigev_value.sigval_ptr); //? poor design choice
					free(aioc); //? will this not free substructs, what I want
				} else if (event->flags & EV_ERROR) {
					perror_exit("EVENTING ERROR");
				}
			}
		}
	}
	close(listen_sd);
	close(listen_usd);
	close(kernel_queue);
}

/* TCP LAYER */
static void 
inet_serv(void)
{
	int n = 1;
	struct 	sockaddr_in in_addr;
	if (0 > (listen_sd = socket(AF_INET, SOCK_STREAM, 0)))
		perror_exit("socket() failed");
	if (0 > setsockopt(listen_sd, SOL_SOCKET, SO_REUSEADDR,
		   			(char *)&n, sizeof(n)))
		perror_exit("setsockopt() failed");
	memset(&in_addr, 0, sizeof(in_addr));
	in_addr.sin_family = AF_INET;
	in_addr.sin_port = htons(SERVER_PORT);
	if (0 > bind(listen_sd, (struct sockaddr *)&in_addr, sizeof(in_addr)))
		perror_exit("bind() failed");
	fcntl(listen_sd, F_SETFL, fcntl(listen_sd, F_GETFL, 0) | O_NONBLOCK);
	if (0 > listen(listen_sd, 128))
		perror_exit("listen() failed");
	printf("The server is ready\n");
}

/* Main */
int
main(void)
{
	inet_serv();
	event_loop();
	return 0;
}
