import pandas as pd
import math
from sklearn.ensemble import RandomForestClassifier
from sklearn.model_selection import KFold
from sklearn.model_selection import train_test_split
from sklearn.svm import SVC
from sklearn import metrics
from sklearn.metrics import classification_report
from sklearn.cluster import KMeans
from sklearn.linear_model import LogisticRegression
import matplotlib.pyplot as plt
import pickle 
import warnings

warnings.filterwarnings("ignore")
'''
a=1
data_dict={}
hr_list=[]
sp_list=[]
row_num=[]
for i in range(1,54):
    df=pd.read_csv(r'D:\Research\Biomedical Image Proc\PPG records\Data set(Agniva)\Data set\merged'+str(i)+'.csv')
    for j in range(0,60003,125):
        hr=df.iloc[j,7]
        sp=df.iloc[j,10]
        if(hr==' '):
            hr=1
        if(sp==' '):
            sp=1
        hr_list.append(hr)
        sp_list.append(sp)
        point=str(i)+"-"+str(j)
        row_num.append(point)
        a+=1
data_dict['HR']=hr_list
data_dict['SpO2']=sp_list   
data_dict['Pointer']=row_num     
df=pd.DataFrame(data_dict,columns=['HR','SpO2','Pointer'])
df.to_csv(r'D:\Research\Biomedical Image Proc\PPG records\Data set(Agniva)\Data set\new_data_csv.csv')
'''

'''
df=pd.read_csv('D:\Research\Biomedical Image Proc\PPG records\Data set(Agniva)\Data set\intdata_csv.csv')
num_row=df.shape[0]
for j in range(num_row):
    if df.iloc[j,0]<80 or df.iloc[j,0]>120:
        df.iloc[j,2]=0  #needs hospital treatment
        continue
    if df.iloc[j,1]<95:
        df.iloc[j,2]=0
        continue
    df.iloc[j,2]=1  #marked safe
    
df.to_csv(r'D:\Research\Biomedical Image Proc\PPG records\Data set(Agniva)\Data set\intdata1_csv.csv')     
'''

def get_score(model,X_train,X_test,y_train,y_test):
    model.fit(X_train,y_train)
    return model.score(X_test,y_test)

def get_accuracy(model,X_train,X_test,y_train,y_test):
    model.fit(X_train,y_train)
    pickle.dump(model, open('model_trained.sav','wb'))
    y_pred=model.predict(X_test) 
    print(metrics.confusion_matrix(y_test,y_pred))
    return metrics.accuracy_score(y_test,y_pred) 


df=pd.read_csv(r'D:\Research\Biomedical Image Proc\PPG records\Data set(Agniva)\Data set\intdata3_csv.csv')
df=df.sample(frac=1).reset_index(drop=True)
n_row=math.floor(df.shape[0]//2)
df_test=df.loc[0:n_row] 
kf=KFold(10,False)
X=df_test.drop('Class',axis=1)
y=df_test['Class']
X_train,X_test,y_train,y_test=train_test_split(X,y,test_size=0.3)

'''
#SVM 
svcl=SVC(kernel='rbf',probability=True)
cm=get_accuracy(svcl,X_train,X_test,y_train,y_test)
spec=cm[1,1]/(cm[1,0]+cm[1,1])
print(spec)
'''

#Random Forest Classifier
rfc=RandomForestClassifier()
X_train,X_test,y_train,y_test=train_test_split(X,y,test_size=0.3) 
rfc.fit(X_train,y_train)
y_pred=rfc.predict(X_test)
cm=get_accuracy(rfc,X_train,X_test,y_train,y_test)
res=pickle.load(open('model_trained.sav','rb'))
mod_res=res.predict()
spec=cm[1,1]/(cm[1,0]+cm[1,1])
print(spec)


'''
a=1
for train_index,test_index in kf.split(X):
    X_train1,X_test1=X.iloc[train_index],X.iloc[test_index]
    y_train1,y_test1=y.iloc[train_index],y.iloc[test_index] 
    print("Fold"+str(a)+" = "+str(get_accuracy(svcl,X_train1,X_test1,y_train1,y_test1)))
    a+=1
'''

'''
#K Means algorithm
kmeans=KMeans(n_clusters=5,init='k-means++')
kmeans.fit(X_train)
print(kmeans.inertia_)
pred=kmeans.predict(X_test)
frame=pd.DataFrame(X_test)
frame['cluster']=pred
print(frame['cluster'].value_counts())
'''


#Logistic Regression
log_reg=LogisticRegression()
log_reg.fit(X_train,y_train)
y_pred=log_reg.predict(X_test)
cm=get_accuracy(log_reg,X_train,X_test,y_train,y_test)
spec=cm[1,1]/(cm[1,0]+cm[1,1])
print(spec)



'''
SSE=[]
for c in range(1,10):
    km=KMeans(n_jobs=-1,n_clusters=c,init='k-means++')
    km.fit(X)
    SSE.append(km.inertia_)
        
data_plot=pd.DataFrame({'Cluster':range(1,10),'SSE':SSE})
plt.plot(data_plot['Cluster'],data_plot['SSE'],marker='o')
plt.xlabel('Clusters')
plt.show()
#Verified - Cluster size 4 or 5
'''

def show_state(hr,spo2):
    data_dict={'HR':hr,'SpO2':spo2}
    X_df=pd.DataFrame(data_dict,index=[0])
    y_df=rfc.predict(X_df)
    if y_df==[0]:
        print("Patient is ill !")
    else:
        print("Patient is healthy")

probs=rfc.predict_proba(X_test)[:,-1]
fpr, tpr, threshold = metrics.roc_curve(y_test, probs)
roc_auc = metrics.auc(fpr, tpr)

plt.title('Receiver Operating Characteristic')
plt.plot(fpr, tpr, 'b', label = 'AUC = %0.2f' % roc_auc)
plt.legend(loc = 'lower right')
plt.plot([0, 1], [0, 1],'r--')
plt.xlim([0, 1])
plt.ylim([0, 1])
plt.ylabel('True Positive Rate')
plt.xlabel('False Positive Rate')
plt.show()


'''
df=pd.read_csv('D:\Research\Biomedical Image Proc\PPG records\Data set(Agniva)\Data set\intdata2_csv.csv')
data_dict={}
hr_list=[]
sp_list=[]
annot_list=[]
for i in range(0,df.shape[0]-2,3):
    s1=(df.iloc[i,0]+df.iloc[i+1,0]+df.iloc[i+2,0])//3
    s2=(df.iloc[i,1]+df.iloc[i+1,1]+df.iloc[i+2,1])//3
    s3=(df.iloc[i,2]+df.iloc[i+1,2]+df.iloc[i+2,2])/3
    hr_list.append(s1)
    sp_list.append(s2)
    annot_list.append(s3)
data_dict={'HR':hr_list,'SpO2':sp_list,'Class':annot_list}    
df1=pd.DataFrame(data_dict,columns=data_dict.keys())
df1.to_csv('D:\Research\Biomedical Image Proc\PPG records\Data set(Agniva)\Data set\intdata3_csv.csv')    
'''