# OS Project - README file

## Introduction
Here we'll explain everything needed to get our server up and running and there's a short description of each module. Some modules have their own README file. Please consult these files for more information on the independent usage of the modules.


## Modules
- `queue`: A queue to store the jobs we receive.
- `networking`: Opens a socket on a given port and can handle multiple clients on that socket.
- `hash_table`: The hash table is used to store the (key,value) pairs.
- `misc`: Contains a program to generate randomly (key,value) pairs and store them into a text file.
- `ecommerce`: Is the overlay applied on the `server.c` file. Here we implement the functions asked by the `sever.c` file. 
- `scenarios`: Here you can find the three scenarios that were being asked by the project requirements. 
- `sema`: Is a nice workaround found on stack overflow to make this project UNIX and macOS compatible, since macOS has depracted unnamed semaphores. More about that [here](https://stackoverflow.com/questions/27736618/why-are-sem-init-sem-getvalue-sem-destroy-deprecated-on-mac-os-x-and-w/27847103#27847103).


## Make

A makefile is provided to compile the code. Just run `make` inside the folder. But before, I suggest you to configure the server inside the `config.h` file. 

The make will generate multiple files:

## `$ ./server`
This program takes no arguments. It opens multiple sockets on multiple ports, starting at 8000. Set the `config.h` file where you can configure how many ports should be opened. Also, there one can configure where the data should be stored, once the server gets killed. Furhter, the program prints debugging information inside the `stdout`. 

One can interact with the server by connection to it through TCP. One simple way would be to use `telnet`:

```
$ telnet localhost 8000 
```

Now you can send messages to the server. The server accepts a particular kind of message syntax:

**GET command**

Syntax: `GET item:str`

- `item` the item for which you'll get the current amount on stock available
- Server response: availabe amount of items or failure message.
- Example: `GET shoes`





**BUY command**

Syntax: `BUY item:str amount:int(+)`

- `item` the item you want to buy
- `amount` should be positive and bigger than 0
- Server response: success or failure message
- Example: `BUY shoes 10`



**UPD command**

Syntax: `UPD item:str amount:int(+/-)`

- `item` the item you want to update
- `amount` either positive to increase or negative to decrease
- Server response: success or failure message
- Example: `UPD shoes -3`


**ADD command**

Syntax: `ADD item:str amount:int(+)`

- `item` the item you want to add
- `amount` either positive to increase or negative to decrease



**SER commands**

- `SER KILL` kills the server
- `SER PRINT_JOBS` prints the current job list to the stdout of the server
- `SER PRINT_HT` prints the current hash table contents to the stdout of the server.



## `$ ./random_kv_gen`
To generate some random key value pairs, you can use this program. 

Syntax: `./random_kv_gen FILE:str NUMBER_OF_ITEMS:int VAL_RANGE:int`

- `FILE` should be the path to the file where you want your randomly generated data to be stored
- `NUMBER_OF_ITEMS` is the number of different pairs (items) you want to generate
- `VAL_RANGE` the range of the value (amount) for the items

**Example:**

`./random_kv_gen server_database.txt 100 20`

This generates 100 items with values between 0 and 19.



## Scenarios

To get the scenarios working, run first the `./server` program and then run one of these three scenarios, which can be found inside the `scenarios` folder:

### Scenario 1
This is straight forward. This tests the basic commands like `ADD`, `UPD`, and `BUY`. Responses of the server will be displayed to the stdout of the scenario program. The program also sends some `SER PRINT_HT` commands to see the changes on the stdout of the server program running. 

Usage: 

`$ ./scenario1`

### Scenario 2
This is scenario accepts a port as input, so one could connect to the server with multiple clients on different ports. Further, it executs some case-corner situations like buying items that don't exist, buying an amount of items not available, etc. To be able to also test mutual exclusion, set inside the `config.h` file the `SIMULATION_MUTLIACCESS` to 1. This will make the worker wait, such that a mutex situation is forced. 

Usage: 

`$ ./scenario2 PORT:int`

Example: 

`$ ./scenario2 8000`

### Scenario 3 
This is our stress-test, testing 1'000 connections on 100 different ports, 10 connections per port. Each connection sending 500 commands. The server handles thus 500'000 commands, with almost no delay in-between. Currently, this works for the most part, however, it looses a large amount of jobs because of a segmentation problem. This could be fixed to make the server handle even such amount of data. This segmentation problem happens only on these numbers of connections and works like a charm for lower numbers.

To run this scenario, there is a bash script provided, which will run the `./scenario3` programs 1'000 times. 

Usage:

`$ ./stress_test.sh`

