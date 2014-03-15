/*                                                    knn.c
This is kmeans clustering  implementation. Theoretically it should be able to take any amount of data, of any given dimension.
This will take a dataset of any size and divides it into chunks of approximately 2G and do k-means clustering. It will then update the 
centoids on the next ~2G data. This goes on till the end of the dataset. Only one pass(chunk size) is made through the whole data set
 						Written by   punnu07
*/



float distance(float *, float *, int);

#include<stdio.h>
#include<stdlib.h>
#include<math.h> 

int main(int argc, char *argv[])
{
int dim,row,k;
row=atoi(argv[2]);
dim=atoi(argv[3]);
k=atoi(argv[4]);
int max_iteration=25;
float **data;
float **cluster_centroid;
float **cluster_distance;
float **tmp;
float *dist_sum;
int *cluster_element_count;
cluster_element_count=(int *)malloc(k*sizeof(int));
int count=0,i,div,j,current_distance,current_cluster;
int t;
int iteration=0;
int max_possible_floats = 20000000;                                   // max possible floating point values
int total_floats ;                                                    // to check for memmory limits
int  streaming=0;
total_floats=row*dim;
int max_rows_possible=max_possible_floats/dim;
int total_streaming_counts;
int streaming_count;
if(total_floats>max_possible_floats)
{
     
 	total_streaming_counts=row/max_rows_possible;  
 	streaming=1;                                                          // need streaming algorithm
 	row=max_rows_possible;
 	printf("\nstreaming k-means activated. Total Number of data streams=%d\tTotal datapoints processed in each data stream=%d\n",total_streaming_counts, row);
  
}
else
{
	streaming=0;
	total_streaming_counts=1;
}


// to store the data
data=(float **)malloc(row*sizeof(float *));
if(data==NULL){printf("\nallocation failed\n");getchar();}
for(i=0;i<row;i++)
{
	data[i]=(float *)malloc(dim*sizeof(float)); 
}

// to store cluster centroids
cluster_centroid=(float **)malloc(k*sizeof(float **));
for(i=0;i<k;i++)
{
	cluster_centroid[i]=(float *)malloc(dim*sizeof(float ));
}

// to store the distance to all the centroids from all the datapoints
cluster_distance=(float **)malloc(row*sizeof(float *));
for(i=0;i<row;i++)
{
	cluster_distance[i]=(float *)malloc(k*sizeof(float ));
}

// array to store cumulative sum of the distance from data to each centroid
dist_sum=(float *)malloc(k*sizeof(float));

//initialize a temporal array
tmp=(float **)malloc(k*sizeof(float *));
for(i=0;i<k;i++)
{
	tmp[i]=(float *)malloc(dim*sizeof(float));
}



FILE *fp=fopen(argv[1],"r");
FILE *fa=fopen("centroids.out","w");


for(streaming_count=1;streaming_count<=total_streaming_counts+1;streaming_count++)             
{
if(streaming==0&&streaming_count==2){break;}
if(streaming==1&& streaming_count==total_streaming_counts+1) // find the last chunk
{       
        
	t=atoi(argv[2]);
	row=t-(total_streaming_counts*max_rows_possible);            
        printf("\nlast chunk of dataset\n");
}
printf("\nstream %d\n",streaming_count);
count=0;
iteration=0;

while(count<row)
{
   
	for(i=0;i<dim;i++)
	{
 	fscanf(fp,"%f",&data[count][i]);
	}
	count=count+1;
}

// memory for  cluster centroids	
//kmeans iterations
while(iteration<max_iteration)
{
	if(iteration==0&&streaming_count==1)
	{
	//initialize centroids. if first iteration, get centroids from the original data array in a uniform distribution
	div=(row-k)/k;
	for(i=0;i<k;i++)
	{
	for(j=0;j<dim;j++)
	{
	cluster_centroid[i][j]=data[i*div][j];
        }
        }
	}// first iteration
        



	//find distance between each datapoint and each cluster
	for(i=0;i<row;i++)
	{
	for(j=0;j<k;j++)
	{
	cluster_distance[i][j]=distance(data[i],cluster_centroid[j],dim);
        }
	}


      


	//initialize to zero
	for(i=0;i<k;i++)
	{
	cluster_element_count[i]=0;
	for(j=0;j<dim;j++)
	{
	tmp[i][j]=0;
	}
	}



        // clear the error array
        for(i=0;i<k;i++)
        {
         dist_sum[i]=0;
        }

	
	//find to which cluster centroids, each data point is nearest to
	for(i=0;i<row;i++)
	{
         current_distance=10000000;
	 current_cluster=-1;
	for(j=0;j<k;j++)
	{
	if(cluster_distance[i][j]<current_distance)
	{
	current_cluster=j;
	current_distance=cluster_distance[i][j];
	}
        
	}
        dist_sum[current_cluster]=dist_sum[current_cluster]+current_distance;
	cluster_element_count[current_cluster]=cluster_element_count[current_cluster]+1;
	for(j=0;j<dim;j++)
	{
	tmp[current_cluster][j]=tmp[current_cluster][j]+data[i][j];
	}
        
	}


       

        // update centroids
	for(i=0;i<k;i++)
	{
         
         if(cluster_element_count[i]==0)                                             // no points in that cluster
          {
          continue;
          }
	for(j=0;j<dim;j++)
	{
	cluster_centroid[i][j]=tmp[i][j]/(float)cluster_element_count[i];
	}
        
        printf("\nIteration %d ", iteration);
         for(i=0;i<k;i++)
	{
         printf("%d ",cluster_element_count[i]);
	} 
       
        dist_sum[i]=dist_sum[i]/(float)cluster_element_count[i];  	

	}
      
      	
	iteration=iteration+1;
}// iterations
} // streaming chunks


//All the iterations are over. Now write the centroids to the file
for(i=0;i<k;i++)
{
	for(j=0;j<dim;j++)
	{
	fprintf(fa,"%f ",cluster_centroid[i][j]);
	}
	fprintf(fa,"\n");
}


//free the arrays
free(dist_sum);
free(cluster_element_count);
free(tmp);
free(data);
free(cluster_centroid);
free(cluster_distance);
fclose(fp);
fclose(fa);
printf("closed");
return 0;


}



float distance(float *a, float *b, int dim)
{
int i;
float sum=0,res=0;
for(i=0;i<dim;i++)
{
sum=sum+((a[i]-b[i])*(a[i]-b[i]));
}
res=sqrt(sum);
return res;
}


 
