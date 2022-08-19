/*Author-Divyayan Dey
This code is for educational purposes. I do not own any copyright
The initial database is taken from the BIDMC PPG and Respiration dataset v1.0.0 which is a subset of the MIMIC II matched waveform Database
PPG signal is sampled at 125 Hz
The saturation number of samples threshold has been kept at 5, observing the data 
*/

#include <math.h>
#include "arduinoFFT.h"
#include <EEPROM.h>

//N=10 pt moving average
//For bidmc data of first 15 patients, Moving average values lie in the range 0.413 to 0.502
double baseline_lower=0.413;
double baseline_upper=0.502;
float max_amp=1.0; //amplitude threshold, obtained from PPG dataset
float ref_voltage=1.0; //saturation reference voltage
float zero_amp=0.01;
float cutoff=4.0;  //Filter cutoff frequency
float fs=0.02;

float pi=3.14159265;  //Universal Constants
float e=2.71828;
const int flen=61;

#define SAMPLES 128           //Must be a power of 2
#define SAMPLING_FREQUENCY 50 //Hz, must be less than 10000 due to ADC

arduinoFFT FFT = arduinoFFT(); 
unsigned int sampling_period_us;
unsigned long microseconds;

double vReal[SAMPLES];
double vImag[SAMPLES];

void setup() {
  Serial.begin(9600);
  sampling_period_us = round(1000000*(1.0/SAMPLING_FREQUENCY));
}

void loop() {
  //float arr[]={0.43597,0.43206,0.42815,0.42424,0.42131,0.41838,0.4174,0.41642,0.41544,0.41447,0.41251,0.41153,0.40958,0.40762,0.40665,0.40665,0.40665,0.40762,0.4086,0.4086,0.41056,0.41153,0.41153,0.41056,0.4086,0.40665,0.40469,0.40469,0.40567,0.4086,0.41251,0.42033,0.43206,0.45259,0.4741,0.4956,0.51808,0.5347,0.55034,0.56403,0.57576,0.58456,0.59238,0.59629,0.59922,0.60117,0.60215,0.60313,0.60313,0.60117,0.59824,0.59335,0.58553,0.57771,0.57185,0.56207,0.54741,0.53372,0.52102,0.50929,0.50244,0.49756,0.4956,0.49462,0.49365,0.49365,0.49365,0.49169,0.48876,0.4868,0.48387,0.48094,0.47801,0.47507,0.47116,0.46823,0.46334,0.45846,0.45357,0.44868,0.44379,0.43891,0.435,0.43109,0.4262,0.42326,0.42033,0.41838,0.41642,0.41447,0.41447,0.41349,0.41251,0.41251,0.41251,0.41251,0.41251,0.41349,0.41544,0.4174,0.41935,0.42033,0.42131,0.42131,0.42033,0.41935,0.41642,0.41447,0.41349,0.41349,0.41642,0.42131,0.42815,0.43695,0.45064,0.46823,0.4868,0.50538,0.52395,0.54154,0.55816,0.57185,0.58162,0.58749,0.59042,0.59042,0.5914,0.59042,0.58944,0.58651,0.5826,0.57771,0.57087,0.56305,0.5523,0.54057,0.52884,0.51711,0.50733,0.49756,0.48778,0.47996,0.4741,0.47019,0.46823,0.46725,0.46628,0.46432,0.46041,0.4565}; 
  float arr[]={0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523,0.31523};
  int len=sizeof(arr)/sizeof(arr[0]);
  
  for(int i=0;i<len;i++) //ppg data Array display
  {
    float x=arr[i]*10;
    Serial.println(x,5);  
    delay(20);
  }
  
  //SAMPLES=len;
  /*for(int i=0; i<SAMPLES; i++)
    {
        microseconds = micros();    //Overflows after around 70 minutes!
        vReal[i] = arr[SAMPLES]; 
        vImag[i] = 0;
        while(micros() < (microseconds + sampling_period_us)){
        }
    }
    
  FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
  FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);
  double peak = FFT.MajorPeak(vReal, SAMPLES, SAMPLING_FREQUENCY);
   
  //PRINT RESULTS/
  //Serial.println(peak);     //Print out what frequency is the most dominant.
  
  for(int i=0;i<(SAMPLES/2); i++)
  {
       //View all these three lines in serial terminal to see which frequencies has which amplitudes
       Serial.print((i * 1.0 * SAMPLING_FREQUENCY) / SAMPLES, 2);
       Serial.print(" ");
       Serial.println(vReal[i], 1);  //View only this line in serial plotter to visualize the bins
       delay(50);
  }
   
  //delay(1000);  //Repeat the process every second OR:
  //while(1);       //Run code once
  Serial.println("Hello");
  //int freq_N=61;
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
  }
  */
  
  /*double D[3][freq_N]; //1st row for frequencies, 2nd row for magnitude, 3rd row for phase
  for(int i=0;i<freq_N;i++)
  {
    D[0][i]=-(pi/2)+i*0.1;
    double *d=fourier_tran(arr,len,D[0][i]);
    D[1][i]=*(d+0);
    D[2][i]=*(d+1); 
    Serial.println(i);
    delay(300);
  }*/
  
  //int z_crsg=zero_cross(arr,len);
  //Serial.println(z_crsg);
  //delay(1000);
  //float diff_arr[len-1];   //Differenced PPG signal
  //for(int i=1;i<=len-1;i++)
  //{
  //  diff_arr[i-1]=arr[i]-arr[i-1];
  //}
  
  int rise=rise_count(arr,len);
  int fall=fall_count(arr,len);
  float fr=(float)fall/rise;      //fall/rise count ratio 
  uint16_t m=max_amplitude(arr,len);   //maximum amplitude detection
  int sc[]={0,0,0};  //sensor connectivity vector
  int frame_vec[]={0,0,0,0}; //indicates if the current frame is okay
  if(fr>1.75 && fr<2.25)
  { 
    sc[0]=1;
  }
  if(m<max_amp)
  {
    sc[1]=1;
  }
  if(m>zero_amp)
  {
    sc[2]=1;;
  }
  
  if(sum(sc,3)>=2)
  {
    frame_vec[0]=1;
    Serial.println("Sensor well connected");
  }
  else
  {
    Serial.println("Sensor not connected properly");
  }
  delay(500);
  float base=base_drift(arr,len);
  Serial.print("Value is ");
  Serial.println(base);
  if(base_drift(arr,len)>=baseline_lower && base_drift(arr,len)<=baseline_upper)
  {
    frame_vec[1]=1;
    Serial.println("No Motion artefacts detected");
  }
  else
  {
    Serial.println("Motion detected");
  }
  delay(500);

  int sd=sig_saturation(arr,len);
  if(sd==0)
  {
    Serial.println("Saturated signal");
  }
  else
  {
    frame_vec[2]=1;
    Serial.println("Non-saturated signal");
  }
  delay(500); 
  //int d_z_crsg=zero_cross(diff_arr,len-1);
  int loc_max=local_maxima(arr,len);
  if(loc_max==1)
  {
    frame_vec[3]=1;
    Serial.println("Noise free signal");
  }
  else
  {
    Serial.println("Pulse noise detected");
  }
  delay(200);
  if(sum(frame_vec,4)==4)
  {
    Serial.println("Okay Frame");
    //transfer frame to feature extraction module here 
  }
  else
  {
    Serial.println("Not Okay with frame");
  }
  delay(300);
}

//Sensor connectivity detection 
int fall_count(float arr[],int l)
{
  int c=0;
  for(int i=0;i<l-1;i++)
  {
    if(arr[i]>arr[i+1])
    {
      c=c+1;
    }
  }
  return c;
}
int rise_count(float arr[],int l)
{
  int c=0;
  for(int i=0;i<l-1;i++)
  {
    if(arr[i]<arr[i+1])
    {
      c=c+1;
    }
  }
  return c;
}
int max_amplitude(float arr[],int l)
{
  float h=arr[0];
  for(int i=1;i<l;i=i+1)
  {
    if(arr[i]>h)
    {
      h=arr[i];
    }
  }
  if(h>max_amp)
  {
    return 0;
  }
  return 1;
  
}

//Noise segment detection
int zero_cross(float arr[],int l)
{
  int c=0;
  for(int i=0;i<l-1;i++)
  {
    if((arr[i]==0.0) && arr[i+1]!=0.0)
    {
      c=c+1; 
    }
  }
  return c;
}
int local_maxima(float arr[], int l)
{
  int frame_len=15;
  int a=0;
  int frames=l/frame_len;
  int maxima[frames];
  for(int k=0;k<frames;k++)
  {
    uint16_t h=0;
    for(int i=a;i<a+frame_len;i++)
    {
      if(arr[i]>h)
      {
        h=arr[i];
      }
    }
    maxima[k]=h;
    a=a+frame_len;
  }
  for(int i=0;i<frames-1;i++)
  {
    if(maxima[i]>max_amp && maxima[i+1]>max_amp)
    {
      return 1;
    }
    if(i<=frames-4)
    {
      if(maxima[i]<max_amp && maxima[i+1]<max_amp && maxima[i+2]<max_amp && maxima[i+3]<max_amp)
      {
        return 1;
      }
    }
    if(i<=frames-3)
    {
      if(maxima[i]<max_amp && maxima[i+2]<max_amp)
      {
        return 1;
      }
    }
  }
  return 0;
}


//Saturation detection
int sig_saturation(float arr[],int l)
{
  int c=0;
  for(int i=0;i<l-1;i++)
  {
    if(arr[i]<zero_amp && arr[i+1]<zero_amp)
    {
      c++;
    }
    if(arr[i]>max_amp && arr[i+1]>max_amp)
    {
      c++;
    }
  }
  if(c>=5)
  {
    return 0;
  }
  return 1;
}

//motion detection module
double base_drift(float arr[],int l)
{
  int N=10; //N pt moving average
  int frame=l-N+1; //number of frames
  float s;
  float sum=0.0;
  for(int i=0;i<frame;i++)
  {
   s=0.0;
   for(int j=i;j<N+i;j++)
   {
     s=s+arr[j];
   }
   float mavg=s/N;
   sum=sum+mavg;
  }
  double avg=sum/frame;
  return avg;
}

int sum(int n[],int l)
{
  float s=0;
  for(int i=0;i<l;i++)
  {
    s=s+n[i];
  }
  return s;
}

float butterworth(float f, float fc,int n)
{
  float h;
  h=1/sqrt(1+pow((fc/f),2*n)); 
  return h;
}

double *fourier_tran(float arr[],int n,float omega)
{
   double Xr=0.00;
   double Xi=0.00;
   double DTFT[2];
   for(int j=0;j<n;j++)
   {
     Xr=Xr+arr[j]*cos(omega*j);
     Xi=Xi+arr[j]*sin(omega*j);
   }
   float X=sqrt(pow(Xr,2)+pow(Xi,2));  //Fourier transform magnitude
   float ph=-atan(Xi/Xr);  //Fourier transform phase
   
   DTFT[0]=X;
   DTFT[1]=ph;
   double *p=DTFT;
   Serial.println(*p);
   delay(100);
   Serial.println(*(p+1));
   delay(100);
   return p;
}
/*float *inv_fourier(float DTFT[][flen],int l,int flen,int n) //n is the length of the time array 
{
  float arr[n];
  for(int i=0;i<n;i++)
  {
    float X=0.000;
    for(int j=0;j<l;j++)
    {
      X=X+DTFT[1][j]*cos(DTFT[2][j]+DTFT[0][j]*n);
    }
    arr[i]=1/(2*pi) * X;
  }
  float *p=arr;
  return p;
}
*/
