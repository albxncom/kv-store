/**
 * Configuration file
 */



#define PORTS 100					// For each port a thread will start listening on a TCP socket.
#define PORTS_START 8000			// Where the port numbering will start, i.e. opens a port on 
									// 8000, 8001, 8002, ..., 8000+PORTS


#define BACKLOG_PER_THREAD 10		// The amount of connections allowed to be queued per thread
									// at most (provided to listen(., int backlog_size))

// Queue
#define QL 278						// max length of chars for queue entry

#define SERVER_DATABASE_FILE "./server_database.txt" // Where to store state


#define SIMULATION_MUTLIACCESS 0	// Set this to 1 if you want to test scenario 2
#define DISABLE_WORKER 0			// If you want to test the job count