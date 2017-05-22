#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <random>
#include <iomanip>
#include <math.h>

using namespace std;


int main(int argc, const char * argv[]) {
    float lambda = 0.5;
    
    default_random_engine generator(time(0));
    poisson_distribution<int> dis_p(lambda);
    
    int test_time = 10000;
    
    
    int* container = new int[test_time];
    for (int i = 0; i < test_time; ++i)
        container[i] = 0;
    int test_cout = 0;
    float sum = 0;
    for (int i = 0; i < test_time; ++i){
        if(!dis_p(generator)) // request = 0
            test_cout++;
        else{
            container[test_cout]++;
            sum++;
            test_cout = 0;
        }
    }
    cout << endl;
    for (int i = 0; i < 20; ++i)
        //cout << (container[i]/sum)*100 <<"% ";
        cout << "No." << i+1 <<" inter_arrival_time_pdf:" <<(container[i]/sum)*100 <<"%" << endl;
    
    return 0;
}


/*float pmf(int k, double lambda) {
    return pow(M_E, k * log(lambda) - lambda - lgamma(k + 1.0));
}

int request_poi(double* Prob, int n){
    int i, pos;
    double CP[50];
    double ram_pro; // 隨機機率
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(1, RAND_MAX);
    
    //累計機率計算
    CP[0] = Prob[0];
    for(i=1; i<n; ++i)
        CP[i] = CP[i-1] + Prob[i];
    
    ram_pro = dis(gen) / (RAND_MAX + 1.0) ; // 產生 [0, 1) 亂數
    for(pos=0; pos < n; ++pos)             // 查詢所在區間
        if(ram_pro <= CP[pos]) break;
    return pos;
}*/
