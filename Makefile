all : server random_kv_gen scenario1 scenario2 scenario3 clean_objs

clean:
	rm server random_kv_gen scenarios/scenario1 scenarios/scenario2 scenarios/scenario3

clean_objs:
	rm *.o */*.o

# server
#-------
server : server.o ./sema/sema.o ./networking/networking.o ./ecommerce/ecommerce.o ./queue/queue.o ./hash_table/hash_table.o
	gcc -o server server.o ./sema/sema.o ./networking/networking.o ./ecommerce/ecommerce.o ./queue/queue.o ./hash_table/hash_table.o
	
server.o : server.c ./sema/sema.h ./networking/networking.h ./ecommerce/ecommerce.h ./config.h
	gcc -c server.c	


# sema module
#-------------
./sema/sema.o : ./sema/sema.c ./sema/sema.h
	gcc -c ./sema/sema.c -o ./sema/sema.o

# queue module
#-------------
./queue/queue.o : ./queue/queue.c ./queue/queue.h
	gcc -c ./queue/queue.c -o ./queue/queue.o

# networking module
#-------------
./networking/networking.o : ./networking/networking.c ./networking/networking.h
	gcc -c ./networking/networking.c -o ./networking/networking.o

# ecommerce module
#-------------
./ecommerce/ecommerce.o : ./ecommerce/ecommerce.c ./ecommerce/ecommerce.h
	gcc -c ./ecommerce/ecommerce.c -o ./ecommerce/ecommerce.o


# hash_table module
#-------------
./hash_table/hash_table.o : ./hash_table/hash_table.c ./hash_table/hash_table.h
	gcc -c ./hash_table/hash_table.c -o ./hash_table/hash_table.o

# miscs
#----------
random_kv_gen : ./misc/random_kv_gen.c
	gcc misc/random_kv_gen.c -o random_kv_gen



# scenarios
#---------
scenario1 : ./scenarios/scenario1.c
	gcc ./scenarios/scenario1.c -o ./scenarios/scenario1
scenario2 : ./scenarios/scenario2.c
	gcc ./scenarios/scenario2.c -o ./scenarios/scenario2
scenario3 : ./scenarios/scenario3.c
	gcc ./scenarios/scenario3.c -o ./scenarios/scenario3
