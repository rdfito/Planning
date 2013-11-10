#include <stdio.h>
#include <math.h>
#include <mpi.h>

#define DOTDAT "datos.dat"

#define CTRL 0
#define DATA 1

int vecino(int, int);

int main(int argc, char *argv[]) {
    int rank, size;
    double valor;
    double aux;
    int parada;
    int dimen;
    int i = 0;
    MPI_Status st;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    dimen = log2(size);

    /* RANK 0 
    *************************/
    if(rank == 0) {
        double datos[size];
        double n;
        FILE *fp; 
        parada = 0;

        printf("Red hipercubo de %d procesos\n", size);
        /* Verificacion de procesos */
        if(pow(2, dimen) != size) {
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
        printf("[%d] Recibido: %f\n", rank, valor);

        for(i=0;i<dimen;i++) {
        /* Cominucacion vecinal */
            MPI_Send(&valor, 1, MPI_DOUBLE, vecino(rank, i), 
                    DATA, MPI_COMM_WORLD);
            MPI_Recv(&aux,   1, MPI_DOUBLE, vecino(rank, i), 
                    DATA, MPI_COMM_WORLD, &st);
            if(aux > valor) valor = aux;
        }
        /*if (rank == 0)*/
            printf("[%d] Maximo: %f\n", rank, valor);
    }
    
    MPI_Finalize();
    
    return 0;
}

int vecino(int id, int eje) {
    /* id XOR (1 desplazado eje posiciones) */

    return id ^ (1 << eje);
}

