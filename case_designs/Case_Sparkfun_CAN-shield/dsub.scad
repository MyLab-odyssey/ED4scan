// OpenSCAD code: Dsub-Connector
// (C) 2018 by MyLab-odyssey

// sc = scale, set larger if your part doesn't fit. default is the exact dimensions of the connector housing. recomended 1.1 for most mounting holes,
// sz = size, set to the correct size for your dsub. Common values are 17.04 for db9 or high density db15, 25.37 for standard db15, 39.09 for db25. 
//dp= depth, set to a size that can penetrate the panel you are using it with

module dsub(sc,sz,dp){
    $fn=64;
    sc_d = 1.75;
    
    cs=(sz/2)-2.6;
    cs2=(sz/2)-4.095;
    ns=(sz/2)+4.04;
    translate([1.66,-ns+3,0]){
        cylinder(r=1.6*sc_d,h=10);
    }
    translate([1.66,ns-3,0]){
        cylinder(r=1.6*sc_d,h=10);
    }
    
    scale([sc,sc,sc]){
    
        hull(){
            translate([0,-cs,0]){
                cylinder(r=2.6,h=10);
            }
            translate([0,cs,0]){
                cylinder(r=2.6,h=10);
            }
            translate([3.28,-cs2,0]){
                cylinder(r=2.6,h=10);
            }
            translate([3.28,cs2,0]){
                cylinder(r=2.6,h=10);
            }
        }
    }
}
