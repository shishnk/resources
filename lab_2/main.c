#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>  // for pid_t
#include <unistd.h>     // for fork

#define PI_COMPUTION_MAX_ITERS 1e+6
#define SINH_ACCURACY 1e-14
#define SINH_COMPUTION_MAX_ITERS 1e+3
#define FILENAME "data.dat"

// Struct to store info
typedef struct {
    pid_t pid;
    double value;
} info_proc;

// Wallis product
double calculate_pi() {
    double result = 1.;
    double a = 2.;
    double b = 1.;

    for (int i = 0; i < PI_COMPUTION_MAX_ITERS; i++) {
        result *= a / b;

        if (a < b) {
            a += 2.;
        } else {
            b += 2.;
        }
    }

    return result * 2.;
}

// get sinh(x)
double calculate_sinh(double x) {
    int i = 1;
    double cur = x;
    double acc = 1;
    double fact = 1.;
    double pow = x;

    while (fabs(acc) > SINH_ACCURACY && i < SINH_COMPUTION_MAX_ITERS) {
        fact *= ((2. * i) * (2. * i + 1.));
        pow *= x * x;
        acc = pow / fact;
        cur += acc;
        i++;
    }

    return cur;
}

// compute sinh(x) * PI
int compute(double x, double *result) {
    info_proc fst, snd, info_all[2];
    pid_t fst_tmp, snd_tmp;
    FILE *fp;

    if ((fp = fopen(FILENAME, "w+")) == NULL) {
        perror("Error occured while opening file\n");
        return EXIT_FAILURE;
    }

    // Create process to compute PI
    // ------------------------------------------------
    if ((fst.pid = fork()) == -1) {
        perror("Fork failed\n");
        return EXIT_FAILURE;
    }

    if (fst.pid == 0) {
        printf("I am child with pid %d, my parent is %d and I am calculating pi\n",
               fst.pid = getpid(), getppid());

        fst.value = calculate_pi();

        if (fwrite(&fst, sizeof(info_proc), 1, fp) == 0) {
            perror("Error writing file\n");
            return EXIT_FAILURE;
        }

        exit(0);
    }

    // Create process to compute sinh(x)
    // ------------------------------------------------
    if ((snd.pid = fork()) == -1) {
        perror("Fork failed\n");
        return EXIT_FAILURE;
    }

    if (snd.pid == 0) {
        printf("I am child with pid %d, my parent is %d and I am calculating sinh(x)\n",
               snd.pid = getpid(), getppid());

        snd.value = calculate_sinh(x);

        if (fwrite(&snd, sizeof(info_proc), 1, fp) == 0) {
            perror("Error writing file\n");
            return EXIT_FAILURE;
        }

        exit(0);
    }

    // Read file untill there will be
    // two info_proc structs
    // ------------------------------------------------
    int proc_count = 0;
    while (proc_count != 2) {
        proc_count = 0;

        while (fread(info_all + proc_count, sizeof(info_proc), 1, fp)) {
            proc_count++;
        }

        fseek(fp, 0, SEEK_SET);
    }

    fclose(fp);

    // Remove 'data.dat'
    // ------------------------------------------------
    if (remove(FILENAME) != 0) {
        perror("Error: unable to delete the file\n");
        return EXIT_FAILURE;
    }

    // Create pretty console output
    // ------------------------------------------------
    if (info_all[0].pid == fst.pid && info_all[1].pid == snd.pid) {
        printf("Child with pid %d calculated pi = %lf\n", info_all[0].pid, info_all[0].value);
        printf("Child with pid %d calculated sinh(x) = %lf\n", info_all[0].pid, info_all[1].value);

    } else if (info_all[1].pid == fst.pid && info_all[0].pid == snd.pid) {
        printf("Child with pid %d calculated pi = %lf\n", info_all[1].pid, info_all[1].value);
        printf("Child with pid %d calculated sinh(x) = %lf\n", info_all[0].pid, info_all[0].value);

        // Or error message
    } else {
        perror("Error: Processes PIDs do not match\n");
        return EXIT_FAILURE;
    }

    *(result) = info_all[0].value * info_all[1].value;
    // Don't forget about parent process
    printf("I am parent with pid %d and I calculated sinh(x) * pi\n", getpid());
}

int main(int argc, char *argv[]) {
    double x, result;

    printf("Enter x: ");
    scanf("%lf", &x);

    compute(x, &result);

    printf("sinh(x) * pi = %lf\n", result);

    return EXIT_SUCCESS;
}