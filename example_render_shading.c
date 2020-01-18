#include "rasterizer.h"
#include "world.h"

#include <stdio.h>

int main() {
  const int w = 1000;
  const int h = 1000;

  // define camera
  Camera *camera = CameraPerspectiveProjection(V(2, 0, 0), V(0, 0, 0), V(0, 1, 0), w, h, 0.1, 1000, 60);

  // define material like gold
  const Material goldMaterial = (Material){V(0.831373, 0.686275, 0.215686), 1, 1, 1, 120};

  // load ball model from STL file
  Polygon *polygon = PolygonReadSTL("models/ball.stl");

  // calculate vertex normal vector
  PolygonCalculateVertexNormals(polygon);

  // define a Thing combined with polygon model, material and Transformer
  Thing *thing = ThingCreate(polygon, TransformerCreate(V(0, 0, 0), V(0, 0, 0), V(0.5, 0.5, 0.5)), &goldMaterial);

#ifdef _OPENMP
#pragma omp parallel for default(none) schedule(dynamic) shared(camera, thing)
#endif
  for (int i = 0; i < 360; ++i) {
    // create empty Bitmap
    Bitmap *bmp = BitmapNewImage(w, h);

    // define rotate light
    Transformer *transformer = TransformerCreate(V0, V(0, RADIAN(i), 0), V1);
    Vector lightPos = TransformerTransformPoint(transformer, V(10, -10, 0));
    Light light = LightCreatePointLight(V(1, 1, 1), V(1, 1, 1), lightPos);

    // create empty scene
    Scene *scene = SceneCreateEmpty();

    // set camera to the scene
    SceneSetCamera(scene, camera);

    // append light source to the scene
    SceneAppendLight(scene, &light);

    // append "thing" to the scene
    SceneAppendThing(scene, thing);

    // create empty z-buffer
    ZBuffer *zbuffer = ZBufferCreate(w, h);

    /*
     * Render type
     *  - WireframeRender: wire-frame
     *  - WireframeNormalsRender: wire-frame + normal vector
     *  - WorldRender: THIS IS WHAT YOU WANT
     *
     * Shading type
     *  - NullShading: Solid shading
     *  - FlatShading: Flat shading
     *  - GouraudShading: Gouraud shading
     *  - PhongShading: Phong shading
     *
     * Reflection model type
     *  - NullReflectionModel: nothing happens :)
     *  - PhongReflectionModel: Phong reflection model
     *  - BlinnPhongReflectionModel: Blinn Phong reflection model
     */
    SceneRender(scene, bmp, zbuffer, WorldRender, PhongShading, BlinnPhongReflectionModel);

    // save image to Bitmap file
    char buf[100];
    sprintf(buf, "render_shading_%d.bmp", i);
    BitmapWriteFile(bmp, buf);

    // clean-up
    SceneDestroy(scene);
    ZBufferDestroy(zbuffer);
    BitmapDestroy(bmp);
  }
  ThingDestroy(thing);
  PolygonDestroy(polygon);

  return 0;
}
