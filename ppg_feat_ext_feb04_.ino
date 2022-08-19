void setup() {
 Serial.begin(9600);
}

void loop() {
  int j=0;
  int pos[len];
  int pos_len=0; //length of pos array
  for(int i=0;i<len;i++)
  {
    pos[i]=-1;
  }
  for(int i=1;i<len-1;i++)
  {
    if PPG1_R[i]> PPG1_R[i-1] && PPG1_R[i]>= PPG1_R[i+1] && PPG1_R[i]> 0.45*max(PPG1_R,len)
    {
       val[j]= PPG1_R[i];
       pos[j]=i, //take the peak positions for calculation of variance
       j=j+1;
    }
  }
  pos_len=j;
  float xmax=max(x,pos_len),
  float xmin=min(x,pos_len);
  float tmax=(xmax-xmin)/25, //obtain the maximum time span value in sec
  float Mean[pos_len-1];
  Mean=(xmax-xmin)./(length(x)-1);
  Mean_time=Mean./25,%%obtain the Mean value in sec

  y=Mean.*ones(size(x)),
  z=(x-y),
  z2=immultiply(z,z);
  var=sum(z2)./(length(x)-1),
  var_time=var/25, %%obtain the value of variance in sec
  Accutance=Mean_time*tmax,

}
