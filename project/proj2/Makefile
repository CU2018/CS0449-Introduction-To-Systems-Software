
mydriver: mydriver.c mymalloc.c mymalloc.h
	gcc --std=gnu99 -Wall -Werror -m32 -g -o mydriver mydriver.c mymalloc.c

bigdriver: bigdriver.c mymalloc.c mymalloc.h
	gcc --std=gnu99 -Wall -Werror -m32 -g -o bigdriver bigdriver.c mymalloc.c
	
extradriver: extradriver.c extramalloc.c extramalloc.h
	gcc --std=gnu99 -Wall -Werror -m32 -g -o extradriver extradriver.c extramalloc.c 

clean:
	rm -f mydriver bigdriver