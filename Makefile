CXX=g++
.PHONY: setup create_rootfs sendfs clean fclean

build: src/containd.cpp src/containd.hpp src/cgroupsv2.hpp
	$(CXX) src/containd.cpp -o containd $(CFLAGS)

clean:
	rm -f containd *.o 
fclean:
	sudo rm -rf rootfs containd alpine-minirootfs-3.17.0-x86_64.tar.gz
setup:
	sudo ./setup ${O}

create_rootfs:
	make fclean && \
	make setup O="--no-cgroups" && \
	make build && \
	./containd -c "apk update && apk add ${PKG}" && \
	tar czf rootfs.tar.gz rootfs && \
	make fclean
sendfs:
	mv $(f) rootfs/$(t)/${f}
