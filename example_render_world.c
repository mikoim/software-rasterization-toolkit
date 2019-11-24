#include <stdio.h>

#include "rasterizer.h"
#include "world.h"

int main() {
  // image width, height
  const int w = 1000;
  const int h = 1000;

  // define materials
  const Material monkeyRedMaterial = (Material){V(0.8274, 0.2196, 0.1098), 1, 1, 1, 30};
  const Material monkeyPurpleMaterial = (Material){V(0.4156, 0.2039, 0.5333), 0.5, 0.5, 0.5, 60};
  const Material ballMaterial = (Material){V(1, 1, 1), 1, 1, 1, 90};

  // load polygon from STL files
  Polygon *monkeyPolygon = PolygonReadSTL("models/monkey.stl");
  Polygon *ballPolygon = PolygonReadSTL("models/ball.stl");

  // calculate vertex normal vectors (if you like to use GouraudShading or PhongShading, you need to call PolygonCalculateVertexNormals)
  PolygonCalculateVertexNormals(monkeyPolygon);
  PolygonCalculateVertexNormals(ballPolygon);

  // define object position and transformation in world space
  Transformer *monkeyRedPos = TransformerCreate(V(0, 0.5, -0.4), V(RADIAN(-45), RADIAN(45), 0), V(0.5, 0.5, 0.5));
  Transformer *monkeyPurplePos = TransformerCreate(V(0, -0.5, 0.4), V(RADIAN(45), RADIAN(-45), 0), V(0.5, 0.5, 0.5));
  Transformer *topBallPos = TransformerCreate(V(0, 0.5, 0.4), V(RADIAN(45), RADIAN(-45), 0), V(0.2, 0.2, 0.2));
  Transformer *bottomBallPos = TransformerCreate(V(0, -0.5, -0.4), V(0, 0, 0), V(0.2, 0.2, 0.2));

  // define object in a world
  Thing *monkeyRed = ThingCreate(monkeyPolygon, monkeyRedPos, &monkeyRedMaterial);
  Thing *monkeyPurple = ThingCreate(monkeyPolygon, monkeyPurplePos, &monkeyPurpleMaterial);
  Thing *topBall = ThingCreate(ballPolygon, topBallPos, &ballMaterial);
  Thing *bottomBall = ThingCreate(ballPolygon, bottomBallPos, &ballMaterial);

  // create perspective camera
  Camera *camera = CameraPerspectiveProjection(V(2, 0, 0), V(0, 0, 0), V(0, 1, 0), w, h, 0.1, 1000, 60);

#ifdef _OPENMP
#pragma omp parallel for schedule(dynamic)
#endif
  for (int i = 0; i < 360; ++i) {
    Bitmap *bmp = BitmapNewImage(w, h);

    // create point source rotating around objects
    Transformer *lightTransformer = TransformerCreate(V0, V(RADIAN(i), RADIAN(i), 0), V1);
    Vector lightPos = TransformerTransformPoint(lightTransformer, V(10, 10, 10));
    TransformerDestroy(lightTransformer);
    Light light = LightCreatePointLight(V(1, 1, 1), V(1, 1, 1), lightPos);

    // create empty scene
    Scene *scene = SceneCreateEmpty();

    // set camera
    SceneSetCamera(scene, camera);

    // append light source to the scene
    SceneAppendLight(scene, &light);

    // append objects to the scene
    SceneAppendThing(scene, monkeyRed);
    SceneAppendThing(scene, monkeyPurple);
    SceneAppendThing(scene, topBall);
    SceneAppendThing(scene, bottomBall);

    // create Z-buffer
    ZBuffer *zbuffer = ZBufferCreate(w, h);

    // render scene to Bitmap
    SceneRender(scene, bmp, zbuffer, WorldRender, PhongShading, BlinnPhongReflectionModel);

    SceneDestroy(scene);

    ZBufferDestroy(zbuffer);

    char buf[100];
    sprintf(buf, "render_world_%d.bmp", i);

    BitmapWriteFile(bmp, buf);
    BitmapDestroy(bmp);
  }

  CameraDestroy(camera);

  ThingDestroy(bottomBall);
  ThingDestroy(topBall);
  ThingDestroy(monkeyPurple);
  ThingDestroy(monkeyRed);

  TransformerDestroy(bottomBallPos);
  TransformerDestroy(topBallPos);
  TransformerDestroy(monkeyPurplePos);
  TransformerDestroy(monkeyRedPos);

  PolygonDestroy(ballPolygon);
  PolygonDestroy(monkeyPolygon);

  return 0;
}
