# Lidar PlayBACK (LiBACK)

LiBACK (CubePlugin) is an Unreal Engine 5.0.3 (UE5) project designed to simultaneously and faithfully display a global point cloud and a sequence of lidar point clouds in Virtual Reality (VR) or Simulated Virtual Reality (sVR). The sequence of lidar point clouds should smoothly evolve over time, such as by following the trajectory of the vehicle collecting the data, enabling the code to display the movement of any dynamic objects captred by the point cloud. Most typically, the point cloud would be generated using Light Detection and Ranging (LiDAR) but other options could be used so long as the file format chosen matches the compatability list shown below.
  * It accepts .las files for global point clouds
  * It accepts .csv files for odometry readings
  * It accepts .png files for images
  * User-friendly configuration file to change important simulation parameters such as sampling rate and playback speed without the need for re-building
  * User-friendly keyboard interface to interact with the program during runtime, for example, to switch into a bird's eye view

Despite what the UE5 project name suggests, LiBACK (CubePlugin) is not in fact a plugin. The important files all live within the Source folder.

LiBACK was only tested on the VIVE headset. However, after enabling the appropriate UE5 plugins, use with other headsets should be possible.

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

