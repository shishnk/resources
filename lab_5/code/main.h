#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include<sys/ipc.h>
#include<sys/sem.h>
#include<sys/msg.h>
#include<sys/shm.h>

void put_data(char *data);
int init_sems(key_t key, int nsems);
