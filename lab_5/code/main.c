#include "main.h"
#include "subprocess.h"
#include "msg.h"

// * Вариант 9
// Родительский процесс помещает в сегмент разделяемой памяти имена
// программ из предыдущих лабораторных работ, которые могут быть запущены.
// Выполняя некоторые циклы работ, порожденные процессы случайным образом
// выбирают имена программ из таблицы сегмента разделяемой памяти, запускают эти
// программы, и продолжают свою работу. Посредством аппарата семафоров должно
// быть обеспечено, чтобы не были одновременно запущены две одинаковые
// программы. В процессе работы через очередь сообщений родительский процесс
// информируется, какие программы и от имени кого запущены.
// P.S. Вместо программ из предыдущих лабораторных работ, мы сделали программу, которая в качестве аргумента принимает строку, которую затем в консоль как чьё-то имя, а затем ждёт случайное количество времени.


// Функция, нужная только для того чтобы
// засунуть в разделяемую память нужную информацию.
// Код вынесен сюда, чтобы не мешаться под ногами.
void put_data(char *data) {
    char *str, *token, *context;

    data[0] = '\0';
    strcat(data, "olds/p1 olds/p2 olds/p3 olds/p4");

    token = strtok_r(data, " ", &context);

    for (int i = 0; i < FILES_CNT - 1; i++) {
        token = strtok_r(NULL, " ", &context);
    }
}

// Создаём семафоры
int init_sems(key_t key, int nsems) {
    union semun arg;
    int semid;

    semid = semget(key, nsems, IPC_CREAT | 0666);
    arg.val = 1;

    for(int isem = 0; isem < nsems; isem++) { 
        if (semctl(semid, isem, SETVAL, arg) == -1) {
            semctl(semid, 0, IPC_RMID); // Отщистка
            return -1; // Ошибка
        }
    }

    return semid;
}

int main(int argc, char *argv[]) {
    pid_t pids[SUBPROC_CNT];
    key_t key;

    int msgid, shmid, semid;
    msgbuf msg;

    char *data; // Разделяемая память

    // Получаем уникальный ключ
    if ((key = ftok("main.c", 'F')) == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    // Создаём сегмент разделяемой памяти
    if ((shmid = shmget(key, 1024, 0666 | IPC_CREAT)) == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }
    
    // Получаем ссылку на сегмент памяти и
    // запихиваем в него нужную информацю
    data = shmat(shmid, NULL, 0);
    put_data(data);

    // Создаём семафоры
    if ((semid = init_sems(key, FILES_CNT)) == -1) {
        perror("init_sems");
        exit(EXIT_FAILURE);
    }

    // Создаём подпроцессы
    for (int num = 0; num < SUBPROC_CNT; num++) {
        pids[num] = subprocess();
    }

    // Подключаемся к очереди сообщений
    if ((msgid = msgget(key, 0666 | IPC_CREAT)) == -1) {
        perror("msgget");
        exit(EXIT_FAILURE);
    }

    // Просушиваем сообщения из очереди
    for (int i = 0; i < SUBPROC_CNT; i++) {
        if (msgrcv(msgid, &msg, sizeof(msg.mtext), 1, 0) == -1) {
            perror("msgrcv");
            exit(EXIT_FAILURE);
        }

        printf("Got message: %s\n", msg.mtext);
    }

    // Закрываем очередь сообщений
    if (msgctl(msgid, IPC_RMID, NULL) == -1) {
        perror("msgctl");
        exit(EXIT_FAILURE);
    }

    // Закрываем разделяемую память
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("shmctl");
        exit(EXIT_FAILURE);
    }

    // Уничтожаем набор семафоров
    if (semctl(semid, FILES_CNT, IPC_RMID, NULL) == -1) {
        perror("semctl");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
