#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <random>
#include <iomanip>
#include <math.h>

#define CELL_NUM 12
#define LAMBDA 1
#define MICRO 0.2
#define SIMULATION_TIME 100

using namespace std;

int find_first_z(int*);

int main(int argc, const char * argv[]) {
    default_random_engine generator(time(0));
    vector<int> lambda_vec;
    vector<float> micro_vec;
    
    
    int channel[12][5] = {0};
    float service_time[12][5] = {0};
    
    
    for(int i = 1; i <= CELL_NUM ; ++i){
        lambda_vec.push_back(((i%4)+1)*LAMBDA);      // Cell i arrival rate
        micro_vec.push_back(((i%4)+1)*MICRO);       // Cell i service rate
    }
    
    poisson_distribution<int>  dis_p1(lambda_vec[0]),dis_p2(lambda_vec[1]),dis_p3(lambda_vec[2]),
                            dis_p4(lambda_vec[3]),dis_p5(lambda_vec[4]),dis_p6(lambda_vec[5]),
                            dis_p7(lambda_vec[6]),dis_p8(lambda_vec[7]),dis_p9(lambda_vec[8]),
                            dis_p10(lambda_vec[9]),dis_p11(lambda_vec[10]),
                            dis_p12(lambda_vec[11]);
    
    exponential_distribution<double> dis_e1(micro_vec[0]),dis_e2(micro_vec[1]),
                                dis_e3(micro_vec[2]),dis_e4(micro_vec[3]),
                                dis_e5(micro_vec[4]),dis_e6(micro_vec[5]),
                                dis_e7(micro_vec[6]),dis_e8(micro_vec[7]),
                                dis_e9(micro_vec[8]),dis_e10(micro_vec[9]),
                                dis_e11(micro_vec[10]),dis_e12(micro_vec[11]);
    
    int block_count[12] = {0};
    float arrival_call[12] = {0};
    
    for (int time = 0 ; time < 1000000 ; ++time){                     // test for 100 min
        if( time%1000 == 0){                                  // customer request per minute
            
            int request[12] = {0};
            request[0] = dis_p1(generator); request[6] = dis_p7(generator);
            request[1] = dis_p2(generator); request[7] = dis_p8(generator);
            request[2] = dis_p3(generator); request[8] = dis_p9(generator);
            request[3] = dis_p4(generator); request[9] = dis_p10(generator);
            request[4] = dis_p5(generator); request[10] = dis_p11(generator);
            request[5] = dis_p6(generator); request[11] = dis_p12(generator);
            
            for (int channel = 0; channel < 12 ; ++channel){
                if(request[channel]){
                    request[channel] = 1;
                    ++arrival_call[channel] ;
                }
            }
            
            //cout << "request:" << request << endl;
            int index[12] = {0};
            int indent[12] = {0};
            for (int i = 0 ; i < 12 ; ++i){
                index[i] = find_first_z(channel[i]);
                if ((5-index[i]) < request[i]){
                    block_count[i] += request[i] - (5-index[i]);
                    request[i] = 5 - index[i];
                }
                while (request[i]) {    // allocate channel
                    channel[i][index[i]+indent[i]] = 1;
                    indent[i]++;
                    request[i]--;
                }
                
                //cout << endl;
                
                for (int j = 0; j < 5 ; ++j){
                    //cout << channel[i][j] ;
                    if (i==0 && channel[i][j]!=0 && service_time[i][j]==0)
                        service_time[i][j] = dis_e1(generator);
                    else if (i==1 && channel[i][j]!=0 && service_time[i][j]==0)
                        service_time[i][j] = dis_e2(generator);
                    else if (i==2 && channel[i][j]!=0 && service_time[i][j]==0)
                        service_time[i][j] = dis_e3(generator);
                    else if (i==3 && channel[i][j]!=0 && service_time[i][j]==0)
                        service_time[i][j] = dis_e4(generator);
                    else if (i==4 && channel[i][j]!=0 && service_time[i][j]==0)
                        service_time[i][j] = dis_e5(generator);
                    else if (i==5 && channel[i][j]!=0 && service_time[i][j]==0)
                        service_time[i][j] = dis_e6(generator);
                    else if (i==6 && channel[i][j]!=0 && service_time[i][j]==0)
                        service_time[i][j] = dis_e7(generator);
                    else if (i==7 && channel[i][j]!=0 && service_time[i][j]==0)
                        service_time[i][j] = dis_e8(generator);
                    else if (i==8 && channel[i][j]!=0 && service_time[i][j]==0)
                        service_time[i][j] = dis_e9(generator);
                    else if (i==9 && channel[i][j]!=0 && service_time[i][j]==0)
                        service_time[i][j] = dis_e10(generator);
                    else if (i==10 && channel[i][j]!=0 && service_time[i][j]==0)
                        service_time[i][j] = dis_e11(generator);
                    else if (i==11 && channel[i][j]!=0 && service_time[i][j]==0)
                        service_time[i][j] = dis_e12(generator);
                }
            }
        }


        for (int i = 0 ; i < 12 ; ++i){
            for (int j = 0 ; j < 5 ; ++j){              // computer service time sample with 0.01min
                //cout << service_time[0][k] << "|";
                if(service_time[i][j] > 0){
                    service_time[i][j] -= 0.001;
                    //cout << "minus 0.01" << endl;
                }
                else if (service_time[i][j] <= 0){
                    //cout << "free channel: " << k << endl;
                    service_time[i][j] = 0;
                    channel[i][j] = 0;
                }
            }
        }
        //cout << service_time[0][0] << endl;
    }

    
    /*for (int i = 0; i < 12 ; ++i){
        for (int j = 0 ; j < 5 ; ++j)
            cout << channel[i][j];
        cout << endl;
    }*/
    
    
    for (int i = 0; i < 12 ; ++i){
        //cout << "channel request of " << i << " " << arrival_call[i] << endl;
        cout << "channel " << i+1 << " blocking probability: " << (block_count[i]/arrival_call[i])*100 << "%" << endl;
    }
    
    return 0;
}

int find_first_z(int* arr){ //find first zero index!
    int length = 5;
    int index = 0;
    for(int round=0; round < length; round++)
    {
        //cout << arr[round];
        if(arr[round] == 0){
            index = round;
            return index;
        }
    }
    return length;
}

