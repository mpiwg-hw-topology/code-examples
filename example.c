#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>


int main(int rgc, char *argv[])
{
  int res;
  int myrank, mysize;

  MPI_Init(NULL,NULL);

  MPI_Comm_size(MPI_COMM_WORLD, &mysize);
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  
  MPI_Comm my_cart_comm; // normal MPI

  #if USE_WAIT
  MPI_Request comm_create_request; // normal MPI
  int reorder = 1;
  MPI_Icart_create(..., reorder, ..., &my_cart_comm, &comm_create_request); //new
  #else
  MPI_Icart_create(..., reorder, ..., &my_cart_comm); //new
  #endif

  MPI_Neighor_alltoallw_init(..., my_cart_comm, &halo_exchange_request); // normal MPI
  MPI_Allreduce_init(..., my_cart_comm, &reduction_request); // normal MPI
  
  #if USE_WAIT
  MPI_Wait(&comm_create_request, MPI_STATUS_IGNORE); //new
  #else
  MPI_Comm_commit(&my_cart_comm, MPI_INFO_NULL); //new
  #endif
  
  // normal MPI
  MPI_Start(&halo_exchange_request);
  MPI_Start(&reduction_request);
  MPI_Wait(&halo_exchange_request);
  MPI_Wait(&reduction_request);
  MPI_Request_free(&halo_exchange_request);
  MPI_Request_free(&reduction_request);

  MPI_Comm_disconnect(&my_cart_comm);

  
  MPI_Topology virtual_topo;
  MPI_Topology hw_topo;

  /*1- build virtual topo */
  MPI_Topo_create(MPI_CART, mysize, myrank,  &virtual_topo);
  /* Topo types: CART / DIST_GRPAH/ OTHER ? */ 
  /* Question: what about other parameters: - periodicity
                                            - number of dimensions
                                            - euclidian/metric/elliptic space
                                            - metric (if applicable)
  */

  /*2- get hw topo from COMM_WORLD */
  MPI_Comm_topo_get(MPI_COMM_WORLD, MPI_HW_TOPO, &hw_topo);
  /* TOPO TYPES: MPI_HW_TOPO / MPI_VIRT_TOPO */
  /* Question: several virtual topologies could be attached to a communicator */
  
  /*3- Map virtual topo onto hw one */
  
  int flag, quality;
  int *mapping_res;

  MPI_Topo_embedd(virtual_topo, hw_topo, mapping_res, &flag, &quality);
  /* Question: what should be the type of mapping_res?? */


  
  MPI_Finalize();
  
  exit(EXIT_SUCCESS);  
}
