#define _GNU_SOURCE

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
    return sqrt(sum);
}

void calculate_centroid(double* centroid, double** cluster, int c_size, int dim) {
    int i, j;
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

    double **centroids;
    double max_cent_dist;
    int i, j;
    int iter_i;

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

    /* we now need to check how many "," are in a line. This will be the dimension of each point! */
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

    /* first assign points as centroids */
    centroids = calloc(k, sizeof(double *));
    for (i = 0; i < k; ++i) {
        centroids[i] = calloc(dimension, sizeof(double));
        for (j = 0; j < dimension; j++) centroids[i][j] = points[i][j];
    }

    for (iter_i = 0; iter_i < iter; iter_i++) {
        /* initiate new centroids and clusters */
        double **new_centroids = calloc(k, sizeof(double *));
        double ***clusters = calloc(k, sizeof(double **));
        /* storing cluster lengths for appending new elements */
        int *c_sizes = calloc(k, sizeof(int));
        for (i = 0; i < k; ++i) {
            new_centroids[i] = calloc(dimension, sizeof(double));
            clusters[i] = calloc(size, sizeof(double *));
            for (j = 0; j < size; j++) clusters[i][j] = calloc(dimension, sizeof(double));
            c_sizes[i] = 0;
        }

        /* assign points to clusters */
        for (i = 0; i < size; i++) {
            /* calculate distance from point i to the first centroid and then calculate the rest */
            double min_dist = euclidian_distance(points[i], centroids[0], dimension);
            int closest = 0;
            for (j = 1; j < k; j++) {
                double curr_dist = euclidian_distance(points[i], centroids[j], dimension);
                if (curr_dist < min_dist) {
                    min_dist = curr_dist;
                    closest = j;
                }
            }
            clusters[closest][c_sizes[closest]++] = points[i];
        }

        /* calculate new centroids */
        max_cent_dist = 0;
        for (i = 0; i < k; i++) {
            calculate_centroid(new_centroids[i], clusters[i], c_sizes[i], dimension);
            if (max_cent_dist < euclidian_distance(centroids[i], new_centroids[i], dimension))
                max_cent_dist = euclidian_distance(centroids[i], new_centroids[i], dimension);
        }

        for (i = 0; i < k; i++) {
            for (j = 0; j < dimension; j++) centroids[i][j] = new_centroids[i][j];
        }

        free(new_centroids);
        free(clusters);
        free(c_sizes);

        if (max_cent_dist < epsilon) {
            break;
        }
    }

    for (i = 0; i < k; i++) {
        for (j = 0; j < dimension; j++) {
            printf("%.4f", centroids[i][j]);
            if (j != dimension - 1) printf(",");
        }
        printf("\n");
    }

    free(points);
    free(centroids);

    /* error checking */
    return 0;
}
