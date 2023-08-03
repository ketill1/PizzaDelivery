# Autonomous Pizza Delivery
Wait no more. Help me with this project and get your pizza delivered in the upcoming years.
![Alt text](./images/60410425-3a77-4030-a485-ff4c6c5be6b1.jpg)

# Introduction 
The goal of this project is to do autonomous pizza deliveries from a simple user interface. The system needs to work both outdoors and indoor since my local pizzeria is situated inside a shopping center. Further, it has to plan the path, follow the path, avoid obstacles, and order the pizza upon arrival. To accomplish that safe video feed to the user is needed and the possibility to manually take control when the robot misbehaves.  

# Build and Test

## ROS2 Humble [Humble docs](https://docs.ros.org/en/humble/index.html)
To use ROS2 Humble Ubuntu 22.04 is strongly recomended. 
### Set locale
Make sure you have a locale which supports UTF-8. If you are in a minimal environment (such as a docker container), the locale may be something minimal like POSIX. We test with the following settings. 
However, it should be fine if you’re using a different UTF-8 supported locale.

```console
locale  # check for UTF-8
sudo apt update && sudo apt install locales
sudo locale-gen en_US en_US.UTF-8
sudo update-locale LC_ALL=en_US.UTF-8 LANG=en_US.UTF-8
export LANG=en_US.UTF-8

locale  # verify settings
```
### Setup Sources
You will need to add the ROS 2 apt repository to your system. First ensure that the Ubuntu Universe repository is enabled.
`sudo apt install software-properties-common && sudo add-apt-repository universe`

Now add the ROS 2 GPG key with apt.
```console
sudo apt update && sudo apt install curl -y
sudo curl -sSL https://raw.githubusercontent.com/ros/rosdistro/master/ros.key -o /usr/share/keyrings/ros-archive-keyring.gpg
```
Then add the repository to your sources list.
```console
echo "deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/ros-archive-keyring.gpg] http://packages.ros.org/ros2/ubuntu $(. /etc/os-release && echo $UBUNTU_CODENAME) main" | sudo tee /etc/apt/sources.list.d/ros2.list > /dev/null
```

### Install ROS 2 packages
Update your apt repository caches after setting up the repositories.
```console
sudo apt update
```

ROS 2 packages are built on frequently updated Ubuntu systems. It is always recommended that you ensure your system is up to date before installing new packages.
```console
sudo apt upgrade
```

Desktop Install (Recommended): ROS, RViz, demos, tutorials.
```consoel
sudo apt install ros-humble-desktop
```

ROS-Base Install (Bare Bones): Communication libraries, message packages, command line tools. No GUI tools. This can be efficent on the robot for added performace. 
```console
sudo apt install ros-humble-ros-base
```

Development tools: Compilers and other tools to build ROS packages
```console
sudo apt install ros-dev-tools
```

## Nav2 [Nav2 docs](https://navigation.ros.org/).
### The NAV2 packages can be installed as binary or from source.
```console
sudo apt install ros-humble-navigation2
sudo apt install ros-humble-nav2-bringup
```

## SLAM [slam_toolbox](https://github.com/SteveMacenski/slam_toolbox)
### For the localization and mapping SLAM is used.
```console
sudo apt install ros-humble-slam-toolbox
```

## Other packages and dependencies
```console
sudo apt-get install libserial-dev
sudo apt install python3-serial
sudo apt install ros-humble-ros2-control ros-humble-ros2-controllers ros-humble-gazebo-ros2-control
sudo apt install joystick jstest-gtk evtest
sudo apt-get install ros-humble-twist-mux
sudo apt-get install ros-humble-rosbridge-suite
sudo apt install ros-humble-robot-localization
sudo apt-get install ros-humble-velodyne
pip install setuptools==58.2.0
```

## Environment setup
### Sourcing the setup script in bash
```console
source /opt/ros/humble/setup.bash
source /home/<user>/<path>/install/setup.bash
```
### Alias
To easier work with ssh these aliases is used on the robot
```console
alias bridge='ros2 launch rosbridge_server rosbridge_websocket_launch.xml'
alias sensor='ros2 launch elcore_autonomous imu_encoder_lidar.launch.py'
alias start='ros2 launch auto_gpr bringup.launch.py'
alias gpr_trigger='ros2 run gpr_trigger listener'
```
and these on the remote machine:
```console
alias robot='ssh <user>@<ip>'
alias corc='scp <user>@<ip>:/home/<user>/p_r_0001.corc'
```
This will make it possible to ssh to the robot writing the "robot" and get the .corc writing corc. (Use keygen to remove the password for the ssh)

## Build the project
```console
git clone https://github.com/ketill1/PizzaDelivery.git
````
At the root of your workspace:
```console
colcon build --symlink-install && source install/setup.bash
```

## Robot setup
To be able to controll the robot remotely websocket is used to send mesages for the start up, turn off and various navigation task. The websocket is started on boot of the robot together with a node controlling subprocesses from boolean service calls.  
### The three processes that start on boot
1. `sudo vim /etc/systemd/system/ros2node1.service`
   ```console
   [Unit]
   Description=Start ROS2 Node
   After=network.target
   
   [Service]
   ExecStart=/bin/bash -c 'source /opt/ros/humble/setup.bash; ros2 launch rosbridge_server rosbridge_websocket_launch.xml'
   User=ggeo
   Environment="DISPLAY=:0"
   Environment="XAUTHORITY=/run/user/1000/gdm/Xauthority"
   Restart=always
   
   [Install]
   Wa`tedBy=multi-user.target
   ```
   ```console
   sudo systemctl enable ros2node1.service
   sudo systemctl start ros2node1.service
   ```

2. `sudo vim /etc/systemd/system/ros2node2.service`
   ```console
   [Unit]
   Description=Start ROS2 Node
   After=network.target
   
   [Service]
   ExecStart=/bin/bash -c 'source /opt/ros/humble/setup.bash; source /home/<user>/<path_to_project>/install/setup.bash; ros2 run remote_node_launcher robotClient'
   User=ggeo
   Environment="DISPLAY=:0"
   Environment="XAUTHORITY=/run/user/1000/gdm/Xauthority"
   Restart=always
   
   [Install]
   WantedBy=multi-user.target
   ```
   ```console
   sudo systemctl enable ros2node2.service
   sudo systemctl start ros2node2.service
   ```

3. `sudo vim /etc/systemd/system/ros2node3.service`
   ```console
   [Unit]
   Description=Start ROS2 Node
   After=network.target
   
   [Service]
   ExecStart=/bin/bash -c 'source /opt/ros/humble/setup.bash; source /home/<user>/<path_to_project>/install/setup.bash; ros2 run remote_node_launcher navigationClient'
   User=ggeo
   Environment="DISPLAY=:0"
   Environment="XAUTHORITY=/run/user/1000/gdm/Xauthority"
   Restart=always
   
   [Install]
   WantedBy=multi-user.target
   ```
   ```console
   sudo systemctl enable ros2node3.service
   sudo systemctl start ros2node3.service
   ```

## USB
### To get all the devices has to commuicate with the right driver the USB port names is static. This will change depending on the harware setup
PORT Forwarding:
`cd /etc/udev/rules.d`
`sudo vim usb.rules`

Give the USB port names:
```console
SUBSYSTEM=="tty",ENV{ID_PATH}=="pci-0000:00:14.0-usb-0:3:1.0",SYMLINK+="arduino"
SUBSYSTEM=="tty",ENV{ID_PATH}=="pci-0000:00:14.0-usb-0:7:1.0",SYMLINK+="IMU"
SUBSYSTEM=="tty",ENV{ID_PATH}=="pci-0000:00:14.0-usb-0:5.1:1.0",SYMLINK+="lidar"
```
After "usb-" is the hardware path to the port in the machine. (Different for each device!)
`cd /dev/serial/by-path && ls -la` to find the path 
## Network
To enable the autostart of the mobile hotspot on the NUC, first enter:
 `cd /etc/NetworkManager/system-connections/`
set autoconnect=true in the "hotspot" files.

## Velodyne lidar
Seting for the lidar can be set by the webserver interface by typing the sensor’s
network address, 192.168.1.201, into the address bar [Velodyne](http://192.168.1.201/)

# Hardware components
- Velodyne VLP-16 [VLP-16 User Manual](https://velodynelidar.com/wp-content/uploads/2019/12/63-9243-Rev-E-VLP-16-User-Manual.pdf)
- Intel NUC 11 [NUC 11 User-Manual](https://www.intel.com/content/dam/support/us/en/documents/intel-nuc/NUC11BT_DB_TPS.pdf)
- Arduino Nano [Nano User Manual](https://www.arduino.cc/en/uploads/Main/ArduinoNanoManual23.pdf)
- Motor driver [Sabertooth dual 12A User Manual](https://www.dfrobot.com/product-304.html)
- Mti 3 IMU [mti 3 User Manual](https://www.xsens.com/hubfs/Downloads/Leaflets/MTi-3.pdf)
- Motors (?)

# Known issues and improvements
Add your isssues or improvements here...

# Useful commands when working with ROS2
- `ros2 --help` # To see all available sub-commands
```console
Commands:
  action     Various action related sub-commands
  bag        Various rosbag related sub-commands
  component  Various component related sub-commands
  control    Various control related sub-commands
  daemon     Various daemon related sub-commands
  doctor     Check ROS setup and other potential issues
  interface  Show information about ROS interfaces
  launch     Run a launch file
  lifecycle  Various lifecycle related sub-commands
  multicast  Various multicast related sub-commands
  node       Various node related sub-commands
  param      Various param related sub-commands
  pkg        Various package related sub-commands
  run        Run a package specific executable
  security   Various security related sub-commands
  service    Various service related sub-commands
  topic      Various topic related sub-commands
  wtf        Use `wtf` as alias to `doctor`

```

###Build
- `colcon build` #Builds the workspace
  - `-args`
  - `-symlink-install` # Installs symbolic links that makes it possible to modify launch files, .yaml and .xml files without having to re-build
  - `-packages-select`<package>`# Build a specific package
  - `source install/setup.bash` # Source the build

###Topics
- `ros2 topic list` # See all the topics
  - `ros2 topic list -t` # Will return the same list of topics, this time with the topic type appended
- `ros2 topic echo <topic>` # See the mesages from the topic
- `ros2 topic pub <name> <type>`
  - `--once` # One pub
  - `--rate <rate>` # Hz
- `ros2 topic hz <msg>` # See the hz of the message 
- `ros2 interface show <msg type>` # What structure of data the message expects

###Service
- `ros2 service list` # See all the services
  -`ros2 service list -t` # Add the service type
  - `ros2 service find <type_name>` # Find all the service of a certain type
- `ros2 service type <service_name>` # See the service type
You can call services from the command line, but first you need to know the structure of the input arguments.
- `ros2 interface show <type_name>`
- `ros2 service call <service_name> <service_type> <arguments>"` # Do a service call

### Parameters
- `ros2 param list` # To see the parameters belonging to your nodes
- `ros2 param get <node_name> <parameter_name>` To determine a parameter’s type
- `ros2 param set <node_name> <parameter_name> <value>` # To set a parameter value at runtime
- `ros2 param dump <node_name>` # To get a node currents parameter values
- `ros2 param load <node_name> <parameter_file>` # To load paramters from a file
To load parameters on startup use:
- `ros2 run <package_name> <executable_name> --ros-args --params-file <file_name>`

### Action
- `ros2 action list` # See all the actions

### Nodes
- `ros2 node list` # See all the nodes
- `ros2 node info <node_name> ` # To get more info about the node
- `ros2 run <package_name> <executable_name>` # Run the node
- `ros2 run <package_name> <executable_name> --remap <from>:=<to>` # Can remap Topics, parameters, and services
- `ros2 launch <node_name> <launch_script_name.py>`

###Machine
- `htop` # Monitor processes
- `sensors` # Monitor temperature
- `rqt_graph` # Get ros2 graph, structure and more
- `rviz2` # visualize topics and send some commands
- `gazebo` # Simulate the robot
- `rqt_robot_steering` # simple gui to steer the robot
