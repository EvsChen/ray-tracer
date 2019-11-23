#include <iostream>
#include "float.h"
#include "camera.h"
#include "rect.h"
#include "scene.h"
#include "smartpointerhelp.h"

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
        else {
            return emitted;
        }
    }
    else {
        return vec3(0, 0, 0);
    }
}

int main() {
    // TODO: read the parameters from text file
    int nx = 800;
    int ny = 800;
    // number of samples
    int ns = 100;
    std::cout << "P3\n" << nx << " " << ny << "\n255\n";
    uPtr<Scene> scene = mkU<Scene>();
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
                col += color(r, scene->world, 0);
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
