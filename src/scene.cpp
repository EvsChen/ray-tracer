#include "smartpointerhelp.h"
#include "scene.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

hitable *final_scene() {
     int l = 0;
     hitable **list = new hitable*[30];
     // Ground
     int b = 0;
     int nb = 20;
     hitable **boxlist = new hitable*[10000];
     material *ground = new lambertian(new constant_texture(vec3(0.48, 0.83, 0.53)));
     for (int i = 0; i < nb; i++) {
         for (int j = 0; j < nb; j++) {
             float w = 100;
             float x0 = -1000 + i*w;
             float z0 = -1000 + j*w;
             float y0 = 0;
             float x1 = x0 + w;
             float y1 = 100*(drand48()+0.01);
             float z1 = z0 + w;
             boxlist[b++] = new box(vec3(x0,y0,z0), vec3(x1,y1,z1), ground);
         }
     }
     list[l++] = new bvh_node(boxlist, b, 0, 1);

     // Top light
     material *light = new diffuse_light( new constant_texture(vec3(7, 7, 7)) );
     list[l++] = new xz_rect(123, 423, 147, 412, 554, light);

     // Foam Box
    int ns = 1000;
    hitable **boxlist2 = new hitable*[10000];
    material *white = new lambertian( new constant_texture(vec3(0.73, 0.73, 0.73)) );
    for (int j = 0; j < ns; j++) {
        boxlist2[j] = new sphere(vec3(165*drand48(), 165*drand48(), 165*drand48()), 10, white);
    }
    list[l++] = new translate(
                    new rotate_y(
                        new bvh_node(boxlist2, ns, 0.0, 1.0),
                        15),
                    vec3(-100, 270,395));

     // Moving sphere
     vec3 center(400, 400, 200);
     list[l++] = new moving_sphere(center, center+vec3(30, 0, 0), 0, 1, 50,
                                  new lambertian(new constant_texture(vec3(0.7, 0.3, 0.1))));

     // Glass
     list[l++] = new sphere(vec3(260, 150, 45), 50, new dielectric(1.5));

     // Metal
     list[l++] = new sphere(vec3(0, 150, 145), 50, new metal(vec3(0.8, 0.8, 0.9), 10.0));

     // Constant medium
     hitable *boundary = new sphere(vec3(360, 150, 145), 70, new dielectric(1.5));
     list[l++] = boundary;
     list[l++] = new constant_medium(boundary, 0.2, new constant_texture(vec3(0.2, 0.4, 0.9)));
     boundary = new sphere(vec3(0, 0, 0), 5000, new dielectric(1.5));
     list[l++] = new constant_medium(boundary, 0.0001,
                                     new constant_texture(vec3(1.0, 1.0, 1.0)));

     // Earth
    //  int nx, ny, nn;
    //  unsigned char *tex_data = stbi_load("earthmap.jpg", &nx, &ny, &nn, 0);
    //  material *emat =  new lambertian(new image_texture(tex_data, nx, ny));
    //  list[l++] = new sphere(vec3(400,200, 400), 100, emat);

    // Noise
    texture *pertext = new noise_texture(0.1);
    list[l++] =  new sphere(vec3(220,280, 300), 80, new lambertian( pertext ));
    return new hitable_list(list,l);
}

hitable *cornell_smoke() {
    hitable **list = new hitable*[8];
    int i = 0;
    material *red = new lambertian(new constant_texture(vec3(0.65, 0.05, 0.05)));
    material *white = new lambertian(new constant_texture(vec3(0.73, 0.73, 0.73)));
    material *green = new lambertian(new constant_texture(vec3(0.12, 0.45, 0.15)));
    material *light = new diffuse_light(new constant_texture(vec3(7, 7, 7)));
    list[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
    list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
    list[i++] = new xz_rect(113, 443, 127, 432, 554, light);
    list[i++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
    list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
    list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));
    hitable *b1 = new translate(
                    new rotate_y(
                        new box(vec3(0, 0, 0),
                                vec3(165, 165, 165),
                                white),
                        -18),
                    vec3(130, 0, 65));
    hitable *b2 = new translate(
                    new rotate_y(
                        new box(vec3(0, 0, 0),
                                vec3(165, 330, 165),
                                white),
                        15),
                    vec3(265, 0, 295));
    list[i++] = new constant_medium(b1, 0.01, new constant_texture(vec3(1.0, 1.0, 1.0)));
    list[i++] = new constant_medium(b2, 0.01, new constant_texture(vec3(0.0, 0.0, 0.0)));
    return new hitable_list(list, i);
}


hitable *simple_light() {
    texture *pertext = new noise_texture(4);
    hitable **list = new hitable*[3];
    list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(pertext));
    list[1] = new sphere(vec3(0, 2, 0), 2, new lambertian(pertext));
    //list[2] = new sphere(vec3(0, 7, 0), 2,
                         // new diffuse_light( new constant_texture(vec3(4, 4, 4))));
    list[2] = new xy_rect(3, 5, 1, 3, -2, new diffuse_light(new constant_texture(vec3(4, 4, 4))));
    return new hitable_list(list, 3);
}

hitable *cornell_box() {
    hitable **list = new hitable*[8];
    int i = 0;
    material *red = new lambertian(new constant_texture(vec3(0.65, 0.05, 0.05)));
    material *white = new lambertian(new constant_texture(vec3(0.73, 0.73, 0.73)));
    material *green = new lambertian(new constant_texture(vec3(0.12, 0.40, 0.15)));
    material *light = new diffuse_light(new constant_texture(vec3(15, 15, 15)));
    list[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
    list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
    list[i++] = new xz_rect(163, 393, 177, 382, 554, light);
    list[i++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
    list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
    list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));
    list[i++] = new translate(
                    new rotate_y(
                        new box(vec3(0, 0, 0),
                                vec3(165, 165, 165),
                                white),
                        -18),
                    vec3(130, 0, 65));
    list[i++] = new translate(
                    new rotate_y(
                        new box(vec3(0, 0, 0),
                                vec3(165, 330, 165),
                                white),
                        15),
                    vec3(265, 0, 295));
    return new hitable_list(list, i);
}

hitable *two_perlin_spheres() {
    texture *pertext = new noise_texture(4);
    hitable **list = new hitable*[2];
    list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(pertext));
    list[1] = new sphere(vec3(0, 2, 0), 2, new lambertian(pertext));
    return new hitable_list(list, 2);
}

hitable *test_scene() {
    hitable **list = new hitable*[9];
    int i = 0;
    texture *pertext = new noise_texture(4);
    material *perlin = new lambertian(pertext);
    material *red = new lambertian(new constant_texture(vec3(0.65, 0.05, 0.05)));
    material *white = new lambertian(new constant_texture(vec3(0.73, 0.73, 0.73)));
    material *green = new lambertian(new constant_texture(vec3(0.12, 0.40, 0.15)));
    material *light = new diffuse_light(new constant_texture(vec3(15, 15, 15)));
    list[i++] = new sphere(vec3(200, 300, 200), 100, perlin);
    list[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
    list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
    list[i++] = new xz_rect(213, 343, 227, 332, 554, light);
    list[i++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
    list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
    list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));
    list[i++] = new translate(
                    new rotate_y(
                        new box(vec3(0, 0, 0),
                                vec3(165, 165, 165),
                                white),
                        -18),
                    vec3(130, 0, 65));
    list[i++] = new translate(
                    new rotate_y(
                        new box(vec3(0, 0, 0),
                                vec3(165, 330, 165),
                                white),
                        15),
                    vec3(265, 0, 295));
    return new hitable_list(list, i);
}

hitable *earth() {
    int nx, ny, nn;
    //unsigned char *tex_data = stbi_load("tiled.jpg", &nx, &ny, &nn, 0);
    // unsigned char *tex_data = stbi_load("earthmap.jpg", &nx, &ny, &nn, 0);
    unsigned char *tex_data = stbi_load("earthmap2.png", &nx, &ny, &nn, 0);
    material *mat =  new lambertian(new image_texture(tex_data, nx, ny));
    return new sphere(vec3(0,0, 0), 2, mat);
}


hitable *random_scene() {
    int n = 200;
    hitable **list = new hitable*[n+1];
    texture *checker = new checker_texture(new constant_texture(vec3(0.2, 0.3, 0.1)),
                                           new constant_texture(vec3(0.9, 0.9, 0.9)));
    list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(checker));
    int i = 1;
    for (int a = -10; a < 10; a++) {
        for (int b = -10; b < 10; b++) {
            float choose_mat = drand48();
            vec3 center(a+0.9*drand48(), 0.2, b + 0.9 * drand48());
            if ((center - vec3(4, 0.2, 0)).length() <= 0.9) { continue; }
            if (choose_mat < 0.8) {
                 material* mat = new lambertian(
                    new constant_texture(vec3(drand48() * drand48(),
                                              drand48() * drand48(),
                                              drand48() * drand48())));
                 list[i++] = new moving_sphere(center,
                                               center + vec3(0, 0.5*drand48(), 0),
                                               0.0, 1.0, 0.2, mat);
            }
            else if (choose_mat < 0.95) {
                list[i++] = new sphere(center, 0.2,
                                       new metal(vec3(0.5*(1 + drand48()),
                                                      0.5*(1 + drand48()),
                                                      0.5*(1 + drand48())),
                                                 0.5*drand48()));
            }
            else {
                list[i++] = new sphere(center, 0.2, new dielectric(1.5));
            }
        }
    }
    material* mat = new lambertian(new constant_texture(vec3(0.4, 0.2, 0.1)));
    list[i++] = new sphere(vec3(-4, 1, 0), 1.0, mat);
    list[i++] = new sphere(vec3(0, 1, 0), 1.0, new dielectric(1.5));
    list[i++] = new sphere(vec3(4, 1, 0), 1.0, new metal(vec3(0.7, 0.6, 0.5), 0.0));
    return new hitable_list(list, i);
}

hitable *test_scene() {
  hitable **list = new hitable*[6];
  material *dielec = new dielectric(1.5);
  list[0] = new sphere(vec3(0,0,-1), 0.5, new lambertian(new constant_texture(vec3(0.1, 0.2, 0.5))));
  list[1] = new sphere(vec3(0,-100.5,-1), 100, new lambertian(new constant_texture(vec3(0.8, 0.8, 0.0))));
  list[2] = new sphere(vec3(1,0,-1), 0.5, new metal(vec3(0.8, 0.6, 0.2), 0.3));
  list[3] = new sphere(vec3(-1,0,-1), 0.5, dielec);
  list[4] = new sphere(vec3(-1,0,-1), -0.45, dielec);
  list[5] = new xz_rect(-3, 3, -3, 3, 3, new diffuse_light(new constant_texture(vec3(4, 4, 4))));
  return new hitable_list(list, 6);
}

Scene::Scene() {
    world = cornell_box();
}
