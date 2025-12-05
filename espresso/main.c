#include "espresso.h"

int main(int argc, char* argv[]) {
    printf("ssssssss\n");
    pPLA PLA;

    /* the remaining arguments are argv[optind ... argc-1] */
    PLA = NIL(PLA_t);
    if (read_pla(fopen(argv[1], "r"), &PLA) == EOF) {
        fprintf(stderr, "Unable to find PLA on stdin\n");
        exit(1);
    }
    printf("run espresso......\n");
    /*
     *  Now run espresso
     */
    printf("sss %ld \n", PLA->D);
    PLA->F = espresso(PLA->F, PLA->D, PLA->R);
    printf("Output the solution ......\n");
    /* Output the solution */
    fprint_pla(stdout, PLA);
    printf("clean up memory......\n");
    /* cleanup all used memory */
    free_PLA(PLA);
    FREE(cube.part_size);
    setdown_cube(); /* free the cube/cdata structure data */
    sf_cleanup();   /* free unused set structures */
    sm_cleanup();   /* sparse matrix cleanup */

    exit(0);
}
