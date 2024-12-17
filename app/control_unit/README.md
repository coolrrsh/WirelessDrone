# Requirements
```
$ sudo apt-get install libwebsockets-dev
```

# Commands
```
$ gcc -o motor_server motor_server.c -lwebsockets
./motor_server &
$ gcc -o simulate_client simulate_client.c -lwebsockets
./simulate_client
```

