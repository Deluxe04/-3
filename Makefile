#Компилятор и флаги
CXX      = g++
CXXFLAGS = -Wall -Wextra -std=c++11 -g

#Цели
TARGET_APP   = app
TARGET_TEST = test

#Заголовочные файлы — все, чтобы make отслеживал изменения
HEADERS = exceptions.h     \
          iterator.h        \
          option.h          \
          dynamic_array.h   \
          linked_list.h     \
          sequence.h        \
          array_sequence.h  \
          list_sequence.h   \
          bit_sequence.h    \
          adaptive_sequence.h \
          builder.h

#По умолчанию собираем оба
all: $(TARGET_APP) $(TARGET_TEST)

#Сборка интерактивного приложения
$(TARGET_APP): main.o
	$(CXX) $(CXXFLAGS) -o $(TARGET_APP) main.o

main.o: main.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c main.cpp -o main.o

#Сборка автотестов
$(TARGET_TEST): test.o
	$(CXX) $(CXXFLAGS) -o $(TARGET_TEST) test.o

test.o: test.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c test.cpp -o test.o

#Запуск
run: $(TARGET_APP)
	./$(TARGET_APP)

run-test: $(TARGET_TEST)
	./$(TARGET_TEST)

#Отладка
debug: CXXFLAGS += -DDEBUG -g
debug: all

#Очистка
clean:
	rm -f $(TARGET_APP) $(TARGET_TEST) main.o test.o

.PHONY: all run run-test debug clean


# make              # собрает app и test
# make run          # собирает и запускает app
# make run-test     # собирает и запускает test
# make clean        # удаляет app, test, все .o файлы
# make debug        # собирает с флагом -DDEBUG
