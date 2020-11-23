Motor
=====

Speed ?
-------

Source: https://www.smart-cube.biz/produit/kit-smart-car-2-roue-2-moteur-roue-libre-chassi/

* 3V    -> 125 RPM
* 4.5V  -> 200 RPM

Source https://bc-robotics.com/shop/3-6vdc-hobby-gearmotor/

* 3V  -> 120 RPM 
* 6V  -> 250 RPM


Rotating
--------

* W is the distance between the 2 wheels
* D90 is the distance the wheel needs for the robot to change angle
* r is the radius of the wheel
* D is the distance gone in one rotation
* R90 is the number of rotations to do a 90 degree turn
* Rpm is the number of rotations per minute (rpm)
* S90 is the number of seconds to do a 90 degree turn

R90 = D90 / D
S90 = R90 / (Rpm / 60)

### One wheel fixed

D90 = 1/2 * pi * W

### Two wheels moving

D90 = 1/2 pi * 1/2 * W
