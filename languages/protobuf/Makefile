
all: helloworld



helloworld:
	@echo "helloworld building"; \
		protoc -I helloworld/ helloworld/helloworld.proto --go_out=plugins=grpc:helloworld && \
		go run server/main.go && \
		go run client.go

helloworld-cpp:
	protoc -I helloworld/ --grpc_out=. --plugin=protoc-gen-grpc=grpc_cpp_plugin helloworld/helloworld.proto && \
	protoc -I helloworld/ --cpp_out=. helloworld/helloworld.proto

helloworld-docker:
	docker run --rm -it -v $$(pwd)/helloworld:/helloworld grpc/go \
		protoc --go_out=plugins=grpc,import_path=helloworld:/helloworld -I /helloworld/ /helloworld/helloworld.proto

install-deps:
	go get google.golang.org/grpc
	go get -u github.com/golang/protobuf/protoc-gen-go


clean:
	rm helloworld/*.pb.go
