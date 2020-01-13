#include <chrono>
#include <fstream>
#include <iostream>
#include <limits>
#include <thread>
#include <vector>

#include "camera.h"
#include "core/parallel.h"
#include "float.h"
#include "pdf.h"
#include "rect.h"
#include "scene.h"
#include "smartpointerhelp.h"

vec3 color(const Ray &r, Hitable *world, Hitable *light, int depth) {
  HitRecord hrec;
  // 0.001 for avoiding t close to 0
  if (world->hit(r, 0.001, FLT_MAX, hrec)) {
    scatter_record srec;
    vec3 emitted = hrec.mat_ptr->emitted(r, hrec, hrec.u, hrec.v, hrec.p);
    if (depth < 5 && hrec.mat_ptr->scatter(r, hrec, &srec)) {
      // For specular, we don't care about the pdf distribution
      if (srec.is_specular) {
        return srec.attenuation * color(srec.specular_ray, world, light, depth + 1);
      }
      // Calculate scatter ray
      vec3 v = light->random(hrec.p);
      Ray scattered = Ray(hrec.p, v, r.time());
      float incidentPdf = light->pdf_value(hrec.p, scattered.direction());
      float scatterPdf = hrec.mat_ptr->scattering_pdf(r, hrec, scattered);
      emitted += srec.attenuation * color(scattered, world, light, depth + 1) *
                 scatterPdf / incidentPdf;
    }
    return emitted;
  }
  return vec3(0.f);
}

int main() {
  int nx = 800,  // width
      ny = 800,  // height
      ns = 100,  // number of samples
      tileSize = 16;
  std::cout << "Image size: " << nx << "x" << ny << std::endl;
  std::cout << "Samples per pixel: " << ns << std::endl;
  std::cout << "Tile size: " << tileSize << std::endl;
  raytracer::parallelInit();
  Scene scene = Scene();
  // vec3 lookfrom(0, 0, 10);
  // vec3 lookat(0, 0, -1);
  vec3 lookfrom(278, 278, -600), lookat(278, 278, 0);
  float dist_to_focus = (lookat - lookfrom).length();
  float aperture = 0.0;
  float vfov = 50.0;
  vec3 vup(0, 1, 0);
  camera cam(lookfrom, lookat, vup, vfov, float(nx) / float(ny), aperture,
             dist_to_focus, 0.0, 0.0);
  int res[nx * ny * 3];
  auto start = std::chrono::steady_clock::now();
  int yNumTiles = (ny + tileSize - 1) / tileSize;
  int xNumTiles = (nx + tileSize - 1) / tileSize;
  
  auto renderTile = [&](const Point2i &count) {
    int tileX = count.x, tileY = count.y;
    for (int x = tileX * tileSize; x < std::min((tileX + 1) * tileSize, nx);
         x++) {
      for (int y = tileY * tileSize; y < std::min((tileY + 1) * tileSize, ny);
           y++) {
        vec3 col(0.f);
        for (int s = 0; s < ns; s++) {
          float u = float(x + (float)rand() / RAND_MAX) / nx;
          float v = float(y + (float)rand() / RAND_MAX) / ny;
          Ray r = cam.get_ray(u, v);
          vec3 c = color(r, scene.world, scene.light, 0); 
          de_nan(c);
          col += c;
        }
        col /= float(ns);
        // gamma 2
        col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
        col = clamp(col, 0.f, 1.f);
        int key = 3 * ((ny - 1 - y) * nx + x);
        res[key] = round(255.f * col[0]);
        res[key + 1] = round(255.f * col[1]);
        res[key + 2] = round(255.f * col[2]);
      }
    }
  };

  raytracer::ParallelFor2d(renderTile, Point2i(xNumTiles, yNumTiles));
  // for (int i = 0; i < xNumTiles; ++i) {
  //   for (int j = 0; j < yNumTiles; ++j) {
  //     renderTile(Point2i(i, j));
  //   }
  // }
  raytracer::parallelClean();

  auto end = std::chrono::steady_clock::now();
  std::chrono::duration<double> diff = end - start;
  std::cout << "Render time: " << diff.count() << "s\n";
  std::cout << "Speed: " << std::setprecision(3) << nx * ny * ns / diff.count()
            << " rays per second" << std::endl;
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
  return 0;
}