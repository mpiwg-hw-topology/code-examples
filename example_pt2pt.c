#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define Y_DIM 3

int main(int rgc, char *argv[])
{
  int res;
  int rank, size;

  MPI_Init(NULL,NULL);

  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  fprintf(stdout,"Process %i out of %i\n",rank,size);
  assert(size%DIMS_Y == 0);
  
  MPI_Comm topo_comm;
  MPI_Request topo_request;

  // Request the creation of a 2-D Torus 
  // More a naming scheme than a real topology 
  int ndims = 2;
  int dims[ndims];
  dims[0] = size/Y_DIM;
  dims[1] = Y_DIM;  
  int periods[ndims] = {1, 1};
  int reorder = 1; 
                   

  // Naming scheme creation 
  MPI_Icart_coords_create(MPI_COMM_WORLD, ndims, dims, periods, reorder, &topo_comm, &topo_request); // new api 
  
  int out_dims[ndims] = {0};
  int out_periods[ndims] = {0};
  int new_coods[dims] = {0};
 
  MPI_Cart_get(topo_comm,ndims,out_dims,out_periods,new_coords);
    
  MPI_Request send_request;
  MPI_Request recv_request;
  int new_rank;
  int new_src;
  int new_dst;
  
  MPI_Cart_rank(topo_comm,new_coords,&new_rank);

  //shift on X-dimension
  MPI_Cart_shift(topo_comm, 0, 1, &new_src, &new_dst);

  //all processes send an int to their right neighbor on the X direction
  MPI_Send_init(MPI_BUFFER_DEFERRED,1,MPI_INT,new_dst,111,topo_comm,&send_request);
  //all processes recv an int from their left neighbor on the X direction
  MPI_Recv_init(MPI_BUFFER_DEFERRED,1,MPI_INT,new_src,111,topo_comm,&recv_request);

  // Wait for comm effective creation
  MPI_Wait(&topo_request, MPI_STATUS_IGNORE);

  // Recompute as reordering might have modified the ranks
  MPI_Cart_rank(topo_comm,new_coords,&new_rank);
  MPI_Cart_shift(topo_comm, 0, 1, &new_src, &new_dst);

  int send_buffer = 100 + new_rank;
  int recv_buffer = 0;
  
  MPI_Addr_register(&send_buffer,send_request);
  MPI_Addr_register(&recv_buffer,recv_request);
  
  MPI_Start(&send_request);
  MPI_Start(&recv_request);

  // Do some  work here 

  
  MPI_Wait(&send_request);
  MPI_Wait(&recv_request);

  MPI_Request_free(&send_request);
  MPI_Request_free(&recv_request);
  
  MPI_Comm_disconnect(topo_comm);
  
  MPI_Finalize();
  
  exit(EXIT_SUCCESS);  
}
