#include <sys/types.h>
#include <sys/event.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/fcntl.h>
#include <sys/stat.h>

#include <arpa/inet.h>
#include <netinet/in.h>

#include <aio.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "slab.h"

#ifdef DEBUGP_ON
#define DEBUGP(x, y...) \
    printf("(%s@%d:%s) " x "\n", __FILE__, __LINE__, __func__, ##y)
#else
#define DEBUGP(x...)
#endif /*DEBUGP_ON*/

#define FREE(__p) \
    do { \
        if (NULL != __p) { \
            free(__p); \
            __p = NULL; \
        } \
    } while (0)

/*
 * Event Management Interface
 */

#define INIT_EVENT_NUM 5000
#define FILE_EVENT_INIT_SIZE 1000

enum event_type {
    SOCKET_EVENT,
    FILE_EVENT
};

union evhandler {
    int (*socket_handler) (int, void *);
    int (*file_handler) (int, int, void *);
};

struct event_registry {
    enum event_type type;
    union evhandler handler;
    void *arg;
};

struct event_registry *event_map;
struct slab_cache *file_event_cachep;
int event_num = INIT_EVENT_NUM;

int init_event_manager();
void close_event_manager();
int start_event_manager();
// struct event_registry *next_event();

int regist_socket_read(int evid, int (*handler)(int, void *), void *read_arg);
int regist_socket_write(int evid, int (*handler)(int, void *), void *write_arg);
int regist_file_read(int fd, char *buf, int nbyte, int offset,  int (*handler)(int, int, void *), void *arg);

/*
 * KQueue Event Manageement Specifics
 */

struct kevent *chlist;
struct kevent *evlist;
int kq, nch;

/*
 * Services Provided
 */

/* Contol Service */
#define UNIXSOCK "cntl_unsock"
#define REFUSE_PROMPT "The server cannot accept new response content now.\n"

int init_cntl_service();
int cntlsvc_handler(int fd, void *ignored);

/* HTTP Service */
#define HTTPPORT 8080
#define QCONNMAX 32
#define REQUESTBUF_INITSIZE 1024
#define CHUNK_SIZE 1024

enum http_method {
    HTTP_GET,
    HTTP_POST,
    HTTP_PUT,
    HTTP_DELETE
};

struct request_data {
    enum http_method method;
    int len;
    int size;
    char *buf;
};

struct response_data {
    char *content_buf;
    int offset;
    
    int cfd;
    int fd;
    int is_chunked;
    int header_offset;
    int chunk_len;
    int chunk_offset;
    int size;
};

struct sockaddr_in remote_sain;
int sain_size = sizeof(remote_sain);
char *current_content_buf;
char *content_buf1;
char *content_buf2;
int prev_content_ref_count;
int curr_content_ref_count;

struct slab_cache *req_cachep;

int init_http_service();
void close_http_service();
int httpsvc_accept(int sfd, void *ignored);
int httpsvc_recv(int cfd, void *req_data);
int httpsvc_file_read(int fd, int nbytes, void *rsp_data);
int httpsvc_chunk_send(int fd, void *rsp_data);
int httpsvc_send(int cfd, void *rsp_data);

void dtor_reqdata(void *rdp, struct slab_cache *cachep, unsigned long flags);
void ctor_reqdata(void *rdp, struct slab_cache *cachep, unsigned long flags);

/*
 * HTTP Service Implementation
 */

void
ctor_reqdata(void *rdp, struct slab_cache *cachep, unsigned long flags)
{
    struct request_data *data = rdp;
    data->len = 0;
    data->size = REQUESTBUF_INITSIZE;
    data->buf = malloc(sizeof(char) * data->size);
    if (NULL == data->buf) {
        perror("ctor_reqdata: malloc");
        exit(1);
    }
}

void
dtor_reqdata(void *rdp, struct slab_cache *cachep, unsigned long flags)
{
    struct request_data *data = rdp;
    FREE(data->buf);
}

int
init_http_service()
{
    int sfd, on = 1;
    struct sockaddr_in local_sain;

    if ((sfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("server: socket");
        return -1;
    }

    if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on)) < 0) {
        perror("server: setsockopt");
        close(sfd);
        return -1;
    }
   
    int flags = fcntl(sfd, F_GETFL);
    if (-1 == flags) {
        perror("server: fcntl get");
        return -1;
    }
    if (-1 == fcntl(sfd, F_SETFL, flags | O_NONBLOCK)) {
        perror("server: fcntl set");
        return -1;
    }

    local_sain.sin_family = AF_INET;
    local_sain.sin_port = htons(HTTPPORT);
    local_sain.sin_addr.s_addr = INADDR_ANY;
    
    if (bind(sfd, (struct sockaddr *)&local_sain, sizeof(local_sain)) < 0) {
        perror("server: bind");
        return -1;
    }
    
    if (listen(sfd, QCONNMAX) < 0) {
        perror("server: listen");
        return -1;
    }

    req_cachep = scache_create(sizeof(struct request_data), 0, 0, &ctor_reqdata, &dtor_reqdata);
    if (NULL == req_cachep) {
        perror("server: slab_create");
        exit(1);
    }

    current_content_buf = malloc(256);
    if (NULL == current_content_buf) {
        perror("server: malloc content buf");
        exit(1);
    }
    strcpy(current_content_buf, "Test Content");
    
    atexit(&close_http_service);
    // DEBUGP("listen on: %d", sfd);
    regist_socket_read(sfd, &httpsvc_accept, NULL);

    return 0;
}

void
close_http_service()
{
    scache_destroy(req_cachep);
}

int
httpsvc_accept(int sfd, void *ignored)
{
    // DEBUGP("httpsvc_accept: %d", sfd);
    for(;;) {
        // DEBUGP("new conns coming, %d", num_conn);                       
        int cfd = accept(sfd, (struct sockaddr *)&remote_sain, (unsigned int *)&sain_size);
        if(cfd < 0) {
            if(errno != EWOULDBLOCK) {
                perror("server: accept");
                exit(1);
            }
            break;
        }  

        // DEBUGP("new conn: %d", cfd);
        regist_socket_read(cfd, &httpsvc_recv, NULL);
    }

    regist_socket_read(sfd, &httpsvc_accept, NULL);
    
    return 0;
}

int
httpsvc_recv(int cfd, void *req_data)
{
    // DEBUGP("httpsvc_recv: %d", cfd);
    if (NULL == req_data) {
        req_data = scache_alloc(req_cachep, 0);
        if (NULL == req_data) {
            perror("server: slab alloc");
            exit(1);
        }
    }
    struct request_data *data = req_data;

    int rc;
    int remain_size = data->size - 1;
    // DEBUGP("%d", remain_size);
    while ((rc = read(cfd, data->buf + data->len, remain_size - data->len)) > 0) {
        data->len += rc;

        if (data->len == remain_size) {
            char *temp_buf = realloc(data->buf, data->size * 2);
            if (NULL == temp_buf) {
                perror("server: realloc recv_buf");
                exit(1);
            }
            data->size *= 2;
            data->buf = temp_buf;
            remain_size = data->size - 1;
        }
    }
    data->buf[data->len] = '\0';
    // DEBUGP("read: %d\n%s", offset, recv_buf);

    int req_start = 0;
    int pos = 0;
    int breakline = 0;
    char *path;
    int path_len = 0;
    int req_valid = 0;
    for (; pos < data->len; pos++) {
        
        if (pos == req_start) {
            // start parse a new request
            if (0 == strncmp("GET", data->buf, 3)) {
                data->method = HTTP_GET;
                
                // Parse file path
                if (data->buf[3] != ' ' || '/' != data->buf[4]) {
                    DEBUGP("Invalid Path requested");
                    req_valid = 0;
                    continue;
                } else {
                    req_valid = 1;
                    path = data->buf + 4;
                    // do not get if path == '/'
                    if (' ' == *(path + 1)) {
                        path_len = 1;
                    } else {
                        path_len = 0;
                        while((path_len + 4) < data->len && 
                                path[path_len] != ' ' && 
                                path[path_len] != '\r' && 
                                path[path_len] != '\n' && 
                                path[path_len] != '\0') {
                            path_len++;
                        }
                    }
                }
            } else if (0 == strncmp("POST", data->buf, 4)) {
                data->method = HTTP_POST;
            }
        }

        if ('\n' == data->buf[pos]) {
            if (breakline) {
                // find \n\n, complete request
                if (req_valid && HTTP_GET == data->method) {
                    if (1 == path_len) {
                        
                        /* do normal simple response */
                        DEBUGP("do socket read/write");
                
                        scache_free(req_cachep, req_data);
                        regist_socket_write(cfd, &httpsvc_send, NULL);
                        return 0;
                    } else {
                        DEBUGP("do file read/write");
                        
                        /* prepare file read */
                        char file_path[path_len];
                        strncpy(file_path, path + 1, path_len - 1);
                        file_path[path_len - 1] = '\0';

                        DEBUGP("open file: %s", file_path);
                        int fd = open(file_path, O_RDONLY);
                        struct stat fd_stat;
                        if (fd < 0) {
                            DEBUGP("Cannot open file: %s", file_path);
                            scache_free(req_cachep, req_data);
                            close(cfd);
                            return -1;
                        }
                        if (fstat(fd, &fd_stat) < 0) {
                            DEBUGP("Error on fstat: %s", file_path);
                            scache_free(req_cachep, req_data);
                            close(cfd);
                            close(fd);
                            return -1;
                        }

                        struct response_data *rsp_data = malloc(sizeof(struct response_data));
                        if (NULL == rsp_data) {
                            perror("server: malloc response data");
                            exit(1);
                        }
                        rsp_data->content_buf = malloc(CHUNK_SIZE);
                        if (NULL == rsp_data->content_buf) {
                            perror("server: malloc response buffer");
                            exit(1);
                        }
                        //HHHHHHEHHERHEHRHERHEHREHRHEHREHHHHEEEEEERRRRREE HERE
                        rsp_data->is_chunked = 1;
                        rsp_data->cfd = cfd;
                        rsp_data->offset = 0;
                        rsp_data->header_offset = 0;
                        rsp_data->chunk_len = 0;
                        rsp_data->chunk_offset = 0;
                        rsp_data->fd = fd;
                        rsp_data->size = fd_stat.st_size;

                        scache_free(req_cachep, req_data);
                        if (regist_file_read(rsp_data->fd, rsp_data->content_buf, CHUNK_SIZE, 0, &httpsvc_file_read, rsp_data) < 0) {
                            FREE(rsp_data->content_buf);
                            FREE(rsp_data);
                            close(fd);
                            close(cfd);

                            return -1;
                        }
                        return 0;
                    }
                } else if (HTTP_POST == data->method) {
                    DEBUGP("Unsupported method");
                    scache_free(req_cachep, req_data);
                    close(cfd);
                    return 0;
                }
                
                // could go on with next request
                req_start = pos + 1;
                break;
            } else {
                breakline = 1;
            }
        } else if (data->buf[pos] != '\r') {
            breakline = 0;
        }
    }

    if (pos == data->len && rc < 0 && EAGAIN == errno) {
        DEBUGP("EAGAIN");
        regist_socket_read(cfd, &httpsvc_recv, req_data);
        return 0;
    }

    scache_free(req_cachep, req_data);
    close(cfd);
    return -1;
}

int
httpsvc_file_read(int fd, int nbytes, void *rsp_data) 
{
    if (nbytes < 0) {
        perror("server: file read; internal error");
        exit(1);
    }

    DEBUGP("file %d, read chunk of %d bytes", fd, nbytes);
    struct response_data *data = rsp_data;
    data->chunk_len = nbytes;
    data->chunk_offset = 0;
    regist_socket_write(data->cfd, &httpsvc_chunk_send, data);
    
    return 0;
}

int
httpsvc_chunk_send(int cfd, void *rsp_data) 
{
    struct response_data *data = rsp_data;
    int ret;
    
    if (0 == data->offset) {
        char buf[2048];
        time_t t = time(NULL);
        struct tm *gmt = gmtime(&t);
        const int response_length = snprintf(buf, sizeof(buf), 
            "HTTP/1.1 200 OK\r\nDate: %sServer: KQ Server\r\nContent-Length: %d\r\nContent-Type: text/html; charset=ISO-8859-1\r\nConnection: Keep-Alive\r\n\r\n", asctime(gmt), data->size);
        if (response_length > sizeof(buf)) {
            DEBUGP("content is too long!");
            exit(1);
        }
        // DEBUGP("write:\n%s", buf);

        while ((ret = write(cfd, buf + data->header_offset, response_length - data->header_offset)) > 0) {
            // printf("%d    %d\n", ret, len);
            data->header_offset += ret;
        }
    
        if (response_length != data->header_offset && ret < 0 && errno == EAGAIN) {
            // incomplete send header
            regist_socket_write(cfd, &httpsvc_chunk_send, data);
            return data->header_offset;
        }
    }
    
    /* send file chunk */
    char *chunk = data->content_buf;
    int prev_offset = data->offset - data->chunk_offset;
    while((ret = write(cfd, chunk + data->chunk_offset, data->chunk_len - data->chunk_offset)) > 0) {
        data->chunk_offset += ret;
    }

    data->offset = prev_offset + data->chunk_offset;
    if (data->chunk_len == data->chunk_offset) {
        // complete send chunk
        if (data->size == data->offset) {
            // finish send whole file
            DEBUGP("done with file read fd: %d", data->fd);
            close(data->fd);
            FREE(data->content_buf);
            FREE(data);
            close(cfd);
            return 0;
        } else {
            // aio_read next chunk
            DEBUGP("regist read next chunk, fd: %d, offset: %d", data->fd, data->offset);
            if (regist_file_read(data->fd, data->content_buf, CHUNK_SIZE, data->offset, &httpsvc_file_read, rsp_data) < 0) {
                close(data->fd);
                FREE(data->content_buf);
                FREE(data);
                close(cfd);
                return -1;
            }

            return 0;
        }
    } else if (ret < 0 && errno == EAGAIN) {
        regist_socket_write(cfd, &httpsvc_chunk_send, data);
        return data->chunk_offset;
    }

    close(data->fd);
    FREE(data->content_buf);
    FREE(data);
    close(cfd);
    return -1;
}


/*
 * Control Service Implementation
 */

int
init_cntl_service()
{
    int sfd;
    struct sockaddr_un saun;

    if ((sfd = socket(PF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("unix socket");
        return -1;
    }

    int flags = fcntl(sfd, F_GETFL);
    if (-1 == flags) {
        perror("server: fcntl get");
        return -1;
    }
    if(-1 == fcntl(sfd, F_SETFL, flags | O_NONBLOCK)) {
        perror("server: fcntl set");
        return -1;
    }

    saun.sun_family = AF_UNIX;
    strcpy(saun.sun_path, UNIXSOCK);
    saun.sun_len = strlen(saun.sun_path);
    unlink(saun.sun_path);

    if (bind(sfd, (struct sockaddr *)&saun, sizeof(saun)) < 0) {
        perror("unix socket: bind");
        return -1;
    }

    if (listen(sfd, 5) < 0) {
        perror("unix socket: listen");
        return -1;
    }

    regist_socket_read(sfd, &cntlsvc_handler, NULL);

    return 0;
}

int
cntlsvc_handler(int sfd, void *ignored)
{
    int cfd;
    if((cfd = accept(sfd, NULL, NULL)) < 0) {
        perror("unix socket: accept");
        exit(1);
    }
    
    /* start handle cntl request */
    int rc;
    if (prev_content_ref_count != 0) {
        // do not accept content
        char buf[256];
        while ((rc = read(cfd, buf, 256)) > 0);
        
        DEBUGP("send: %s", REFUSE_PROMPT);
        if (send(cfd, REFUSE_PROMPT, strlen(REFUSE_PROMPT), 0) < 0) {
            perror("unix socket: send");
            exit(1);
        }
    } else {
        int buf_size = 256;
        char *buf = malloc(buf_size);
        if (NULL == buf) {
            perror("server: malloc content buf");
            exit(1);
        }

        int len = 0;
        int rc;
        while ((rc = read(cfd, buf, 256)) > 0) {
            len += rc;
            if (len == buf_size) {
                buf_size *= 2;
                char *tmp_buf = realloc(buf, buf_size);
                if (NULL == tmp_buf) {
                    perror("server: realloc content buf");
                    exit(1);
                }
                buf = tmp_buf;
            }
        }
        buf[len] = '\0';
        DEBUGP("recv: %s", buf);

        prev_content_ref_count = curr_content_ref_count;
        curr_content_ref_count = 0;
        FREE(current_content_buf);
        current_content_buf = buf;
    }
        
    close(cfd);
    regist_socket_read(sfd, &cntlsvc_handler, NULL);
    
    return 0;
}


/*
 * KQueue Event Management Implementation
 */

int
init_event_manager()
{    
    kq = kqueue();
    chlist = (struct kevent *) malloc(sizeof(struct kevent) * INIT_EVENT_NUM);
    evlist = (struct kevent *) malloc(sizeof(struct kevent) * INIT_EVENT_NUM);
    event_map = (struct event_registry *) malloc(sizeof(struct event_registry) * INIT_EVENT_NUM);
    file_event_cachep = scache_create(sizeof(struct event_registry), 0, 0, NULL, NULL);
    if (NULL == file_event_cachep) {
        perror("server: slab_create: event registry");
        exit(1);
    }

    // regist exit function
    atexit(&close_event_manager);

    if (NULL == chlist || NULL == evlist || NULL == event_map) {
        perror("event mgmt: realloc");
        exit(1);
    }

    return kq;
}

void
close_event_manager() {

    close(kq);
    kq  = -1;

    scache_destroy(file_event_cachep);
    FREE(chlist);
    FREE(evlist);
    FREE(event_map);
}

int
increase_eventmap()
{
    void *new_chlist = realloc(chlist, sizeof(struct kevent) * 2 * event_num);
    void *new_evlist = realloc(evlist, sizeof(struct kevent) * 2 * event_num);
    void *new_event_map = realloc(event_map, sizeof(struct event_registry) * 2 * event_num);
    event_num *= 2;

    if(NULL == new_chlist || NULL == new_evlist || NULL == new_event_map) {
        perror("event mgmt: realloc");
        FREE(new_chlist);
        FREE(new_evlist);
        FREE(new_event_map);
        exit(1);
    }   
    
    chlist = new_chlist;
    evlist = new_evlist;
    event_map = new_event_map;

    return 0;
}

int
regist_socket_read(int evid, int (*handler)(int, void *), void *arg)
{       
    // DEBUGP("regist_read: %d", evid);
    if(evid >= event_num) {
        DEBUGP("Increase event map, current size: %d", event_num);
        increase_eventmap();
    }

    event_map[evid].handler.socket_handler = handler;
    event_map[evid].arg = arg;
    event_map[evid].type = SOCKET_EVENT;

    EV_SET(&chlist[nch], evid, EVFILT_READ, EV_ADD | EV_ONESHOT, 0, 0, 0);
    nch++;

    return 0;
}

int
regist_socket_write(int evid, int (*handler)(int, void*), void *arg)
{
    // DEBUGP("regist_write: %d", evid);
    if(evid >= event_num) {
        DEBUGP("Increase event map, current size: %d", event_num);
        increase_eventmap();
    }
  
    event_map[evid].handler.socket_handler = handler;
    event_map[evid].arg = arg;
    event_map[evid].type = SOCKET_EVENT;

    EV_SET(&chlist[nch], evid, EVFILT_WRITE, EV_ADD | EV_ONESHOT, 0, 0, 0);
    nch++;

    return 0;
}

int
regist_file_read(int fd, char *buf, int nbytes, int offset, int (*handler) (int, int, void*), void *arg)
{
    // DEBUGP("regist_file_read: %d", evid);
    if (fd >= event_num) {
        DEBUGP("Increase event map, current size: %d", event_num);
        increase_eventmap();
    }

    struct event_registry *evr = scache_alloc(file_event_cachep, 0);
    evr->handler.file_handler = handler;
    evr->arg = arg;
    evr->type = FILE_EVENT;

    struct aiocb *iocb = malloc(sizeof(struct aiocb));
    if (NULL == iocb) {
        perror("server: malloc aiocb");
        exit(1);
    }
    iocb->aio_buf = buf;
    iocb->aio_nbytes = nbytes;
    iocb->aio_offset = offset;
    iocb->aio_fildes = fd;

    iocb->aio_sigevent.sigev_notify_kqueue = kq;
    iocb->aio_sigevent.sigev_notify = SIGEV_KEVENT;
    iocb->aio_sigevent.sigev_value.sigval_ptr = evr;
                
    return aio_read(iocb); //check errors
}

int
start_event_manager()
{     
    int nev, evidx;
    struct event_registry *evr;
    int ret, nbytes;
    struct aiocb *iocb;

    for (;;) {
        nev = kevent(kq, chlist, nch, evlist, event_num, NULL);
        /*if (nev) {
            DEBUGP("nev: %d; nch: %d", nev, nch);
        }*/

        nch = 0;
        if(nev < 0) {
            // DEBUGP("nev: %d", nev);
            perror("server: kevent");
            exit(1);
        }
        
        if(0 == nev) {
            continue;
        }
        
        evidx = 0;
        for (; evidx < nev; evidx++) {
            if(evlist[evidx].flags & EV_ERROR) {
                // perror("server: EV_ERROR");
                DEBUGP("fd: %d, nev: %d", evlist[evidx].ident, nev);
                exit(1);
            }
            if (EVFILT_AIO == evlist[evidx].filter) {
                // file read or write
                iocb = (struct aiocb *) evlist[evidx].ident;
                evr = iocb->aio_sigevent.sigev_value.sigval_ptr;

                nbytes = aio_return(iocb);
                ret = evr->handler.file_handler(iocb->aio_fildes, nbytes, evr->arg);
                scache_free(file_event_cachep, evr);
                FREE(iocb);
            } else {
                // socket read or write
                int evid = evlist[evidx].ident;
                evr = event_map + evid;
                ret = evr->handler.socket_handler(evid, evr->arg);
            }

            if (ret < 0) {
                DEBUGP("Error on executing handler for evid: %d", evlist[evidx].ident);
                continue;
            }
        }
    }

    return 0;
}

int
main(int argc, char *argv[])
{    
    if(init_event_manager() < 0) {
        perror("server: init_event_mgmt");
        exit(1);
    }
    
    if(init_cntl_service() < 0) {
        perror("server: init_cntl_svc");
        exit(1);
    }

    if(init_http_service() < 0) {
        perror("server: init_http_svc");
        exit(1);
    }

    printf("Starting HTTP Sever on port 8080 and Control Server...\n");
    
    int ret = start_event_manager();

    close_event_manager();
    return ret;
}
