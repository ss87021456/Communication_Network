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
#define NO_CANDIDATE_BLOCK 100
#define NO_NEED 25

using namespace std;

int find_first_z(int*);
int find_first_nz(int*);
int find_free_num (int* channel);
void check_free_channel (int center, int BS[][5], int inside[][12], int* free_channel);

int main(int argc, const char * argv[]) {
    default_random_engine generator(time(0));
    vector<int> lambda_vec;
    vector<float> micro_vec;
    int inside_distance[12][12] = {{1,1,1,0,0,0,0,1,0,0,0,0},
                                  {1,1,1,0,0,0,0,0,0,1,0,0},
                                  {1,1,1,1,1,0,0,1,0,1,0,0},
                                  {0,0,1,1,1,1,0,1,1,0,0,0},
                                  {0,0,1,1,1,1,0,0,0,1,0,1},
                                  {0,0,0,1,1,1,0,0,0,0,0,0},
                                  {0,0,0,0,0,0,1,1,1,0,0,0},
                                  {1,0,1,1,0,0,1,1,1,0,0,0},
                                  {0,0,0,1,0,0,1,1,1,0,0,0},
                                  {0,1,1,0,1,0,0,0,0,1,1,1},
                                  {0,0,0,0,0,0,0,0,0,1,1,1},
                                  {0,0,0,0,1,0,0,0,0,1,1,1}};
    
    int channel[12][5] = {0};
    float service_time[12][5] = {0};
    float arrival_call[12] = {0};

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
    
    for (int time = 0 ; time < 10000 ; ++time){                     // test for 100 min
        if( time%100 == 0){                                  // customer request per minute
            
            int request[12] = {0};
            int index[12] = {0};
            
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

            
            for (int i = 0 ; i < CELL_NUM ; ++i){
                int free[15];
                int candidate = NO_NEED;
                check_free_channel(i, channel, inside_distance, free);  // find free channel
                
                index[i] = find_first_z(channel[i]);
                
                if ((5-index[i]) < request[i]){    // suspicious block
                    //cout << "CELL " << i << " may blocked!" << endl;
                    candidate = NO_CANDIDATE_BLOCK;
                    int num = 0;
                    for (int ch = 0; ch < 15 ; ++ch){
                        if (free[ch]){
                            if (ch>= 0 && ch <5){       //search 1,4,7,10
                                for (int i = 0 ; i < 4 ; ++i){
                                    int ch_num = i*3;
                                    if (find_free_num(channel[ch_num]) > num){
                                        candidate = ch_num;
                                        num = find_free_num(channel[ch_num]);
                                    }
                                }
                            }
                            else if (ch>= 5 && ch <10){   //search 2,5,8,11
                                for (int i = 0 ; i < 4 ; ++i){
                                    int ch_num = i*3 + 1;
                                    if (find_free_num(channel[ch_num]) > num){
                                        candidate = ch_num;
                                        num = find_free_num(channel[ch_num]);
                                    }
                                }
                            }
                            else if (ch>= 10 && ch <15){  //search 3,6,9,12
                                for (int i = 0 ; i < 4 ; ++i){
                                    int ch_num = i*3 + 2;
                                    if (find_free_num(channel[ch_num]) > num){
                                        candidate = ch_num;
                                        num = find_free_num(channel[ch_num]);
                                    }
                                }
                            }
                        }
                    }
                    if( candidate == NO_CANDIDATE_BLOCK){         // represent no channel can borrow
                        block_count[i] += 1;
                        request[i] = 0;
                    }
                    else{
                        request[i] = candidate+10;
                    }
                }

                                                                // allocate channel
                if (request[i] == 1 && candidate == NO_NEED)
                    channel[i][index[i]] = 1;
                if (request[i] >1 && candidate!= NO_CANDIDATE_BLOCK){  // for borrow channel generate s_time
                    int index = find_first_z(channel[candidate]);
                    channel[candidate][index] = request[i];
                    //cout << i << " borrow from " << candidate << endl;
                    switch (candidate) {
                        case 0:
                            service_time[candidate][index] = dis_e1(generator);
                            break;
                        case 1:
                            service_time[candidate][index] = dis_e2(generator);
                            break;
                        case 2:
                            service_time[candidate][index] = dis_e3(generator);
                            break;
                        case 3:
                            service_time[candidate][index] = dis_e4(generator);
                            break;
                        case 4:
                            service_time[candidate][index] = dis_e5(generator);
                            break;
                        case 5:
                            service_time[candidate][index] = dis_e6(generator);
                            break;
                        case 6:
                            service_time[candidate][index] = dis_e7(generator);
                            break;
                        case 7:
                            service_time[candidate][index] = dis_e8(generator);
                            break;
                        case 8:
                            service_time[candidate][index] = dis_e9(generator);
                            break;
                        case 9:
                            service_time[candidate][index] = dis_e10(generator);
                            break;
                        case 10:
                            service_time[candidate][index] = dis_e11(generator);
                            break;
                        case 11:
                            service_time[candidate][index] = dis_e12(generator);
                            break;
                        default:
                            break;
                    }
                }
                
                for (int j = 0; j < 5 ; ++j){
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
                if(service_time[i][j]>0){
                    service_time[i][j] -= 0.01;
                }
                else if (service_time[i][j] <= 0){   // free a channel
                    service_time[i][j] = 0;
                    channel[i][j] = 0;
                    for (int m = 0 ; m < CELL_NUM ; ++m)
                        for (int n = 0 ; n < 5 ; ++n){
                            if (channel[m][n] == i+10){
                                channel[m][n] = 0;
                                channel[i][j] = 1;
                                service_time[i][j] = service_time[m][n];
                            }
                        }
                }
            }
        }
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

int find_first_nz(int* arr){ //find first nonzero index!
    int length = 15;
    int index = 0;
    for(int round=0; round < length; round++)
    {
        if(arr[round] != 0){
            index = round;
            return index;
        }
    }
    return length;
}

int find_free_num (int* channel){
    int free_num = 0;
    for (int i = 0 ; i < 5 ; ++i){
        if (!channel[i])
            free_num++;
    }
    return free_num;
}


void check_free_channel (int center, int BS[][5], int inside[][12], int* free_channel){
    
    for (int i = 0 ; i < 15 ; ++i)
        free_channel[i] = 1 ;
    
    for (int i = 0 ; i < CELL_NUM ; ++i){
        if (inside[center][i]){                 // first find interfere BS
            for(int j = 0 ; j < 5 ; ++j){
                // then check for free channel
                if (BS[i][j]){
                    int cluster = (i+1)%3 ;
                    switch (cluster) {
                        case (1):
                            free_channel[j] = 0;
                            break;
                        case (2):
                            free_channel[5+j] = 0;
                            break;
                        case (0):
                            free_channel[10+j] = 0;
                            break;
                        default:
                            break;
                    }
                }
            }
        }
    }
}
