#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "csr.h"
#define true 1
#define false 0

void parsec_roi_begin() 
{

}

void parsec_roi_end() 
{

}

static void sgemm_csr(int M, int N, int K, struct csr A, struct csr B, struct csr C){
	int n, m;
	int a_nnz_pr, b_nnz_pr;
	int nnz_C;
	int c1,c2;
	float sum;
	int pointer_A, pointer_B;
	c1 = 0;
	c2 = 0;
	nnz_C = 0;
	printf("M = %d, N = %d \n", M, N);
	C.row_ptr = (int*)malloc((M+1)*sizeof(int));
	C.row_ptr[0] = 0;
	C.col_ptr = (int*)malloc(M*M*sizeof(int));
	for(m=0; m<M; m++){

		//if( m == M/128) return;
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

	//printf("A had %d non zero rows \n", c1 );
	//printf("B had %d non zero rows \n", c2 );

}


int main(int argc, char **argv) {

	//printf("Compute SGEMM using csr\n");
	csr A,B,C;



	//A = csr_generator(10, 5); //read_csr(argv[1]);
    //B = csr_generator(10, 3); //read_csr(argv[2]); 
	A = read_csr(argv[1]);
    B = read_csr(argv[2]); 
	//printf("Matrix A and B is ready\n");
	
	parsec_roi_begin();
	sgemm_csr(A.size,A.size,A.size,A,B,C);
	parsec_roi_end();
}
