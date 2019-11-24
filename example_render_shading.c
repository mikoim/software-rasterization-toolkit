#include "rasterizer.h"
#include "world.h"

#include <stdio.h>

int main() {
  const int w = 1000;
  const int h = 1000;

#ifdef _OPENMP
#pragma omp parallel for schedule(dynamic)
#endif
  for (int i = 0; i < 360; ++i) {

    Bitmap *bmp = BitmapNewImage(w, h);

    Camera *camera = CameraPerspectiveProjection(V(2, 0, 0), V(0, 0, 0), V(0, 1, 0), w, h, 0.1, 1000, 60);

    Transformer *transformer = TransformerCreate(V0, V(0, RADIAN(i), 0), V1);
    Vector lightPos = TransformerTransformPoint(transformer, V(10, -10, 0));
    Light light = LightCreatePointLight(V(1, 1, 1), V(1, 1, 1), lightPos);

    const Material goldMaterial = (Material){V(0.831373, 0.686275, 0.215686), 1, 1, 1, 120};

    Polygon *polygon = PolygonReadSTL("models/ball.stl");
    PolygonCalculateVertexNormals(polygon);
    Thing *thing = ThingCreate(polygon, TransformerCreate(V(0, 0, 0), V(0, 0, 0), V(0.5, 0.5, 0.5)), &goldMaterial);

    Scene *scene = SceneCreateEmpty();
    SceneSetCamera(scene, camera);
    SceneAppendLight(scene, &light);
    SceneAppendThing(scene, thing);

    ZBuffer *zbuffer = ZBufferCreate(w, h);

    SceneRender(scene, bmp, zbuffer, WorldRender, FlatShading, BlinnPhongReflectionModel);

    SceneDestroy(scene);

    ZBufferDestroy(zbuffer);

    char buf[100];
    sprintf(buf, "render_shading_%d.bmp", i);

    BitmapWriteFile(bmp, buf);
    BitmapDestroy(bmp);
  }

  return 0;
}
