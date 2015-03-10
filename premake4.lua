if _ACTION == "clean" then
	os.rmdir("build")
	os.rmdir("bin")
	os.rmdir("include/pb")
else
	os.mkdir("include/pb")
	os.execute("cd include; protoc --cpp_out=pb cluster.proto")
end

solution "Demo"
	objdir "build"
	language "C++"


	buildoptions { "-Wall",
			       "-Wextra",
			       "-Wno-uninitialized",
			       "-Wno-unused-parameter",
	}
	buildoptions { "`pkg-config opencv --cflags`" }
	linkoptions { "`pkg-config opencv --libs`" }

	includedirs { "include", "modules/cam1394/src", "modules/apriltags" }

	if os.outputof("cat /proc/cpuinfo | grep sse2") ~= "" then
		flags { "EnableSSE2" }
	end
	if os.outputof("cat /proc/cpuinfo | grep sse4_2") ~= "" then
		--flags { "EnableSSE42" }
	end

	configurations { "release", "debug", "benchmark" }

	configuration "release"
		buildoptions { "-O3" }
		linkoptions { "-O3" }
		targetdir "bin"
		flags { "OptimizeSpeed" }
		defines { "NDEBUG" }
		
	configuration "debug"
		targetdir "bin/debug"
		flags { "Symbols" }
		defines { "DEBUG" }

	configuration "benchmark"
		--buildoptions { "-O3" }
		--linkoptions { "-O3" }
		targetdir "bin"
		flags { "OptimizeSpeed" }
		--flags { "Symbols" }
		defines { "NDEBUG", "BENCHMARK" }


-- 
-- Main executables
-- 
	project "server"
		kind "ConsoleApp"
		files { "src/server.cpp" }
		links { "rt", "pthread", "protobuf" }
		links { "ClusterServer", "DataManager", "fxbcam", "Configuration", "ClusterComms", "common", "network", "jpeg" }

	project "client"
		kind "ConsoleApp"
		files { "src/client.cpp" }
		links { "rt", "dc1394", "protobuf" }
		links { "camera", "fxbcam", "Configuration", "ClusterComms", "common", "network", "jpeg", "m" }


-- 
-- Helper programs
-- 
	project "virtual_client"
		kind "ConsoleApp"
		files { "src/tools/virtual_client.cpp" }
		links { "rt", "dc1394", "protobuf" }
		links { "fxbcam", "Configuration", "ClusterComms", "common", "network", "jpeg", "m" }

	project "markertrainer"
		kind "ConsoleApp"
		files { "src/tools/markertrainer.cpp" }
		links { "rt", "dc1394" }
		links { "camera", "fxbcam", "jpeg" }

	project "capture_cali"
		kind "ConsoleApp"
		files { "src/tools/capture_cali.cpp" }
		links { "rt", "dc1394" }
		links { "camera", "Configuration" }

	project "capture"
		kind "ConsoleApp"
		files { "src/tools/capture.cpp" }
		links { "rt", "dc1394" }
		links { "camera"}

	project "intrinsic"
		kind "ConsoleApp"
		files { "src/tools/calibrate/intrinsic.cpp" }
		links { "rt", "dc1394" }
		links { "camera"}

    project "extrinsic"
		kind "ConsoleApp"
		files { "src/tools/calibrate/extrinsic.cpp" }
		links { "m", "dc1394" }
		links { "apriltags", "cvsba" }
    links { "pthread" }
        buildoptions { "-std=gnu++11" }
	
    project "testing"
		kind "ConsoleApp"
		files { "src/tools/testing.cpp" }
		links { "rt", "dc1394" }
		links { "camera"}
        buildoptions { "-std=c++11" }
-- 
-- Core components
-- 
	project "ClusterServer"
		kind "StaticLib"
		--location "src"
		targetdir "build"
		files { "ClusterServer.h", "src/ClusterServer.cpp", "BlockingQueue.h" }

	project "ClusterComms"
		kind "StaticLib"
		targetdir "build"
		files { "ClusterComms.h", "src/ClusterComms.cpp", "pb/*.h", "include/pb/*.cc" }

	project "DataManager"
		kind "StaticLib"
		--location "src"
		targetdir "build"
		files { "DataManager.h", "src/DataManager.cpp", "ThreadDeque.h" }

	project "fxbcam"
		kind "StaticLib"
		--location "src"
		targetdir "build"
		files { "fxbcam.h", "src/fxbcam.cpp" }

	project "Configuration"
		kind "StaticLib"
		--location "src"
		targetdir "build"
		files { "Configuration.h", "src/Configuration.cpp", "json/json.h", "json/json-forwards.h", "src/jsoncpp.cpp" }
	

-- 
-- Shared libraries
-- 
	project "common"
		kind "StaticLib"
		--location "src"
		targetdir "build"
		files { "common.h", "src/common.cpp" }

	project "network"
		kind "StaticLib"
		--location "src"
		targetdir "build"
		files { "network.h", "src/network.cpp" }

	project "camera"
		kind "StaticLib"
		--location "src"
		includedirs "modules/cam1394/src"
		targetdir "build"
		files { "camera.h", "cameraconstants.h", "modules/cam1394/src/camera.cpp" }

	project "apriltags"
		kind "StaticLib"
		buildoptions { "-std=gnu99", "-Wno-sign-compare" }
		includedirs "modules/apriltags"
		targetdir "build"
		files { "apriltag.h", "common.h", "g2d.h", "graymodel.h", "gridder.h", "homography.h", "image_f32.h", "image_u32.h", "image_u8.h", "line_fit.h", "matd.h", "tag36h10.h", "tag36h11.h", "timeprofile.h", "unionfind.h", "workerpool.h", "zarray.h", "zhash.h", "modules/apriltags/apriltag.c", "modules/apriltags/g2d.c", "modules/apriltags/graymodel.c", "modules/apriltags/homography.c", "modules/apriltags/image_f32.c", "modules/apriltags/image_u32.c", "modules/apriltags/image_u8.c", "modules/apriltags/matd.c", "modules/apriltags/segment2.c", "modules/apriltags/tag36h10.c", "modules/apriltags/tag36h11.c", "modules/apriltags/unionfind.c", "modules/apriltags/workerpool.c", "modules/apriltags/zarray.c", "modules/apriltags/zhash.c" }

	project "uart"
		kind "StaticLib"
		--location "src"
		targetdir "build"
		files { "uart.h", "src/uart.c" }


-- 
-- Test programs
-- 
	project "findblobs"
		kind "ConsoleApp"
		files { "src/tools/findblobs.cpp" }
		links { "rt", "dc1394" }
		links { "camera", "fxbcam", "jpeg" }

--	project "benchmark_vision"
--		kind "ConsoleApp"
--		files { "src/tools/benchmark_vision.cpp" }
--		links { "rt", "dc1394", "protobuf" }
--		links { "camera", "fxbcam", "Configuration", "ClusterComms", "common", "jpeg", "m" }

	project "serialtest"
		kind "ConsoleApp"
		files { "src/tools/serialtest.cpp" }
		links { "uart" }

	project "ping"
		kind "ConsoleApp"
		files { "src/tools/ping.cpp" }
		links { "rt" }
		links { "common", "network" }

	project "rays_test"
		kind "ConsoleApp"
		files { "src/tools/rays_test.cpp" }
		links { "rt" }
		links { "camera", "fxbcam", "jpeg" }
	
	project "hash_test"
		kind "ConsoleApp"
		buildoptions { "-std=c++0x" }
		files { "src/tools/hash_test.cpp" }
		links { "rt" }

	project "hashmap_test"
		kind "ConsoleApp"
		buildoptions { "-std=c++0x" }
		files { "src/tools/hashmap_test.cpp" }
		links { "rt" }

	project "jpeg_test"
		kind "ConsoleApp"
		files { "src/tools/jpeg_test.cpp" }
		links { "rt", "dc1394" }
		links { "common", "camera", "jpeg" }

