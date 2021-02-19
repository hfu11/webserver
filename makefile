files = msg_center.cpp taskGET.cpp taskPOST.cpp taskHEAD.cpp

webserver:webserver.cpp $(files)
	g++ $^ -o $@ -pthread -levent

clean:
	rm webserver