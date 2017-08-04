./output/cluster_sever: ./obj/main.o ./obj/wrap.o ./obj/cluster_event.o ./obj/cluster_network.o ./obj/cluster_sockfd.o ./obj/threadpool.o
	gcc -g -o ./output/cluster_sever ./obj/main.o ./obj/wrap.o ./obj/cluster_event.o ./obj/cluster_network.o ./obj/cluster_sockfd.o ./obj/threadpool.o -l pthread
./obj/main.o:./src/cluster_main.c ./include
	gcc -c ./src/cluster_main.c -I ./include -o ./obj/main.o
./obj/cluster_event.o:./src/cluster_event.c ./include
	gcc -c ./src/cluster_event.c -I ./include -o ./obj/cluster_event.o
./obj/cluster_network.o:./src/cluster_network.c ./include
	gcc -c ./src/cluster_network.c -I ./include/ -o ./obj/cluster_network.o
./obj/cluster_sockfd.o:./src/cluster_sockfd.c ./include
	gcc -c ./src/cluster_sockfd.c -I ./include -o ./obj/cluster_sockfd.o
./obj/threadpool.o:./src/threadpool.c ./include
	gcc -c ./src/threadpool.c -I ./include/ -o ./obj/threadpool.o
./obj/wrap.o:./src/wrap.c ./include
	gcc -c ./src/wrap.c -I ./include/ -o ./obj/wrap.o
clean:
	rm -rf ./obj/*.o
