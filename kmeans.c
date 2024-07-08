#include <stdio.h>
#include <stdlib.h>

void error(char *msg) {
    printf("%s\n", msg);
    exit(1);
}



int main(int argc, char *argv[]) {
    int k;
    char *endptr;
    int iter = 200;
    double epsilon = 0.001;

    int dimension = 1;
    int coord_i;

    double **points;
    int size = 256;

    char *line = NULL;
    size_t len = 0;
    int line_i = 0;

    k = (int) strtol(argv[1], &endptr, 10);
    if (*endptr != '\0') error("An Error Has Occurred");

    if (argc >= 3) {
        iter = (int) strtol(argv[2], &endptr, 10);
        if (*endptr != '\0') error("An Error Has Occurred");
    }

    getline(&line,&len,stdin);

    /* we now need: check how many "," are in line. this is our dimension! */
    for (coord_i = 0; line[coord_i]; coord_i++) if (line[coord_i] == ',') dimension++;

    points = calloc(size, sizeof(double *));

    do {
        if (line_i >= size) {
            double **tmp;
            size *= 2;
            tmp = realloc(points, size * sizeof(double *));
            if (tmp == NULL) free(tmp);
            else points = tmp;
        }

        points[line_i] = calloc(dimension, sizeof(double));
        for (coord_i = 0; coord_i < dimension; coord_i++, line++) {
            points[line_i][coord_i] = strtod(line, &line);
        }
        line_i++;
    } while (getline(&line,&len,stdin) != -1);

    printf("dimension: %d\n", dimension);

    printf("%s\n", line);

    printf("%d %d %f\n", k, iter, epsilon);

    free(points);

    /* error checking */
    return 0;
}
