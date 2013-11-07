#include <stdio.h>
#include <math.h>
#include <mpi.h>

#define DOTDAT "datos.dat"

#define CTRL 0
#define DATA 1

#define NORTE 0
#define ESTE  1 
#define SUR   2
#define OESTE 3

int vecino(int dir, int lado, int id);

int main(int argc, char *argv[]) {
	int rank, size;
    double valor;
    double aux;
    int parada;
    int lado;
    int i = 0;
    MPI_Status st;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    lado = sqrt(size);

    /* RANK 0 
    *************************/
    if(rank == 0) {
        double datos[size];
        double n;
        FILE *fp; 
        parada = 0;

        printf("Red toroidal de %d procesos\n", size);
        /* Verificacion de procesos */
        if((lado*lado) != size) {
            printf("Error: Numero de procesos incorrecto\n");
            parada = 1;
        } else {
            /* Verificacion del archivo de datos */
            fp = fopen(DOTDAT, "r");
            if(fp == NULL) {
                printf("Error: Archivo \"%s\" no encontrado\n", DOTDAT);
                parada = 1;
            } else {
                while(!feof(fp) && i<size) {
                    /* lee double seguido de char (que ignora) */
                    if(fscanf(fp, "%lf%*c", &n) == 1) {
                        datos[i] = n;
                        i++;
                    }
                }
                fclose(fp);

                /* Verificacion de numero de valores */
                if(i != size) {
                    printf("Error: %d valores para %d procesos\n", i, size);
                    parada = 1;
                }
            }
        }
        /* Envio de control */
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
    } /* Fin rank 0 */
    
    /* Todos los procesos 
    *************************/
    MPI_Recv(&parada, 1, MPI_INT, 0, CTRL, MPI_COMM_WORLD, &st);
    if(parada == 0) {
        MPI_Recv(&valor, 1, MPI_DOUBLE, 0, DATA, MPI_COMM_WORLD, &st);
        printf("[%d] Recibido: %.3f\n", rank, valor);
        
        /* Cominucacion Norte-Sur */
        for(i=1;i<lado;i++) {
            MPI_Send(&valor, 1, MPI_DOUBLE, vecino(NORTE, lado, rank), 
                    DATA, MPI_COMM_WORLD);
            MPI_Recv(&aux,   1, MPI_DOUBLE, vecino(SUR,   lado, rank), 
                    DATA, MPI_COMM_WORLD, &st);
            if(aux < valor) valor = aux;
        }

        /* Comunicacion Este-Oeste */
        for(i=1;i<lado;i++) {
            MPI_Send(&valor, 1, MPI_DOUBLE, vecino(ESTE,  lado, rank), 
                    DATA, MPI_COMM_WORLD);
            MPI_Recv(&aux,   1, MPI_DOUBLE, vecino(OESTE, lado, rank), 
                    DATA, MPI_COMM_WORLD, &st);
            if(aux < valor) valor = aux;
        }
        /*if (rank == 0)*/
            printf("[%d] Minimo: %.3f\n", rank, valor);
    }
    
    MPI_Finalize();
	
    return 0;
}
/* id+2^(d-1)   XOR    1 << i-1 */
int vecino(int dir, int lado, int id) {
    int v = -1;
    switch(dir) {
        case NORTE:
            v = (id+lado)%(lado*lado); break;
        case SUR:
            v = (id-lado)%(lado*lado);
            if(v < 0) v += lado*lado; break;
        case ESTE:
            v = (id+1)%lado+lado*(id/lado); break;
        case OESTE:
            v = (id-1)%lado+lado*(id/lado); 
            if(v < 0) v += lado; break;
    }
    return v;
}

