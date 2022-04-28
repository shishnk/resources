#include "subprocess.h"
#include "msg.h"

// Получаем случайное слово из data
// * res -- сюда записывается имя файла
// * data -- буффер из разделяемой памяти со словами, разделёнными '\0'
// * words -- количество слов в буфере разделяемой памяти
// Программа возвращает номер слова в спсике слов
static int get_random_word(char *res, char *data, int words) {
    char buf[MTEXT_SIZE];

    // Получаем случайный id
    int random_id = rand() % words;

    // Указатели на начало строки
    char *word = data;
    char *symbol = data;

    // Перебираем слово за словом (в качестве разделителя '\0')
    for (int word_id = 0; word_id <= random_id; word_id++) {

        // Буква за буквой, пока не '\0'
        for (; *symbol != 0; symbol++) {

            // Нам нужно только то самое случайное слово
            if (word_id == random_id) {
                buf[symbol - word] = *symbol;
            }
        }

        // Пропускаем нулевой символ
        word = ++symbol;
    }

    // Запихиваем найденное слово в буфер
    memcpy(res, buf, MTEXT_SIZE);

    return random_id;
}

// Функция для подпроцесса
static void run() {
    key_t key;
    int msgid, shmid;
    char *data;
    char filename[MTEXT_SIZE];

    // Запускаем генератор случайных чисел
    srand(getpid());

    // Получаем уникальный ключ
    if ((key = ftok("main.c", 'F')) == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    // Создаём сегмент разделяемой памяти
    if ((shmid = shmget(key, 1024, 0)) == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    // Получаем ссылку на сегмент памяти
    data = shmat(shmid, (void *)0, 0);

    // Подключаемся к очереди сообщений
    if ((msgid = msgget(key, 0666 | IPC_CREAT)) == -1) {
        perror("msgget");
        exit(EXIT_FAILURE);
    }

    // Получаем случайное имя файла и его номер в списке
    int fid = get_random_word(filename, data, FILES_CNT);
    
    // Начинается работа с семафором
    struct sembuf sb;
    int semid;

    semid = semget(key, FILES_CNT, 0);

    sb.sem_op = -1;
    sb.sem_num = fid;
    sb.sem_flg = 0;

    // Закрываем семафор
    if (semop(semid, &sb, 1) == -1) {
        perror("semop");
        exit(EXIT_FAILURE);
    }

    char kek[MTEXT_SIZE + 32] = "./";
    char pidstr[16];
    sprintf(pidstr, "%d", getpid());

    strcat(kek, filename);
    strcat(kek, " ");
    strcat(kek, pidstr);

    int status = system(kek);
    sb.sem_op = 1;

    // Открываем семафор
    if (semop(semid, &sb, 1) == -1) {
        perror("semop");
        exit(EXIT_FAILURE);
    }

    // Готовим сообщение
    msgbuf msg;
    msg.mtype = 1;
    strcpy(msg.mtext, filename);
    strcat(msg.mtext, " executed by ");
    strcat(msg.mtext, pidstr);

    // Отправляем сообщение
    if (msgsnd(msgid, &msg, sizeof(msg.mtext), 0) == -1) {
        perror("msgsnd");
        exit(EXIT_FAILURE);
    }
}

// Функция для создания подпроцесса
int subprocess() {
    pid_t pid;

    // Создаём подпроцесс
    if ((pid = fork()) == -1) {
        perror("Fork failed\n");
        exit(EXIT_FAILURE);
    }

    // Выполняем дочерний процесс
    if (pid == 0) {
        run();
        exit(EXIT_SUCCESS);
    }

    return pid;
}
