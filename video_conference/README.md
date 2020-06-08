# Video Conference

This is a simple real time video conference application which is deployed on cloud and can be tested.



## USAGE

### Server

```shell
# requirements 
sudo apt-get install -y gcc make git

# Make and seploy server

git clone https://github.com/risikesh/RTOS.git
cd RTOS/video_conference
make server

# deploy 

./build/turn_server 8080
```

### Client

```shell
# requirements 
sudo apt-get install -y git make  libsdl2-dev libpulse-dev libsdl2-image-dev libsdl2-mixer-dev libsdl2-ttf-dev

# Make and seploy server

git clone https://github.com/risikesh/RTOS.git
cd RTOS/video_conference
make client

# deploy 

./build/client meet.risikesh.xyz 8080 <username> /dev/video1 

#Note here meet.risikesh.xyz is mapped to server ip address
```

