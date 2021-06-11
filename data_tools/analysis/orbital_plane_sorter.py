#!/usr/bin/env python
# coding: utf-8

# In[158]:


from pprint import pprint
import json
import matplotlib.pyplot as plt

with open("oneweb.txt", "r") as file:
    data = json.load(file)

raans = [sat["RA_OF_ASC_NODE"] for  sat in data]
incs = [sat["INCLINATION"] for sat in data]

plt.scatter([range(len(raans))], raans)
plt.show()
planes = [[data[0]]]
nice = 0 
for sat1 in data[1:]:
#     print("sat1: ", sat1["OBJECT_NAME"])
    for idx in range(0,len(planes)+1):
#         print(len(planes))
#         if plane[idx] is None:
#             planes[planeidx] = [sat1]
#         print(idx)
        if idx == len(planes): # last 
#             print("new breaak / new plane")
            planes.append([sat1])
            break
        else:
            sat2 = planes[idx][0]
            if abs(sat1["RA_OF_ASC_NODE"] - sat2["RA_OF_ASC_NODE"]) < 2:
                planes[idx].append(sat1)
                nice += 1
                break


# In[175]:


for plane in planes[:]:
    plt.scatter( [range(len(plane))], [sat["RA_OF_ASC_NODE"] for sat in plane ], )
plt.title(f"OneWeb {len(planes)} Current Orbital Planes ");
plt.ylabel("RAAN [deg]");
plt.xlabel("Number of Satellites per plane");
plt.legend([f"Plane {idx}" for idx in range(len(planes))], bbox_to_anchor=(1,1), loc="upper left");


# In[177]:


for plane in planes[8:9]:
    plt.scatter( [range(len(plane))], [sat["MEAN_ANOMALY"] for sat in plane ], )
plt.title(f"OneWeb {len(planes)} Current Orbital Planes ");
plt.ylabel("MEAN ANOMALY [deg]");


# In[155]:


plt.scatter([range(len(data))], [sat["BSTAR"] for sat in data] )


# In[ ]:





# In[165]:


planes[0][0]


# In[72]:


len(planes[0][1])


# In[29]:


abs(data[0]["RA_OF_ASC_NODE"] - data[5]["RA_OF_ASC_NODE"])


# In[31]:


data[10]["RA_OF_ASC_NODE"]


# In[ ]:




