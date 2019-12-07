#include <stdio.h>

#include "csg.h"
#include "world.h"

int main() {
  // create CSG primitive
  CSGSets *csgSets = CSGPrimitiveSetsCreate();
  // CSGPrimitiveSetsAppend(csgSets, (CSGPrimitive *)CSGPrimitiveCubeCreate(2));
  // CSGPrimitiveSetsAppend(csgSets, (CSGPrimitive *)CSGPrimitiveCylinderCreate(1, 2, 128));
  // CSGPrimitiveSetsAppend(csgSets, (CSGPrimitive *)CSGPrimitiveTriangularCreate(1, 3, 128));
  CSGPrimitiveSetsAppend(csgSets, (CSGPrimitive *)CSGPrimitiveBallCreate(1, 8));
  CSGPrimitiveSetsReduce(csgSets);

  // convert CSG primitive set to polygon
  Polygon *polygon = CSGPrimitiveSetsPolygon(csgSets);
  PolygonCalculateVertexNormals(polygon);

  // now, CSG primitive set can be destroyed
  CSGPrimitiveSetsDestroy(csgSets);

  for (int i = 0; i < 90; ++i) {
    const int w = 1000, h = 1000;
    Bitmap *bmp = BitmapNewImage(w, h);

    // create camera
    Camera *camera = CameraPerspectiveProjection(V(3, -3, 3), V(1, -2, 1), V(0, 1, 0), w, h, 0.1, 1000, 60);

    // create empty scene
    Scene *scene = SceneCreateEmpty();

    // set camera
    SceneSetCamera(scene, camera);

    // create Z-buffer
    ZBuffer *zbuffer = ZBufferCreate(w, h);

    // append CSG primitive to scene
    Transformer *transformer = TransformerCreate(V0, V(0, RADIAN(i), 0), V1);

    // append light
    Light light = LightCreatePointLight(V(1, 1, 1), V(1, 1, 1), V(0, -6, 3));
    SceneAppendLight(scene, &light);

    const Material redMaterial = (Material){V(0.8274, 0.2196, 0.1098), 1, 1, 1, 30};
    Thing *thing = ThingCreate(polygon, transformer, &redMaterial);
    SceneAppendThing(scene, thing);

    // render scene to Bitmap
    SceneRender(scene, bmp, zbuffer, WorldRender, GouraudShading, PhongReflectionModel);

    char buf[100];
    sprintf(buf, "csg_%03d.bmp", i);
    BitmapWriteFile(bmp, buf);

    // clean-up
    ThingDestroy(thing);
    TransformerDestroy(transformer);
    ZBufferDestroy(zbuffer);
    SceneDestroy(scene);
    CameraDestroy(camera);
    BitmapDestroy(bmp);
  }

  PolygonDestroy(polygon);
}