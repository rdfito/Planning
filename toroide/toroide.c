#include <stdio.h>
#include <string.h>
#include <mpi.h>

#define DOTDAT "datos.dat"
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
    double valor;
    int parada;
    MPI_Status st;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    /* RANK 0 */
    if(rank == 0) {
        double datos[size];
        double n;
        int i = 0;
        FILE *fp; 
        parada = 0;

        printf("[%d] Red toroidal de %d procesos\n", rank, size);

        /* Verificacion del archivo de datos */
        fp = fopen(DOTDAT, "r");
        if(fp == NULL) {
            printf("Error: Archivo \"%s\" no encontrado\n", DOTDAT);
            parada = 1;
        } else {
            while(!feof(fp)) {
                /* lee double seguido de char (que ignora) */
                if(fscanf(fp, "%lf%*c", &n) == 1) {
                    if(i <= size-1) {
                        datos[i] = n;
                    }
                    i++;
                }
            }
            fclose(fp);
        }
        /* Verificacion de valores del archivo de datos */
        if(i != size) {
            printf("Error: %d valores para %d procesos\n", i, size);
            parada = 1;
        }
        /* Ennvio de control */
        i=0;
        while(i < size) {
            MPI_Send(&parada, 1, MPI_INT, i, CTRL, MPI_COMM_WORLD);
            i++;
        }
        /* Envio de datos */
        if(parada == 0) {
            i=0;
            while(i < size) {
                MPI_Send(&datos[i], 1, MPI_DOUBLE, i, DATA, MPI_COMM_WORLD);
                i++;
            }
        }
    }
    
    MPI_Recv(&parada, 1, MPI_INT, 0, CTRL, MPI_COMM_WORLD, &st);
    if(parada == 0) {
        MPI_Recv(&valor, 1, MPI_DOUBLE, 0, DATA, MPI_COMM_WORLD, &st);
        printf("[%d] Valor = %.3f\n", rank, valor);
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

