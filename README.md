# Lidar PlayBACK (LiBACK)

LiBACK (CubePlugin) is an Unreal Engine 5.0.3 (UE5) project designed to simultaneously and faithfully display a global point cloud and a sequence of lidar point clouds in Virtual Reality (VR) or Simulated Virtual Reality (sVR). The sequence of lidar point clouds should smoothly evolve over time, such as by following the trajectory of the vehicle collecting the data, enabling the code to display the movement of any dynamic objects captred by the point cloud. Most typically, the point cloud would be generated using Light Detection and Ranging (LiDAR) but other options could be used so long as the file format chosen matches the compatability list shown below.
  * It accepts .las files for global point clouds
  * It accepts .csv files for odometry readings
  * It accepts .png files for images
  * User-friendly configuration file to change important simulation parameters such as sampling rate and playback speed without the need for re-building
  * User-friendly keyboard interface to interact with the program during runtime, for example, to switch into a bird's eye view

Despite what the UE5 project name suggests, LiBACK (CubePlugin) is not in fact a plugin. The important files all live within the Source folder.

LiBACK was only tested on the VIVE headset. However, after enabling the appropriate UE5 plugins, use with other headsets should be possible. The specifications on which the test was performed: Windows 11, RTX 4090 GPU, i9-14900KF processor, 64 GB RAM. Set the configuration file appropriately according to your hardware availabilities.

## Example
The following images are screenshots of the sVR playback of point clouds in Katoomba.

![Screenshot 2024-01-19 091001](https://github.com/lightning-falcons/CubePlugin/assets/126124751/72419c69-0693-4b7a-92a7-0e3ccd850959)

![Screenshot 2024-01-19 091018](https://github.com/lightning-falcons/CubePlugin/assets/126124751/a37ee84c-468c-43d3-844d-d52dd8fcba1c)

The following configuration file (config.toml) was used (file paths must be changed before use as appropriate).

```
# This is the config file for the VR playback of the point clouds
# Note: After launching CubePlugin, the "LevelWithCharacter" level
# must be loaded

[Loading]

# The path to the folder containing the simulation data
data_path = "C:\\Users\\its\\Documents\\Unreal Projects\\CubePlugin\\Data"
# data_path = "C:\\Users\\its\\Documents\\Unreal Projects\\CubePlugin\\Chippendale"

# The name of the odometry file
odometry_name = "Odometry.csv"

# The name of the global point cloud
global_name = "Global Point Cloud.las"

# The name of the folder containing the local point clouds
local_name = "Local Point Clouds"

# The name of the folder containing the images
image_name = "Images"

[VR_Parameters]

# The number of TOTAL imported frames
number_frames = 300

# Import at a rate of 1 per import_every frames
import_every = 1

# Playback at this times the realtime speed
playback_speed = 0.1

# Whether photos are imported for the video playback
# This must be set to false if there are no images available
photo_import = true
```

## Usage Instructions
### The Head Mounted Display
The Head Mounted Display (HMD) of the VIVE headset determines the orientation of the camera in the scene, *relative* to the direction of motion of the data collection device (such as a vehicle). Therefore, if the data collection device is rotated, the orientation of the camera will also be rotated in exactly the same way, assuming the headset does not move. The location of the camera is initially at the location of the data collection device. This gives the VR user the experience that they are travelling "in" or "on" the data collection device. 
* To play in sVR mode, the "Selected Viewport" play mode should be chosen. In this play mode, the HMD will still control the orientation in the specified manner but the HMD **will not display the scene**.
* To play in VR mode, the "VR Preview" play mode should be chosen, and the HMD will additionally display the scene as also shown on the device running the code.

Please note that the HMD needs to be detected by the VR sensor units in order to control the orientation of the character camera. 

### Keyboard Controls
LiBACK incorporates a large number of convenient keyboard shortcuts during runtime (while the playback is in progress) to access certain functionality. The functionality that is included was selected on the basis of user experience, comfort, and scene reproduction and interrogation.

| Key | Description |
| --- | --- |
| &#8593; | Toggle between road (vehicle) perspective and the bird's eye perspective. The video is automatically set to disabled when the bird's eye persepctive is enabled. |
| &#8595; | Toggle the movement of the camera (paused or fixed with data collection device). The position becomes fixed but the orientation still changes. Do not toggle between road and bird's eye perspectives when the position is fixed as it will lead to undesired results. |
| &#8592; | Wind back time by one second (real-life time, regardless of the playback speed). If the beginning of the data is reached, the starting scene will be displayed until the display time of subsequent data is reached. |
| &#8594; | Wind forward time by one second (real-life time, regardless of the playback speed). If the end of the data is reached, the playback will re-start from the beginning. |
| V | Toggle the visibility of the video. |
| W | Move the camera upwards by one metre. |
| S | Move the camera down by one metre. |
| A | Move the camera left by one metre. |
| D | Move the camera right by one metre. |

### Installation Instructions (Windows 11)
1. Go to [Download Unreal Engine](https://www.unrealengine.com/en-US/download) and following instructions to install UE 5.0.3
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
12. Open this project in Unreal Engine, then enable the SteamVR, OpenXR and Lidar Point Cloud plugins
13. Open the solution file in VS Code 2022 and press CTRL+SHIFT+B to build the project

### Running the Project for New Data
1. Create a folder (for example, `Data`) that contains all the data files that you will be using. This folder is best placed inside the project (CubePlugin) folder.
2. Paste the odometry file into the folder, ensuring that it follows the specified format.
3. Paste the global point cloud into the folder, ensuring that it is in the .las format.
4. Create a folder for the local point clouds and paste all the local point clouds in the folder. **There must be an EXACT ONE TO ONE correspondence between the local point clouds and the odometry readings for the local point clouds to be properly aligned with the global point cloud.** Each local point cloud must have a file name that is exactly the timestamp in *seconds*. Use a floating point number.
5. Create a folder for the images and paste all the images in the folder. There is no requirement on any time correspondence between the point clouds or odometry with the images. However, each image must be a .png file with the name as the timestamp in **nanoseconds**.
6. Change the configuration file to reflect the locations of the data
7. Change the other parameters of the configuration file. Note: Importing 300 frames with video enabled took around 20 seconds on our hardware. Reduce the number of frames imported and downsample (by increasing `import_every`) as necessary. Note that `photo_import` MUST be set to false if no images are available
8. Open the project in unreal engine. Import the LevelWithCharacter Level from the Contents Drawer, the Contents folder (double click on it)
9. Click the play button (either selected viewport or VR preview)

Note that for the images and local point clouds you are recommended to ensure they are in increasing order of the timestamps. No guarantees on the correct functioning of the project if this is not the case.
