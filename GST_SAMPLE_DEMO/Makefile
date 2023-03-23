

SHELL	:= /bin/bash
ifndef PREFIX_DIR
	PREFIX_DIR=/usr
endif

CXX=g++ -w

SI_INCLUDES   = -I${PREFIX_DIR}/include/gstreamer-1.0 -I${PREFIX_DIR}/include -I${PREFIX_DIR}/include/cnn  -I$(PREFIX_DIR)/include/opencv4 -I$(PREFIX_DIR)/include/VX -I$(PREFIX_DIR)/include/isp `pkg-config --cflags --libs gstreamer-1.0 gstreamer-app-1.0 gstreamer-gsp-1.0 gstreamer-base-1.0 gstreamer-video-1.0 gstreamer-audio-1.0` `pkg-config --cflags  glib-2.0` -I/usr/include/gstreamer-1.0 -I/usr/include/glib-2.0 
SI_LIBS       = `pkg-config --libs gstreamer-1.0 gstreamer-video-1.0 gstreamer-base-1.0 gstreamer-app-1.0 gstreamer-gsp-1.0 gstreamer-audio-1.0 opencv4`
app_src = gstAppDemo.cpp

PYTHON=$(shell command -v python3)
PYTHON_VERSION_CUR=$(shell $(PYTHON) -c 'import sys; print("%d.%d"% sys.version_info[0:2])')
ifeq ($(PYTHON_VERSION_CUR), 3.6)
    SI_INCLUDES += `python3-config --includes`
    SI_LIBS += `python3-config --ldflags`
endif
ifeq ($(PYTHON_VERSION_CUR), 3.8)
    SI_INCLUDES += `python3-config --includes --embed`
    SI_LIBS += `python3-config --ldflags --embed`
endif

ifndef CFG
	CFG=Release
else ifeq ($(CFG), $(filter $(CFG), debug DEBUG))
	CFG=Debug
else ifeq ($(CFG), $(filter $(CFG), release RELEASE))
	CFG=Release
else ifneq ($(CFG), $(filter $(CFG), Release Debug))
	$(error "Unknown build configuration: $(CFG)")
endif

ifeq ($(CFG), Debug)
	CXX+= -g -gdwarf-2
endif

CXX+= -std=c++0x

.PHONY : all

all: ${CFG}/blzDemo
si: ${CFG}/blzDemo

${CFG}/blzDemo: $(CFG) $(app_src) $(app_headers)
	@echo "INFO: Building $@ targeting Hardware Accelerator..."
	@$(CXX) \
	$(OPTIONS) \
	$(SI_INCLUDES) \
	$(app_src) \
	-o $@ \
	-L$(PREFIX_DIR)/lib ${SI_LIBS}

$(CFG):
	@mkdir -p $@

clean:
	@rm -rf Debug Release .tcdnn/ .gpu/ *.y
