CXX = g++
DEMO_INC = -I$(ROOT_DIR)/include -I$(ROOT_DIR) -I$(ROOT_DIR)/src -I/usr/include -I/usr/local/include
DEMO_LIBS = -lbioMicroKernel -lz -lpthread -ldl -lrt -lssl -lcrypto 
DEMO_LIB_PATH = -L$(ROOT_DIR)/build -L$(ROOT_DIR)/lib -L/usr/local/lib
FLASGS = -Wall -std=c++11 -g -D__const__= -pipe -W -Wall -Wno-unused-parameter \
			-fPIC -fno-omit-frame-pointer -Wno-implicit-fallthrough \
			-fstack-protector-all -Wno-deprecated-declarations \
			-Wno-class-memaccess \
			-Wno-unused-result -Wno-maybe-uninitialized
DEMOS =  demo_kernel

all: $(DEMOS)
	@echo "BUILD DEMO SUCCESS."

demo_kernel:
	@echo "$(CXX) demo_kernel.cpp -o $(OUTPUT_DIR)/$@ $(DEMO_INC)"
	@$(CXX) demo_kernel.cpp -o $(OUTPUT_DIR)/$@ $(DEMO_INC) $(DEMO_LIBS) $(DEMO_LIB_PATH) $(FLASGS)
