# KITTI-to-Darkent

## * Purpose    
    This code is for converting KITTI Data set to VOC Data set.
    
    * KITTI Format
    0 - class
    1 - truncated float : 절단된 정도 (0 < 1)
    2 - occluded integer (0,1,2,3) (맞물림?)
        0 : fully visible
        1 : partly occluded
        2 : largely occluded
        3 : unknown
    3 - alpha Observation angle of object, ranging [-pi..pi]
    4~7 - 4 bbox 2D bounding box of object in the image (0-based index), contains left, top, right, bottom
    8~10 dimensions 3D object dimensions height, width, length (in meters)
    11~13 location 3D object location x,y,z in camera coordinates (in meters)
    14 rotation_y Rotation ry around Y-axis in camera coordinates [-pi..pi]
    
    * VOC Format
    0 - class
    1 - x (center of box)
    2 - y (center of box)
    3 - w (width of box)
    4 - h (width of box)
    
## Usage
    
     
