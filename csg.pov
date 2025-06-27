#include "colors.inc"
camera{
 location <4, 4, -5>
 look_at <0, 0, 0>
 angle 36
}
light_source{ <500, 500, -1000> White }
background { color White }

 torus {
    1, .7              // major and minor radius
    rotate -90*x      // so we can see it from the top
    pigment { Green }
  }
/*
difference{
    intersection{
        box{-1, 1}
        sphere{0, 1.5}
        pigment{ Red }
    }
    union{
        cylinder{ <0, 0, 2>, <0, 0, -2>, 0.7}
        cylinder{ <2, 0, 0>, <-2, 0, 0>, 0.7}
        cylinder{ <0, 2, 0>, <0, -2, 0>, 0.7}
        //rotate <0, 135, 0> // <x°, y°, z°>
        //translate <0.4, 0, 0>
        //scale <0.2, 1.2, 1>
    }
    pigment{ Blue }
}
*/
