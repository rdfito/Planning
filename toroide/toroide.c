#include <stdio.h>
#include <string.h>
#include <mpi.h>

#define BUFFSIZE 128
#define TAG 0
#define N 0
#define E 1 
#define S 2
#define W 3

int vecino(int dir, int lado, int id,);

int main(int argc, char *argv[]) {
	int idr, numr;  /* rank y size */
	char buff[BUFFSIZE];
    MPI_Status st;

    MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numr);
    MPI_Comm_rank(MPI_COMM_WORLD, &idr);
    
    if(idr == 0) {
	    int datos[numr];
        int i = 0;
        FILE *fp; 
        
        printf("#%d: Red toroidal de %d procesadores.\n", idr, numr);
        fp = fopen("datos.dat", "r");
        if(fp == NULL) {
            printf("Error: Archivo de datos no encontrado\n");
        } else {
            int c;
            while(!feof(fp)) {
                if(fscanf(fp, "%d%*c", &c) == 1) {
                    if(i <= numr-1) {
                        datos[i] = c;
                    }
                    i++;
                }
            }
            fclose(fp);
            if(i != numr) {
                printf("Error: numero de datos incorrecto.\n");
                printf("  Procs:%d Datos:%d\n", numr, i);
                i=0;
                while(i<numr) {
                    MPI_Send(1, 1, MPI_INT, i, TAG, MPI_COMM_WORLD);
                }
            }
            else {
                i=0;
                while(i<numr) {
                    MPI_Send(0, 1, MPI_INT, i, TAG, MPI_COMM_WORLD);
                    sprintf(buff, "n:%d", datos[i]);
                    MPI_Send(buff, BUFFSIZE, MPI_CHAR, i, TAG, MPI_COMM_WORLD);
                    i++;
                }
            }
        }
    }
    MPI_Recv(buff, BUFFSIZE, MPI_CHAR, 0, TAG, MPI_COMM_WORLD, &st);
    printf("#%d: %s\n", idr, buff);

    for(i=1;i<lado;i++) {
        Send(vecino(N));
        Recv(vecino(S));
        comparar;
    }

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

