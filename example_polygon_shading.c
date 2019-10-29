#include "rasterizer.h"
#include "world.h"

int main() {
  const int w = 1000;
  const int h = 1000;

  Bitmap *bmp = BitmapNewImage(w, h);
  Bitmap *bmpDepth = BitmapNewImage(w, h);

  Camera *camera = CameraPerspectiveProjection(V(2, 0, 0), V(0, 0, 0), V(0, 1, 0), w, h, 0.1, 1000, 60);
  Light light = LightCreatePointLight(V(1, 1, 1), V(1, 1, 1), V(10, 10, 0));

  const Material cubeMaterial = (Material){V(1, 0, 0), 0.2, 0.2, 1, 50};
  const Material aliceMaterial = (Material){V(0, 1, 0), 0.4, 0.4, 1, 50};
  const Material monkeyMaterial = (Material){V(0, 0, 1), 0.6, 0.6, 1, 50};
  const Material ballMaterial = (Material){V(1, 1, 1), 1, 1, 1, 50};

  Thing *cube = ThingCreate(PolygonReadSTL("models/cube.stl"), TransformerCreate(V(0, 0.75, 0), V(RADIAN(45), RADIAN(45), RADIAN(45)), V(0.2, 0.2, 0.2)), cubeMaterial);
  Thing *alice = ThingCreate(PolygonReadSTL("models/alice.stl"), TransformerCreate(V(0, 0, 0.5), V(0, 0, 0), V1), aliceMaterial);
  Thing *monkey = ThingCreate(PolygonReadSTL("models/monkey.stl"), TransformerCreate(V(0, 0, -0.3), V(RADIAN(-45), RADIAN(45), 0), V(0.5, 0.5, 0.5)), monkeyMaterial);

  Thing *ballTopRight = ThingCreate(PolygonReadSTL("models/ball.stl"), TransformerCreate(V(0, 1, -0.8), V(0, 0, 0), V(0.1, 0.1, 0.1)), ballMaterial);
  Thing *ballBottomRight = ThingCreate(PolygonReadSTL("models/ball.stl"), TransformerCreate(V(0, -1, -0.8), V(0, 0, 0), V(0.1, 0.1, 0.1)), ballMaterial);
  Thing *ballTopLeft = ThingCreate(PolygonReadSTL("models/ball.stl"), TransformerCreate(V(0, 1, 0.8), V(0, 0, 0), V(0.1, 0.1, 0.1)), ballMaterial);
  Thing *ballBottomLeft = ThingCreate(PolygonReadSTL("models/ball.stl"), TransformerCreate(V(0, -1, 0.8), V(0, 0, 0), V(0.1, 0.1, 0.1)), ballMaterial);

  Scene *scene = SceneCreateEmpty();
  SceneSetCamera(scene, camera);
  SceneAppendLight(scene, &light);
  SceneAppendThing(scene, cube);
  SceneAppendThing(scene, alice);
  SceneAppendThing(scene, monkey);
  SceneAppendThing(scene, ballTopRight);
  SceneAppendThing(scene, ballBottomRight);
  SceneAppendThing(scene, ballTopLeft);
  SceneAppendThing(scene, ballBottomLeft);

  ZBuffer *zbuffer = ZBufferCreate(w, h);

  SceneRender(scene, bmp, zbuffer, BlinnPhongReflectionModel);

  SceneDestroy(scene);

  ZBufferExportToImage(zbuffer, bmpDepth);
  ZBufferDestroy(zbuffer);

  BitmapWriteFile(bmp, "polygon_shading.bmp");
  BitmapDestroy(bmp);

  BitmapWriteFile(bmpDepth, "polygon_shading_depth.bmp");
  BitmapDestroy(bmpDepth);

  return 0;
}
