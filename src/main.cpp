#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <thread>

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
        if (depth < 5 && rec.mat_ptr -> scatter(r, rec, attenuation, scattered)) {
            return emitted + attenuation * color(scattered, world, depth + 1);
        }
        return emitted;
    }
    return vec3(0, 0, 0);
}

int main() {
    int nx = 800;
    int ny = 800;
    // number of samples
    int ns = 100;
    int tileSize = 16;
    std::cout << "Image size: " << nx << "x" << ny << std::endl;
    std::cout << "Samples per pixel: " << ns << std::endl;
    std::cout << "Tile size: " << tileSize << std::endl;
    uPtr<Scene> scene = mkU<Scene>();
    vec3 lookfrom(278, 278, -800);
    vec3 lookat(278, 278, 0);
    // vec3 lookfrom(0, 0, 10);
    // vec3 lookat(0, 2, 0);
    float dist_to_focus = 10.0;
    float aperture = 0.0;
    float vfov = 50.0;
    vec3 vup(0, 1, 0);
    camera cam(lookfrom, lookat, vup, vfov,
               float(nx)/float(ny), aperture, dist_to_focus,
               0.0, 1.0);
    int res[nx * ny * 3];
    auto start = std::chrono::steady_clock::now();
    std::vector<std::thread> workers;
    int yNumTiles = (ny + tileSize - 1) / tileSize;
    int xNumTiles = (nx + tileSize - 1) / tileSize;

    auto renderTile = [&](int tileX, int tileY) {
      for (int x = tileX * tileSize; x < std::min((tileX + 1) * tileSize, nx); x++) {
        for (int y = tileY * tileSize; y < std::min((tileY + 1) * tileSize, ny); y++) {
          vec3 col(0, 0, 0);
          for (int s = 0; s < ns; s++) {
              float u = float(x + drand48()) / float(nx);
              float v = float(y + drand48()) / float(ny);
              ray r = cam.get_ray(u, v);
              col += color(r, scene->world, 0);
          }
          col /= float(ns);
          // gamma 2
          col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
          int key = 3 * ((ny - 1 - y) * nx + x);
          res[key] = int(255.99 * col[0]);
          res[key + 1] = int(255.99 * col[1]);
          res[key + 2] = int(255.99 * col[2]);
        }
      }
    };
    
    for (int j = 0; j < yNumTiles; j++) {
        for (int i = 0; i < xNumTiles; i++) {
            #ifdef MULTI_THREAD
              workers.push_back(renderTile, i, j));
            #else
              renderTile(i, j);
            #endif
        }
    }

#ifdef MULTI_THREAD
    for (auto &worker : workers) {
      worker.join();
    }
#endif

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
