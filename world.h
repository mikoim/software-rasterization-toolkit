#ifndef RENDER_WORLD_H
#define RENDER_WORLD_H

#include "camera.h"
#include "polygon.h"
#include "transformer.h"
#include "vector.h"
#include "rasterizer.h"

typedef Vector Color; // RGB [0 - 1]

typedef enum { PointLight, DirectionalLight } LightType;

typedef struct tagLight {
    LightType type;
    Color specular;   // 鏡面反射成分 i_s
    Color diffuse;    // 拡散反射成分 i_d
    Vector position;  // [Point light] world space
    Vector direction; // [Directional light]
} Light;

typedef struct tagMaterial {
    Color color;    // i_a: define surface color as ambient color
    Real specular;  // k_s which is a specular reflection constant, the ratio of reflection of the specular term of incoming light
    Real diffuse;   // k_d which is a diffuse reflection constant, the ratio of reflection of the diffuse term of incoming light (Lambertian reflectance)
    Real ambient;   // k_a which is an ambient reflection constant, the ratio of reflection of the ambient term present in all points in the scene rendered
    Real shininess; // alpha: which is a shininess constant for this material, which is larger for surfaces that are smoother and more mirror-like.
} Material;

typedef struct tagThing {
    Polygon *polygon;
    Material material;
    Transformer *transformer;
} Thing;

typedef struct tagScene {
    Camera *camera;
    uint64_t thing; // number of things
    Thing **things;
    uint64_t light; // numbre of lights
    Light **lights;
} Scene;

Light LightCreatePointLight(Color specular, Color diffuse, Vector position);
Light LightCreateDirectionalLight(Color specular, Color diffuse, Vector direction);

Vector LightGetDirection(Light light, Vector position);
Vector LightGetPosition(Light light);

Thing *ThingCreate(Polygon *polygon, Transformer *transformer, Material material);
bool ThingDestroy(Thing *thing);

Color PhongReflectionModel(const Scene *scene, const Thing *thing, Vector surfacePosition, Vector normal);
Color BlinnPhongReflectionModel(const Scene *scene, const Thing *thing, Vector surfacePosition, Vector normal);

Scene *SceneCreateEmpty();
bool SceneDestroy(Scene *scene);
bool SceneSetCamera(Scene *scene, Camera *camera);
bool SceneAppendThing(Scene *scene, Thing *thing);
bool SceneAppendLight(Scene *scene, Light *light);
bool SceneRender(const Scene *scene, Bitmap *bitmap, ZBuffer *zbuffer, Color reflectionModel(const Scene *, const Thing *, const Vector, const Vector));

#endif // RENDER_WORLD_H
