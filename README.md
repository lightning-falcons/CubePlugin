# Lidar PlayBACK (LiBACK)

LiBACK (CubePlugin) is an Unreal Engine **5.3.2** (UE5) project designed to simultaneously and faithfully display a global point cloud and a sequence of lidar point clouds in Virtual Reality (VR). The sequence of lidar point clouds should smoothly evolve over time, such as by following the trajectory of the vehicle collecting the data, enabling the code to display the movement of any dynamic objects captured by the point cloud. Most typically, the point cloud would be generated using Light Detection and Ranging (LiDAR) but other options could be used so long as the file format chosen matches the compatability list shown below.
  * It accepts .las files for global point clouds
  * It accepts .csv files for odometry readings
  * It accepts .png files for images
  * User-friendly configuration file to change important simulation parameters such as sampling rate and playback speed without the need for re-building
  * User-friendly keyboard and controller interface to interact with the program during runtime, for example, to switch into a bird's eye view

Despite what the UE5 project name suggests, LiBACK (CubePlugin) is not in fact a plugin. The important files all live within the Source folder.

LiBACK was only tested on the HTC VIVE headset. However, after enabling the appropriate UE5 plugins, use with other headsets should be possible. The specifications on which the test was performed: Windows 11, RTX 4090 GPU, i9-14900KF processor, 64 GB RAM. Set the configuration file appropriately according to your hardware availabilities.

## Example
The following images are screenshots of the VR playback of point clouds in Chippendale and Katoomba as viewed on the computer screen. The Chippendale dataset has real-to-life colour incoporated in the point clouds.

![Screenshot 2024-02-01 155346](https://github.com/lightning-falcons/CubePlugin/assets/126124751/9ded2f94-e1cf-4a6d-803a-3e48c15df8a2)

![Screenshot 2024-02-01 154328](https://github.com/lightning-falcons/CubePlugin/assets/126124751/fd8e3506-3e4b-4c07-8b2c-6c8e3b8caf46)

The following configuration file (config.toml) is an example of what could be used (file paths must be changed before use as appropriate).

```
# This is the config file for the VR playback of the point clouds
# Note: After launching CubePlugin, the "LevelWithCharacter" level
# must be loaded

[Loading]

# The path to the folder containing the simulation data
data_path = "C:\\Users\\its\\Documents\\Unreal Projects\\CubePlugin 5.3\\Data"

# The name of the odometry file
odometry_name = "Odometry5.csv"

# The name of the global point cloud
global_name = "Global Point Cloud.las"

# The name of the folder containing the local point clouds
local_name = "Local Point Clouds"

# The name of the folder containing the images
image_name = "Images"

# Correction angle for the global map PITCH in degrees
pitch_correction = 0.00

# Correction angle for the global map YAW in degrees
yaw_correction = 0.00

# Correction angle for the global map ROLL in degrees
roll_correction = -0.29

[VR_Parameters]

# The number of TOTAL imported frames
number_frames = 1000

# The height that the bird's eye view is set to this
# The units must be in CENTIMETRES
# Typical values are 5000-20000 cm
height_birds = 10000.0

# Import at a rate of 1 per import_every frames
import_every = 1

# Playback at this times the realtime speed
playback_speed = 0.5

# Display points at these sizes when in road or bird's eye view
local_ground_size = 0.5
local_bird_size = 0.5
global_ground_size = 0.05
global_bird_size = 0.1

# Whether photos are imported for the video playback
# This must be set to false if there are no images available
photo_import = true

# Set the camera exposure (lower value means brighter)
exposure = 0.1
```

## Usage Instructions
### The Head Mounted Display
The Head Mounted Display (HMD) of the VIVE headset determines the orientation of the camera in the scene, *relative* to the direction of motion of the data collection device (such as a vehicle). Therefore, if the data collection device is rotated, the orientation of the camera will also be rotated in exactly the same way, assuming the headset does not move. The location of the camera is initially at the location of the data collection device. This gives the VR user the experience that they are travelling "in" or "on" the data collection device. To play in VR mode, the "VR Preview" play mode should be chosen, and the HMD will display the scene as shown on the device running the code. The HMD controls the orientation of the camera.

Please note that the HMD needs to be detected by the VR sensor units in order to control the orientation of the character camera. 

The *position* of the HMD also controls the position of the character relative to their "nominal" location. Therefore, walking around the room has the same physical effect as walking around the (moving) VR world.

### VIVE Controller and Keyboard Controls
LiBACK incorporates a large number of convenient VIVE controller input and keyboard shortcuts during runtime (while the playback is in progress) to access certain functionality. The functionality that is included was selected on the basis of user experience, comfort, and scene reproduction and interrogation. If available, we recommend using the controller instead of the keyboard as it offers a more comfortable experience. The controllers, when appropriately paired, will appear as a pair of glowing hands. Hold each controller in the correct hand.

| Controller Input (Left or Right Controller)| Key | Description |
| --- | --- | --- |
| Trigger (L or R)| &#8593; | Toggle between road (vehicle) perspective and the bird's eye perspective. The video is automatically set to disabled when the bird's eye persepctive is enabled. |
| Trackpad Down (R)| &#8595; | Toggle between playback and paused. When paused, everything is paused - that means, the video, local point cloud evolution and character evolution are all paused. |
| Trackpad Left (R)| &#8592; | Wind back time by one second (real-life time, regardless of the playback speed). If the beginning of the data is reached, the starting scene will be displayed until the display time of subsequent data is reached. |
| Trackpad Right (R)| &#8594; | Wind forward time by one second (real-life time, regardless of the playback speed). If the end of the data is reached, the playback will re-start from the beginning. |
| Trackpad Up (R)| V | Toggle the visibility of the video. |
| Trackpad Up (L) | W | Move the camera upwards by one metre. |
| Trackpad Down (L) | S | Move the camera down by one metre. |
| Trackpad Left (L) | A | Move the camera left by one metre. |
| Trackpad Right (L) | D | Move the camera right by one metre. |

You may wish to note additionally that all objects within a fixed radius of the camera are hidden to prevent obstruction of the camera.

### Installation Instructions (Windows 11)
1. Go to [Download Unreal Engine](https://www.unrealengine.com/en-US/download) and following instructions to install UE 5.3.2
2. Install VS Code 2022 from the following link https://visualstudio.microsoft.com/vs/ and during installation, make sure to tick all the options for “Desktop & Mobile” as well as tick the “Game Development with C++”
3. Install git and git bash from https://gitforwindows.org/ opting to use VS Code 2022 as the default editor and `main` for the default branch name
4. Find the `Unreal Projects` folder, which is usually located in the `Documents` folder, and open git bash and `cd` into that folder
5. `git clone` this project
6. Find the UnrealVersionSelector tool and copy it into the `Epic Games\UE_5.0\Engine\Binaries\Win64` folder
7. Run the **newly copied** UnrealVersionSelector
8. Left click on the UPROJECT file then right click. Click “Show More Options” then click “Generate Visual Studio Project Files”
9. Install steam (if not using the VIVE headset, follow instruction for your particular headset) and connect the VR headset appropriately
10. Install SteamVR (you may also need to install Vive Console for SteamVR)
11. Follow prompted instructions to setup the play area
12. Open this project in Unreal Engine, then enable the OpenXR and Lidar Point Cloud plugins
13. **Disable the SteamVR plugin in UE5**
14. Open the solution file in VS Code 2022 and press CTRL+SHIFT+B to build the project

### Running the Project for New Data
1. Create a folder (for example, `Data`) that contains all the data files that you will be using. This folder is best placed inside the project (CubePlugin) folder.
2. Paste the odometry file into the folder, ensuring that it follows the specified format.
3. Paste the global point cloud into the folder, ensuring that it is in the .las format.
4. Create a folder for the local point clouds and paste all the local point clouds in the folder. **There must be an EXACT ONE TO ONE correspondence between the local point clouds and the odometry readings for the local point clouds to be properly aligned with the global point cloud.** Each local point cloud must have a file name that is exactly the timestamp in *seconds*. Use a floating point number.
5. Create a folder for the images and paste all the images in the folder. There is no requirement on any time correspondence between the point clouds or odometry with the images. However, each image must be a .png file with the name as the timestamp in **nanoseconds**.
6. Change the configuration file to reflect the locations of the data
7. Change the other parameters of the configuration file. Note: Importing 300 frames with video enabled took around 20 seconds on our hardware. Reduce the number of frames imported and downsample (by increasing `import_every`) as necessary. Note that `photo_import` MUST be set to false if no images are available
8. Open the project in unreal engine. Import the LevelWithCharacter Level from the Contents Drawer, the Contents folder (double click on it)
9. Click the play button (VR preview)

Note that for the images and local point clouds you are recommended to ensure they are in increasing order of the timestamps. No guarantees on the correct functioning of the project if this is not the case.

### Odometry File Format
The following file format is compulsory for the odometry csv file. Do NOT include any headers.

Please note: the roll, pitch and yaw should be in the `ZYX` format.

| Any number | Timestamp in nanoseconds | x | y | z | Yaw | Pitch | Roll |
| --- | --- | --- | --- | --- | --- | --- | --- |

Part of an example file is shown below.

```
10,1.68845145808643e+18,14.9628734589,-0.327040761709,-2.13114643097,0.0028084916830639086,2.526134255448693,-2.9307556074935013
11,1.68845145818651e+18,16.3280982971,-0.313888370991,-2.13569712639,0.023859830589336536,2.5836716569154956,-2.5798181615590945
12,1.68845145828653e+18,17.724023819,-0.347656875849,-2.16523313522,0.03207923717778796,2.5442754344835428,-2.4758874553543015
13,1.6884514583865e+18,19.1412677765,-0.345240205526,-2.17043399811,-0.04494835823620712,2.528895613076077,-2.3749588024451125
14,1.6884514584866e+18,20.5634174347,-0.395400494337,-2.18928194046,-0.09463631251943462,2.599710009021327,-2.4830989925655986
15,1.68845145858678e+18,21.9164810181,-0.427899003029,-2.18481206894,-0.25302422201812075,2.7933528757940786,-2.755307574520795
16,1.68845145868656e+18,23.2771778107,-0.421871423721,-2.2093231678,-0.3309935716382575,3.0185587639265776,-3.223781566934451
```
## Acknowledgments
This project could not be possible without the support of the ITS group including Dr Stewart Worrall, Dr Stephany Berrio Perez and Dr Mao Shan, who provided invaluable advice and support. The project is also partly funded by the Vacation Research Internship project at the University of Sydney and Transport for NSW. 
