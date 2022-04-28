#include <iostream>
#include <mpi.h>
#include <cmath>
#include <cstdlib>
#include <zconf.h>

MPI_Comm GridComm; // Grid РєРѕРјРјСѓРЅРёРєР°С‚РѕСЂ
MPI_Comm ColComm; // Column РєРѕРјРјСѓРЅРёРєР°С‚РѕСЂ
MPI_Comm RowComm; // Row РєРѕРјРјСѓРЅРёРєР°С‚РѕСЂ
int GridCoords[2];
int size = 0;
int rank = 0;
int p1 = 3;
int p2 = 6;
int n1 = 6;
int n2 = 6;
int n3 = 6;

void InitMatrixB(double* pMatrix, int rowCount, int colCount) {
    for (int i = 0; i < rowCount; i++) {
        for (int j = 0; j < colCount; j++) {
            if (i == j) {
                pMatrix[i * colCount + j] = 1;
            }
            else {
                pMatrix[i * colCount + j] = 0;
            }
        }
    }
}
void InitMatrix(double* pMatrix, int rowCount, int colCount) {
    for (int i = 0; i < rowCount; i++) {
        for (int j = 0; j < colCount; j++) {
            pMatrix[i * colCount + j] = i * colCount + j;
        }
    }
}

void SetToZero(double* pMatrix, int rowCount, int colCount) {
    for (int i = 0; i < rowCount; i++) {
        for (int j = 0; j < colCount; j++) {
            pMatrix[i * colCount + j] = 0;
        }
    }
}

// Р¤СѓРЅРєС†РёСЏ РІС‹РІРѕРґР° С„РѕСЂРјР°С‚РёСЂРѕРІР°РЅРЅРѕР№ РјР°С‚СЂРёС†С‹
void PrintMatrix(double* pMatrix, int RowCount, int ColCount) {
    int i, j;
    for (i = 0; i < RowCount; i++) {
        for (j = 0; j < ColCount; j++)
            printf("%6.2f ", pMatrix[i * ColCount + j]);
        printf("\n");
    }
}

// Р¤СѓРЅРєС†РёСЏ РґР»СЏ СѓРјРЅРѕР¶РµРЅРёСЏ РјР°С‚СЂРёС†
void MatrixMul(double* pA, double* pB, double* pC, int partAsize, int n2, int partBsize) {
    int i, j, k;  
    for (i = 0; i < partAsize; i++) {
        for (j = 0; j < partBsize; j++)
            for (k = 0; k < n2; k++)
                pC[i * partBsize + j] += pA[i * n2 + k] * pB[k * partBsize + j];
    }
}

void Communicators() {
    int DimSize[2]; // Р§РёСЃР»Рѕ РїСЂРѕС†РµСЃСЃРѕРІ РІ РєР°Р¶РґРѕРј РёР·РјРµСЂРµРЅРёРё СЃРµС‚РєРё
    int Periodic[2]; // =1, РµСЃР»Рё СЂР°Р·РјРµСЂРЅРѕСЃС‚СЊ СЃРµС‚РєРё РґРѕР»Р¶РЅР° Р±С‹С‚СЊ РїРµСЂРёРѕРґРёС‡РµСЃРєРѕР№
    int SubDimension[2]; // =1, РµСЃР»Рё СЂР°Р·РјРµСЂРЅРѕСЃС‚СЊ СЃРµС‚РєРё РґРѕР»Р¶РЅР° Р±С‹С‚СЊ С„РёРєСЃРёСЂРѕРІР°РЅРЅРѕР№

    DimSize[0] = p1;
    DimSize[1] = p2;

    Periodic[0] = 1;
    Periodic[1] = 1;
    // РЎРѕР·РґР°РЅРёРµ РґРµРєР°СЂС‚РѕРІР° РєРѕРјРјСѓРЅРёРєР°С‚РѕСЂР°
    MPI_Cart_create(MPI_COMM_WORLD, 2, DimSize, Periodic, 0, &GridComm);
    // РѕРїСЂРµРґРµР»РµРЅРёСЏ РґРµРєР°СЂС‚РѕРІС‹С… РєРѕРѕСЂРґРёРЅР°С‚ РїСЂРѕС†РµСЃСЃР° РїРѕ РµРіРѕ СЂР°РЅРіСѓ
    MPI_Cart_coords(GridComm, rank, 2, GridCoords);

    // РЎРѕР·РґР°РЅРёРµ РєРѕРјРјСѓРЅРёРєР°С‚РѕСЂРѕРІ РґР»СЏ СЃС‚СЂРѕРє
    SubDimension[0] = 0; 
    SubDimension[1] = 1; 
    MPI_Cart_sub(GridComm, SubDimension, &RowComm);

    // РЎРѕР·РґР°РЅРёРµ РєРѕРјРјСѓРЅРёРєР°С‚РѕСЂРѕРІ РґР»СЏ СЃС‚РѕР»Р±С†РѕРІ
    SubDimension[0] = 1; 
    SubDimension[1] = 0; 
    MPI_Cart_sub(GridComm, SubDimension, &ColComm);
}

void DataDistribution(double* A, double* B, double* partA,
    double* partB, int partAsize, int partBsize) {

    if (GridCoords[1] == 0) {
        MPI_Scatter(A, partAsize * n2, MPI_DOUBLE, partA, partAsize * n2, MPI_DOUBLE, 0, ColComm);
    }
    //РўСЂР°РЅСЃР»РёСЂСѓРµС‚ СЃРѕРѕР±С‰РµРЅРёРµ РѕС‚ РїСЂРѕС†РµСЃСЃР° СЃ СЂР°РЅРіРѕРј "root" РІСЃРµРј РґСЂСѓРіРёРј РїСЂРѕС†РµСЃСЃР°Рј РєРѕРјРјСѓРЅРёРєР°С‚РѕСЂР°
    MPI_Bcast(partA, partAsize * n2, MPI_DOUBLE, 0, RowComm);
    MPI_Datatype col, coltype;

    MPI_Type_vector(n2, partBsize, n3, MPI_DOUBLE, &col);
    MPI_Type_commit(&col);
    MPI_Type_create_resized(col, 0, partBsize * sizeof(double), &coltype);
    MPI_Type_commit(&coltype);

    if (GridCoords[0] == 0) {
        MPI_Scatter(B, 1, coltype, partB, n2 * partBsize, MPI_DOUBLE, 0, RowComm);
    }

    MPI_Bcast(partB, partBsize * n2, MPI_DOUBLE, 0, ColComm);
}

int main(int argc, char* argv[]) {
    double* A = NULL;      // РџРµСЂРІР°СЏ РјР°С‚СЂРёС†Р°
    double* B = NULL;      // Р’С‚РѕСЂР°СЏ РјР°С‚СЂРёС†Р°
    double* C = NULL;      // РњР°С‚СЂРёС†Р° СЂРµР·СѓР»СЊС‚Р°С‚РѕРІ

    int partAsize = n1 / p1;
    int partBsize = n3 / p2;

    double* partA = new double[n2 * partAsize];       // РўРµРєСѓС‰РёР№ Р±Р»РѕРє РјР°С‚СЂРёС†С‹ A
    double* partB = new double[n2 * partBsize];     // РўРµРєСѓС‰РёР№ Р±Р»РѕРє РјР°С‚СЂРёС†С‹ B
    double* partC = new double[partAsize * partBsize];       // Р‘Р»РѕРє СЂРµР·СѓР»СЊС‚РёСЂСѓСЋС‰РµР№ РјР°С‚СЂРёС†С‹ C

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if ((n1 % p1 != 0) || (n3 % p2 != 0)) {
        if (rank == 0) {
            printf("i grid size\n");
        }
    }
    else {
        if (rank == 0)
        printf("Parallel matrix multiplication program, on %d processes\n", size);
        Communicators(); // РЎРѕР·РґР°РЅРёРµ Grid-РєРѕРјРјСѓРЅРёРєР°С‚РѕСЂР°
    }


    if (rank == 0) {
         A = new double[n1 * n2];
         B = new double[n2 * n3];
         C = new double[n1 * n3];
         InitMatrix(A, n1, n2);
         InitMatrixB(B, n2, n3);
    }

    SetToZero(partC, partAsize, partBsize);

    double startTime;

    if (rank == 0) {
        printf("Initial matrix A \n");
        PrintMatrix(A, n1, n2);
        printf("Initial matrix B \n");
        PrintMatrix(B, n2, n3);
        startTime = MPI_Wtime();
    }
    // СЂР°СЃРїСЂРµРґРµР»РµРЅРёРµ РґР°РЅРЅС‹С… РјРµР¶РґСѓ РїСЂРѕС†РµСЃСЃР°РјРё
    DataDistribution(A, B, partA, partB, partAsize, partBsize);
    // РЈРјРЅРѕР¶РµРЅРёРµ РјР°С‚СЂРёС‡РЅС‹С… С‡Р°СЃС‚РµР№ С‚РµРєСѓС‰РµРіРѕ РїСЂРѕС†РµСЃСЃР°
    MatrixMul(partA, partB, partC, partAsize, n2, partBsize);

    // РЎР±РѕСЂ РІСЃРµ РґР°РЅРЅС‹С… РІ РѕРґРЅСѓ РјР°С‚СЂРёС†Сѓ
    MPI_Datatype block, blocktype;
    MPI_Type_vector(partAsize, partBsize, n3, MPI_DOUBLE, &block);
    MPI_Type_commit(&block);

    MPI_Type_create_resized(block, 0, partBsize * sizeof(double), &blocktype);
    MPI_Type_commit(&blocktype);

    int* displ = new int[p1 * p2];
    int* rcount = new int[p1 * p2];
    int BlockCount = 0;
    int BlockSize = partAsize * partBsize;
    int NumCount = 0;
    int Written;
    int j = 0;

    while (NumCount < p1 * p2 * BlockSize) {
        Written = 0;
        for (int i = 0; i < n3; i += partBsize) {
            displ[j] = BlockCount;
            rcount[j] = 1;
            j++;
            BlockCount++;

            Written++;
        }
        NumCount += Written * BlockSize;
        BlockCount += Written * (partAsize - 1);
    }

    MPI_Gatherv(partC, BlockSize, MPI_DOUBLE, C, rcount, displ, blocktype, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        double endTime = MPI_Wtime();
        printf("matrix C \n");

        PrintMatrix(C, n1, n3);
        printf("That took %lf seconds\n", endTime - startTime);
    }

    if (rank == 0) {
        delete[] A;
        delete[] B;
        delete[] C;
    }
    delete[] partA;
    delete[] partB;
    delete[] partC;
    delete[] displ;
    delete[] rcount;
    MPI_Finalize();
    return 0;
}
