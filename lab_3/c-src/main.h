#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define DATA_SIZE 44 // Размер структуры Data

// Структура с передаваемыми данными
typedef struct {
    pid_t pid;      // id процесса
    char msg[40];   // сообщение
} Data;

// Структура для наглядного
// представления каналов
typedef struct {
    int rcv; // получатель
    int snd; // отправитель
} Pipe;

