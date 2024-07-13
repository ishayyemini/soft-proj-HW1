#define _GNU_SOURCE
#define err(x) { printf(x); exit(1); }

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

double euclidian_distance(double *point1, double *point2, int dim) {
    double sum = 0;
    int i;
    for (i = 0; i < dim; ++i) sum += pow(point1[i] - point2[i], 2);
    return sqrt(sum);
}

void calculate_centroid(double *centroid, double **cluster, int c_size, int dim) {
    int i, j;
    for (j = 0; j < dim; j++) centroid[j] = 0.0;
    for (i = 0; i < c_size; i++)
        for (j = 0; j < dim; j++)
            centroid[j] += cluster[i][j];
    for (j = 0; j < dim; j++) centroid[j] /= c_size;
}

int parse_integer(char *str) {
    char *ptr;
    double val = strtod(str, &ptr);
    if (*ptr == '\0' && floor(val) == val) return (int) val;
    return 0;
}

/* free allocated memory according to the allocated entry size */
void free_memory(double **points, int size){
    int i;
    for(i = 0; i < size; i++){
        if(points[i] != NULL) {
            free(points[i]);
        }
    }
    free(points);
}

int main(int argc, char *argv[]) {
    int k;
    int iter = 200;
    double epsilon = 0.001;

    double **points;
    int size = 256;
    int dimension = 1;

    double **centroids;
    double max_cent_dist;
    int i, j;
    int iter_i;

    int line_i = 0, coord_i;
    char *line = NULL;
    size_t len = 0;

    if (argc < 2) err("An Error Has Occurred\n")

    k = parse_integer(argv[1]);
    if (k <= 1) err("Invalid number of clusters!\n")

    if (argc == 3) {
        iter = parse_integer(argv[2]);
        if (iter <= 1 || iter >= 1000) err("Invalid maximum iteration\n")
    }

    points = calloc(size, sizeof(double *));
    if (points == NULL) err("An Error Has Occurred\n")

    getline(&line, &len, stdin);

    /* we need to check how many "," are in a line. This will be the dimension of each point! */
    for (coord_i = 0; line[coord_i]; coord_i++)
        if (line[coord_i] == ',') dimension++;

    do {
        char *line_ptr;
        if (line_i >= size) {
            double **tmp;
            size *= 2;
            tmp = realloc(points, size * sizeof(double *));
            if (tmp == NULL) {
                free_memory(points,line_i);
                err("An Error Has Occurred\n")
            }
            points = tmp;
        }

        points[line_i] = calloc(dimension, sizeof(double));
        if (points[line_i] == NULL) {
            free_memory(points, line_i);
            err("An Error Has Occurred\n")
        }
        line_ptr = line;
        for (coord_i = 0; coord_i < dimension; coord_i++) {
            points[line_i][coord_i] = strtod(line_ptr, &line_ptr);
            /* Move past the comma */
            if (*line_ptr == ',') line_ptr++;
        }
        line_i++;
    } while (getline(&line, &len, stdin) != -1);
    size = line_i;

    if (k >= size) err("Invalid number of clusters!\n")

    /* first assign points as centroids */
    centroids = calloc(k, sizeof(double *));
    if (centroids == NULL) err("An Error Has Occurred\n")
    for (i = 0; i < k; ++i) {
        centroids[i] = calloc(dimension, sizeof(double));
        if (centroids[i] == NULL) err("An Error Has Occurred\n")
        for (j = 0; j < dimension; j++) centroids[i][j] = points[i][j];
    }

    for (iter_i = 0; iter_i < iter; iter_i++) {
        /* initiate new centroids and clusters */
        double **new_centroids = calloc(k, sizeof(double *));
        double ***clusters = calloc(k, sizeof(double **));
        int *c_sizes = calloc(k, sizeof(int)); /* storing cluster lengths for appending new elements */
        if (new_centroids == NULL || clusters == NULL || c_sizes == NULL) err("An Error Has Occurred\n")

        for (i = 0; i < k; ++i) {
            new_centroids[i] = calloc(dimension, sizeof(double));
            clusters[i] = calloc(size, sizeof(double *));
            if (new_centroids[i] == NULL || clusters[i] == NULL) err("An Error Has Occurred\n")
            for (j = 0; j < size; j++) {
                clusters[i][j] = calloc(dimension, sizeof(double));
                if (clusters[i][j] == NULL) err("An Error Has Occurred\n")
            }
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
            double curr_dist;
            calculate_centroid(new_centroids[i], clusters[i], c_sizes[i], dimension);
            curr_dist = euclidian_distance(centroids[i], new_centroids[i], dimension);
            if (max_cent_dist < curr_dist) max_cent_dist = curr_dist;
            for (j = 0; j < dimension; j++) centroids[i][j] = new_centroids[i][j];
        }

        free_memory(new_centroids,k);
        free(clusters);
        free(c_sizes);

        if (max_cent_dist < epsilon) {
            break;
        }
    }

    /* printing the centroids */
    for (i = 0; i < k; i++) {
        for (j = 0; j < dimension; j++) {
            printf("%.4f", centroids[i][j]);
            if (j != dimension - 1) printf(",");
        }
        printf("\n");
    }

    free_memory(points, size);
    free_memory(centroids, k);

    return 0;
}
