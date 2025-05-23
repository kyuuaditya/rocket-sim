all: compile icon link run clean

compile:
	g++ -I"C:\SFML-2.6.2\include" -c src/main.cpp -o src/main.o

icon: 
	windres src/resource.rc -o src/resource.o

link: 
	g++ -I"C:\SFML-2.6.2\include" src/main.o src/resource.o -o app/app.exe -L"C:\SFML-2.6.2\lib" -lmingw32 -lsfml-graphics -lsfml-window -lsfml-system -lsfml-main 

run:
	./app/main.exe

clean:
	del /Q "src\*.o" 2>nul || exit 0
	@echo _____ deleted object files and executable.