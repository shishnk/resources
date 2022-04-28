#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

int main(int argc, char *argv[]) {
    int num = atoi(argv[1]);

    if (argc >= 2 && num != 0) {
        srand(num);
        sleep(rand() % 3 + 1);
        printf("Hello, process %s!\n", argv[1]);

    } else {
        srand(time(0));
        sleep(rand() % 3 + 1);
        printf("Hello, unknown!\n");
    }

    return 0;
}
