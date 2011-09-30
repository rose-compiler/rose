typedef struct
{
        int x;
        int y;
        int z;
} coords;

bool goodIdx(coords* data, int idx)
{
        return idx%22;
}

int main()
{
        int commBuf[3000];
        coords sData[1000];
        coords rData[1000];
        int len=0;
        
        // Packing Data
        {
                for(int i=0; i<1000; i++) {
                        if(goodIdx(sData, i)) {
                                commBuf[len]   = sData[i].x;
                                commBuf[len+1] = sData[i].y;
                                commBuf[len+2] = sData[i].z;
                                len+=3;
                        }
                }
        }
                
        // MPI_Send / MPI_Recv
        
        // Unpacking Data
        {
                for(int j=0; j<len; j++) {
                        rData[j].x = commBuf[j*3];
                        rData[j].y = commBuf[j*3+1];
                        rData[j].z = commBuf[j*3+2];
                }
        }
        
        // Code that uses rData
        int ret = 0;
        for(int k=0; k<len; k++) ret += rData[k].x;
        return 0;
}

// Code with fused loops:
/*int main()
{
        coords sData[1000];
        coords rData[1000];
        
        {
                int len=0;
                int j=0;
                for(int i=0; i<1000; i++) {
                        if(goodIdx(data, i)) {
                                rData[j].x = sData[i].x;
                                rData[j].y = sData[i].y;
                                rData[j].z = sData[i].z;
                        }
                }
        }
        
        // Code that uses rData
        int ret = 0;
        for(int k=0; k<len; k++) ret += rData[k].x;
        return 0;
}
*/

