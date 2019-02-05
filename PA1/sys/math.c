#include <math.h>

double pow(double x, int y){
    if(y == 0)
        return 1;
    double result = x;
    int n;
    for(n = 1; n < y; n++){
        result *= x;
    }
    return result;
}

double log(double x){
    double result = 0;
    int n;
    for(n = 1; n <= 20; n++){
        result = result + pow(-1, n+1) * (1.0/n) * pow(x - 1, n);
    }
    return result;
}

double expdev(double lambda){
    double dummy;
    do
        dummy = (double)rand() / RAND_MAX;
    while (dummy == 0.0);
    return -log(dummy) / lambda;
}

