The following instructions are applicable to a Linux environment(Ubuntu in particular)

Steps:

Update the package manager

```
sudo apt-get update
sudo apt-get upgrade
```

Install the dependencies

```
sudo apt-get install git-core cmake freeglut3-dev pkg-config build-essential libxmu-dev libxi-dev libusb-1.0-0-dev

sudo apt-get install cmake libglut3-dev pkg-config build-essential libxmu-dev libxi-dev libusb-1.0-0-dev python
```

if you get “_Unable to locate package libglut3-dev_”, use this command instead:

```
sudo apt-get install cmake freeglut3-dev pkg-config build-essential libxmu-dev libxi-dev libusb-1.0-0-dev python
```

Clone the following repositories

```
mkdir KinectLibs
cd KinectLibs
git clone https://github.com/OpenKinect/libfreenect
git clone https://github.com/OpenNI/OpenNI
git clone https://github.com/avin2/SensorKinect
```

(The git clone command didnt work for me, so in case this happens to you guys as well, download the github repo as a zip file, and extract it).

Open the libfreenect folder using terminal, then do as follows:
Install libfreenect

```
mkdir build
cd build
cmake -L ..
make
sudo make install
sudo ldconfig /usr/local/lib64/
```

To use Kinect as a non-root user do the following
sudo adduser $USER video
sudo adduser $USER plugdev

Also make a file with rules for the Linux device manager

```
sudo nano /etc/udev/rules.d/51-kinect.rules              //(u can use gedit instead of nano, as well)
```

Then paste the following:

```
# ATTR{product}=="Xbox NUI Motor"
SUBSYSTEM=="usb", ATTR{idVendor}=="045e", ATTR{idProduct}=="02b0", MODE="0666"
# ATTR{product}=="Xbox NUI Audio"
SUBSYSTEM=="usb", ATTR{idVendor}=="045e", ATTR{idProduct}=="02ad", MODE="0666"
# ATTR{product}=="Xbox NUI Camera"
SUBSYSTEM=="usb", ATTR{idVendor}=="045e", ATTR{idProduct}=="02ae", MODE="0666"
# ATTR{product}=="Xbox NUI Motor"
SUBSYSTEM=="usb", ATTR{idVendor}=="045e", ATTR{idProduct}=="02c2", MODE="0666"
# ATTR{product}=="Xbox NUI Motor"
SUBSYSTEM=="usb", ATTR{idVendor}=="045e", ATTR{idProduct}=="02be", MODE="0666"
# ATTR{product}=="Xbox NUI Motor"
SUBSYSTEM=="usb", ATTR{idVendor}=="045e", ATTR{idProduct}=="02bf", MODE="0666"
```

Run the following command in a terminal to test if libfreenect is correctly installed:

```
sudo freenect-glview
```

Kindly check the following link as well:

https://www.kdab.com/setting-up-kinect-for-programming-in-linux-part-1/

https://rjw57.github.io/freenect2-python/

Extra links(which I didn’t use):
https://www.youtube.com/watch?v=Gx4jk_zeOgY

Install Freenect Module for Python (UBUNTU)

Steps:

After following the previous steps, set KinectLibs/libfreenect as the active directory.
Install freenect python library

```
cd wrappers/python
sudo python3 setup.py install
```

//For installing in python3. If not working use
sudo python setup.py install

The module can be imported into codes written inside the wrapper/python folder using:

```
import freenect
```

Example codes are also available in the folder.

If the code still returns the error freenect module not found, try changing the shebang line from `#!/usr/bin/env python` to `#!/usr/bin/env python3`

Reference: https://stackoverflow.com/questions/57250885/state-of-kinect-python-opencv-in-2019

To import freenect outside the wrappers/python

Run:

```
sudo ln -s /usr/local/lib/python3/dist-packages/freenect.so /usr/local/lib/python3.8/dist-packages/.
```

Replace “python3.8” in the above command with the python version installed on your machine.

Reference:
https://stackoverflow.com/questions/21437758/nameerror-name-freenect-is-not-defined
