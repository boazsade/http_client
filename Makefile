# this would be used for building the source here, this assumes that you are using this throug the docker file!
# if you are trying to use this make file, you need to setup the correct path to boost
CLIENT_DIR=client
BASE=http_base

PHONY:  clean

all:
	make -C $(BASE) all
	make -C $(CLIENT_DIR) all

clean:
	@make -C $(BASE) clean
	@make -C $(CLIENT_DIR) clean

