#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#define true 1
#define false 0

typedef struct csr{

	int *row_ptr;
	int *col_ptr;
	float *val;
	int size;
} csr;

void parsec_roi_begin() 
{

}

void parsec_roi_end() 
{

}

csr csr_generator(int size,int nnz ){

	csr csr_out;
	int total_nnz = 0 ;
	srand(time(NULL));
	csr_out.row_ptr = (int *) malloc((size+1)*sizeof(int));
	csr_out.col_ptr = (int *) malloc(5*sizeof(int));
	csr_out.val = (float*) malloc(5*sizeof(float));

	printf("Create CSR with size %d and sparsity %d\n", size, nnz);
	csr_out.size = size;
	int index; 
	int last_nnz = 0;
	int nnz_row = 0;
	int i = 0; 
	int j = 0;
	int *visited;
	int *temp;
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
	printf("Read matrix %s\n",path);
	FILE *fd;
	fd = fopen(path,"r");

	int rows, columns, nnz;

	fscanf(fd,"%d",&rows);
	fscanf(fd,"%d",&nnz);
	printf("Rows = %d Non-zero = %d \n",rows,nnz);

	csr csr_out;
	int total_nnz = 0 ;
	
	csr_out.row_ptr = (int *) malloc((rows+1)*sizeof(int));
	csr_out.col_ptr = (int *) malloc(nnz*sizeof(int));
	csr_out.val = (float*) malloc(nnz*sizeof(float));
	csr_out.size = rows;

	int row,col;
	float value;
	for(int w = 0 ; w < rows + 1; w++)
	{
		fscanf(fd,"%d",&row);
		csr_out.row_ptr[w] = row;
	}
	for(int w= 0 ; w < nnz; w++)
	{	
		fscanf(fd,"%d",&col);	
		csr_out.col_ptr[w] = col;
	}
	for(int w= 0 ; w < nnz; w++)
	{
		fscanf(fd,"%f",&value);	
		csr_out.val[w] = value;
	}

	printf("Finished reading csr %s\n",path);
	return csr_out ;

}

static void sgemm_csr(int M, int N, int K, struct csr A, struct csr B, struct csr C){
	int n, k, m, nnz=0;
	int a_nnz_pr, b_nnz_pr;
	int nnz_C;
	int a_i, b_i;
	int c1,c2;
	float a, b, sum;
	int pointer_A, pointer_B;
	c1 = 0;
	c2 = 0;
	nnz_C = 0;
	printf("M = %d, N = %d \n", M, N);
	C.row_ptr = (int*)malloc((M+1)*sizeof(int));
	C.row_ptr[0] = 0;
	C.col_ptr = (int*)malloc(M*M*sizeof(int));
	for(m=0; m<M; m++){

		if( m == M/128) return;
		a_nnz_pr = A.row_ptr[m+1] - A.row_ptr[m];
		if(a_nnz_pr == 0 )		
		{	
			
			C.row_ptr[m+1] = C.row_ptr[m];
			continue;
		}
		c1++;
		for(n=0; n<N; n++){

			b_nnz_pr = B.col_ptr[n+1] - B.col_ptr[n];
			if(b_nnz_pr == 0){
				sum = 0;
				c2++;
			}
			else{
				nnz_C++;	    	
			

			C.row_ptr[m+1] = C.row_ptr[m];
			pointer_A = A.row_ptr[m];
			pointer_B = B.col_ptr[n];
			sum = 0;
			int c=0;
			for(int i = 0; i < a_nnz_pr;i++){
				if( A.col_ptr[pointer_A]  == B.row_ptr[pointer_B] ){
					sum += A.val[pointer_A]*B.val[pointer_B]; 
					pointer_A++;	
				}
				else if( A.col_ptr[pointer_A] >  B.row_ptr[pointer_B] && b_nnz_pr!=1 ){
					
				
					do {
						c++;
						pointer_B++;
					}
					while( (A.col_ptr[pointer_A] >  B.row_ptr[pointer_B]) && b_nnz_pr >= c );


				}
				else if( A.col_ptr[pointer_A] <  B.row_ptr[pointer_B] ){

					pointer_A++;

				 }
				}
			}

			if( sum != 0 ){
				C.row_ptr[m+1]++;
				C.col_ptr[nnz_C]=n;
			}      
		}
	}

	printf("A had %d non zero rows \n", c1 );
	printf("B had %d non zero rows \n", c2 );

}


int main(int argc, char **argv) {

	printf("Compute SGEMM using csr\n");
	csr A,B,C;



	A = csr_generator(10, 5); //read_csr(argv[1]);
        B = csr_generator(10, 3); //read_csr(argv[2]); 
	printf("Matrix A and B is ready");
	
	parsec_roi_begin();
	sgemm_csr(A.size,A.size,A.size,A,B,C);
	parsec_roi_end();
}
