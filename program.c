/* A simplified learned index implementation:
 *
 * Skeleton code written by Jianzhong Qi, April 2023
 * Edited by: [Zachary Williams 1172598] 
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define STAGE_NUM_ONE 1						  /* stage numbers */ 
#define STAGE_NUM_TWO 2
#define STAGE_NUM_THREE 3
#define STAGE_NUM_FOUR 4
#define STAGE_HEADER "Stage %d\n==========\n" /* stage header format string */

#define DATASET_SIZE 100					  /* number of input integers */
#define DATA_OUTPUT_SIZE 10					  /* output size for stage 1 */

#define BS_NOT_FOUND (-1)					  /* used by binary search */
#define BS_FOUND 0


typedef int data_t; 				  		  /* data type */

/****************************************************************/

/* used examples of structs to create my own, adapted from
   https://stackoverflow.com/questions/10468128/how-do-you-make-an-array-of-structs-in-c 
*/
typedef struct {
	data_t a[DATASET_SIZE], b[DATASET_SIZE], key[DATASET_SIZE];
} triple_t;

/* function prototypes */
void print_stage_header(int stage_num);
int cmp(data_t *x1, data_t *x2);
void swap_data(data_t *x1, data_t *x2);
void partition(data_t dataset[], int n, data_t *pivot,
		int *first_eq, int *first_gt);
void quick_sort(data_t dataset[], int n);
int binary_search(data_t dataset[], int lo, int hi, 
	data_t *key, int *locn);

void read_input(data_t dataset[]);
void stage_one(data_t dataset[]);

void find_a_b(data_t dataset[], data_t arr[], int index);
float calc_fkey(int key, int a, int b);
int find_pred_err(float fkey, int index);
void stage_two(data_t dataset[]);

int check_cover(int key, int index, int a, int b, int ERRm);
int check_cover_partial(data_t dataset[], int a, int b, int *ERRm, int i);
int check_cover_all(data_t dataset[], int i, int *ERRm, triple_t *cover);
int stage_three(data_t dataset[], triple_t *cover, int *ERRm);

int binary_search_edited_step_2(data_t dataset[], int lo, int hi, 
		data_t *key, int *locn);
int step_1_check(data_t dataset[], int key_new);
int binary_search_edited_step_3(data_t dataset[], int lo, int hi, data_t *key, int *locn);
int find_min(int structindex, int max_err, triple_t *cover, int key_new);
int find_max(int structindex, int max_err, triple_t *cover, int key_new);
void stage_four(triple_t *cover, data_t dataset[], int structlen, int max_err);

/* add your own function prototypes here */

/****************************************************************/

/* main function controls all the action */
int
main(int argc, char *argv[]) {
	/* to hold all input data */
	data_t dataset[DATASET_SIZE];
	int structlen, max_err;
	triple_t cover;

	/* to hold the mapping functions */


	/* stage 1: read and sort the input */
	stage_one(dataset); 
	
	/* stage 2: compute the first mapping function */
	stage_two(dataset);
	
	/* stage 3: compute more mapping functions */ 
	structlen = stage_three(dataset, &cover, &max_err);

	
	/* stage 4: perform exact-match queries */
	stage_four(&cover, dataset, structlen, max_err);
	
	/* algorithms are fun!! */ 
	/* all done; take some rest */
	return 0;
}

/****************************************************************/

/* add your code below; you can also modify the function return type 
   and parameter list 
*/

/* read int values and put it into an in array */
void read_input(data_t dataset[]) { 

	for (int newint = 0; newint < DATASET_SIZE; newint++) { 
		/* iterate through the lines of input data */
		scanf("%d", &dataset[newint]); /* scanf each int value into array */
	}

}

/* stage 1: read and sort the input */
void 
stage_one(data_t dataset[]) {
	/* add code for stage 1 */
	/* print stage header */
	print_stage_header(STAGE_NUM_ONE);

	read_input(dataset); /* read int values into dataset array */

	quick_sort(dataset, DATASET_SIZE);		/* quick sort the data from lowest to highest */

	printf("First 10 numbers:"); 

	for (int i = 0; i < DATA_OUTPUT_SIZE; i++) { /* print the first 10 values of the sorted dataset */
		printf(" %d", dataset[i]); 	
	}	

	printf("\n\n");
}

/* calculate the a and b parameters */
void find_a_b(data_t dataset[], data_t arr[], int index) { 

	if (index == 99) {
		/* consider case where we reach last element */
		arr[0] = index - 1;
		arr[1] = 0;
	}

	else if (dataset[index] == dataset[index + 1]) { 
		/* consider case where first and second element of sorted dataset is equal */
		arr[0] = index;
		arr[1] = 0;
	}

	else { /* find a and b using the equation in f(key) */
		arr[0] = (dataset[index + 1]*index) - (dataset[index]*(index + 1));
		arr[1] = dataset[index + 1] - dataset[index];
	}
}

/* function to calculate f(key) using a, b and key */
float calc_fkey(int key, int a, int b) { 

	if (b == 0) { /* if dataset[0] and dataset[1] are equal then make fkey = a */
		return a;
	}

	return (key + (float)a)/(float)b; /* return f(key) output */
}

/* function to calculate prediction error */
int find_pred_err(float fkey, int index) {
	return abs((int)ceil(fkey) - index);
}

/* stage 2: compute the first mapping function */
void stage_two(data_t dataset[]) { 
	/* add code for stage 2 */
	/* print stage header */
	print_stage_header(STAGE_NUM_TWO);

	data_t arr[2];
	int num, max = 0; 
	int x;
	int a, b;

	find_a_b(dataset, arr, 0);

	/* allocate values for a and b */
	a = arr[0];
	b = arr[1];

	for (int i = 0; i < DATASET_SIZE; i++) { /* iterate through dataset */

		num = find_pred_err(calc_fkey(dataset[i], a, b), i);
		/* calculate prediction error for current key in dataset */

		if (num > max) { 
			/* make new max prediction error if num > max*/
			max = num;
			x = i; /* index of key with max prediction error */
		}
	}

	printf("Maximum prediction error: %d\n", max); /* print max prediction error */
	printf("For key: %d\n", dataset[x]); /* print key with max prediction error */
	printf("At position: %d\n", x); /* print index of key */
	printf("\n");
}

/* function to check if key is covered */
int check_cover(int key, int index, int a, int b, int ERRm) {
	int pred_err;

	/* find prediction error of key */
	pred_err = find_pred_err(calc_fkey(key, a, b), index);

	/* check if prediction error is greater than target max error */
	if (pred_err > ERRm) {
		return 0;
	}
	else {
		return 1;
	}
}

/* function to find the cover of a segment of the dataset */
int check_cover_partial(data_t dataset[], int a, int b, int *ERRm, int i) {

	/* iterate through dataset */
	while (i < DATASET_SIZE) {
		/* check if key is covered and if not return index of uncovered key */
		if (check_cover(dataset[i], i, a, b, *ERRm) == 0) {
			return i;
		}
		i++;
	}
	/* return last index */
	return i - 1;
}

/* find cover for whole dataset and return the struct length */
int check_cover_all(data_t dataset[], int i, int *ERRm, triple_t *cover) {
	int x = 1;
	data_t arr[2];

	while (i < 100) {
		find_a_b(dataset, arr, i);

		/* find index of segment where key exceeds target max prediction error */
		i = check_cover_partial(dataset, arr[0], arr[1], ERRm, i);

		/* if we reach final index make that the max element */
		if (i == DATASET_SIZE - 1) {
			cover -> key[x] = dataset[i];
			cover -> a[x] = arr[0];
			cover -> b[x] = arr[1];
			return x;
			break;
		}

		/* assign values to struct */
		cover -> key[x] = dataset[i - 1];
		cover -> a[x] = arr[0];
		cover -> b[x] = arr[1];

		/* increase index of struct */
		x++;
	}
	return x;
}

/* stage 3: compute more mapping functions */ 
int stage_three(data_t dataset[], triple_t *cover, int *ERRm) {
	/* add code for stage 3 */
	/* print stage header */
	print_stage_header(STAGE_NUM_THREE);

	int i, structlen;
	data_t arr[2];

	scanf("%d", ERRm);

	/* find a and b for the index 1 and 2 of dataset */
	find_a_b(dataset, arr, 0);

	/* get index of first key that has pred_err > ERRm */
	i = check_cover_partial(dataset, arr[0], arr[1], ERRm, 2);

	/* assign values of first struct index */
	cover -> key[0] = dataset[i - 1];
	cover -> a[0] = arr[0];
	cover -> b[0] = arr[1];

	/* find rest of functions for the dataset */
	structlen = check_cover_all(dataset, i, ERRm, cover);

	/* print out values */
	printf("Target maximum prediction error: %d\n", *ERRm);
	for (int z = 0; z < structlen + 1; z++) {
		printf("Function %2d: a = %4d, b = %3d, max element = %3d\n",
		z, cover -> a[z], cover -> b[z], cover -> key[z]);
	}

	printf("\n");
	return structlen;
}

/* function to check if new key is within range of dataset */
int step_1_check(data_t dataset[], int key_new) {

	if (dataset[0] > key_new || dataset[DATASET_SIZE - 1] < key_new) {
		return 0;
	}
	else {
		return 1;
	}
}

/* edited binary search function that returns 
index of struct array for desired function */
int binary_search_edited_step_2(data_t dataset[], int lo, int hi, data_t *key, int *locn) {
	int mid, outcome;

	/* return high which is the index for our max element key */
	if (lo>=hi) {
		printf("\n");
		return hi;
	}
	mid = (lo+hi)/2;
	
	if ((outcome = cmp(key, dataset+mid)) < 0) {
		printf(" %d", dataset[mid]);
		return binary_search_edited_step_2(dataset, lo, mid, key, locn);
	} else if (outcome > 0) {
		printf(" %d", dataset[mid]);
		return binary_search_edited_step_2(dataset, mid+1, hi, key, locn);
	} 
	/* return the index where the match was found */
	else {
		*locn = mid;
		printf(" %d\n", dataset[mid]);
		return *locn;
	}
}

/* binary search of same datapoint for new key */
int binary_search_edited_step_3(data_t dataset[], int lo, int hi, data_t *key, int *locn) {
	int mid, outcome;

	/* return 0 if datapoint not found */
	if (lo>=hi) {
		return 0;
	}
	mid = (lo+hi)/2;
	
	if ((outcome = cmp(key, dataset+mid)) < 0) {
		printf("%d ", dataset[mid]);
		return binary_search_edited_step_3(dataset, lo, mid, key, locn);
	} else if (outcome > 0) {
		printf("%d ", dataset[mid]);
		return binary_search_edited_step_3(dataset, mid+1, hi, key, locn);
	} 
	/* return 1 if key was found in dataset */
	else {
		*locn = mid;
		printf("%d @ dataset[%d]!\n", dataset[mid], *locn);
		return 1;
	}
}

/* function to find minimum index of to search for new key */
int find_min(int structindex, int max_err, triple_t *cover, int key_new) {
	float fkey;

	fkey = calc_fkey(key_new, cover -> a[structindex], cover -> b[structindex]);

	/* return max of minimum values */
	if ((int)(ceil(fkey) - max_err) < 0) {
		return 0;
	}
	else {
		return (int)(ceil(fkey) - max_err);
	}
}

/* function to find maximum index of to search for new key */
int find_max(int structindex, int max_err, triple_t *cover, int key_new) {
	float fkey;

	fkey = calc_fkey(key_new, cover -> a[structindex], cover -> b[structindex]);

	/* return min of maximum values */
	if ((int)(ceil(fkey) + max_err) < DATASET_SIZE - 1) {
		return (int)(ceil(fkey) + max_err);
	}
	else {
		return DATASET_SIZE - 1;
	}
}

/* stage 4: perform exact-match queries */
void stage_four(triple_t *cover, data_t dataset[], int structlen, int max_err) {
	/* add code for stage 4 */
	/* print stage header */
	print_stage_header(STAGE_NUM_FOUR);

	int key_new, locn, structindex, outcome, min, max;

	/* scanf the final input values */
	while (scanf("%d", &key_new) != EOF) {
		printf("Searching for %d:\n", key_new);

		/* check if the new key is within the range of our dataset */
		if (step_1_check(dataset, key_new) == 0) {
			printf("Step 1: not found!\n");
			continue;
		}

		printf("Step 1: search key in data domain.\n");
		printf("Step 2:");
		
		/* perform binary search to find appropriate function for each new key */
		structindex = binary_search_edited_step_2(cover -> key, 0, structlen + 1, &key_new, &locn);
		
		printf("Step 3: ");

		/* assign values to min and max */
		min = find_min(structindex, max_err, cover, key_new);
		max = find_max(structindex, max_err, cover, key_new);

		/* perform binary search for desired new key */
		outcome = binary_search_edited_step_3(dataset, min, max + 1, &key_new, &locn);

		if (outcome == 0) {
			printf("not found!\n");
		}
	}

	printf("\n");
}

/****************************************************************/
/* functions provided, adapt them as appropriate */

/* print stage header given stage number */
void 
print_stage_header(int stage_num) {
	printf(STAGE_HEADER, stage_num);
}

/* data swap function used by quick sort, adpated from
   https://people.eng.unimelb.edu.au/ammoffat/ppsaa/c/sortscaffold.c
*/
void
swap_data(data_t *x1, data_t *x2) {
	data_t t;
	t = *x1;
	*x1 = *x2;
	*x2 = t;
}

/* partition function used by quick sort, from
   https://people.eng.unimelb.edu.au/ammoffat/ppsaa/c/quicksort.c 
*/
void
partition(data_t dataset[], int n, data_t *pivot,
		int *first_eq, int *first_gt) {
	int next=0, fe=0, fg=n, outcome;
	
	while (next<fg) {
		if ((outcome = cmp(dataset+next, pivot)) < 0) {
			swap_data(dataset+fe, dataset+next);
			fe += 1;
			next += 1;
		} else if (outcome > 0) {
			fg -= 1;
			swap_data(dataset+next, dataset+fg);
		} else {
			next += 1;
		}
	}
	
	*first_eq = fe;
	*first_gt = fg;
	return;
}

/* quick sort function, adapted from
   https://people.eng.unimelb.edu.au/ammoffat/ppsaa/c/quicksort.c 
*/
void
quick_sort(data_t dataset[], int n) {
	data_t pivot;
	int first_eq, first_gt;
	if (n<=1) {
		return;
	}
	/* array section is non-trivial */
	pivot = dataset[n/2]; // take the middle element as the pivot
	partition(dataset, n, &pivot, &first_eq, &first_gt);
	quick_sort(dataset, first_eq);
	quick_sort(dataset + first_gt, n - first_gt);
}

/* comparison function used by binary search and quick sort, from
   https://people.eng.unimelb.edu.au/ammoffat/ppsaa/c/binarysearch.c 
*/
int
cmp(data_t *x1, data_t *x2) {
	return (*x1-*x2);
}

/* binary search between dataset[lo] and dataset[hi-1], adapted from
   https://people.eng.unimelb.edu.au/ammoffat/ppsaa/c/binarysearch.c 
*/
int
binary_search(data_t dataset[], int lo, int hi, 
		data_t *key, int *locn) {
	int mid, outcome;
	/* if key is in dataset, it is between dataset[lo] and dataset[hi-1] */
	if (lo>=hi) {
		return BS_NOT_FOUND;
	}
	mid = (lo+hi)/2;
	
	if ((outcome = cmp(key, dataset+mid)) < 0) {
		return binary_search(dataset, lo, mid, key, locn);
	} else if (outcome > 0) {
		return binary_search(dataset, mid+1, hi, key, locn);
	} else {
		*locn = mid;
		return BS_FOUND;
	}
}