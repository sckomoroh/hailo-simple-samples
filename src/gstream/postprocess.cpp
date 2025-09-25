#include "postprocess.h"

#include <iostream>
#include <map>

#include "common/labels/coco_ninety.hpp"
#include "common/labels/coco_visdrone.hpp"
#include "common/nms.hpp"
#include "common/structures.hpp"
#include "hailo/hailort.h"
#include "hailo_objects.hpp"
// #include "hailo_detection.hpp"

const std::map<int, std::string> kCoco80 = {
    {0,  "person"},
    {1,  "bicycle"},
    {2,  "car"},
    {3,  "motorcycle"},
    {4,  "airplane"},
    {5,  "bus"},
    {6,  "train"},
    {7,  "truck"},
    {8,  "boat"},
    {9,  "traffic light"},
    {10, "fire hydrant"},
    {11, "stop sign"},
    {12, "parking meter"},
    {13, "bench"},
    {14, "bird"},
    {15, "cat"},
    {16, "dog"},
    {17, "horse"},
    {18, "sheep"},
    {19, "cow"},
    {20, "elephant"},
    {21, "bear"},
    {22, "zebra"},
    {23, "giraffe"},
    {24, "backpack"},
    {25, "umbrella"},
    {26, "handbag"},
    {27, "tie"},
    {28, "suitcase"},
    {29, "frisbee"},
    {30, "skis"},
    {31, "snowboard"},
    {32, "sports ball"},
    {33, "kite"},
    {34, "baseball bat"},
    {35, "baseball glove"},
    {36, "skateboard"},
    {37, "surfboard"},
    {38, "tennis racket"},
    {39, "bottle"},
    {40, "wine glass"},
    {41, "cup"},
    {42, "fork"},
    {43, "knife"},
    {44, "spoon"},
    {45, "bowl"},
    {46, "banana"},
    {47, "apple"},
    {48, "sandwich"},
    {49, "orange"},
    {50, "broccoli"},
    {51, "carrot"},
    {52, "hot dog"},
    {53, "pizza"},
    {54, "donut"},
    {55, "cake"},
    {56, "chair"},
    {57, "couch"},
    {58, "potted plant"},
    {59, "bed"},
    {60, "dining table"},
    {61, "toilet"},
    {62, "tv"},
    {63, "laptop"},
    {64, "mouse"},
    {65, "remote"},
    {66, "keyboard"},
    {67, "cell phone"},
    {68, "microwave"},
    {69, "oven"},
    {70, "toaster"},
    {71, "sink"},
    {72, "refrigerator"},
    {73, "book"},
    {74, "clock"},
    {75, "vase"},
    {76, "scissors"},
    {77, "teddy bear"},
    {78, "hair drier"},
    {79, "toothbrush"},
};

void detection_postprocess(HailoROIPtr roi) {
    std::vector<HailoTensorPtr> tensors = roi->get_tensors();

    std::vector<HailoDetection> detections;
    if (tensors.empty() == false) {
        auto tensor = roi->get_tensor("yolov8m/yolov8_nms_postprocess");
        uint8_t* buffer = tensor->data();
        int buffer_offset = 0;
        for (int class_id = 0; class_id < 80; class_id++) {
            float32_t bbox_count = 0;
            memcpy(&bbox_count, buffer + buffer_offset, sizeof(bbox_count));

            buffer_offset += sizeof(bbox_count);
            for (auto bbox_index = 0; bbox_index < bbox_count; bbox_index++) {
                common::hailo_bbox_float32_t* bbox_struct = (common::hailo_bbox_float32_t*)(&buffer[buffer_offset]);
                float32_t w, h = 0.0f;
                w = bbox_struct->x_max - bbox_struct->x_min;
                h = bbox_struct->y_max - bbox_struct->y_min;
                HailoBBox bbox = HailoBBox(bbox_struct->x_min, bbox_struct->y_min, w, h);
                auto label = kCoco80.at(class_id);
                HailoDetection det = HailoDetection(bbox, class_id, label, bbox_struct->score);
                detections.push_back(det);
                buffer_offset += sizeof(common::hailo_bbox_float32_t);
            }
        }
    }

    hailo_common::add_detections(roi, detections);
}