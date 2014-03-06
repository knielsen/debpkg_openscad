// Used to cause issue #349

// Very slightly non-planar polyhedron
polyhedron(faces=[[3,2,1,0],[7,6,5,4],[0,1,6,7],[1,2,5,6],[2,3,4,5],[3,0,7,4]],
points=[
    [0.0174497,-0.0174524,0.999695],
    [1.0173,-0.0174524,0.982243],
    [1.0176,0.982395,0.999693],
    [0.0177543,0.982395,1.01715],
    [0.000304586,0.999848,0.0174497],
    [1.00015,0.999848,-0.00000265809],
    [0.999848,-0.0000000000000271051,-0.0174524],
    [0,0,0]]);

// Really non-planar polyhedron
translate([2,0,0]) polyhedron(faces=[[3,2,1,0],[7,6,5,4],[0,1,6,7],[1,2,5,6],[2,3,4,5],[3,0,7,4]],
points=[
    [0,0,1],
    [1,0.2,1],
    [1,1,1],
    [0,1,1],
    [0,1,0],
    [1,1,0],
    [1,0,0],
    [0,0,0]]);

// Real-world example: truncated icosidodecahedron
translate([4.5,0.5,0.5]) scale(0.02) polyhedron(points = [
[-10., -13.090169943749475, -34.270509831248425], 
[-10., -13.090169943749475, 34.270509831248425], 
[-10., 13.090169943749475, -34.270509831248425], 
[-10., 13.090169943749475, 34.270509831248425], 
[-5., -5., -37.3606797749979], [-5., -5., 37.3606797749979], 
[-5., 5., -37.3606797749979], [-5., 5., 37.3606797749979], 
[-5., -37.3606797749979, -5.], [-5., -37.3606797749979, 5.], 
[-5., -21.18033988749895, -31.18033988749895], 
[-5., -21.18033988749895, 31.18033988749895], [-5., 37.3606797749979, -5.], 
[-5., 37.3606797749979, 5.], [-5., 21.18033988749895, -31.18033988749895], 
[-5., 21.18033988749895, 31.18033988749895], [5., -5., -37.3606797749979], 
[5., -5., 37.3606797749979], [5., 5., -37.3606797749979], 
[5., 5., 37.3606797749979], [5., -37.3606797749979, -5.], 
[5., -37.3606797749979, 5.], [5., -21.18033988749895, -31.18033988749895], 
[5., -21.18033988749895, 31.18033988749895], [5., 37.3606797749979, -5.], 
[5., 37.3606797749979, 5.], [5., 21.18033988749895, -31.18033988749895], 
[5., 21.18033988749895, 31.18033988749895], [10., -13.090169943749475, 
 -34.270509831248425], [10., -13.090169943749475, 34.270509831248425], 
[10., 13.090169943749475, -34.270509831248425], 
[10., 13.090169943749475, 34.270509831248425], 
[-34.270509831248425, -10., -13.090169943749475], 
[-34.270509831248425, -10., 13.090169943749475], 
[-34.270509831248425, 10., -13.090169943749475], 
[-34.270509831248425, 10., 13.090169943749475], 
[-29.270509831248425, -18.090169943749473, -16.18033988749895], 
[-29.270509831248425, -18.090169943749473, 16.18033988749895], 
[-29.270509831248425, 18.090169943749473, -16.18033988749895], 
[-29.270509831248425, 18.090169943749473, 16.18033988749895], 
[-18.090169943749473, -16.18033988749895, -29.270509831248425], 
[-18.090169943749473, -16.18033988749895, 29.270509831248425], 
[-18.090169943749473, 16.18033988749895, -29.270509831248425], 
[-18.090169943749473, 16.18033988749895, 29.270509831248425], 
[-13.090169943749475, -34.270509831248425, -10.], 
[-13.090169943749475, -34.270509831248425, 10.], 
[-13.090169943749475, -24.270509831248425, -26.18033988749895], 
[-13.090169943749475, -24.270509831248425, 26.18033988749895], 
[-13.090169943749475, 24.270509831248425, -26.18033988749895], 
[-13.090169943749475, 24.270509831248425, 26.18033988749895], 
[-13.090169943749475, 34.270509831248425, -10.], 
[-13.090169943749475, 34.270509831248425, 10.], 
[-26.18033988749895, -13.090169943749475, -24.270509831248425], 
[-26.18033988749895, -13.090169943749475, 24.270509831248425], 
[-26.18033988749895, 13.090169943749475, -24.270509831248425], 
[-26.18033988749895, 13.090169943749475, 24.270509831248425], 
[-37.3606797749979, -5., -5.], [-37.3606797749979, -5., 5.], 
[-37.3606797749979, 5., -5.], [-37.3606797749979, 5., 5.], 
[-16.18033988749895, -29.270509831248425, -18.090169943749473], 
[-16.18033988749895, -29.270509831248425, 18.090169943749473], 
[-16.18033988749895, 29.270509831248425, -18.090169943749473], 
[-16.18033988749895, 29.270509831248425, 18.090169943749473], 
[-31.18033988749895, -5., -21.18033988749895], 
[-31.18033988749895, -5., 21.18033988749895], 
[-31.18033988749895, 5., -21.18033988749895], 
[-31.18033988749895, 5., 21.18033988749895], 
[-21.18033988749895, -31.18033988749895, -5.], 
[-21.18033988749895, -31.18033988749895, 5.], 
[-21.18033988749895, 31.18033988749895, -5.], 
[-21.18033988749895, 31.18033988749895, 5.], 
[-24.270509831248425, -26.18033988749895, -13.090169943749475], 
[-24.270509831248425, -26.18033988749895, 13.090169943749475], 
[-24.270509831248425, 26.18033988749895, -13.090169943749475], 
[-24.270509831248425, 26.18033988749895, 13.090169943749475], 
[16.18033988749895, -29.270509831248425, -18.090169943749473], 
[16.18033988749895, -29.270509831248425, 18.090169943749473], 
[16.18033988749895, 29.270509831248425, -18.090169943749473], 
[16.18033988749895, 29.270509831248425, 18.090169943749473], 
[24.270509831248425, -26.18033988749895, -13.090169943749475], 
[24.270509831248425, -26.18033988749895, 13.090169943749475], 
[24.270509831248425, 26.18033988749895, -13.090169943749475], 
[24.270509831248425, 26.18033988749895, 13.090169943749475], 
[37.3606797749979, -5., -5.], [37.3606797749979, -5., 5.], 
[37.3606797749979, 5., -5.], [37.3606797749979, 5., 5.], 
[21.18033988749895, -31.18033988749895, -5.], 
[21.18033988749895, -31.18033988749895, 5.], 
[21.18033988749895, 31.18033988749895, -5.], 
[21.18033988749895, 31.18033988749895, 5.], 
[13.090169943749475, -34.270509831248425, -10.], 
[13.090169943749475, -34.270509831248425, 10.], 
[13.090169943749475, -24.270509831248425, -26.18033988749895], 
[13.090169943749475, -24.270509831248425, 26.18033988749895], 
[13.090169943749475, 24.270509831248425, -26.18033988749895], 
[13.090169943749475, 24.270509831248425, 26.18033988749895], 
[13.090169943749475, 34.270509831248425, -10.], 
[13.090169943749475, 34.270509831248425, 10.], 
[26.18033988749895, -13.090169943749475, -24.270509831248425], 
[26.18033988749895, -13.090169943749475, 24.270509831248425], 
[26.18033988749895, 13.090169943749475, -24.270509831248425], 
[26.18033988749895, 13.090169943749475, 24.270509831248425], 
[31.18033988749895, -5., -21.18033988749895], 
[31.18033988749895, -5., 21.18033988749895], 
[31.18033988749895, 5., -21.18033988749895], 
[31.18033988749895, 5., 21.18033988749895], 
[18.090169943749473, -16.18033988749895, -29.270509831248425], 
[18.090169943749473, -16.18033988749895, 29.270509831248425], 
[18.090169943749473, 16.18033988749895, -29.270509831248425], 
[18.090169943749473, 16.18033988749895, 29.270509831248425], 
[29.270509831248425, -18.090169943749473, -16.18033988749895], 
[29.270509831248425, -18.090169943749473, 16.18033988749895], 
[29.270509831248425, 18.090169943749473, -16.18033988749895], 
[29.270509831248425, 18.090169943749473, 16.18033988749895], 
[34.270509831248425, -10., -13.090169943749475], 
[34.270509831248425, -10., 13.090169943749475], 
[34.270509831248425, 10., -13.090169943749475], 
[34.270509831248425, 10., 13.090169943749475]],faces = 
[[41, 53, 65, 67, 55, 43, 3, 7, 5, 1], [100, 104, 106, 102, 110, 30, 18, 16, 
 28, 108], [11, 1, 5, 17, 29, 23], [18, 30, 26, 14, 2, 6], 
[33, 37, 73, 69, 68, 72, 36, 32, 56, 57], [91, 90, 82, 114, 118, 86, 87, 
 119, 115, 83], [81, 113, 117, 85, 84, 116, 112, 80, 88, 89], 
[59, 58, 34, 38, 74, 70, 71, 75, 39, 35], [0, 10, 22, 28, 16, 4], 
[15, 27, 31, 19, 7, 3], [64, 52, 40, 0, 4, 6, 2, 42, 54, 66], 
[19, 31, 111, 103, 107, 105, 101, 109, 29, 17], [96, 110, 102, 114, 82, 78], 
[53, 41, 47, 61, 73, 37], [43, 49, 15, 3], [94, 108, 28, 22], 
[23, 29, 109, 95], [2, 14, 48, 42], [36, 72, 60, 46, 40, 52], 
[79, 83, 115, 103, 111, 97], [69, 45, 9, 8, 44, 68], 
[24, 98, 90, 91, 99, 25], [77, 95, 109, 101, 113, 81], 
[42, 48, 62, 74, 38, 54], [40, 46, 10, 0], [97, 111, 31, 27], 
[44, 8, 20, 92, 76, 94, 22, 10, 46, 60], [63, 51, 13, 25, 99, 79, 97, 27, 
 15, 49], [26, 30, 110, 96], [1, 11, 47, 41], [55, 39, 75, 63, 49, 43], 
[80, 112, 100, 108, 94, 76], [48, 14, 26, 96, 78, 98, 24, 12, 50, 62], 
[61, 47, 11, 23, 95, 77, 93, 21, 9, 45], [71, 70, 50, 12, 13, 51], 
[93, 89, 88, 92, 20, 21], [102, 106, 118, 114], [65, 53, 37, 33], 
[74, 62, 50, 70], [77, 81, 89, 93], [101, 105, 117, 113], [66, 54, 38, 34], 
[73, 61, 45, 69], [78, 82, 90, 98], [32, 36, 52, 64], [115, 119, 107, 103], 
[92, 88, 80, 76], [71, 51, 63, 75], [56, 32, 64, 66, 34, 58], 
[107, 119, 87, 85, 117, 105], [35, 39, 55, 67], [112, 116, 104, 100], 
[99, 91, 83, 79], [68, 44, 60, 72], [57, 59, 35, 67, 65, 33], 
[116, 84, 86, 118, 106, 104], [4, 16, 18, 6], [7, 19, 17, 5], 
[12, 24, 25, 13], [9, 21, 20, 8], [56, 58, 59, 57], [85, 87, 86, 84]]
);
