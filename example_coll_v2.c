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
  int reorder = 1; // allow for "intial" processes to be reordered b/c "Row-major
                   // numbering is always used for the processes in a Cartesian
                   // structure"
                   // Is reorder useful for other graph naming schemes then? (e.g
                   // MPI_Igraph_create or MPI_Idist_graph_create ) 

  // Naming scheme creation 
  MPI_Icart_coords_create(MPI_COMM_WORLD, ndims, dims, periods, reorder, &topo_comm, &topo_request); // new api 
  
  int out_dims[ndims] = {0};
  int out_periods[ndims] = {0};
  int new_coods[dims] = {0};
  int new_rank;
  
  MPI_Cart_get(topo_comm,ndims,out_dims,out_periods,new_coords);
  MPI_Cart_rank(topo_comm,new_coords,&new_rank);

  fprintf(stdout,(new_rank == 0) ?
	  "I'm root process in cart comm (rank in COMM_WORLD %i)\n" :
	  "Not root process (rank in COMM_WORLD %i)\n",rank);
  
    
  MPI_Request bcast_request;
    
  MPI_Bcast_init(MPI_BUFFER_DEFERRED,MPI_COUNT_DEFERRED,MPI_INT,0,topo_comm,MPI_INFO_NULL,&bcast_request);

  //MPI_Request reduction_request;

  // Schedule another op.
  // 
  //MPI_Allreduce_init(MPI_BUFFER_DEFERRED, MPI_BUFFER_DEFERRED, 1, MPI_INT, MPI_SUM, topo_comm, MPI_INFO_NULL, &reduction_request); 
    

  MPI_Wait(&bcast_request, MPI_STATUS_IGNORE);

  MPI_Cart_get(topo_comm,ndims,out_dims,out_periods,new_coords);
  MPI_Cart_rank(topo_comm,new_coords,&new_rank);

  int count = 1;
  void *add_array[count] = {NULL};
  int data = 0;

  if( !new_rank )
    data = 333;

  add_array[0] = &data;

  MPI_Params_register(bcast_request, count, add_array, 0, NULL);

  MPI_Start(&bcast_request);
  //MPI_Start(&reduction_request);


  // Do some  work here 

  
  MPI_Wait(&bcast_request);
  // MPI_Wait(&reduction_request);

  MPI_Request_free(&bcast_request);
  //MPI_Request_free(&reduction_request);
  


  
  MPI_Comm_disconnect(topo_comm);
  
  MPI_Finalize();
  
  exit(EXIT_SUCCESS);  
}
