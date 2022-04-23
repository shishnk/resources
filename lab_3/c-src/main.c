#include "main.h"

int main(int argc, char *argv[]) {
    Pipe pipe1, pipe2;
    Data data;
    pid_t pid1, pid2;

    // Создаём каналы
    pipe((int *) &pipe1);
    pipe((int *) &pipe2);
    

    // Первый дочерний процесс (т.н. P2)
    // -------------------------------------------------------------
    if ((pid1 = fork()) == -1) {
        perror("Fork failed\n");
        return EXIT_FAILURE;
    }

    if (pid1 == 0) {
        pid1 = getpid();
        printf("P1 with pid %d is created.\n", pid1);


        // Мы знаем, что это сообщение полностью не поместится в data.msg
        // Как раз не хватит места для имени убийцы :D
        const char *msg = " His name is DIO BRANDO.";

        void func() { }

        signal(SIGUSR1, func);
        pause();

        // Вытаскиваем данные из канала K1
        read(pipe1.rcv, &data, DATA_SIZE);
        printf("P1 with pid %d have got data from pipe K1 from process with pid %d.\n", pid1, data.pid);

        // Дополняем полученные данные
        strncat(data.msg, msg, DATA_SIZE - 4 - strlen(data.msg));
        data.pid = pid1;

        // Отправляем данные по каналу K2
        write(pipe2.snd, &data, DATA_SIZE);
        printf("P1 with pid %d have sent data to pipe K2.\n", pid1);

        // Закрываем каналы
        close(pipe1.rcv);
        close(pipe1.snd);
        close(pipe2.rcv);
        close(pipe2.snd);

        exit(EXIT_SUCCESS);
    }


    // Второй дочерний процесс (т.н. P2)
    // -------------------------------------------------------------
    if ((pid2 = fork()) == -1) {
        perror("Fork failed\n");
        return EXIT_FAILURE;
    }

    if (pid2 == 0) {
        pid2 = getpid();
        printf("P2 with pid %d is created.\n", pid2);

        // Вот такое сообщение мы собираемся отправить
        const char *msg = "There is imposter among us.";

        // Заполняем данные в форму
        data.pid = pid2;
        strcpy(data.msg, msg);

        // Отправляем данные по каналу K1
        write(pipe1.snd, &data, DATA_SIZE);
        printf("P2 with pid %d have sent data to pipe K1.\n", pid2);

        kill(pid1, SIGUSR1);

        // Закрываем каналы
        close(pipe1.rcv);
        close(pipe1.snd);
        close(pipe2.rcv);
        close(pipe2.snd);

        exit(EXIT_SUCCESS);
    }
    

    // В дело вступает родительский процесс (т.н. P0)
    // -------------------------------------------------------------

    // Вытаскиваем данные из канала K2
    read(pipe2.rcv, &data, DATA_SIZE);

    printf("P0 with pid %d have got data from pipe K2 from process with pid %d.\n", pid2, data.pid);
    printf("- Got message: %s\n", data.msg);

    // Закрываем каналы
    close(pipe1.rcv);
    close(pipe1.snd);
    close(pipe2.rcv);
    close(pipe2.snd);

    // Ждём завершения процессов-потомков
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    return EXIT_SUCCESS;
}
