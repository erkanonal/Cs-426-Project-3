#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "util.h"
#include <omp.h>

//function prototypes
//Creates a histogram for image given by int **img and returns histogram as int * hist
void create_histogram(int * hist, int ** img, int num_rows, int num_cols);

//Finds the distance between two vectors
double distance(int * a, int *b, int size);

//Finds the closest histogram for test image's histogram from training set histograms
//Returns person id of the closest histogram
int find_closest(int ***training_set, int num_persons, int num_training, int size, int *test_image);
char* getFilename(int person_id, int photo_id);
char* getFilenameExt(int per_id, int pho_id);

int k = 0;
int num_of_persons = 18;
int num_of_pics = 20;
int main(int argc, char ** argv){
	//omp_set_num_threads(8);
	if (argc < 2)
		return -1;
		
	k = atoi(argv[1]);
	double wtime = omp_get_wtime();
	printf("parallel...\n");
	//read files
	//allocate 3D
	int ***training_set;
	training_set = alloc_3d_matrix(18, 20, 256);
	initialize_3D(training_set, 18, 20, 256);

	#pragma omp parallel for 
	for(int i = 1; i < num_of_persons+1; i++){
		#pragma omp parallel for
		for(int j = 1; j < k+1; j++){	
			char *filename = getFilenameExt(i,j);
			//printf("%s\n", filename);
			int **image = read_pgm_file(filename, 180, 200);
	
			int *hist = malloc(sizeof(int)*256);
			for(int a = 0; a < 256; a++){
				hist[a] = 0;
			}
			create_histogram(hist, image, 180, 200);
			for(int a = 0; a < 256; a++){
				training_set[i-1][j-1][a] = hist[a];
			}
			free(hist);						
		}
	}
	int same = 0;
	#pragma omp parallel for 
	for(int i = 1; i < num_of_persons+1; i++){
		#pragma omp parallel for
		for(int j = k+1; j < num_of_pics+1; j++){	
			char *filename = getFilenameExt(i,j);
			//printf("%s\n", filename);
			int **image = read_pgm_file(filename, 180, 200);
		
			int *hist = malloc(sizeof(int)*256);
			for(int a = 0; a < 256; a++){
				hist[a] = 0;
			}
			create_histogram(hist, image, 180, 200);
			int close = find_closest(training_set, 18, k, 256, hist);
		
			if(close == i)
				same++;
			printf("%s %d %d\n", getFilename(i,j), close, i);
			free(hist);						
		}
	}	
	printf("Accuracy: %d correct answers for %d tests\n", same, (20-k)*18);	
	wtime = omp_get_wtime() - wtime;
	printf("Parallel time: %f\n", wtime);	
	return 0;
}

int find_closest(int ***training_set, int num_persons, int num_training, int size, int *test_image){
	double min_distance = distance(training_set[0][0], test_image, 256);
	double cur_distance = min_distance;
	int min_per_id = 0;

	for(int i = 0; i < num_persons; i++){
		for(int j = 0; j < num_training; j++){
			cur_distance = distance(training_set[i][j], test_image, 256);
			//printf("current distance: %d\n", cur_distance);
			if(cur_distance < min_distance){
				min_distance = cur_distance;
				min_per_id = i;
				//printf("I am here\n");
			}
		}
	}
	//printf("min_per_id: %d distance: %f\n", min_per_id, min_distance);
	return min_per_id+1;
}

double distance(int * a, int *b, int size){
	double distance = 0;

	#pragma omp parallel for reduction(+:distance)
	for(int i = 0; i < size; i++){
		if(a[i] + b[i] == 0){
			distance = distance + 0;
		}
		else{
			distance = distance + ((double)pow((a[i]-b[i]),2))/(double)(2*(a[i]+b[i]));
		}
	}
	//printf("distance: %f\n",distance);
	return distance;
}

void create_histogram(int * hist, int ** img, int num_rows, int num_cols){
	#pragma omp parallel for
	for(int i = 0; i < num_rows; i++){
		#pragma omp parallel for
		for(int j = 0; j < num_cols; j++){
			int result = 0;
			int pixel_start_row = i - 1;
			int pixel_start_col = j - 1;
			int counter = 7;
			for(int a = 0; a < 3; a++){
				for(int b = 0; b < 3; b++){
					if(pixel_start_row != num_rows && pixel_start_col != num_cols){
						if(pixel_start_row+a < 0 || pixel_start_row+a >= num_rows ||
						 pixel_start_col+b < 0 || pixel_start_col+b >= num_cols){
						 	
						 	result = result + 0;
						 	counter--;
						 }
						 else{
						 	//printf("pixel_start_row + a: %d\n", pixel_start_row + a);
						 	if(img[i][j] > img[pixel_start_row + a][pixel_start_col + b]){
						 		result = result + 0;
						 		counter--;
						 	}
						 	else{
						 		result = result + pow(2,counter);
						 		counter--;
						 	}
						 }
					}
				}
			}
			#pragma omp atomic
			hist[result] = hist[result] + 1;
		}
	}
}
char* getFilename(int per_id, int pho_id){
	char *filename = malloc(sizeof(int)*10);
	sprintf(filename, "%d.%d.txt", per_id, pho_id);
	return filename;
}
char* getFilenameExt(int per_id, int pho_id){
	char *filename = malloc(sizeof(int)*10);
	sprintf(filename, "images/%d.%d.txt", per_id, pho_id);
	return filename;
}

//test functions
/*
int sum = 0;
for(int i = 0; i < 256; i++){
	printf("hist[%d]: %d\n", i, hist[i]);
	sum = sum + hist[i];
}
printf("sum: %d\n", sum);
*/


