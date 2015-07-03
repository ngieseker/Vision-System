# GNU Make solution makefile autogenerated by Premake
# Type "make help" for usage help

ifndef config
  config=release
endif
export config

PROJECTS := server client virtual_client markertrainer capture_cali capture intrinsic extrinsic testing ClusterServer ClusterComms DataManager fxbcam Configuration common network camera apriltags uart findblobs serialtest ping rays_test hash_test hashmap_test jpeg_test

.PHONY: all clean help $(PROJECTS)

all: $(PROJECTS)

server: ClusterServer DataManager fxbcam Configuration ClusterComms common network
	@${MAKE} --no-print-directory -C . -f server.make

client: camera fxbcam Configuration ClusterComms common network
	@${MAKE} --no-print-directory -C . -f client.make

virtual_client: fxbcam Configuration ClusterComms common network
	@${MAKE} --no-print-directory -C . -f virtual_client.make

markertrainer: camera fxbcam
	@${MAKE} --no-print-directory -C . -f markertrainer.make

capture_cali: camera Configuration
	@${MAKE} --no-print-directory -C . -f capture_cali.make

capture: camera
	@${MAKE} --no-print-directory -C . -f capture.make

intrinsic: camera
	@${MAKE} --no-print-directory -C . -f intrinsic.make

extrinsic: apriltags
	@${MAKE} --no-print-directory -C . -f extrinsic.make

testing: camera
	@${MAKE} --no-print-directory -C . -f testing.make

ClusterServer: 
	@${MAKE} --no-print-directory -C . -f ClusterServer.make

ClusterComms: 
	@${MAKE} --no-print-directory -C . -f ClusterComms.make

DataManager: 
	@${MAKE} --no-print-directory -C . -f DataManager.make

fxbcam: 
	@${MAKE} --no-print-directory -C . -f fxbcam.make

Configuration: 
	@${MAKE} --no-print-directory -C . -f Configuration.make

common: 
	@${MAKE} --no-print-directory -C . -f common.make

network: 
	@${MAKE} --no-print-directory -C . -f network.make

camera: 
	@${MAKE} --no-print-directory -C . -f camera.make

apriltags: 
	@${MAKE} --no-print-directory -C . -f apriltags.make

uart: 
	@${MAKE} --no-print-directory -C . -f uart.make

findblobs: camera fxbcam
	@${MAKE} --no-print-directory -C . -f findblobs.make

serialtest: uart
	@${MAKE} --no-print-directory -C . -f serialtest.make

ping: common network
	@${MAKE} --no-print-directory -C . -f ping.make

rays_test: camera fxbcam
	@${MAKE} --no-print-directory -C . -f rays_test.make

hash_test: 
	@${MAKE} --no-print-directory -C . -f hash_test.make

hashmap_test: 
	@${MAKE} --no-print-directory -C . -f hashmap_test.make

jpeg_test: common camera
	@${MAKE} --no-print-directory -C . -f jpeg_test.make

clean:
	@${MAKE} --no-print-directory -C . -f server.make clean
	@${MAKE} --no-print-directory -C . -f client.make clean
	@${MAKE} --no-print-directory -C . -f virtual_client.make clean
	@${MAKE} --no-print-directory -C . -f markertrainer.make clean
	@${MAKE} --no-print-directory -C . -f capture_cali.make clean
	@${MAKE} --no-print-directory -C . -f capture.make clean
	@${MAKE} --no-print-directory -C . -f intrinsic.make clean
	@${MAKE} --no-print-directory -C . -f extrinsic.make clean
	@${MAKE} --no-print-directory -C . -f testing.make clean
	@${MAKE} --no-print-directory -C . -f ClusterServer.make clean
	@${MAKE} --no-print-directory -C . -f ClusterComms.make clean
	@${MAKE} --no-print-directory -C . -f DataManager.make clean
	@${MAKE} --no-print-directory -C . -f fxbcam.make clean
	@${MAKE} --no-print-directory -C . -f Configuration.make clean
	@${MAKE} --no-print-directory -C . -f common.make clean
	@${MAKE} --no-print-directory -C . -f network.make clean
	@${MAKE} --no-print-directory -C . -f camera.make clean
	@${MAKE} --no-print-directory -C . -f apriltags.make clean
	@${MAKE} --no-print-directory -C . -f uart.make clean
	@${MAKE} --no-print-directory -C . -f findblobs.make clean
	@${MAKE} --no-print-directory -C . -f serialtest.make clean
	@${MAKE} --no-print-directory -C . -f ping.make clean
	@${MAKE} --no-print-directory -C . -f rays_test.make clean
	@${MAKE} --no-print-directory -C . -f hash_test.make clean
	@${MAKE} --no-print-directory -C . -f hashmap_test.make clean
	@${MAKE} --no-print-directory -C . -f jpeg_test.make clean

help:
	@echo "Usage: make [config=name] [target]"
	@echo ""
	@echo "CONFIGURATIONS:"
	@echo "   release"
	@echo "   debug"
	@echo "   benchmark"
	@echo ""
	@echo "TARGETS:"
	@echo "   all (default)"
	@echo "   clean"
	@echo "   server"
	@echo "   client"
	@echo "   virtual_client"
	@echo "   markertrainer"
	@echo "   capture_cali"
	@echo "   capture"
	@echo "   intrinsic"
	@echo "   extrinsic"
	@echo "   testing"
	@echo "   ClusterServer"
	@echo "   ClusterComms"
	@echo "   DataManager"
	@echo "   fxbcam"
	@echo "   Configuration"
	@echo "   common"
	@echo "   network"
	@echo "   camera"
	@echo "   apriltags"
	@echo "   uart"
	@echo "   findblobs"
	@echo "   serialtest"
	@echo "   ping"
	@echo "   rays_test"
	@echo "   hash_test"
	@echo "   hashmap_test"
	@echo "   jpeg_test"
	@echo ""
	@echo "For more information, see http://industriousone.com/premake/quick-start"
