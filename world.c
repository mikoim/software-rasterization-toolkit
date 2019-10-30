#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "world.h"

Light _CreateLight(const LightType type, const Color specular, const Color diffuse, const Vector position) {
  Light light = (Light){type, specular, diffuse, position, V0};
  return light;
}

Light LightCreatePointLight(const Color specular, const Color diffuse, const Vector position) { return _CreateLight(PointLight, specular, diffuse, position); }

Light LightCreateDirectionalLight(const Color specular, const Color diffuse, const Vector direction) {
  Light light = _CreateLight(DirectionalLight, specular, diffuse, V0);
  light.direction = VectorL2Normalization(direction);
  return light;
}

Vector LightGetDirection(const Light light, const Vector position) {
  switch (light.type) {
  case PointLight:
    return VectorL2Normalization(VectorSubtraction(position, light.position));
  case DirectionalLight:
    return light.direction;
  default:
#ifndef NDEBUG
    fprintf(stderr, "%s: Invalid light type (%d)\n", __FUNCTION_NAME__, light.type);
#endif
    return V0;
  }
}

Vector LightGetPosition(const Light light) {
  switch (light.type) {
  case PointLight:
    return light.position;
  case DirectionalLight:
    return VectorScalarMultiplication(light.direction, LDBL_MAX);
  default:
#ifndef NDEBUG
    fprintf(stderr, "%s: Invalid light type (%d)\n", __FUNCTION_NAME__, light.type);
#endif
    return V0;
  }
}

Thing *ThingCreate(Polygon *polygon, Transformer *transformer, const Material material) {
  Thing *new = (Thing *)calloc(1, sizeof(Thing));
  new->polygon = polygon;
  new->material = material;
  new->transformer = transformer;
  return new;
}

bool ThingDestroy(Thing *thing) {
  if (thing == NULL) {
#ifndef NDEBUG
    fprintf(stderr, "%s: trying to free null pointer, ignored.\n", __FUNCTION_NAME__);
#endif
    return false;
  }
  PolygonDestroy(thing->polygon);
  TransformerDestroy(thing->transformer);
  free(thing);
  return true;
}

Scene *SceneCreateEmpty() {
  Scene *new = calloc(1, sizeof(Scene));
  return new;
}

bool SceneDestroy(Scene *scene) {
  CameraDestroy(scene->camera);
  uint64_t thingCount = scene->thing;
  for (uint64_t i = 0; i < thingCount; ++i) {
    ThingDestroy(scene->things[i]);
  }
  free(scene->things);
  uint64_t lightCount = scene->light;
  for (uint64_t i = 0; i < lightCount; ++i) {
    // TODO: call LightDestroy() here
  }
  free(scene->lights);
  free(scene);
  return true;
}

bool SceneSetCamera(Scene *scene, Camera *camera) {
  scene->camera = camera;
  return true;
}

bool SceneAppendThing(Scene *scene, Thing *thing) {
  // TODO: extract duplicated codes to dynamic array allocator
  uint64_t thingCount = scene->thing;
  Thing **things;
  // TODO: optimize dynamic array allocation
  if (thingCount == 0) {
    things = (Thing **)calloc(1, sizeof(Thing *));
    scene->things = things;
  } else {
    things = (Thing **)calloc(thingCount + 1, sizeof(Thing **));
    memcpy(things, scene->things, sizeof(Thing *) * thingCount);
    free(scene->things);
    scene->things = things;
  }
  scene->things[thingCount] = thing;
  scene->thing = thingCount + 1;
  return true;
}

bool SceneAppendLight(Scene *scene, Light *light) {
  // TODO: extract duplicated codes to dynamic array allocator
  uint64_t lightCount = scene->thing;
  Light **lights;
  // TODO: optimize dynamic array allocation
  if (lightCount == 0) {
    lights = (Light **)calloc(1, sizeof(Light *));
    scene->lights = lights;
  } else {
    lights = (Light **)calloc(lightCount + 1, sizeof(Light *));
    memcpy(lights, scene->lights, sizeof(Light **) * lightCount);
    free(scene->lights);
    scene->lights = lights;
  }
  scene->lights[lightCount] = light;
  scene->light = lightCount + 1;
  return true;
}

// TODO: merge with DrawTriangle
void _DrawTriangleGouraud(Bitmap *bitmap, const Vector v1, const Vector v2, const Vector v3, Color c1, Color c2, Color c3, ZBuffer *zbuffer) {
  const uint32_t maxX = (uint32_t)fminl(fmaxl(fmaxl(v1.x, fmaxl(v2.x, v3.x)), 0), bitmap->dibHeader.bcWidth - 1);
  const uint32_t minX = (uint32_t)fmaxl(fminl(v1.x, fminl(v2.x, v3.x)), 0);
  const uint32_t maxY = (uint32_t)fminl(fmaxl(fmaxl(v1.y, fmaxl(v2.y, v3.y)), 0), bitmap->dibHeader.bcHeight - 1);
  const uint32_t minY = (uint32_t)fmaxl(fminl(v1.y, fminl(v2.y, v3.y)), 0);

  for (uint32_t y = minY; y <= maxY; ++y) {
    for (uint32_t x = minX; x <= maxX; ++x) {
      if (VectorInsideTriangle2D(V(x, y, 0), v1, v2, v3)) {
        Vector weight = VectorBarycentricCoordinateWeight(V(x, y, 0), v1, v2, v3);
        Real depth = v1.z * weight.x + v2.z * weight.y + v3.z * weight.z;
        Color color = VectorAddition(VectorScalarMultiplication(c1, weight.x), VectorAddition(VectorScalarMultiplication(c2, weight.y), VectorScalarMultiplication(c3, weight.z)));
        if (zbuffer == NULL || ZBufferTestAndUpdate(zbuffer, x, y, depth)) {
          BitmapSetPixelColor(bitmap, x, bitmap->dibHeader.bcHeight - y - 1, BMP_COLOR(color.x * 255, color.y * 255, color.z * 255));
        }
      }
    }
  }
}

bool SceneRender(const Scene *scene, Bitmap *bitmap, ZBuffer *zbuffer, Color reflectionModel(const Scene *, const Thing *, const Vector, const Vector)) {
  for (uint64_t thingIndex = 0; thingIndex < scene->thing; ++thingIndex) {
    Thing *thing = scene->things[thingIndex];
    for (uint32_t triangleIndex = 0; triangleIndex < thing->polygon->triangle; ++triangleIndex) {
      Triangle triangle = thing->polygon->triangles[triangleIndex];
      Triangle triangleWorld = TransformerTransformTriangle(thing->transformer, triangle);
      Triangle triangleNDC = rasterize(scene->camera, triangleWorld);

      // NOTE: ReflectionModel uses position in world space
      Color c1 = reflectionModel(scene, thing, triangle.vertexes[0], triangleWorld.normal);
      Color c2 = reflectionModel(scene, thing, triangle.vertexes[1], triangleWorld.normal);
      Color c3 = reflectionModel(scene, thing, triangle.vertexes[2], triangleWorld.normal);

      _DrawTriangleGouraud(bitmap, triangleNDC.vertexes[0], triangleNDC.vertexes[1], triangleNDC.vertexes[2], c1, c2, c3, zbuffer);
    }
  }
  return true;
}

Color PhongReflectionModel(const Scene *scene, const Thing *thing, const Vector surfacePosition, const Vector normal) {
  Color illumination = VectorScalarMultiplication(V(0.1, 0.1, 0.1), thing->material.ambient);
  for (uint64_t lightIndex = 0; lightIndex < scene->light; ++lightIndex) {
    const Light *light = scene->lights[lightIndex];

    const Real corr = fmaxl(VectorDotProduct(LightGetDirection(*light, surfacePosition), normal), 0); // negative value must be ignored
    const Vector R_m = VectorSubtraction(VectorScalarMultiplication(normal, 2 * corr), normal);

    Color diffuse = VectorScalarMultiplication(light->diffuse, fmaxl(thing->material.diffuse * corr, 0));
    Color specular =
        VectorScalarMultiplication(light->specular, thing->material.specular * fmaxl(powl(VectorDotProduct(R_m, CameraGetDirection(scene->camera, surfacePosition)), thing->material.shininess), 0));

    illumination = VectorAddition(illumination, VectorAddition(diffuse, specular));
  }

  illumination = VectorConfine(illumination, 0, 1); // limit range [0-1]

  return V(illumination.x * thing->material.color.x, illumination.y * thing->material.color.y, illumination.z * thing->material.color.z);
}

Color BlinnPhongReflectionModel(const Scene *scene, const Thing *thing, const Vector surfacePosition, const Vector normal) {
  Color illumination = VectorScalarMultiplication(V(0.1, 0.1, 0.1), thing->material.ambient);
  for (uint64_t lightIndex = 0; lightIndex < scene->light; ++lightIndex) {
    const Light *light = scene->lights[lightIndex];

    const Real corr = fmaxl(VectorDotProduct(LightGetDirection(*light, surfacePosition), normal), 0); // negative value must be ignored
    const Vector H = VectorL2Normalization(VectorAddition(LightGetDirection(*light, surfacePosition), CameraGetDirection(scene->camera, surfacePosition)));

    Color diffuse = VectorScalarMultiplication(light->diffuse, fmaxl(thing->material.diffuse * corr, 0));
    Color specular = VectorScalarMultiplication(light->specular, thing->material.specular * fmaxl(powl(VectorDotProduct(normal, H), thing->material.shininess), 0));

    illumination = VectorAddition(illumination, VectorAddition(diffuse, specular));
  }

  illumination = VectorConfine(illumination, 0, 1); // limit range [0 - 1]

  return V(illumination.x * thing->material.color.x, illumination.y * thing->material.color.y, illumination.z * thing->material.color.z);
}