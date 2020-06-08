# Voice chat

## Requirements
```
sudo apt-get install -y libpulse-dev
```

## SERVER 

``` shell
#To compile  
make
#To Run
./build/server 8080
```

### Client

``` shell
#To compile  
make

#TO RUN
./build/client 127.0.0.1 8080 risikesh grp1
```