#include <cstdio>
#include <cmath>
#include <mpi.h>

#define Nx 256
#define Ny 256
#define Nz 256
#define a 100000

double* (function[2]);
double* (last_layer);
double* (next_layer);

MPI_Request send_req[2];
MPI_Request recv_req[2];

double Fi;
double Fj;
double Fk;

double Dx = 2.0;
double Dy = 2.0;
double Dz = 2.0;
double e = 10e-8;
double constant;

int l0 = 1;
int l1 = 0;

bool IsBorder(int Dx, int Dy, int Dz)
{
    return ((Dx == 0) || (Dy == 0) || (Dz == 0) || (Dx == Nx) || (Dy == Ny) || (Dz == Nz));
}

double phi(double x, double y, double z)
{
    return x * x + y * y + z * z;
}

double rho(double Dx, double Dy, double Dz)
{
    return 6 - a * phi(Dx, Dy, Dz);
}

void calculations(int& f, int J, int K, double hx, double hy, double hz, int rank, int size, const int* numNode, const int* slip, double owx, double owy, double owz)
{
    //вычисления на границе
    for (int j = 1; j < Ny; ++j)
        for (int k = 1; k < Nz; ++k)
        {

            if (rank != 0)
            {
                int i = 0;
                Fi = (function[l0][(i + 1) * J * K + j * K + k] + last_layer[j * K + k]) / owx;
                Fj = (function[l0][i * J * K + (j + 1) * K + k] + function[l0][i * J * K + (j - 1) * K + k]) / owy;
                Fk = (function[l0][i * J * K + j * K + (k + 1)] + function[l0][i * J * K + j * K + (k - 1)]) / owz;
                function[l1][i * J * K + j * K + k] = (Fi + Fj + Fk - rho((i + slip[rank]) * hx, j * hy, k * hz)) / constant;

                if (fabs(function[l1][i * J * K + j * K + k] - phi((i + slip[rank]) * hx, j * hy, k * hz)) > e)
                    f = 0;
            }

            if (rank != size - 1)
            {
                int i = numNode[rank] - 1;
                Fi = (next_layer[j * K + k] + function[l0][(i - 1) * J * K + j * K + k]) / owx;
                Fj = (function[l0][i * J * K + (j + 1) * K + k] + function[l0][i * J * K + (j - 1) * K + k]) / owy;
                Fk = (function[l0][i * J * K + j * K + (k + 1)] + function[l0][i * J * K + j * K + (k - 1)]) / owz;
                function[l1][i * J * K + j * K + k] = (Fi + Fj + Fk - rho((i + slip[rank]) * hx, j * hy, k * hz)) / constant;

                if (fabs(function[l1][i * J * K + j * K + k] - phi((i + slip[rank]) * hx, j * hy, k * hz)) > e)
                    f = 0;
            }

        }
    if (rank != 0)
    {
        MPI_Wait(&recv_req[1], MPI_STATUS_IGNORE);
        MPI_Wait(&send_req[0], MPI_STATUS_IGNORE);
    }
    if (rank != size - 1)
    {
        MPI_Wait(&recv_req[0], MPI_STATUS_IGNORE);
        MPI_Wait(&send_req[1], MPI_STATUS_IGNORE);
    }
    //вычисления в центре
    for (int i = 1; i < numNode[rank] - 1; ++i)
        for (int j = 1; j < Ny; ++j)
            for (int k = 1; k < Nz; ++k)
            {
                Fi = (function[l0][(i + 1) * J * K + j * K + k] + function[l0][(i - 1) * J * K + j * K + k]) / owx;
                Fj = (function[l0][i * J * K + (j + 1) * K + k] + function[l0][i * J * K + (j - 1) * K + k]) / owy;
                Fk = (function[l0][i * J * K + j * K + (k + 1)] + function[l0][i * J * K + j * K + (k - 1)]) / owz;
                function[l1][i * J * K + j * K + k] = (Fi + Fj + Fk - rho((i + slip[rank]) * hx, j * hy, k * hz)) / constant;

                if (fabs(function[l1][i * J * K + j * K + k] - phi((i + slip[rank]) * hx, j * hy, k * hz)) > e)
                    f = 0;

            }
}

void send_recv_data(int J, int K, int rank, int size, const int* numNode)
{
    if (rank != 0)
    {
        MPI_Isend(&(function[l0][0]), K * J, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD, &send_req[0]);
        MPI_Irecv(last_layer, K * J, MPI_DOUBLE, rank - 1, 1, MPI_COMM_WORLD, &recv_req[1]);
    }
    if (rank != size - 1)
    {
        MPI_Isend(&(function[l0][(numNode[rank] - 1) * J * K]), K * J, MPI_DOUBLE, rank + 1, 1,MPI_COMM_WORLD, &send_req[1]);
        MPI_Irecv(next_layer, K * J, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD, &recv_req[0]);
    }
    //ожидание завершения обменов
    if (rank != 0)
    {
        MPI_Wait(&recv_req[1], MPI_STATUS_IGNORE);
        MPI_Wait(&send_req[0], MPI_STATUS_IGNORE);
    }
    if (rank != size - 1)
    {
        MPI_Wait(&recv_req[0], MPI_STATUS_IGNORE);
        MPI_Wait(&send_req[1], MPI_STATUS_IGNORE);
    }
}

void delta(int J, int K, double hx, double hy, double hz, int rank, const int* numNode, const int* slip)
{
    double max = 0;
    double F1 = 0;

    for (int i = 1; i < numNode[rank] - 2; i++)
        for (int j = 1; j < Ny; j++)
            for (int k = 1; k < Nz; k++)
                if ((F1 = fabs(function[l1][i * J * K + j * K + k] - phi((i + slip[rank]) * hx, j * hy, k * hz))) > max)
                    max = F1;

    double tmpMax = 0;
    MPI_Allreduce(&max, &tmpMax, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);

    if (rank == 0)
    {
        max = tmpMax;
        printf("Max diff = %.9lf\n", max);
    }
}

int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);

    int rank;
    int size;
    int F, tmpF;

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0)
        printf("Thread count: %d\n", size);

    int* numNode = new int[size]();
    int* slip = new int[size]();//смещение

    int residue = size - ((Nz + 1) % size);
    int curLine = 0;

    for (int i = 0; i < size; ++i)//заполнение массива отступов и массива узов для каждого процесса
    {
        slip[i] = curLine;
        numNode[i] = (Nz + 1) / size + (residue > 0 ? 0 : 1);//распределение количества узлов с учетом остатка от деления на количество процессов
        --residue;

        curLine += numNode[i];
    }
    /*
    int I = Ni + 1, J = Nj + 1, K = numNode[rank];
    */

    int I = numNode[rank];
    int J = (Nx + 1);
    int K = (Ny + 1);

    //информация о двух итерациях алгоритма
    function[0] = new double[I * J * K]();
    function[1] = new double[I * J * K]();

    //информация о двух соседних к обрабатываемому блоку слоях узлов
    last_layer = new double[K * J]();
    next_layer = new double[K * J]();

    /* Размеры шагов */
    double hx = Dx / (Nx);
    double hy = Dy / (Ny);
    double hz = Dz / (Nz);

    double owx = pow(hx, 2);
    double owy = pow(hy, 2);
    double owz = pow(hz, 2);
    constant = 2 / owx + 2 / owy + 2 / owz + a;

    int Line = slip[rank];
    for (int i = 0; i <= numNode[rank] - 1; i++){
        for (int j = 0; j <= Ny; j++)
            for (int k = 0; k <= Nz; k++)
            {
                if (IsBorder(Line, j, k))
                {
                    function[0][i * J * K + j * K + k] = phi(Line * hx, j * hy, k * hz);
                    function[1][i * J * K + j * K + k] = phi(Line * hx, j * hy, k * hz);
                }
                else
                {
                    function[0][i * J * K + j * K + k] = 0;
                    function[1][i * J * K + j * K + k] = 0;
                }
            }
        Line++;
    }
    double start = MPI_Wtime();

    int f = 0;
    while (f == 0){
        f= 1;
        //обмен уровнем
        l0 = 1 - l0;
        l1 = 1 - l1;

        send_recv_data(J, K, rank, size, numNode);//обмен граничными значениями
        calculations(f, J, K, hx, hy, hz, rank, size, numNode, slip, owx, owy, owz);//все процессы одновременно вычисляют значения в своей подобласти(этап вычислений)

        MPI_Allreduce(&f, &tmpF, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);
        f = tmpF;
    }

    double finish = MPI_Wtime();

    if (rank == 0)
        printf("Time: %lf\n", finish - start);

    delta(J, K, hx, hy, hz, rank, numNode, slip);

    delete[] last_layer;
    delete[] next_layer;
    delete[] function[0];
    delete[] function[1];
    delete[] slip;
    delete[] numNode;

    MPI_Finalize();
    return 0;
}
