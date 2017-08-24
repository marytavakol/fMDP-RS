recom: DataProcessing.o model.o main.o
	g++ -o recom -g DataProcessing.o model.o main.o
DataProcessing.o: DataProcessing.cpp DataProcessing.h
	g++ -c -g DataProcessing.cpp
model.o: model.cpp model.h DataProcessing.h
	g++ -c -g model.cpp
main.o: main.cpp model.h DataProcessing.h	
	g++ -c -g main.cpp
