#include "Render2D_v1.h"
#include "bezier.h"
#include "polygon_triangulation.h"

int main(){
   std::vector<vec2> P = loadCurve("curvaC.txt");
   std::vector<vec2> Q = sample_bezier_spline<3>(P, 30);
   std::vector<unsigned int> indices = triangulate_polygon(Q);

   LineStrip LS{P.size()};
   Elements<Triangles> T{indices};
  
   ImageRGB G(600, 600);
   G.fill(white);
   render2d(Q, T, red, G);
   G.save("output.png");
}
