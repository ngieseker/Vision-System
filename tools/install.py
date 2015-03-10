#!/usr/bin/python
import getopt, sys
import subprocess as sp
import time
import string

co = False
verb = False
force = False

def main():
	global co
	global verb
	global force
	try:
		opts, args = getopt.getopt(sys.argv[1:], "cvf", ["with-cuda", "check", "force-install"])
	except getopt.GetoptError, err:
		print str(err)
		sys.exit(2)

	for o, a in opts:
		if o in ("-c", "--check"):
			co = True
		if o in ("-v", "--verbose"):
			verb = True
		if o in ("-f", "--force-instal"):
			force = True

	#run(checkOnly, verbose, forceIns)
	installAptGet("compiler", "build-essential")
	installAptGet("DC1394-2 Library", "libdc1394-22-dev")
	installAptGet("GTK2.0 Dev", "libgtk2.0-dev")
	installAptGet("cmake", "cmake-gui")
	installAptGet("ProtoBuf", "libprotobuf-dev")
	installAptGet("nasm", "nasm")
	cmds = ["wget http://liyiying.com/pf/OpenCV-2.4.0.tar.bz2",\
			"tar xaf OpenCV-2.4.0.tar.bz2", \
			"cd OpenCV-2.4.0; mkdir -p BUILD; cd BUILD;\
				cmake -D ENABLE_SSE=ON -D ENABLE_SSE2=ON -D ENABLE_SSE3=ON \
				-D ENABLE_SSE41=ON -D ENABLE_SSE42=ON -D ENABLE_SSSE3=ON ..",\
			"cd OpenCV-2.4.0; cd BUILD; make -j4 && sudo make install",\
			"sudo ldconfig"]
	installOther("OpenCV", "pkg-config opencv --exists", cmds)
	cmds = ["wget http://liyiying.com/pf/libjpeg-turbo-1.2.0.tar.gz",\
			"tar xaf libjpeg-turbo-1.2.0.tar.gz", \
			"cd libjpeg-turbo-1.2.0; ./configure --with-jpeg8 --prefix=/usr/local ",\
			"cd libjpeg-turbo-1.2.0; make -j4 && sudo make install"]
	installOther("libjpeg-turbo", " ls /usr/local/lib/ | grep libjpeg.so.8.0.2", cmds)
	cleanSource()

def cleanSource():
	sys.stdout.write("Cleaning"+'.'*10)
	sys.stdout.flush()
	ret = sp.Popen("rm -rf installtemp", stdout=sp.PIPE, shell=True);
	if not ret.wait():
		print "done"
	else:
		print "failed"

def installAptGet(name, pkgname):
	sys.stdout.write("Checking for "+name+'.'*10)
	sys.stdout.flush()
	ret = sp.Popen("sudo dpkg -l | grep "+pkgname, stdout=sp.PIPE, shell=True);
	if not ret.wait():
		print "yes"
		
	if (not co and ret.returncode) or (not co and force):
		sys.stdout.write("Installing "+name+"."*10)
		sys.stdout.flush()
		verbpipe = sp.PIPE;
		if verb:
			verbpipe = None;
			print "\n"
		
		ret = sp.Popen("sudo apt-get install "+pkgname, stdout=verbpipe, stdin=sp.PIPE, shell=True);
		ret.communicate("Y");
		while not verb and ret.poll() == None:
			sys.stdout.write('.');
			sys.stdout.flush()
			time.sleep(.3);
		if ~ret.wait():
			print "done"

def installOther(name, checkcmd, cmds):
	ret = sp.Popen("mkdir -p installtemp", stdout=sp.PIPE, shell=True)
	sys.stdout.write("Checking for "+name+'.'*10)
	sys.stdout.flush()
	ret = sp.Popen(checkcmd, stdout=sp.PIPE, shell=True);
	if not ret.wait():
		print "yes"

	if (not co and ret.returncode) or (not co and force):
		sys.stdout.write("Installing "+name+"."*10)
		sys.stdout.flush()

		verbpipe = sp.PIPE;
		if verb:
			verbpipe = None;
			print "\n"
	
		for c in cmds:
			if verb:
				c = c.replace("tar xaf", "tar xvaf", 1)
				print "Running cmd: \""+c+"\""+'.'*10,
				sys.stdout.flush()
			c = "cd installtemp; "+c
			ret = sp.Popen(c, stdout=verbpipe, stderr=verbpipe, shell=True);
			while not verb and ret.poll() == None:
				sys.stdout.write('.');
				sys.stdout.flush()
				time.sleep(.3);
			if ret.wait() != 0:
				print "\nError"
				exit(2)
			if verb:
				print "done"

		print "done"
	
if __name__ == "__main__":
	main()
