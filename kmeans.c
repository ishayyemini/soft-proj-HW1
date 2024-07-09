#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void error(char *msg) {
    printf("%s\n", msg);
    exit(1);
}

double euclidian_distance(double* point1, double* point2, int dim) {
    double sum = 0;
    int i;
    for (i = 0; i < dim; ++i) sum += pow(point1[i] - point2[i], 2);
    return pow(sum, 0.5);
}

void calculate_centroid(double* centroid, double** cluster, int c_size, int dim) {
    int i, j;

    for (i = 0; i < c_size; i++) {
        for (j = 0; j < dim; j++) printf("%.4f,", cluster[i][j]);
        printf("\n");
    }

    for (j = 0; j < dim; j++) centroid[j] = 0.0;
    for (i = 0; i < c_size; i++)
        for (j = 0; j < dim; j++)
            centroid[j] += cluster[i][j];
    for (j = 0; j < dim; j++) centroid[j] /= c_size;
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

    double **centroids, **new_centroids;
    int i, j;
    int closest = 0;
    double min_dist;
    double curr_dist;

    double ***clusters;
    int *c_sizes;

    char *line = NULL;
    char *line_ptr;
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
    for (coord_i = 0; line[coord_i]; coord_i++)
        if (line[coord_i] == ',') dimension++;

    points = calloc(size, sizeof(double *));

    /* read all points and input them into points matrix */
    do {
        if (line_i >= size) {
            double **tmp;
            size *= 2;
            tmp = realloc(points, size * sizeof(double *));
            if (tmp == NULL) free(tmp);
            else points = tmp;
        }

        points[line_i] = calloc(dimension, sizeof(double));
        line_ptr = line;
        for (coord_i = 0; coord_i < dimension; coord_i++) {
            points[line_i][coord_i] = strtod(line_ptr, &line_ptr);
            /* Move past the comma */
            if (*line_ptr == ',') line_ptr++;
        }
        line_i++;
    } while (getline(&line,&len,stdin) != -1);
    size = line_i;

    /* first assign */
    centroids = calloc(k, sizeof(double *));
    new_centroids = calloc(k, sizeof(double *));
    clusters = calloc(k, sizeof(double **));
    c_sizes = calloc(k, sizeof(int));
    for (i = 0; i < k; ++i) {
        centroids[i] = points[i];
        new_centroids[i] = calloc(dimension, sizeof(double));
        clusters[i] = calloc(size, sizeof(double *));
        c_sizes[i] = 0;
    }

    for (i = 0; i < size; i++) {
        min_dist = euclidian_distance(points[i], centroids[0], dimension);
        for (j = 1; j < k; j++) {
            curr_dist = euclidian_distance(points[i], centroids[j], dimension);
            if (curr_dist < min_dist) {
                min_dist = curr_dist;
                closest = j;
            }
        }
        clusters[closest][c_sizes[closest]++] = points[i];
    }

    for (i = 0; i < k; i++) {
        printf("cluster %d\n", i);
        calculate_centroid(new_centroids[i], clusters[i], c_sizes[i], dimension);
        printf("c_size: %d ", c_sizes[i]);
        for (j = 0; j < dimension; j++) printf("%.4f,", new_centroids[i][j]);
        printf("\n\n");
    }

    printf("dimension: %d\n", dimension);

    printf("%s\n", line);

    printf("%d %d %f\n", k, iter, epsilon);

    free(points);
    free(centroids);
    free(clusters);
    free(c_sizes);
    free(new_centroids);

    /* error checking */
    return 0;
}
