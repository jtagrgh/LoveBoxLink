$(shell test -d obj || mkdir obj)
$(shell test -d bin || mkdir bin)

CXXFLAGS += -std=c++17

core_modules := Message PixelBroadcastMessage SerialData Shared
core_modules_paths := $(addprefix src/, $(core_modules))
core_include := $(addprefix -I, $(core_modules_paths))
core_objects := $(addsuffix .o, $(core_modules))
core_objects_paths := $(addprefix obj/, $(core_objects))

vpath %.hpp $(core_modules_paths)
vpath %.cpp $(core_modules_paths)
-include $(core_objects_paths:.o=.d)

test_modules := Tester Test TestSerialData
test_modules_paths := $(addprefix tests/, $(test_modules))
test_include := $(addprefix -I, $(test_modules_paths))
test_objects := $(addsuffix .o, $(test_modules))
test_objects_paths := $(addprefix obj/, $(test_objects))

vpath %.hpp $(test_modules_paths)
vpath %.cpp $(test_modules_paths)
-include $(test_objects_paths:.o=.d)

Tester: CXXFLAGS += $(core_include) $(test_include) 
Tester: $(core_objects_paths) $(test_objects_paths)
	g++ $(CXXFLAGS) -o bin/Tester $^

server_modules := Server
server_modules_paths := $(addprefix src/, $(server_modules))
server_objects := $(addsuffix .o, $(server_modules))
server_objects_paths := $(addprefix obj/, $(server_objects))

vpath %.hpp $(server_modules_paths)
vpath %.cpp $(server_modules_paths)
Server: CXXFLAGS += $(core_include)
Server: $(server_objects_paths) $(core_objects_paths)
	g++ $(CXXFLAGS) -o bin/Server $^

Client:
	arduino-cli compile \
		--fqbn arduino:renesas_uno:unor4wifi \
		--library "src/Message","src/PixelBroadcastMessage","src/SerialData","src/Shared" \
		src/client/client.ino

clean:
	rm -rf bin/* obj/*

obj/%.o: %.cpp
	g++ $(CXXFLAGS) -c -o $@ $< -MMD -MP


