#ePIC BHCal Energy Calibration Multilayer Perceptron
# David Ruth 4/18/2025
# Contact druth@jlab.org with any questions

import sys
sys.path.append('/work/eic3/users/druth/lib')
import torch
import torch.nn as nn
from torch.autograd import Variable
import torch.nn.functional as F
import torch.optim as optim
from torch.utils.data import random_split
from torch.utils.data import TensorDataset, DataLoader
import torchvision
import torchvision.transforms as transforms
import sys
import random
from tqdm import tqdm
import numpy as np
import matplotlib.pyplot as plt
import matplotlib
import math
#from ray import tune
#from ray.air import Checkpoint, session
#from ray.tune.schedulers import ASHAScheduler
import statistics
#import pyro
#import pyro.distributions as dist
#from pyro.nn import PyroModule, PyroSample
#from pyro.infer import MCMC, NUTS
import random
from ROOT import TFile,TTree, TChain, TBranch, TH1F, TH2F, TCutG,TCanvas
from sklearn.model_selection import train_test_split

#Set data type for PyTorch to float
torch.set_default_dtype(torch.float64)

# ------------- HYPERPARAMETERS --------------

num_epochs = 10000 #How many iterations to run the training for

# Fundamentals

hidden_layers = 3 #Each hidden layer after the first will have half the neurons on the previous
h1 = 256 #Number of neurons on first hidden layer
neuron_decay = 0.5 #Fraction that the number of neurons will be reduced to on each hidden layer
init_lr = 0.02 # Initial Learning Rate

#Learning Rate Scheduler
lr_patience = 100 #How many epochs without significant progress needed to say we've stalled
lr_cd = 100 #Minimum epochs to wait after reducing learning rate before reducing again
lr_factor = 0.5 #What factor to multiply the learning rate by every time we need to decay
lr_thresh = 1e-4 #How much of a reduction in the learning rate counts as "progress"

# Method Dependent
leakRate = 0.01 #For LeakyReLU activation function

# More exotic tools
dropout = 0.0 #If dropout = 0.0, no dropout layer will be used


# ALSO CONSIDER CHANGING THE FOLLOWING TO DIFFERENT FUNCTIONS:
# Activation Function
# Loss Function (It is possible to define a custom loss function if needed)
# Optimizer

# -------------------------------------------

# FUNCTIONS LIST


def Scaler(val):
    # Used for "feature scaling", which scales the input matrix so all the values run over a common range.
    # This makes it easier for a neural network to understand
	dims = list(range(val.dim() - 1))
	mean = torch.mean(val, dim=dims)
	std = torch.std(val, dim=dims)
	transform = (val - mean) / (std + 1e-8)
	return transform




#Declare training file location
training_file = "/work/eic3/users/druth/EpicBHCalPTDRStudies/reconstruction/macros/TDR_pi-_tuple.root"

#Load rootfile, loop through events and fill a python list with the training information
rootfile = TFile(training_file, "READ")
ntuple = rootfile.Get("ntForCalib")
pre_list = []
#y_list = []
for event in ntuple:
    pre_list.append([[event.eLeadBHCal,
                    event.eLeadBEMC,
                    event.eSumScFiLayer1,
                    event.eSumScFiLayer2,
                    event.eSumScFiLayer3,
                    event.eSumScFiLayer4,
                    event.eSumScFiLayer5,
                    event.eSumScFiLayer6,
                    event.eSumScFiLayer7,
                    event.eSumScFiLayer8,
                    event.eSumScFiLayer9,
                    event.eSumScFiLayer10,
                    event.eSumScFiLayer11,
                    event.eSumScFiLayer12,
                    event.eSumImageLayer1,
                    #event.eSumImageLayer2,
                    event.eSumImageLayer3,
                    event.eSumImageLayer4,
                    #event.eSumImageLayer5,
                    event.eSumImageLayer6,
                    ],[event.ePar]])
    #y_list.append([event.ePar])


#Randomly Split into Test and Train sets

train_set, test_set = train_test_split(pre_list,test_size=0.4)
x_list = []
y_list = []
x_list_test = []
y_list_test = []

#print(train_set[0)
#print(len(train_set))
#print(test_set[0])
#print(len(test_set))

for el in train_set:
    x_list.append(el[0])
    y_list.append(el[1])
for el in test_set:
    x_list_test.append(el[0])
    y_list_test.append(el[1])

#Get size that input layer needs to be based on training set
inp_size = len(x_list[0])


#Convert training and test data into NumPy Array, then PyTorch Tensor and apply feature scaling
x_list = np.array(x_list)
x_tensor = torch.DoubleTensor(x_list)
x_tensor = Scaler(x_tensor)
y_list = np.array(y_list)
y_tensor = torch.DoubleTensor(y_list)


x_list_test = np.array(x_list_test)
x_tensor_test = torch.DoubleTensor(x_list_test)
x_tensor_test = Scaler(x_tensor_test)
y_list_test = np.array(y_list_test)
y_tensor_test = torch.DoubleTensor(y_list_test)


# old 
#h2 = int(np.floor(0.5*h1))
#h3 = int(np.floor(0.5*h2))

#Build the neural network's architecture

layers = [nn.Linear(inp_size,h1)]

for i in range(hidden_layers):
    layers.append(nn.LeakyReLU(leakRate)) # Activation Function
    if dropout > 0.0:
        layers.append(nn.Dropout(dropout)) # Dropout Layer
    layers.append(nn.Linear(h1,math.ceil(neuron_decay*h1))) #Hidden Layer
    h1 = math.ceil(neuron_decay*h1) #Reduce size for next hidden layer

layers.append(nn.LeakyReLU(leakRate))
layers.append(nn.Linear(h1,1))

model = nn.Sequential(*layers)

# old
#model = nn.Sequential(
#		nn.Linear(inp_size,h1),
#		nn.LeakyReLU(leakRate),
#		#nn.Dropout(0.1),
#		nn.Linear(h1,h2),
#		nn.LeakyReLU(leakRate),
#		#nn.Dropout(0.1),
#		nn.Linear(h2,h3),
#		nn.LeakyReLU(leakRate),
#		nn.Linear(h3,1)
#		)


loss_fn = nn.HuberLoss() # Loss Function

optimizer = torch.optim.Adam(model.parameters(), lr=init_lr) # Optimizer
scheduler = torch.optim.lr_scheduler.ReduceLROnPlateau(optimizer, 'min',patience=lr_patience,cooldown=lr_cd,factor=lr_factor,threshold=lr_thresh,eps=1e-11) # Learning Rate Scheduler

losslist = []
epochlist = []

#Set up a progress bar to track the training	
pbar = tqdm(range(num_epochs),desc="Loss: ")
minloss = 100.0

#Train the model
for n in pbar:
	y_pred = model(x_tensor) #Run the model once
	loss = loss_fn(y_pred, y_tensor) #Check how close the result is to truth
	optimizer.zero_grad() #Tweak the weights
	loss.backward()
	optimizer.step() #Move the optimizer forward
	pbar.set_description("Loss: "+str(round(loss.item(),8))+" | "+"LR: "+str(scheduler.get_last_lr()))
	losslist.append(loss.item())
	if loss.item() < minloss:
		minloss = loss.item()
	epochlist.append(n)
	scheduler.step(loss)

#Save a plot of the loss
plt.plot(epochlist,losslist,'r-')
plt.ylabel("Loss")
plt.xlabel("Epochs")
plt.xlim(0,num_epochs)
plt.yscale('log')
plt.savefig("Loss_Plot.png")
plt.clf()


#Set up test dataset
y_final = y_tensor_test.tolist()

des_y = model(x_tensor_test).tolist()


#hist_2gev = []
#hist_5gev = []
#hist_10gev = []

energy_list = []
hist_list = []
#Compile list of energies
for index,item in enumerate(y_final):
    item_true = item[0]
    for k in np.arange(0,30,0.5):
        if item_true > k - 0.25 and item_true < k + 0.25 and k not in energy_list:
            energy_list.append(k)
            hist_list.append([])

#Loop through test dataset and see how well we did
for index,item in enumerate(y_final):
    item_true = item[0]
    item_calc = des_y[index][0]
    for i,j in enumerate(energy_list):
        if(item_true > j - 0.25 and item_true < j + 0.25):
            hist_list[i].append(item_calc)
    #if(item_true > 1.0 and item_true < 3.0):
    #    hist_2gev.append(item_calc)
    #elif(item_true >= 3.0 and item_true < 7.0):
    #    hist_5gev.append(item_calc)
    #elif(item_true >= 7.0 and item_true < 13.0):
    #    hist_10gev.append(item_calc)

#Generate energy resolution histogram
for index,item in enumerate(energy_list):
    plt.hist(hist_list[index], bins=100, histtype='step', label=str(item)+" GeV")
#plt.hist(hist_2gev, bins=100, histtype='step', edgecolor='blue')
#plt.hist(hist_5gev, bins=100, histtype='step', edgecolor='green')
#plt.hist(hist_10gev, bins=100, histtype='step', edgecolor='red')
plt.xlabel("Energy (GeV)")
plt.xlim(0.0,20.0)
plt.legend()
plt.savefig("NN_Hist.png")
plt.clf()


