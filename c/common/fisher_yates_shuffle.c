#include <stdio.h>
#include <stdlib.h>


void shuffle_test(int * array, int len)
{
	int array_len = len;

	while(array_len) {
		if (array_len == 1)
			break;
		int r = rand() % (array_len--);
		int tmp = array[array_len];
		array[array_len] = array[r];
		array[r] = tmp;
	}
}



int main(int argc, char *argv[])
{
	int array[9] = {0,1,2,3,4,5,6,7,8};
	shuffle_test(array, 9);

	int i;
	for ( i = 0; i < 9; i++ ) {
		printf("%d ", array[i]);
	}
}
