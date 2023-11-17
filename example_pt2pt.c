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
  
  int out_dims[ndims]    = {0};
  int out_periods[ndims] = {0};
  int new_coods[dims]    = {0};

  int new_rank, new_src, new_dst;
  int num_ops = 2;
  MPI_Request req_array[num_ops];

  
  MPI_Cart_get(topo_comm,ndims,out_dims,out_periods,new_coords);  
  MPI_Cart_rank(topo_comm,new_coords,&new_rank);
  MPI_Cart_shift(topo_comm, 0, 1, &new_src, &new_dst);  //shift on X-dimension
  
  //all processes send an int to their right neighbor on the X direction
#ifdef COUNT_DEFERRED
  MPI_Send_init(MPI_BUFFER_DEFERRED,MPI_COUNT_DEFERRED,MPI_INT,new_dst,111,topo_comm,req_array[0]);
#else
  MPI_Send_init(MPI_BUFFER_DEFERRED,1,MPI_INT,new_dst,111,topo_comm,&req_array[0]);
#endif
  
  //all processes recv an int from their left neighbor on the X direction
#ifdef COUNT_DEFERRED
  MPI_Recv_init(MPI_BUFFER_DEFERRED,MPI_COUNT_DEFERRED,MPI_INT,new_src,111,topo_comm,&req_array[1]);
#else
  MPI_Recv_init(MPI_BUFFER_DEFERRED,1,MPI_INT,new_src,111,topo_comm,&req_array[1]);
#endif
  
  // Wait for comm effective creation
  MPI_Wait(&topo_request, MPI_STATUS_IGNORE);

  // Recompute as reordering might have modified the ranks
  MPI_Cart_get(topo_comm,ndims,out_dims,out_periods,new_coords);
  MPI_Cart_rank(topo_comm,new_coords,&new_rank);
  MPI_Cart_shift(topo_comm, 0, 1, &new_src, &new_dst);

  int send_buffer = 100 + new_rank;
  int recv_buffer = 0;

  //MPI_Params_register(MPI_Request req, int addr_array_size, void *addr_array, int count_array_size, int *count_array);

  int count = 1;
  void *add_array[count] = {NULL};
#ifdef COUNT_DEFERRED
  int c_array[count] = 1;
#end
  
  add_array[0] = &send_buffer;  
#ifdef COUNT_DEFERRED
  MPI_Params_register(send_request, count, add_array, count, c_array);
#else
  MPI_Params_register(send_request, count, add_array, 0, NULL);
#endif
  
  add_array[0] = &recv_buffer;
#ifdef COUNT_DEFERRED
  MPI_Params_register(recv_request, count, add_array, count, c_array);
#else
  MPI_Params_register(recv_request, count, add_array, 0, NULL);
#endif
  
  MPI_Startall(num_ops, req_array);
  

  // Do some  work here 

  
  MPI_Waitall(2, req_array);

  for(int i  = 0 ; i < num_ops ; i++)
    MPI_Request_free(req_array + i);
    
  
  MPI_Comm_disconnect(topo_comm);
  
  MPI_Finalize();
  
  exit(EXIT_SUCCESS);  
}
