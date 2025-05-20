#include <iostream>
#include <mpi.h>


using namespace std;

int main(int argc, char* argv[]){

    int numProcs;
    int myRank;
    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

    //if worker
    if(myRank > 0){

        cout<<"Worker running, Rank: "<<myRank<<endl;
    }
    else{
        cout<<"Main running, there are a total of "<<numProcs<<" processes."<<endl;
        
    }






    MPI_Finalize();
    return 0;
}