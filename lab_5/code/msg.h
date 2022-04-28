#define MSGBUF_SIZE 68
#define MTEXT_SIZE 64

typedef struct {
    long mtype;
    char mtext[MTEXT_SIZE];
} msgbuf;

#define SUBPROC_CNT 10
#define FILES_CNT 4

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};
