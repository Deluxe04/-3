# Компилятор и флаги
CXX      = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -g

# Цели
TARGET_APP   = app
TARGET_TEST  = test

# Заголовочные файлы (добавлены отсутствующие из вашего проекта)
HEADERS = exceptions.h     \
          option.h         \
          dynamic_array.h  \
          square_matrix.h  \
          diagonal_matrix.h\
          complex.h        \
          benchmark.h      \
          slau_solver.h    

# По умолчанию собираем оба
all: $(TARGET_APP) $(TARGET_TEST)

# Сборка интерактивного приложения (main.cpp)
$(TARGET_APP): main.o
	$(CXX) $(CXXFLAGS) -o $(TARGET_APP) main.o

main1.o: main.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c main1.cpp -o main.o

# Сборка автотестов (test1.cpp)
$(TARGET_TEST): test1.o
	$(CXX) $(CXXFLAGS) -o $(TARGET_TEST) test1.o

test1.o: test1.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c test1.cpp -o test1.o

# Запуск
run: $(TARGET_APP)
	./$(TARGET_APP)

run-test: $(TARGET_TEST)
	./$(TARGET_TEST)

# Режим отладки (без оптимизации, для пошаговой отладки)
debug: CXXFLAGS += -DDEBUG -O0
debug: all

# Режим бенчмарков (включает оптимизацию O2 для реалистичных замеров)
release: CXXFLAGS += -O2
release: all

# Очистка
clean:
	rm -f $(TARGET_APP) $(TARGET_TEST) *.o

.PHONY: all run run-test debug release clean
