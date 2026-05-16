#include <iostream>
#include <fstream>
#include <cmath>

// Struktur Data untuk Vektor 3D
struct Vector3 {
    double x, y, z;
    
    Vector3 operator+(const Vector3& v) const { return {x + v.x, y + v.y, z + v.z}; }
    Vector3 operator-(const Vector3& v) const { return {x - v.x, y - v.y, z - v.z}; }
    Vector3 operator*(double s) const { return {x * s, y * s, z * s}; }
    
    double length() const { return std::sqrt(x*x + y*y + z*z); }
    Vector3 normalize() const { double l = length(); return {x/l, y/l, z/l}; }
};

int main() {
    // Dimensi Gambar Output
    const int width = 800;
    const int height = 600;
    
    // Parameter Lubang Hitam
    Vector3 bhPos = {0.0, 0.0, 0.0}; // Posisi di pusat koordinat
    const double Rs = 1.0;            // Radius Event Horizon (Schwarzschild)
    const double dt = 0.05;           // Step size untuk simulasi ray marching
    
    // Setup File Gambar (.ppm)
    std::ofstream updateFile("black_hole.ppm");
    updateFile << "P3\n" << width << " " << height << "\n255\n";
    
    // Kamera
    Vector3 camPos = {0.0, 0.0, -5.0}; // Kamera berada di depan lubang hitam
    
    // Loop untuk setiap piksel di layar
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            
            // Normalisasi koordinat layar ke range [-1, 1]
            double nx = (2.0 * x / width) - 1.0;
            double ny = 1.0 - (2.0 * y / height); // Membalik sumbu Y
            double aspect = (double)width / height;
            nx *= aspect;
            
            // Arah awal sinar dari kamera (foton bergerak maju)
            Vector3 rayPos = camPos;
            Vector3 rayDir = {nx, ny, 1.0};
            rayDir = rayDir.normalize();
            
            bool rabaBlackHole = false;
            
            // Ray Marching Simulation (Integrasi Jalur Foton)
            for (int step = 0; step < 300; ++step) {
                Vector3 toBH = bhPos - rayPos;
                double dist = toBH.length();
                
                // Jika foton masuk ke dalam Event Horizon
                if (dist <= Rs) {
                    rabaBlackHole = true;
                    break;
                }
                
                // Jika foton sudah sangat jauh, hentikan simulasi
                if (dist > 20.0) break;
                
                // Hitung defleksi gravitasi (Pendekatan Newton/Schwarzschild)
                // Percepatan g = M / r^2. Kita gunakan pendekatan arah
                Vector3 gravityDir = toBH.normalize();
                double gravityStrength = (3.0 * Rs) / (dist * dist * dist); // Efek lensa relativistik kuat di dekat r
                
                // Update arah dan posisi foton
                rayDir = rayDir + gravityDir * gravityStrength * dt;
                rayDir = rayDir.normalize();
                rayPos = rayPos + rayDir * dt;
            }
            
            // Berikan Warna Berdasarkan Hasil Simulasi Sinar
            if (rabaBlackHole) {
                // Event Horizon (Hitam Pekat)
                updateFile << "0 0 0\n";
            } else {
                // Membuat pola grid latar belakang untuk melihat efek pembelokan (Lensing)
                int gridX = static_cast<int>(std::floor(rayPos.x * 2.0));
                int gridY = static_cast<int>(std::floor(rayPos.y * 2.0));
                
                if ((gridX + gridY) % 2 == 0) {
                    updateFile << "40 40 120\n";  // Biru tua
                } else {
                    updateFile << "20 20 50\n";   // Biru sangat tua
                }
            }
        }
    }
    
    std::cout << "Render selesai! Buka file 'black_hole.ppm' untuk melihat hasilnya.\n";
    return 0;
}