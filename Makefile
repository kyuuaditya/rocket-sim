all: compile icon link clean run

compile:
	g++ -I"C:\SFML-2.6.2\include" -c src/main.cpp -o src/main.o
	g++ -I"C:\SFML-2.6.2\include" -c src/flight_data.cpp -o src/flight_data.o
	g++ -I"C:\SFML-2.6.2\include" -c src/graphics.cpp -o src/graphics.o
	g++ -I"C:\SFML-2.6.2\include" -c src/rocket.cpp -o src/rocket.o

icon: 
	windres src/resource.rc -o src/resource.o

link: 
	g++ -I"C:\SFML-2.6.2\include" src/main.o src/flight_data.o src/graphics.o src/rocket.o src/resource.o -o app.exe -L"C:\SFML-2.6.2\lib" -lmingw32 -lsfml-graphics -lsfml-window -lsfml-system -lsfml-main 

run:
	./app.exe

clean:
	del /Q "src\*.o" 2>nul || exit 0
	@echo _____ deleted object files and executable.