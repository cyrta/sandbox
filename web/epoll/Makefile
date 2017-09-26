

all: server



server:
	gcc server.c -o server

docker:
	docker run -it -p 1555:1555 -v $(pwd):/src gcc:latest bash

clean:
	rm -f server

