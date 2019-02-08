#include "lab1_sequential.h"
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <algorithm>    // std::find
#include <vector>       // std::vector
#include <limits>
#include <cstddef>
#include <iostream>
#include <ctime>  
#include<bits/stdc++.h>
using namespace std; 
vector<float> centroiddatabase;
float* currentcentroid;
void updateCentroiddb(int K);
int classify(int N,int K,int ** data_points, int ** data_point_cluster);
float dist(int c,int d, int ** data_points);
int updateCentroid(int N, int K, int ** data_points, int ** data_point_cluster);
void kmeans_sequential(int N,
					int K,
					int* data_points,
					int** data_point_cluster,
					float** centroids,
					int* num_iterations
					)
    {
        // cout << "iterations are"<<endl;
        // cout << *num_iterations<<endl;
        //initialize data_point_cluster
        *data_point_cluster = (int*)malloc(sizeof(int)*((N)*4));
        // cout <<"n is "<<N<<endl;
        // *data_point_cluster= new int[4*N];
        for(int i=0;i<N;i++){
            for(int j=0;j<3;j++){
                // cout << "i is "<<i<< " j is "<<j<<endl;
                int temp1= data_points[3*i+j];
                int temp2= (*data_point_cluster)[4*i+j];
                // cout << "temp1 is "<<temp1<< " temp2 is "<<temp2<<endl;
                (*data_point_cluster)[4*i+j]=data_points[3*i+j];
            }
            (*data_point_cluster)[4*i+3] = 0;
        }
        //initialize centroids
        currentcentroid = (float*)malloc(sizeof(float)*K*3);
        int* tempmeanindex = new int[N];
        for(int i=0;i<N;i++){
            tempmeanindex[i] = i;
        }
        random_shuffle(tempmeanindex, tempmeanindex+N);
        
        for(int i=0;i<K;i++){
            for(int j=0;j<3;j++){
                currentcentroid[3*i+j] = data_points[3*(tempmeanindex[i])+j];
            }
        }
        // put the centroids in vector
        updateCentroiddb(K);
        int maxiter=10000;
        int threschanges=1;
        //  start the loop
        int tempiter=0;
        for(int i=0; i< maxiter;i++){
            tempiter=i;
            // cout << "this is " << i << " iteration"<<endl;
            int numchanges1 = classify(N,K, &data_points, data_point_cluster);
            int numchanges2 = updateCentroid(N,K, &data_points, data_point_cluster);
            updateCentroiddb(K);
            if(numchanges1<threschanges && numchanges2<threschanges){
                break;
            }
        }
        // cout << "tempiter is "<< tempiter<<endl;
        *num_iterations= tempiter+1;
        // cout<<"num_iterations is "<<num_iterations <<" vector size is "<< << endl;
        *centroids=(float*)malloc(sizeof(float)*(centroiddatabase.size()));
        for(int i=0;i<centroiddatabase.size();i++){
            (*centroids)[i] = centroiddatabase[i];
        }
        return;
    }
void updateCentroiddb(int K){
                // cout << "calling updatecentroid"<<endl;

    for(int i=0;i<K;i++){
        for(int j=0;j<3;j++){
            centroiddatabase.push_back(currentcentroid[3*i+j]);
        }
    }
}
int classify(int N,int K,int ** data_points, int ** data_point_cluster){
    int numchanges =0;
    for(int i=0;i<N;i++){
        float tempdist = numeric_limits<float>::max();
        int tempbelongsto = -1;
        for(int j=0;j<K;j++){
            float thisdist= dist(j,i,data_points);
            if(thisdist< tempdist){
                tempdist= thisdist;
                tempbelongsto = j;
            }
        }
        if((*data_point_cluster)[4*i+3]!=tempbelongsto){
            numchanges++;
            (*data_point_cluster)[4*i+3]=tempbelongsto;
        }
    }
    return numchanges;
}
float dist(int c,int d, int ** data_points){
    float temp=0;
    for(int i=0;i<3;i++){
        temp += pow((currentcentroid[3*c+i]-(*data_points)[3*d+i]),2);
    }
    return temp;
}
int updateCentroid(int N, int K, int ** data_points, int ** data_point_cluster){
    int numchanges =0;
    float* tempmeans = (float *)malloc(sizeof(float)*K*3);
    int* freqmeans = (int *)malloc(sizeof(int)*K);
    for(int i=0;i<K;i++){
        for(int j=0;j<3;j++){
            tempmeans[3*i+j]=0;
        }
        freqmeans[i]=0;
    }
    for(int i=0;i<N;i++){
        int tempbelongsto = (*data_point_cluster)[4*i+3];
        for(int j=0;j<3;j++){
            tempmeans[3*tempbelongsto+j]=(*data_points)[3*i+j];
            freqmeans[i] +=1;
        }
    }
    for(int i=0;i<K;i++){
        bool shouldchange=false;
        for(int j=0;j<3;j++){
            if(freqmeans[i]!=0){
                float temp1 = currentcentroid[3*i+j];
                float temp2 = (1.0*tempmeans[3*i+j])/(freqmeans[i]);
                if(abs(temp1-temp2)<0.0001){

                }else{
                    shouldchange=true;
                    break;
                }
            }
        }
        if(shouldchange){
            for(int j=0;j<3;j++){
                if(freqmeans[i]!=0){
                    currentcentroid[3*i+j] = (1.0*tempmeans[3*i+j])/(freqmeans[i]);
                    numchanges++;
                }
            }
        }
    }
    return numchanges;
}