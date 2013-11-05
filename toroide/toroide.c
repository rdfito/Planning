#include <stdio.h>
#include <string.h>
#include <mpi.h>

#define TAG 0
#define CTRL 0
#define DATA 1
#define N 0
#define E 1 
#define S 2
#define W 3

int vecino(int dir, int lado, int id);

int main(int argc, char *argv[]) {
	int rank, size;
    int valor;
    MPI_Status st;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    if(rank == 0) {
        int datos[size];
        int i = 0;
        int parada = 0;
        int n;
        FILE *fp; 
        
        printf("#%d: Red toroidal de %d procesadores.\n", rank, size);
        fp = fopen("datos.dat", "r");
        if(fp == NULL) {
            printf("Error: Archivo de datos no encontrado\n");
        } else {
            while(!feof(fp)) {
                if(fscanf(fp, "%d%*c", &n) == 1) {
                    if(i <= size-1) {
                        datos[i] = n;
                    }
                    i++;
                }
            }
            fclose(fp);

            if(i != size) {
                printf("Error: numero de datos incorrecto (%d para %d procs)\n", i, size);
                parada = 1;
            }
            i=0;
            while(i < size) {
                MPI_Send(&parada, 1, MPI_INT, i, CTRL, MPI_COMM_WORLD);
                i++;
            }
            if(parada == 0) {
                i=0;
                while(i < size) {
                    MPI_Send(&datos[i], 1, MPI_INT, i, DATA, MPI_COMM_WORLD);
                    i++;
                }
            }
        }
    }
    
    MPI_Recv(&valor, 1, MPI_INT, 0, CTRL, MPI_COMM_WORLD, &st);
    if(valor == 0) {
        MPI_Recv(&valor, 1, MPI_INT, 0, DATA, MPI_COMM_WORLD, &st);
        printf("#%d: Dato=%d.\n", rank, valor);
    }

    /*for(i=1;i<lado;i++) {
        Send(vecino(N));
        Recv(vecino(S));
        comparar;
    }*/

    MPI_Finalize();
	
    return 0;
}
/* id+2^(d-1)   XOR    1 << i-1 */
int vecino(int dir, int lado, int id) {
    int v = -1;
    switch(dir) {
        case N:
            v = (id+lado)%(lado*lado); break;
        case S:
            v = (id-lado)%(lado*lado); break;
        case E:
            v = (id+1)%lado+lado*(id/lado); break;
        case W:
            v = (id-1)%lado+lado*(id/lado); break;
    }

    return v;
}

