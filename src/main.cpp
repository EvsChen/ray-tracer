#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>

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
            emitted += attenuation * color(scattered, world, depth + 1);
        }
        return clamp(emitted, 0.f, 1.f);
    }
    return vec3(0, 0, 0);
}

int main() {
    int nx = 800;
    int ny = 800;
    // number of samples
    int ns = 100;
    std::cout << "Image size: " << nx << "x" << ny << std::endl;
    std::cout << "Samples per pixel: " << ns << std::endl;
    uPtr<Scene> scene = mkU<Scene>();
    // vec3 lookfrom(0, 0, 10);
    // vec3 lookat(0, 0, -1);
    vec3 lookfrom(278, 278, -800);
    vec3 lookat(278, 278, 0);
    float dist_to_focus = (lookat - lookfrom).length();
    float aperture = 0.0;
    float vfov = 40.0;
    camera cam(lookfrom, lookat, vec3(0, 1, 0), vfov,
               float(nx)/float(ny), aperture, dist_to_focus,
               0.0, 0.0);
    int res[nx * ny * 3];
    auto start = std::chrono::steady_clock::now();
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
            int key = 3 * ((ny - 1 - j) * nx + i);
            res[key] = int(255.99 * col[0]);
            res[key + 1] = int(255.99 * col[1]);
            res[key + 2] = int(255.99 * col[2]);
        }
    }
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> diff = end - start;
    std::cout << "Render time: " << diff.count() << "s\n";
    std::cout << "Speed: " << std::setprecision(3) << nx * ny * ns / diff.count() << " rays per second" << std::endl; 
    std::ofstream outFile;
    outFile.open("img.ppm");
    if (outFile.is_open()) {
      outFile << "P3\n" << nx << " " << ny << "\n255\n";
      for (int i = 0; i < nx * ny * 3; i += 3) {
        outFile << res[i] << " " << res[i + 1] << " " << res[i + 2] << std::endl;
      }
      outFile.close();
    } else {
      std::cout << "Open file failed" << std::endl;
    }
}
