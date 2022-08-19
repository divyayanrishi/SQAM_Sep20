import pandas as pd 
from scipy.stats import kurtosis
from scipy.stats import skew
import statistics
import entropy as ent
import matplotlib.pyplot as plt

dict={}
s_list=[]
k_list=[]
m_list=[]
v_list=[]
st_list=[]
apen_list=[]
sampen_list=[]
permen_list=[]
specen_list=[]
sys_peak=[]
dia_peak=[]
for i in range(1,54):
    df=pd.read_csv(r'D:\Research\Biomedical Image Proc\PPG records\Data set(Agniva)\Data set\merged'+str(i)+'.csv') 
    d_list=df[' PLETH'].tolist()
    for j in range(0,df.shape[0]-375,375):
        n_list=d_list[j:j+375]
        time_stamp=list(i for i in range(len(n_list)))
        plt.plot(time_stamp,n_list)
        plt.show()
        sys_list=[]
        dias_list=[]
        '''s=skew(n_list)
        s_list.append(s)
        k=kurtosis(n_list)
        k_list.append(k)
        m=statistics.mean(n_list)
        m_list.append(m)
        v=statistics.variance(n_list)
        v_list.append(v)
        st=statistics.stdev(n_list)
        st_list.append(st)
        apen=ent.app_entropy(n_list)
        apen_list.append(apen)
        sampen=ent.sample_entropy(n_list)
        sampen_list.append(sampen)
        permen=ent.perm_entropy(n_list,normalize=True)
        permen_list.append(permen)
        specen=ent.spectral_entropy(n_list,sf=100,normalize=True)
        specen_list.append(specen)
        '''
        for k in range(1,len(n_list)-1):
            if n_list[k]>n_list[k-1] and n_list[k]>n_list[k+1]:
                sys_list.append(n_list[k])
        print(sys_list)         
        for k1 in range(1,len(sys_list)-1):
            if sys_list[k1]<sys_list[k1+1] and sys_list[k1]<sys_list[k1-1]:
                dias_list.append(sys_list[k1])
                sys_list.remove(sys_list[k1])
                
        sys_peak.append(sys_list)
        dia_peak.append(dias_list)
dict={'Sys Peak':sys_peak,'Dia Peak':dia_peak}
# ,'ApEn':apen_list,'SampEn':sampen_list,'PEn':permen_list,'Spen':specen_list
# 'Mean':m_list,'Variance':v_list,'StandardDev':st_list,'Skewness':s_list,'Kurtosis':k_list
df1=pd.DataFrame(dict,columns=dict.keys())
df1.to_csv(r'D:\Research\Biomedical Image Proc\PPG records\Data set(Agniva)\Data set\PPG_alldata2.csv')        
  