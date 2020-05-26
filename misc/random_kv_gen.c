/*
* @Author: albin
* @Date:   2020-05-25 02:55:44
* @Last Modified by:   albin
* @Last Modified time: 2020-05-25 03:49:22
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Use like
// ./random_kv_gen FILE:file NUMBER_OF_ITEMS:int VAL_RANGE:int
int main(int argc, char const *argv[]) {

    if(argc != 4) {
        printf("Use like ./random_kv_gen FILE:file NUMBER_OF_ITEMS:int VAL_RANGE:int\n");
        exit(1);
    }

    // Generate a random list of (key,val) pairs
    char key[10];
    int val;
    srand(time(0));

    // Open file
    FILE *fp;
    fp = fopen(argv[1], "w");

    for (int i = 0; i < atoi(argv[2]); ++i) {
        val = rand() % atoi(argv[3]);
        sprintf(key, "item_%i", i);
        fprintf(fp, "%s %i\n", key, val);
    }

    fclose(fp);
    return 0;
}
