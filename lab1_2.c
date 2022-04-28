#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include <time.h>

const long int N = 9000;
const double eps = pow(10, -5);
const double t = 0.0001;

double abcMatrix(const double* A, long M) {
    double norm = 0;
    for (int i = 0; i < M; i++) {
        norm += A[i] * A[i];
    }
    return sqrt(norm);
}

void sub(double* A, double* B, double* C, int n) {
    for (int i = 0; i < n; i++) {
        C[i] = A[i] - B[i];
    }
}

void mul(double* A, double* B, double* res, int n, int NN) {

    for (unsigned int i = 0; i < n; i++) {
        res[i] = 0;
        for (unsigned int j = 0; j < NN; j++) {
            res[i] += A[i * NN + j] * B[j];
        }
    }
}

void scalMul(double* A, double tau, int n) {
    for (int i = 0; i < n; ++i) {
        A[i] = A[i] * tau;
    }
}

void printMatrix(double* A, int n, int m) {

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            printf("%.1lf ", round(A[i * m + j]));
        }
        printf("\n");
    }
    printf("\n");
}

void createMatrix(double* matrix, int NN) {

    for (int i = 0; i < NN; i++) {
        for (int j = 0; j < NN; j++) {
            if (i == j) {
                matrix[i * NN + j] = i;
 }
            else {
                matrix[i * NN  + j] = i+1;
            }
        }
    }
}

int main(int argc, char** argv) {
    srand(time(0));

    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int NN = N;
    int m = N % size;
    int nm = N / size;

    int* NM = (int*)malloc(size * sizeof(double));
    for (int i = 0; i < size; i++) {
        if (m > 0) {
            NM[i] = nm + 1;
            m--;
        }
        else NM[i] = nm;
    }

    double* ABuf = (double*)malloc(NN * NN * sizeof(double));
    double* BBuf = (double*)malloc(NN * NN * sizeof(double));
    double* AxBuf = (double*)malloc(NN * sizeof(double));
    double* lastXBuf = (double*)malloc(NN * sizeof(double));
    double* nextXBuf = (double*)malloc(NN * sizeof(double));

        double* lastX = (double*)malloc(NN * sizeof(double));
    double* Ax = (double*)malloc(NN * sizeof(double));
    double* nextX = (double*)malloc(NN * sizeof(double));
    double* A;
    double* B = (double*)malloc(NN * sizeof(double));
    double* X = (double*)malloc(NN * sizeof(double));
    double start = 0, end = 0;

    double normAxb = 0; // ||A*xn - b||
    double normB = 0;
    double saveRes = 1;
    double res = 1;
    double lastres = 1;
    double tau = t;

    for (long i = 0; i < N; i++) {
        X[i] = 0;
        lastX[i] = 0;
        nextX[i] = 0;
        B[i] = NN + 1;
    }

    if (rank == 0) {
                        A = (double*)malloc(NN * NN * sizeof(double));
        createMatrix(A, NN);
        mul(A, lastX, Ax, NN, NN);
        normB = abcMatrix(B, NN);
        start = MPI_Wtime();
    }
    int* displs = (int*)malloc(N * sizeof(int));
    int* scounts = (int*)malloc(N * sizeof(int));

    int k = 0;
    for (int i = 0; i < size; i++) {
        scounts[i] = NM[i] * NN;
        displs[i] = k;
        k += scounts[i];
    }
    MPI_Scatterv(A, scounts, displs, MPI_DOUBLE, ABuf, NN * NN, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    int kS = 0;
    for (int i = 0; i < size; i++) {
        scounts[i] = NM[i];
        displs[i] = kS;
        kS += scounts[i];
    }
    MPI_Scatterv(B, scounts, displs, MPI_DOUBLE, BBuf, NN, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatterv(X, scounts, displs, MPI_DOUBLE, lastX, NN, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    int count = 1;

//    start = MPI_Wtime();

    while (res > eps) {


        if (rank == 0) {
            for (long i = 0; i < N; i++) {
                lastX[i] = nextX[i];

            }
        }

        MPI_Bcast(lastX, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);//▒~@а▒~A▒~A▒~Kлаем ▒~Gа▒~A▒~B▒~L век▒~Bо▒~@а X из п▒~@о▒~Hлой и▒~Bе▒~@а▒~Fии в▒~Aем п▒~@о▒~Fе▒~A▒~A▒▒
▒м из ко▒~@невого п▒~@о▒~Fе▒~A▒~Aа
        int kS = 0;
        for (int i = 0; i < size; i++) {
            scounts[i] = NM[i];
            displs[i] = kS;
            kS += scounts[i];
        }
        MPI_Scatterv(Ax, scounts, displs, MPI_DOUBLE, AxBuf, NN, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        mul(ABuf, lastX, AxBuf, NM[rank], NN); // A*xn
        sub(AxBuf, B, AxBuf, NM[rank]); // A*xn - b
        scalMul(AxBuf, tau, NM[rank]); // TAU*(A*xn - b)

        int k = 0;
        for (int i = 0; i < size; ++i) {
            displs[i] = k;
            scounts[i] = NM[i];//▒~Gи▒~Aло ▒~Mл в кажд п▒~@о▒~F
                                                                  k = k + scounts[i];
        }
        MPI_Allgatherv(AxBuf, NM[rank], MPI_DOUBLE, Ax, scounts, displs, MPI_DOUBLE, MPI_COMM_WORLD);
        //по▒~Aле ▒~@аз▒~@езани▒~O, о▒~Bп▒~@вки по п▒~@о▒~Fе▒~A▒~Aам, об▒~@або▒~Bки на каждом п▒~@о▒~Fе▒~A▒~Aе век▒~Bо▒~@ TAU*(A*xn - b) ▒~Aо▒▒
▒и▒~@ае▒~B▒~A▒~O в един▒~Kй век▒~Bо▒~@ Ax во в▒~Aе▒~E п▒~@о▒~Fе▒~A▒~Aа▒~E
        if (rank == 0) {
            normAxb = abcMatrix(Ax, NN); // ||A*xn - b||
            sub(lastX, Ax, nextX, NN); // x(n+1) = xn - TAU * (A*xn - b)
            res = normAxb / normB;

            count++;//под▒~A▒~Gе▒~B и▒~Bе▒~@а▒~Fий, ▒~G▒~Bоб▒~K можно б▒~Kло ▒~Aмени▒~B▒~L знак tau, е▒~Aли ▒~@е▒~Hение на▒~Gинае▒~B ▒~@а▒~A▒~Eод▒
и▒~B▒~L▒~A▒~O
            if ((count > 10000000 && lastres > res)) {
                if (tau < 0) {
                    printf("Does not converge\n");
                    saveRes = res;
                    res = 0;
                }
                else {
                    tau = (-1) * t;
                    count = 0;
                }
            }

            lastres = res;
        }
        MPI_Bcast(&res, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);
    }


    end = MPI_Wtime();

    if (rank == 0) {
        if (res == 0) {
            res = saveRes;
        }
        //printf("X\n");
        //printMatrix(nextX, 1,1);

        printf("program running time %lf seconds\n", end - start);

        free(A);
    }

    free(ABuf);
    free(Ax);
    free(nextX);
    free(lastX);
    free(B);
    free(AxBuf);
    MPI_Finalize();
    return 0;
}
                                          
