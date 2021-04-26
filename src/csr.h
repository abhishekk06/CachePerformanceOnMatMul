#include <time.h>
#include <stdlib.h>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include<iostream>
#include <deque>
#include <assert.h>


using namespace std;

typedef struct csr{

	int *row_ptr;
	int *col_ptr;
	float *val;
	int size;
} csr;

csr csr_generator(int size,int nnz ){

	csr csr_out;
	int total_nnz = 0 ;
	srand(time(NULL));
	csr_out.row_ptr = (int *) malloc((size+1)*sizeof(int));
	csr_out.col_ptr = (int *) malloc(5*sizeof(int));
	csr_out.val = (float*) malloc(5*sizeof(float));

	//printf("Create CSR with size %d and sparsity %d\n", size, nnz);
	csr_out.size = size;
	int index; 
	int last_nnz = 0;
	int nnz_row = 0;
	int i = 0; 
	int j = 0;
	int *visited;
	int max;
	int min;
	for( i = 0; i < size; i++)
	{   


		last_nnz += nnz_row;

		max = (int)((nnz + nnz/10.0)/100.0 * size);
		min = (int)((nnz - nnz/10.0)/100.0 * size);
		nnz_row = rand()%max + min ;
		total_nnz += nnz_row; 
		printf("Total_nnz = %d \n",total_nnz);
		csr_out.col_ptr = (int *)realloc(csr_out.col_ptr, sizeof(int)*total_nnz);
		csr_out.val = (float *)realloc(csr_out.val, sizeof(float)*total_nnz);
		visited = (int *)malloc(sizeof(int)*size);

		for(int w =0 ; w < size; w++) visited[w] = 0;
		for ( j=0; j < nnz_row ; j++){
			csr_out.val[last_nnz+j] = 1;
			do{
				index = rand()%size;
			}while(visited[index]);

			visited[index] = true;
			csr_out.col_ptr[last_nnz+j] = index;
		    printf("Column = %d \n", index);
		}   
		csr_out.row_ptr[i+1] = csr_out.row_ptr[i]+nnz_row;
		free(visited);    
	}


	return csr_out;

}

csr read_csr(char *path)
{
	//printf("Read matrix %s\n",path);
	FILE *fd;
	fd = fopen(path,"r");
        if (fd == NULL) perror ("Error opening file");
	int rows, nnz;
    deque <int> row_entry;
    deque <int> col_entry;
    deque <int> val_entry;
    int cummulative_row_val = 0;
    int prev_val = -1;
	
	std::ifstream f(path);
	std::string line;
	long num_line;
	for (num_line = 0; std::getline(f, line); ++num_line)
    {
        //cout<<line<<endl;
        if(num_line) {
            // Populate row dq
            unsigned first = line.find("Row: ");
            unsigned last = line.find(", Col:");
            string row = line.substr(first+5, last-5-first);
            int substr_val = atoi(row.c_str());
            //cout<<"Row:"<<substr_val<<endl;
            if(num_line == 1)
            {
                prev_val = substr_val;
            }
            if(prev_val != substr_val) {
                prev_val = substr_val;
                row_entry.push_back(cummulative_row_val);
            } 

            //Increase the number of NZA values     
            cummulative_row_val++;

            // Populate col dq    
            first = line.find(", Col: ");
            last = line.find(", Val:");
            string col = line.substr((first+7),last-7-first);
            substr_val = atoi(col.c_str());
            //cout<<"Col:"<<substr_val<<endl;
            col_entry.push_back(substr_val);
    
            // Populate val dq
            first = line.find(", Val: ");
            last = line.find(".");
            string val = line.substr((first+7),last-7-first);
            substr_val = atoi(val.c_str());
            //cout<<"Val:"<<substr_val<<endl;
            val_entry.push_back(substr_val);
        } else {
	        fscanf(fd,"%d",&rows);
                row_entry.push_back(0);
        }
    }
	//fscanf(fd,"%d",&rows);
	//fscanf(fd,"%d",&nnz);
    row_entry.push_back(cummulative_row_val);
    nnz = num_line - 1;
	//printf("Rows = %d Non-zero = %d \n",rows, nnz);
    //cout<<"Row dq size "<<row_entry.size()<<endl;
    //cout<<"Col dq size "<<col_entry.size()<<endl;
    //cout<<"Val dq size "<<val_entry.size()<<endl;
    //assert ((int)row_entry.size()==rows);
    assert ((int)col_entry.size()==nnz);
    assert ((int)val_entry.size()==nnz);

	csr csr_out;
	
	csr_out.row_ptr = (int *) malloc((rows+1)*sizeof(int));
	csr_out.col_ptr = (int *) malloc(nnz*sizeof(int));
	csr_out.val = (float*) malloc(nnz*sizeof(float));
	csr_out.size = rows;

	//int row,col;
	//float value;

	for(int w = 0 ; w < rows + 1; w++)
	{
		//fscanf(fd,"%d",&row);
        //cout<<" Row: "<<row_entry.front()<<endl;
		csr_out.row_ptr[w] = row_entry.front();
        row_entry.pop_front();
	}
	for(int w= 0 ; w < nnz; w++)
	{	
        //cout<<" Col: "<<col_entry.front()<<endl;
		//fscanf(fd,"%d",&col);	
		csr_out.col_ptr[w] = col_entry.front();
        col_entry.pop_front();
	}
	for(int w= 0 ; w < nnz; w++)
	{
        //cout<<" Val: "<<val_entry.front()<<endl;
		//fscanf(fd,"%f",&value);	
		csr_out.val[w] = val_entry.front();
        val_entry.pop_front();
	}

	//printf("Finished reading csr %s\n",path);
	return csr_out ;

}
