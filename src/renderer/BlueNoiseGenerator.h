#pragma once
#include <vector>
#include <memory>
#include <future>
#include <random>

class Texture;

struct VoidClusterParams {
    int width = 128;
    int height = 128;
    int maxIterations = 500;
    float sigma = 1.5f;           
    float density = 0.1f;        
    bool medievalWeighting = true; 
};

class BlueNoiseGenerator {
public:
    BlueNoiseGenerator();
    ~BlueNoiseGenerator();
    
 
    std::shared_ptr<Texture> GenerateBlueNoise(int size = 128);
    

    std::vector<std::shared_ptr<Texture>> GenerateSpatiotemporalSequence(
        int width, int height, int frames = 64);
    

    std::shared_ptr<Texture> GenerateVoidClusterNoise(const VoidClusterParams& params);
    

    std::shared_ptr<Texture> GenerateMedievalPattern(int size);
    
private:
    std::mt19937 m_randomEngine;
    
    // Core algorithms
    std::vector<float> GenerateInitialPoints(int width, int height, float density);
    void OptimizeVoidCluster(std::vector<float>& points, int width, int height, 
                           float sigma, int iterations);
    
    // Medieval-specific optimizations
    void ApplyMedievalBias(std::vector<float>& points, int width, int height);
    std::vector<float> GenerateMedievalWeights(int width, int height);
    
    // Utility functions
    void ApplyGaussianFilter(const std::vector<float>& input, 
                           std::vector<float>& output,
                           int width, int height, float sigma);
    float CalculateEnergy(const std::vector<float>& points, int width, int height);
};

