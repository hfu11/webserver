files = msg_center.cpp taskGET.cpp taskPOST.cpp

webserver:webserver.cpp $(files)
	g++ $^ -o $@ -pthread -levent

clean:
	rm webserver