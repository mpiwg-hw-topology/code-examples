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
  // rather than MPI_Icart_create(MPI_COMM_WORLD, ndims, dims, periods, reorder, &topo_comm, &request); ??

  // The naming scheme is likely to be a local operation, but the communicator creation is likely to be
  // non-local.
  // However, in case of reorder, some information might need to be exchanged then non-local?
  
  // The list/kind of operations/procedures that can use the topo_comm handle just
  // after this call returns must be determined with care.
  // Basically, the handle can be used in incomplete procedures calls.
  // or in procedures that handle the naming scheme, e.g:
  // - MPI_Cart_get: info about topology + coords of calling process
  // - MPI_Cart_rank : cartesian coords -> rank
  // - MPI_Cart_coords : linear rank -> coords 

  // Since reordering is enabled, processes need to get their new ranks/coords.

  // Hardware topologies can be leveraged at this step already.

  // However, scheduled communcations cannot.
  
  int out_dims[ndims] = {0};
  int out_periods[ndims] = {0};
  int new_coods[dims] = {0};
 
  MPI_Cart_get(topo_comm,ndims,out_dims,out_periods,new_coords);

  int new_rank;

  MPI_Cart_rank(topo_comm,new_coords,&new_rank);

  fprintf(stdout,(new_rank == 0) ?
	  "I'm root process in cart comm (rank in COMM_WORLD %i)\n" :
	  "Not root process (rank in COMM_WORLD %i)\n",rank);
  
  // Schedule a first operation, e.g a Bcast
  // in this example (0,0) will send an integer to processes E (1, 0), W (size/Y_DIMS-1, 0), N (0, 1) and S (0, DIMS_Y-1) 
  // If the naming scheme restricts the links effectively usable,  what does this bcast mean 
  // for other processes?
  // Does that make sense at all?
  
  /*
   Example with 12 processes in a 4 X 3 grid, with periodicty 1 on X and Y dims.   

   S
   * --- * --- * --- *
   |     |     |     |
    
 N * --- * --- * --- *
   |     |     |     |
    
   * --- * --- * --- * 
  Root   E           W

  */

  
  MPI_Request bcast_request;
  int data = 0;

  if( !new_rank )
    data = 333;
    
  MPI_Bcast_init(&data,1,MPI_INT,0,topo_comm,MPI_INFO_NULL,&bcast_request);

  int data2 = 1;
  int result = 0;
  
  // Schedule another op.
  // 
  MPI_Allreduce_init(&data2, &result, 1, MPI_INT, MPI_SUM, topo_comm, MPI_INFO_NULL, &reduction_request); 
    
  
  // Actual communicator creation, definitely a non-local op.
  // Communication schedules can be taken into account at this moment.
  MPI_Wait(&topo_request, MPI_STATUS_IGNORE);
  //after MPI_Wait returns/completes, the communicator handle can be fully used.
  //Do we want to authorize other non-scheduled ops on the communicator.
  //If not, only non-blocking/immediate and/or persistent communication operations
  //can be supported?
  

  MPI_Start(&bcast_request);
  MPI_Start(&reduction_request);


  // Do some  work here 

  
  MPI_Wait(&bcast_request);
  MPI_Wait(&reduction_request);

  MPI_Request_free(&bcast_request);
  MPI_Request_free(&reduction_request);
  
  
  // authorize non-scheduled comms to occur on topo_comm??
  // ==> means non optimized



  
  MPI_Comm_disconnect(topo_comm);
  
  MPI_Finalize();
  
  exit(EXIT_SUCCESS);  
}
