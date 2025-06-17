CXX = g++

PY_VERSION := $(shell python3.11 -c "import sys; print(f'{sys.version_info.major}.{sys.version_info.minor}')")
PY_INCLUDE := $(shell python3.11 -c "import sysconfig; print(sysconfig.get_path('include'))")
PY_LIB := $(shell python3.11 -c "import sysconfig; print(sysconfig.get_config_var('LIBDIR'))")

PYBIND11_INCLUDE := $(shell python3.11 -m pybind11 --includes)

EIGEN_INCLUDE = -I/opt/homebrew/include/eigen3

CXXFLAGS = -O3 -Wall -shared -std=c++11 -fPIC
CXXFLAGS += $(PYBIND11_INCLUDE)
CXXFLAGS += $(EIGEN_INCLUDE)
CXXFLAGS += -I$(PY_INCLUDE)

# linker flags
# LDFLAGS = -L$(PY_LIB) -lpython$(PY_VERSION)

#macos flags
LDFLAGS += -undefined dynamic_lookup

# Source and target
SOURCE = standalonePca.cpp
TARGET = standalonePca$(shell python3.11-config --extension-suffix)

# default target
all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CXX) $(CXXFLAGS) $(SOURCE) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)

install-deps:
	brew install eigen3
	python3.11 -m pip install pybind11 numpy

# Install dependencies (conda alternative)
install-deps-conda:
	conda install -c conda-forge eigen pybind11 numpy

debug:
	@echo "Python version: $(PY_VERSION)"
	@echo "Python include: $(PY_INCLUDE)"  
	@echo "Python lib: $(PY_LIB)"
	@echo "Pybind11 includes: $(PYBIND11_INCLUDE)"
	@echo "Eigen include: $(EIGEN_INCLUDE)"
	@echo "Full compile command:"
	@echo "$(CXX) $(CXXFLAGS) $(SOURCE) -o $(TARGET) $(LDFLAGS)"

.PHONY: all clean install-deps install-deps-conda debug