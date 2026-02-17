# How to use Geant4 with Docker in Windows

Docker can be used to create a linux environment, files can be saved locally for access and the display can be displayed locally. The image is approx. 10GB
 
## Initial Setup
 
### Docker
* Install docker from the website:
https://www.docker.com/products/docker-desktop/
 
* Docker needs to be running to use Geant4
 
### Xlaunch 
 
* Install xlaunch:
https://sourceforge.net/projects/vcxsrv/
 
* Open xlaunch and use the following settings
 
 ![image](https://user-images.githubusercontent.com/91849139/219609496-c22e7c0a-2163-4b15-9495-a3d66c9a0ae5.png)

![image](https://user-images.githubusercontent.com/91849139/219609512-f495567e-b37b-4cc1-8793-79f749a0de55.png)

![image](https://user-images.githubusercontent.com/91849139/219609561-20827e36-c765-4e98-8c20-299f0b6b55e6.png)


* On the next screen press Finish.
 
### vscode 
* Install vscode
 
### Install vscode extensions 
* Dev containers
 
### Setup Docker
* Navigate to the folder in Windows File Explorer where you would like to save the Geant4 files 
* Create a file in this folder called 'dockerfile' (no extension) with the following contents. This is the instructions for docker
 ```
FROM ubuntu 
RUN apt-get update && apt-get -y upgrade
ENV DEBIAN_FRONTEND=noninteractive
 RUN apt-get install -y --no-install-recommends \ 
 git \ 
 wget \ 
 g++ \ 
 ca-certificates \ 
 make \ 
 cmake \ 
 cmake-curses-gui \ 
 python3-opencv \ 
 && rm -rf /var/lib/apt/lists/* 
ENV PATH="/root/miniconda3/bin:${PATH}" 
ARG PATH="/root/miniconda3/bin:${PATH}" 
RUN wget https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh \ 
&& mkdir /root/.conda \ 
&& bash Miniconda3-latest-Linux-x86_64.sh -b \ 
&& rm -f Miniconda3-latest-Linux-x86_64.sh \ 
&& echo "Running $)conda --version" && \ 
conda init bash && \ 
. /root/.bashrc && \ 
conda update conda && \ 
conda create -c conda-forge -n G4 python=3.8 root geant4 && \ 
echo 'conda activate G4' >> ~/.bashrc 
 ```
* In the directory bar type cmd and press enter. This will open a command line terminal in that folder
* In the terminal run:
```
Ipconfig
```
This will find the PC IP address (IPv4 Address first one 192.....). Save this for later
 
* Create the docker image (this will take a while) by running:
```
 docker build -t geant4 .
 ```
This will create an ubuntu image with all the requirements for Geant4 and root using conda
* When this has finished, create the docker container, run:
```
cd
```
To get the current location, save this for the next step.
* To create the docker container run:
```
docker run -it -v LOCALFILES:/root/myFiles -e DISPLAY=IPADDRESS:0.0 --name geantContainer -m 2g geant4
```
Replace LOCALFILES with the location of the directory where you want to save everything (from cd).
Replace IPADDRESS with the IP address found earlier
 
### vscode - using the container
*	Open vscode
*	Go to view – command pallete
*	Click Dev Containers: Attach to a running container 
*	Click geant4Container
*	The container will open and can be used in VScode as if they were local
*	Any files in /root/myFiles will be available on windows and in the container
*	The Geant4 examples can be found in /root/miniconda3/envs/Geant..../share/examples
The … is the version number, which depends on which the newest version on conda is currently 11.0
 
 
### Finishing
*	Close vscode
*	Close docker
*	Close xlaunch
 
 
## Subsequent use of Container
 
To use the container again
*	Open xlaunch with the above settings
*	Open docker
*	Under containers click start on the geantContainer
 
![image](https://user-images.githubusercontent.com/91849139/219609625-8983e380-7281-4262-83fb-eefb6db50944.png)

*	Go to vscode
*	Go to view – command pallete
*	Click Dev Containers: Attach to a running container 

*	Click geant4Container
*	The container will open and can be used in VScode as if they were local
*	Any files in /root/myFiles will be available on windows and in the container
 

