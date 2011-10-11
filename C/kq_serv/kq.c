#include <aio.h>
#include <netinet/in.h>
#include <pthread.h>
#include <errno.h> //CHECK
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

/* Constants */
#define BUFF_SIZE 4096
#define SERVER_PORT 1340
#define UN_SOCK_PATH "un_path"
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
} interest;

typedef struct {
	char *buf;
	size_t actual_size;
	size_t alloc_size;
	size_t sent_size;
	ctrl *controller;
} track;

typedef struct {
	unsigned long reference_count;
	size_t buf_size; //can I keep track of received just from this
	void *buf;
} ctrl;

typedef struct {
	track *tracker;
	int file;
	int socket;
	void *buf;
	size_t buf_size;
	int offset;
	int buf_offset;
	int file_size;
} aio_transfer;

/* Global Variables */
int conns = 0;
int unix_conns = 0;

int nchanges = 0;
int nevents = 0;
struct kevent event_list[MAX_EVENTS];
struct kevent change_list[MAX_EVENTS];

interest interest_array[MAX_FD];
ctrl *curr_ctrl;

int listen_sd;
int listen_usd;
//from a design standpoint what of these should be just in a structure that
//is initialized in main() at start and what could be global
//Most are trackers of success for programs
//Some (listen_(u)sd, n(events)changes are necessary for the program
// but may go better in a local sense

/* Functions */
static void
perror_exit(const char *str)
{
	perror(str);
	exit(-1);
}

static void 
free_ctrl(ctrl *controller) 
{
	free(controller->buf);
	free(controller);
}

static void
free_track(track *tracker) 
{
	if (tracker->ctrl->reference_count == 0) free_ctrl(tracker->ctrl);
	free(tracker);
}

static void 
register_interest(int fd, void (*func)(int, void*), void *arg, 
					uint16_t flags, int16_t filter)
{
	EV_SET(&change_list[nchanges], fd, flags, filter, 0, 0, 0);
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
register_write_interest(int fd, void (*func)(int, void *), void *arg)
{
	register_interest(fd, func, arg, (EV_ADD | EV_ONESHOT), EVFILT_WRITE);
}

static void
register_aio_read_interest(int fd, void (*func)(int, void *), void *arg, 
							int offset, int cbfd, void (*cbfunc)(void *), void *cbarg)
{
	aiocb *aioc = malloc(sizeof(aiocb));
	aioc->aio_fildes = fd;     
	aioc->aio_buf = arg;
	aioc->aio_nbytes;
    aioc->aio_offset;     
    aioc->aio_reqprio; //?

    aioc->aio_lio_opcode;
	aioc->aio_flags;
    
    aioc->aio_sigevent = malloc(sizeof(struct sigevent));  
    aioc->aio_sigevent->notifyinfo.nifunc = cbfunc;
    aioc->aio_sigevent->notifyinfo.nisigno;
    aioc->aio_sigevent->signo;
	aioc->aio_sigevent->sigev_notify = SIGEV_CALLBACK;
    aioc->aio_sigevent->sigev_notifyinfo;
    aioc->aio_sigevent->sigv
    
	aio_read(aioc);
}

static int
interpret_buf(char *read_buf)
{
	char *cmp = "\r\n\r\n";
	for(line = strtok_r(read_buf, "\n", &brkt); line; 
				line = strtok_r(NULL, "\n", &brkt)) {
				if (0 == strcmp(line, cmp))
					return 1;
	}
}

static void
read_unix_conn(int fd, void *arg)
{
	ctrl *controller = (ctrl *) arg;
	int n;
	while (0 < (n = recv(fd, controller->buf+controller->buf_size, 
							(controller->actual_size%BUFF_SIZE), 0))) {
		controller->actual_size+=n;
		if (controller->actual_size >= controller->alloc_size) {
			controller->alloc_size+=BUFF_SIZE;
			void *temp = realloc(controller->buf, controller->alloc_size);
			if (temp == NULL)
				perror_exit("REALLOC BUG");
			controller->buf = temp;
		}
	}  
	if (0 == n) perror_exit("UNIX peer dc");
	else if (EAGAIN == errno) {
		register_read_interest(fd, read_conn_helper, arg); //........unix?
	} else if (done) { //how is it done? same as interpret?
		close(fd);
		curr_ctrl = controller;
	} else perror_exit("UNIX recv() failed");
}

static void 
accept_unix_conn(int fd, void *arg)
{
	int accept_usd;
	if (0 > (accept_usd = accept(fd, NULL, NULL)) && EAGAIN != errno)
		perror_exit("UNIX accept() failed");
	else if (0 < accept_usd) {
		register_read_interest(accept_usd, read_unix_conn, arg);
		unix_conns++;
	}
	ctrl *controller = malloc(sizeof(ctrl));
	arg = (void *) controller;
	register_read_interest(fd, accept_unix_conn, arg);
}

static void
write_conn_post_aio(int fd, void *arg)
{

}

static void
aio_read_buf(int fd, void *arg)
{
	if (offset + nbytes == file_size) {
		close(file);
	} else if (nbytes? + (offset % BUFFSIZE) == BUFF_SIZE) {
		register_write_interest(); //post_aio
	} else if (nbytes + (offset % BUFFSIZE) < BUFF_SIZE) {
		offset += nbytes;
		register_aio_read_interest();
	} else perror_exit("aio_read_buf() failed");
}
	
static void 
write_conn_pre_aio_helper(int fd, void *arg)
{
	aio_transfer *trans = (aio_transfer *)arg;
}

static void
write_conn_pre_aio(int fd, void *arg)
{
	aio_transfer *trans = malloc(sizeof(aio_transfer));
	trans->tracker = (void *)arg;
	if (0 > (trans->file = open(getUri(tracker->buf), O_NONBLOCK))
		perror_exit("open() failed");
	trans->socket = fd;
	trans->buf = malloc(BUFF_SIZE);
	trans->buf_size = BUFF_SIZE;
	trans->offset = 0;
	trans->buf_offset = 0;
	trans->file_size = 0; //NEED SYSTEM CALL FOR METADATA
	register_write_interest(fd, write_conn_pre_aio_helper, (void *) trans);
}

static void
write_conn_via_unix(int fd, void *arg)
{
	int n;
	track *tracker = (void *)arg;
	if ((0 >= (n = send(fd, tracker->controller->buf + tracker->sent_size, 
						tracker->actual_size - tracker->sent_size, 0))) 
		&& (EAGAIN == errno)) {
		tracker->sent_size += n;
		register_write_interest(fd, write_conn, arg);
	} else if ((0 >= n) && (EAGAIN != errno)) perror_exit("send() failed");
	else if (tracker->actual_size == tracker->sent_size) {
		shutdown(fd, SHUT_RDWR);
		close(fd);
		tracker->controller->reference_count--;
		free_track(tracker);
	} else perror_exit("Write_conn() odd");
}

static void 
read_conn_helper(int fd, void *arg) 
{
	int n;
	track *tracker = (track *) arg;
	while (0 < (n = recv(fd, tracker->buf + tracker->actual_size, 
							tracker->actual_size % BUFF_SIZE, 0))) { /* Remember you changed this */
		tracker->actual_size += n;
		if (tracker->actual_size >= tracker->alloc_size) {
			tracker->alloc_size += BUFF_SIZE;
			void *temp = realloc(tracker->buf, 
									tracker->alloc_size);
			if (temp == NULL)
				perror_exit("REALLOC BUG");
			tracker->buf = (char *)temp;
		}
	} 
	if (0 == n)  
		perror_exit("peer dc hmm");
	else if (EAGAIN == errno)
		register_read_interest(fd, read_conn_helper, arg);
	else if (1 == interpret_buf(tracker->buf))  //unix
		register_write_interest(fd, write_conn_via_unix, arg);
	else if (2 == interpret_buf(tracker->buf)) {
		//if 2 == interpret_buf reduce from /abc/efg.hij to efg.hij
		register_write_interest(fd, write_conn_pre_aio, arg);
	} else perror_exit("recv() failed");
}

static void 
read_conn(int fd, void *arg)
{
	track *tracker = malloc(sizeof(track));
	tracker->buf = malloc(BUFF_SIZE);
	tracker->actual_size = 0;
	tracker->alloc_size = BUFF_SIZE;
	tracker->sent_size = 0;
	tracker->ctrl = curr_ctrl;//elsewhere
	ctrl->reference_count++;//need to be done elsewhere
	arg = (void *) tracker;
	register_read_interest(fd, read_conn_helper, arg);
}

static void
accept_conn(int fd, void *arg) 
{
	int accept_sd;
	if (0 > (accept_sd = accept(fd, NULL, NULL)) && EAGAIN != errno)
		perror_exit("UNIX accept() failed");
	else if (0 < accept_sd) {
		register_read_interest(accept_sd, read_conn, arg);
		conns++;	
	}
	register_read_interest(fd, accept_conn, arg);
}

static void  
event_loop(void)
{
	int kernel_queue;
	struct kevent kernel_event;
	struct timespec time = {0, 0};
	int n;		
	if (0 > (kernel_queue = kqueue())) 
		perror_exit("kqueue() failed");	
	void *arg = NULL;
	register_read_interest(listen_usd, accept_unix_conn, arg);
	register_read_interest(listen_sd, accept_conn, arg);
	while (1) {
		struct kevent *event;
		if (0 > (n = kevent(kernel_queue, change_list, nchanges, 
										event_list, MAX_EVENTS, &time)))
			perror_exit("kevent() failed");
		else {
		    nchanges = 0;
			for (event = event_list; event < event_list+n; event++) {
				if (EVFILT_READ == event->filter) {
					(interest_array[event->ident]).func(event->ident, 
														event->arg);
				} else if (EVFILT_WRITE == event->filter) {
					(interest_array[event->ident]).func(event->ident, 
														event->arg);
				} else if (EVFILT_AIO == event->filter) {
					(interest_array[event->ident]).func(event->ident, 
														event->arg); 
					//does this work? shouldn't it be the callback?
				} else perror_exit("This is an odd event\n");
			}
		}
	}
	close(listen_sd);
	close(listen_usd);
	close(kernel_queue);
}

static void 
unix_serv(void)
{
	int	accept_sd;
	int alloc_size;
	int n;
	int	bool = 1;
	int	bool_1 = 1;
	struct sockaddr_un local; 
	struct sockaddr_un remote;
	alloc_size = BUFF_SIZE;
	buf = malloc(BUFF_SIZE);
	actual_size = 0;
	if (0 > (listen_usd = socket(AF_UNIX, SOCK_STREAM, 0)))
		perror_exit("UNIX socket() failed");
	memset(&local, 0, sizeof(local));
	local.sun_family = AF_UNIX;
	local.sun_len = strlen(UN_SOCK_PATH);
	memcpy(local.sun_path, UN_SOCK_PATH, local.sun_len);
	unlink(local.sun_path);
	if (0 > bind(listen_usd, (struct sockaddr *)&local, sizeof(local)))
		perror_exit("UNIX bind() failed");	
	if (0 > listen(listen_usd, 5))
		perror_exit("UNIX listen() failed");
	while (0 < (accept_sd = accept(listen_usd, (struct sockaddr *)&remote, &n))) {
		while (0 < (n = recv(accept_sd, buf+actual_size, BUFF_SIZE-actual_size, 0))) {
			actual_size+=n;
			if (actual_size >= alloc_size) {
				alloc_size+=BUFF_SIZE;
				void *temp = realloc(buf, alloc_size);
				if (temp == NULL)
					perror_exit("REALLOC BUG");
				buf = temp;
			}
		} if (0 > n) perror_exit("UNIX recv() failed");
		close(accept_sd);
	} if (0 > accept_sd) perror_exit("UNIX accept() failed");
	printf("Received First buffer");
	fcntl(listen_usd, F_SETFL, fcntl(listen_usd, F_GETFL, 0) | O_NONBLOCK);	
}

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
	if(0 > listen(listen_sd, 128))
		perror_exit("listen() failed");
	printf("The server is ready\n");
}

/* Main */
int
main(void)
{
	unix_serv();
	inet_serv();
	event_loop();
	return 0;
}
