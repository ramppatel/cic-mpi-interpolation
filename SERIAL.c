#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <string.h>

typedef struct
{
    double x, y;
} Points;

int GRID_X, GRID_Y, NX, NY;
int NUM_Points, NTHR_C, Maxiter;
double dx, dy;

void initializepoints(Points *points)
{
    for (int i = 0; i < NUM_Points; i++)
    {
        points[i].x = (double) rand() / (double)RAND_MAX;
        points[i].y = (double) rand() / (double)RAND_MAX;
    }
}

void readPoints(FILE *file, Points *points)
{
    for (int i = 0; i < NUM_Points; i++)
    {
        fread(&points[i].x, sizeof(double), 1, file);
        fread(&points[i].y, sizeof(double), 1, file);
    }
}

void printmesh(double *meshValue)
{
    int i, j;
    FILE *fd = fopen("Mesh.out", "w");
    if (fd == NULL)
    {
        printf("File not Created: PIC/out/<DIR_NAME>/rho.out\n");
        exit(1);
    }

    for (i = 0; i < GRID_Y; i++)
    {
        for (j = 0; j < GRID_X - 1; j++)
        {
            fprintf(fd, "%lf ", meshValue[i * GRID_X + j]);
        }
        fprintf(fd, "%lf\n", meshValue[i * GRID_X + j]);
    }
    fclose(fd);
}

void interpolation(double *meshValue, Points *points)
{
    memset(meshValue, 0, GRID_X * GRID_Y * sizeof(double));
    double *privateMeshValue = (double *)calloc(NTHR_C * GRID_X * GRID_Y, sizeof(double));

    #pragma omp parallel for num_threads(NTHR_C)
    for (int i = 0; i < NUM_Points; i++)
    {
        int id = omp_get_thread_num();
        double px = points[i].x;
        double py = points[i].y;
        double weighing = 1.0;

        int gx = (int)(px / dx);
        int gy = (int)(py / dy);

        double lx = px - gx * dx;
        double ly = py - gy * dy;

        int p1 = gy * GRID_X + gx;
        int p2 = gy * GRID_X + (gx + 1);
        int p3 = (gy + 1) * GRID_X + gx;
        int p4 = (gy + 1) * GRID_X + (gx + 1);

        double a1 = (dx - lx) * (dy - ly);
        double a2 = lx * (dy - ly);
        double a3 = (dx - lx) * ly;
        double a4 = lx * ly;

        int offset = id * GRID_X * GRID_Y;

        privateMeshValue[offset + p1] += a1 * weighing;
        privateMeshValue[offset + p2] += a2 * weighing;
        privateMeshValue[offset + p3] += a3 * weighing;
        privateMeshValue[offset + p4] += a4 * weighing;
    }

    for (int id = 0; id < NTHR_C; id++)
    {
        for (int i = 0; i < GRID_X * GRID_Y; i++)
        {
            meshValue[i] += privateMeshValue[id * GRID_X * GRID_Y + i];
        }
    }

    free(privateMeshValue);
}

int main(int argc, char **argv)
{
    char filename[50];
    double start_time, end_time, elapsed_time = 0.0;

    if (argc != 3)
    {
        printf("Usage: %s <input_filename> <num_threads>\n", argv[0]);
        return 1;
    }

    strcpy(filename, argv[1]);
    NTHR_C = atoi(argv[2]);

    FILE *file = fopen(filename, "rb");
    if (file == NULL)
    {
        printf("Error: Unable to open file %s\n", filename);
        exit(1);
    }

    fread(&NX, sizeof(int), 1, file);
    fread(&NY, sizeof(int), 1, file);
    fread(&NUM_Points, sizeof(int), 1, file);
    fread(&Maxiter, sizeof(int), 1, file);

    printf("Read from binary file:\n");
    printf("NX: %d, NY: %d, NUM_Points: %d, Maxiter: %d\n", NX, NY, NUM_Points, Maxiter);

    GRID_X = NX + 1;
    GRID_Y = NY + 1;
    dx = 1.0 / NX;
    dy = 1.0 / NY;

    double *meshValue = (double *)calloc(GRID_X * GRID_Y, sizeof(double));
    Points *points = (Points *)calloc(NUM_Points, sizeof(Points));

    for (int iteration = 0; iteration < Maxiter; iteration++)
    {
        readPoints(file, points);
        start_time = omp_get_wtime();
        interpolation(meshValue, points);
        end_time = omp_get_wtime();
        elapsed_time += end_time - start_time;
    }

    printmesh(meshValue);
    printf("Interpolation execution time = %lf seconds\n", elapsed_time);

    FILE *out = fopen("serial.txt", "w");
    if (!out)
    {
        printf("Error: Cannot open output file\n");
        exit(1);
    }

    for (int i = 0; i < GRID_Y; i++)
    {
        for (int j = 0; j < GRID_X; j++)
        {
            fprintf(out, "%lf ", meshValue[i * GRID_X + j]);
        }
        fprintf(out, "\n");
    }

    fclose(out);
    free(meshValue);
    free(points);
    fclose(file);

    return 0;
}
