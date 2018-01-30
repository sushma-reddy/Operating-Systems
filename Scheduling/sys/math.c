#include <math.h>

double log(double x){
	double sum;
	int i ;
	sum = 0;
	for (i=1; i<21; i++){
		sum = sum - (pow(x,i)/i);
	}
	return sum;
}

double pow(double x, int y){
        int i;
        double prod;
        prod = 1.0;
        for (i=0; i<y; i++){
                prod = prod*x;
        }
        return prod;
}

double expdev(double lambda) {
    double dummy;
    do
        dummy= ((double) rand() / RAND_MAX);
    while (dummy == 0.0);
    return -log(dummy) / lambda;
}


