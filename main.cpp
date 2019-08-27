#include <iostream>
#include "sphere.h"
#include "hitable_list.h"
#include "float.h"
#include "camera.h"
#include "material.h"
#include "texture.h"
#include "rect.h"
#include "box.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

vec3 color(const ray& r, hitable *world, int depth) {
    hit_record rec;
    // 0.001 for avoiding t close to 0
    if (world->hit(r, 0.001, MAXFLOAT, rec)) {
        ray scattered;
        vec3 attenuation;
        vec3 emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);
        if (depth < 50 && rec.mat_ptr -> scatter(r, rec, attenuation, scattered)) {
            return emitted + attenuation * color(scattered, world, depth + 1);
        }
        else
            return emitted;
    }
    else {
        // vec3 unit_direction = unit_vector(r.direction());
        // float t = 0.5 * (unit_direction.y() + 1.0);
        // return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
        return vec3(0, 0, 0);
    }
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
    list[i++] = new xz_rect(213, 343, 227, 332, 554, light);
    list[i++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
    list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
    list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));
    list[i++] = new box(vec3(130, 0, 65), vec3(295, 165, 230), white);
    list[i++] = new box(vec3(265, 0, 295), vec3(430, 330, 460), white);
    return new hitable_list(list, i);
}

hitable *two_perlin_spheres() {
    texture *pertext = new noise_texture(4);
    hitable **list = new hitable*[2];
    list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(pertext));
    list[1] = new sphere(vec3(0, 2, 0), 2, new lambertian(pertext));
    return new hitable_list(list, 2);
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

int main() {
    // TODO: read the parameters from text file
    int nx = 800;
    int ny = 800;
    // number of samples
    int ns = 100;
    std::cout << "P3\n" << nx << " " << ny << "\n255\n";
    // hitable *world = random_scene();
    // hitable *world = two_perlin_spheres();
    // hitable *world = earth();
    // hitable *world = simple_light();
    hitable *world = cornell_box();
    vec3 lookfrom(278, 278, -800);
    vec3 lookat(278, 278, 0);
    float dist_to_focus = 10.0;
    float aperture = 0.0;
    float vfov = 40.0;
    camera cam(lookfrom, lookat, vec3(0, 1, 0), vfov,
               float(nx)/float(ny), aperture, dist_to_focus,
               0.0, 1.0);
    for (int j = ny - 1; j >= 0; j--) {
        for (int i = 0; i < nx; i++) {
            vec3 col(0, 0, 0);
            for (int s = 0; s < ns; s++) {
                float u = float(i + drand48()) / float(nx);
                float v = float(j + drand48()) / float(ny);
                ray r = cam.get_ray(u, v);
                col += color(r, world, 0);
            }
            col /= float(ns);
            // gamma 2
            col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
            int ir = int(255.99*col[0]);
            int ig = int(255.99*col[1]);
            int ib = int(255.99*col[2]);
            std::cout << ir << " " << ig << " " << ib << "\n";
        }
    }
}
