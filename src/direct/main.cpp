#include <hailo/hailort.hpp>
#include <iostream>
#include <string>
#include <vector>

using namespace hailort;

#define HEF_FILE ("/home/dev/Documents/sources/test-hailort/model/yolov8m.hef")

int main() {
    auto vdevice = VDevice::create().expect("Failed create vdevice");
    std::cout << "VDevice created" << std::endl;

    auto infer_model = vdevice->create_infer_model(HEF_FILE).expect("Failed to create infer model");
    std::cout << "InferModel created" << std::endl;

    auto configured_infer_model = infer_model->configure().expect("Failed to create configured infer model");
    std::cout << "ConfiguredInferModel created" << std::endl;
}
