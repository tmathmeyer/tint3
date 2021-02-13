
tint3:
	@make -C src tint3
	@cp src/tint3 ./

clean:
	@make -C src clean
	@rm ./tint3