---------------------------------------------------------------------------------------------------------------------------------------------------
NO OVERLAP CASE :
---------------------------------------------------------------------------------------------------------------------------------------------------
                   Collective on input communicator                     |                   Collective on output communicator 
---------------------------------------------------------------------------------------------------------------------------------------------------
                                                                        |
MPI_Comm newcomm;                                                       | MPI_Comm newcomm;
MPI_Info info;                                                          | MPI_Info info;
int      rank;                                                          | int      rank; 
                                                                        |
MPI_Comm_rank(MPI_COMM_WORLD, &rank);                                   | MPI_Comm_rank(MPI_COMM_WORLD, &rank);
MPI_Info_create(&info);                                                 | MPI_Info_create(&info);
MPI_Info_set(info,"mpi_pset_name","app://ocean" *or* "app://atmos");    | MPI_Info_set(info,"mpi_pset_name","app://ocean" *or* "app://atmos");
                                                                        |
                                                                        |
/* processes in OCEAN or ATMOS need to call: */                         | /* processes in OCEAN or ATMOS need to call: */
MPI_Comm_split_type(MPI_COMM_WORLD,                                     | MPI_Comm_split_type(MPI_COMM_WORLD,
                    MPI_COMM_TYPE_RESOURCE_GUIDED *OR* MPI_UNDEFINED,   |                     MPI_COMM_TYPE_RESOURCE_GUIDED *OR* MPI_UNDEFINED,
		    rank, info, &newcomm);                              |                     rank, info, &newcomm); 
		                                                        |
/* All other processes need to call : */                                |
MPI_Comm_split_type(MPI_COMM_WORLD,                                     |
                    MPI_UNDEFINED,                                      |
		    rank, MPI_INFO_NULL, &newcomm);                     |
                                                                        |
---------------------------------------------------------------------------------------------------------------------------------------------------





















---------------------------------------------------------------------------------------------------------------------------------------------------
OVERLAP CASE 1 :
---------------------------------------------------------------------------------------------------------------------------------------------------
                   Collective on input communicator                     |                   Collective on output communicator 
---------------------------------------------------------------------------------------------------------------------------------------------------
                                                                        |
MPI_Comm newcomm, newcomm2;                                             | MPI_Comm newcomm, newcomm2;
MPI_Info info, info2;                                                   | MPI_Info info, info2;
int      rank;                                                          | int      rank; 
                                                                        |
MPI_Comm_rank(MPI_COMM_WORLD, &rank);                                   | MPI_Comm_rank(MPI_COMM_WORLD, &rank);
                                                                        |
MPI_Info_create(&info);                                                 | MPI_Info_create(&info);
MPI_Info_set(info,"mpi_pset_name","app://ocean");                       | MPI_Info_set(info,"mpi_pset_name","app://ocean");
                                                                        |
MPI_Info_create(&info2);                                                | MPI_Info_create(&info2);
MPI_Info_set(info2,"mpi_pset_name","app://atmos");                      | MPI_Info_set(info2,"mpi_pset_name","app://atmos");
                                                                        |
									|
/* processes in OCEAN *AND* ATMOS need to call: */                      | /* processes in OCEAN *AND* ATMOS need to call: */
MPI_Comm_split_type(MPI_COMM_WORLD,                                     | MPI_Comm_split_type(MPI_COMM_WORLD,
                    MPI_COMM_TYPE_RESOURCE_GUIDED *OR* MPI_UNDEFINED,   |                     MPI_COMM_TYPE_RESOURCE_GUIDED *OR* MPI_UNDEFINED,
		    rank, info, &newcomm);                              |                     rank, info, &newcomm); 
MPI_Comm_split_type(MPI_COMM_WORLD,                                     | MPI_Comm_split_type(MPI_COMM_WORLD,
                    MPI_COMM_TYPE_RESOURCE_GUIDED *OR* MPI_UNDEFINED,   |                     MPI_COMM_TYPE_RESOURCE_GUIDED *OR* MPI_UNDEFINED,
		    rank, info2, &newcomm2);                            |                     rank, info2, &newcomm2);
                                                                        |
									| 
/* All other processes in OCEAN need to call: */                        |/* All other processes in OCEAN need to call: */
MPI_Comm_split_type(MPI_COMM_WORLD,                                     | MPI_Comm_split_type(MPI_COMM_WORLD,
                    MPI_COMM_TYPE_RESOURCE_GUIDED *OR* MPI_UNDEFINED,   |                     MPI_COMM_TYPE_RESOURCE_GUIDED *OR* MPI_UNDEFINED,
		    rank, info, &newcomm);                              |                     rank, info, &newcomm); 
MPI_Comm_split_type(MPI_COMM_WORLD,                                     | 
                    MPI_UNDEFINED,                                      | 
		    rank, MPI_INFO_NULL, &newcomm2);                    | 
                                                                        |
                                                                        |
/* All other processes in ATMOS need to call: */                        |/* All other processes in ATMOS need to call: */
MPI_Comm_split_type(MPI_COMM_WORLD,                                     |
                    MPI_UNDEFINED,                                      |                    
		    rank, MPI_INFO_NULL, &newcomm);                     |                     
MPI_Comm_split_type(MPI_COMM_WORLD,                                     | MPI_Comm_split_type(MPI_COMM_WORLD,
                    MPI_COMM_TYPE_RESOURCE_GUIDED *OR* MPI_UNDEFINED,   |                     MPI_COMM_TYPE_RESOURCE_GUIDED *OR* MPI_UNDEFINED,
		    rank, info2, &newcomm2);                            |                     rank, info2, &newcomm2); 
                                                                        |
/* All other processes need to call : */                                | /* All other processes need to call : */                                
MPI_Comm_split_type(MPI_COMM_WORLD,                                     |
                    MPI_UNDEFINED,                                      |
		    rank, MPI_INFO_NULL, &newcomm);                     | 
MPI_Comm_split_type(MPI_COMM_WORLD,                                     |
                    MPI_UNDEFINED,                                      |
		    rank, MPI_INFO_NULL, &newcomm2);                    |		    
                                                                        |									
---------------------------------------------------------------------------------------------------------------------------------------------------













---------------------------------------------------------------------------------------------------------------------------------------------------
OVERLAP CASE 2 :
---------------------------------------------------------------------------------------------------------------------------------------------------
                   Collective on input communicator                     |                   Collective on output communicator 
---------------------------------------------------------------------------------------------------------------------------------------------------
                                                                        |
                                                                        | MPI_Comm newcomm, newcomm2, newcomm3;
                                                                        | MPI_Info info, info2, info3;
                                                                        | int      rank; 
                                                                        |
                                                                        | MPI_Comm_rank(MPI_COMM_WORLD, &rank);
                                                                        |
                                                                        | MPI_Info_create(&info);
                                                                        | MPI_Info_set(info,"mpi_pset_name","app://ocean");
                                                                        |
                                                                        | MPI_Info_create(&info2);
                                                                        | MPI_Info_set(info2,"mpi_pset_name","app://atmos");
                                                                        |
                                                                        | MPI_Info_create(&info3);
                                                                        | MPI_Info_set(info2,"mpi_pset_name","app://land");
                                                                        |
									|
                                                                        | /* processes in OCEAN *AND* ATMOS *AND* LAND need to call: */
                                                                        | MPI_Comm_split_type(MPI_COMM_WORLD,
                                                                        |                     MPI_COMM_TYPE_RESOURCE_GUIDED *OR* MPI_UNDEFINED,
  /* MPI_Comm_split_type needs to be called 3 times per process */      |                     rank, info, &newcomm); 
                                                                        | MPI_Comm_split_type(MPI_COMM_WORLD,
                                                                        |                     MPI_COMM_TYPE_RESOURCE_GUIDED *OR* MPI_UNDEFINED,
		                                                        |                     rank, info2, &newcomm2);
                                                                        | MPI_Comm_split_type(MPI_COMM_WORLD,
                                                                        |                     MPI_COMM_TYPE_RESOURCE_GUIDED *OR* MPI_UNDEFINED,
		                                                        |                     rank, info3, &newcomm3);|
									| 
                                                                        |/* All other processes in OCEAN need to call: */
                                                                        | MPI_Comm_split_type(MPI_COMM_WORLD,
                                                                        |                     MPI_COMM_TYPE_RESOURCE_GUIDED *OR* MPI_UNDEFINED,
		                                                        |                     rank, info, &newcomm); 
                                                                        |
                                                                        |/* All other processes in ATMOS need to call: */
                                                                        | MPI_Comm_split_type(MPI_COMM_WORLD,
                                                                        |                     MPI_COMM_TYPE_RESOURCE_GUIDED *OR* MPI_UNDEFINED,
		                                                        |                     rank, info2, &newcomm2); 
                                                                        |
                                                                        |/* All other processes in LAND need to call: */
                                                                        | MPI_Comm_split_type(MPI_COMM_WORLD,
                                                                        |                     MPI_COMM_TYPE_RESOURCE_GUIDED *OR* MPI_UNDEFINED,
		                                                        |                     rank, info3, &newcomm3); 
                                                                        |
---------------------------------------------------------------------------------------------------------------------------------------------------
