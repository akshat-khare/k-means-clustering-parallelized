#include "lab1_pthread.h"
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <algorithm>    // std::find
#include <vector>       // std::vector
#include <limits>
#include <cstddef>
#include <iostream>
#include <ctime>  
#include <omp.h>
#include <pthread.h>
#include<bits/stdc++.h>
using namespace std; 
vector<float> centroiddatabase;
float* currentcentroid;
int* classifychanges;
int Ng;
int Kg;
int num_threadsg;
int * data_pointsg;
int * data_point_clusterg;
void updateCentroiddb(int K, float ** currentcentroid);
void * classify(void *);
float dist(int c,int d);
int updateCentroid();
void * plusser(void * );
void * plusser2(void * );
float* tempmeans;
float** tempmeanspl;
int* freqmeans;
int** freqmeanspl;
struct classifyThreadData{
    int N;
    int K;
    int ** data_points;
    int ** data_point_cluster;
};
void kmeans_pthread(int num_threads,
					int N,
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

        Ng = N;
        Kg = K;
        num_threadsg=num_threads;
        data_pointsg= new int[3*N];
        tempmeans = new float[Kg*3];
        freqmeans = new int[Kg];
        tempmeanspl = new float*[Kg];
        freqmeanspl = new int*[Kg];
        for(int i=0;i<Kg;i++){
            tempmeanspl[i] = new float[3*Kg];
            freqmeanspl[i] = new int[Kg];
        }
        for(int i=0;i<3*N;i++){
            data_pointsg[i] = data_points[i];
        }
        data_point_clusterg = new int[4*N];
        int maxiter=10000;
        centroiddatabase.reserve((maxiter+1)*3*K);
        // cout <<"n is "<<N<<endl;
        // *data_point_cluster= new int[4*N];
        for(int i=0;i<N;i++){
            for(int j=0;j<3;j++){
                // cout << "i is "<<i<< " j is "<<j<<endl;
                // int temp1= data_pointsg[3*i+j];
                // int temp2= (data_point_clusterg)[4*i+j];
                // cout << "temp1 is "<<temp1<< " temp2 is "<<temp2<<endl;
                (data_point_clusterg)[4*i+j]=data_pointsg[3*i+j];
            }
            (data_point_clusterg)[4*i+3] = 0;
        }
        //initialize centroids
        currentcentroid = new float[3*K];
        int* tempmeanindex = new int[N];
        for(int i=0;i<N;i++){
            tempmeanindex[i] = i;
        }
        std::srand ( unsigned ( std::time(0) ) );
        random_shuffle(tempmeanindex, tempmeanindex+N);
        
        for(int i=0;i<K;i++){
            for(int j=0;j<3;j++){
                currentcentroid[3*i+j] = data_pointsg[3*(tempmeanindex[i])+j];
            }
        }
        // put the centroids in vector
        updateCentroiddb(K, &currentcentroid);
        int threschanges=1;
        //  start the loop
        int tempiter=0;
        classifychanges = new int[num_threadsg];
        pthread_t threads[num_threads];
        
            int * tid = new int[num_threads];
        for(int i=0; i< maxiter;i++){
            tempiter=i;
            // cout << "this is " << i << " iteration"<<endl;
            // struct classifyThreadData classtd[num_threads];
            int numchanges1=0;
            for(int j=0;j<num_threads;j++){
                // classtd[j].N=N;
                // classtd
                tid[j]=j;
                // cout << "creating thread "<< &threads[j] << " iteration "<<i<<" thread number "<<j << endl;
                pthread_create(&threads[j], NULL, classify,(void *) (&tid[j]));
                // cout << "created thread " << &threads[j] << " iteration "<<i<<" thread number "<<j << endl;

                // int numchanges1 = classify();
            }
            for(int j=0;j<num_threads;j++){
                // cout << "joining thread "<< &threads[j] << " iteration "<<i<<" thread number "<<j << endl;

                pthread_join(threads[j],NULL);
                // cout << "joined thread "<< &threads[j] << " iteration "<<i<<" thread number "<<j << endl;

            }
            for(int j=0;j<num_threadsg;j++){
                numchanges1 +=classifychanges[j];
            }
            // cout << "do updatecentroid function"<<endl;
            int numchanges2 = updateCentroid();
            // cout << "update centroid done then updatedb"<<endl;
            updateCentroiddb(K, &currentcentroid);
            // cout << "update db done"<<endl;
            if(numchanges1<threschanges && numchanges2<threschanges){
                break;
            }
        }
        // cout << "tempiter is "<< tempiter<<endl;
        *num_iterations= tempiter+1;
        // cout<<"num_iterations is "<<num_iterations <<" vector size is "<< << endl;
        *centroids=new float[centroiddatabase.size()];
        for(int i=0;i<centroiddatabase.size();i++){
            (*centroids)[i] = centroiddatabase[i];
        }
        *data_point_cluster = new int[4*N];
        for(int i=0;i<4*N;i++){
            (*data_point_cluster)[i]=data_point_clusterg[i];
        }
        return;
    }
void updateCentroiddb(int K, float ** currentcentroidyo){
                // cout << "calling updatecentroid"<<endl;

    for(int i=0;i<K;i++){
        for(int j=0;j<3;j++){
            // cout << "whats the mfing problem"<<endl;
            float temp = (*currentcentroidyo)[3*i+j];
            // cout << "lets add "<< temp<<endl;
            // cout << "vector size is "<<centroiddatabase.size()<< " capacity is "<<centroiddatabase.capacity()<<endl;
            centroiddatabase.push_back(temp);
        }
    }
}
void * classify(void * stid){
    // cout << "beginning" << endl;
    int* itid=(int*)stid;
    int numchanges =0;
    int beginctr = (*itid)*(Ng/num_threadsg);
    int endctr;
    if(*itid!=num_threadsg-1){
        endctr = (*itid + 1)*(Ng/num_threadsg);
    }else{
        endctr=Ng;
    }
    for(int i=beginctr;i<endctr;i++){
        float tempdist = numeric_limits<float>::max();
        int tempbelongsto = -1;
        for(int j=0;j<Kg;j++){
            float thisdist= dist(j,i);
            if(thisdist< tempdist){
                tempdist= thisdist;
                tempbelongsto = j;
            }
        }
        if((data_point_clusterg)[4*i+3]!=tempbelongsto){
            numchanges++;
            (data_point_clusterg)[4*i+3]=tempbelongsto;
        }
    }
    classifychanges[*itid]=numchanges;
    // cout << "exiting"<<endl;
    return NULL;
}
float dist(int c,int d){
    float temp=0;
    for(int i=0;i<3;i++){
        temp += pow((currentcentroid[3*c+i]-(data_pointsg)[3*d+i]),2);
    }
    return temp;
}
void * plusser(void * tid){
    int * itid = (int *)(tid);
    int itidn = *itid;
    int beginctr = (itidn)*(Ng/num_threadsg);
    int endctr;
    if(itidn!=num_threadsg-1){
        endctr = (itidn + 1)*(Ng/num_threadsg);
    }else{
        endctr=Ng;
    }
    for(int i=beginctr;i<endctr;i++){
        int tempbelongsto = (data_point_clusterg)[4*i+3];
        for(int j=0;j<3;j++){
            tempmeanspl[itidn][3*tempbelongsto+j]+=(data_pointsg)[3*i+j];
        }
        freqmeanspl[itidn][tempbelongsto] +=1;
    }
    return NULL;
}
void * plusser2(void * tid){
    int * itid = (int *)(tid);
    int itidn = *itid;
    int beginctr = (itidn)*(Kg/num_threadsg);
    int endctr;
    if(itidn!=num_threadsg-1){
        endctr = (itidn + 1)*(Kg/num_threadsg);
    }else{
        endctr=Kg;
    }
    for(int j=beginctr;j<endctr;j++){
        for(int i=0;i<num_threadsg;i++){
            for(int k=0;k<3;k++){
                tempmeans[3*j+k]+= tempmeanspl[i][3*j+k];
            }
            freqmeans[j]+=freqmeanspl[i][j];
        }
    }
    return NULL;
}
int updateCentroid(){
    int numchanges =0;

    for(int i=0;i<Kg;i++){
        for(int j=0;j<3;j++){
            tempmeans[3*i+j]=0;
        }
        freqmeans[i]=0;
    }
    for(int k=0;k<num_threadsg;k++){
        for(int i=0;i<Kg;i++){
            for(int j=0;j<3;j++){
                tempmeanspl[k][3*i+j]=0;
            }
            freqmeanspl[k][i]=0;
        }
    }
    for(int i=0;i<Ng;i++){
        int tempbelongsto = (data_point_clusterg)[4*i+3];
        for(int j=0;j<3;j++){
            tempmeans[3*tempbelongsto+j]+=(data_pointsg)[3*i+j];
        }
        freqmeans[tempbelongsto] +=1;
    }
    pthread_t threads[num_threadsg];
    int * tid= new int[num_threadsg];
    for(int i=0;i<num_threadsg;i++){
        tid[i]=i;
        pthread_create(&threads[i],NULL, plusser,(void *)(&tid[i]));
    }
    for(int i=0;i<num_threadsg;i++){
        pthread_join(threads[i],NULL);
    }
    
    for(int i=0;i<num_threadsg;i++){
        tid[i]=i;
        pthread_create(&threads[i],NULL, plusser2,(void *)(&tid[i]));
    }
    for(int i=0;i<num_threadsg;i++){
        pthread_join(threads[i],NULL);
    }
    for(int i=0;i<Kg;i++){
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
    // delete[] tempmeans;
    // delete[] freqmeans;
    return numchanges;
}