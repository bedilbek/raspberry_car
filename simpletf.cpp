//
// Created by bedilbek on 5/19/19.
//

// Copyright 2015 Google Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <pthread.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <queue>
#include <sstream>
#include <string>
#include <chrono>
#include <vector>

#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/model.h"

#include <opencv2/opencv.hpp>

using namespace tflite;
using namespace cv;
using namespace std;

#if 1
#define LOG(x) std::cerr

// Returns the top N confidence values over threshold in the provided vector,
// sorted by confidence in descending order.
static void GetTopN(const float* prediction, const int prediction_size, const int num_results,
                    const float threshold, std::vector<std::pair<float, int> >* top_results) {
    // Will contain top N results in ascending order.
    std::priority_queue<std::pair<float, int>, std::vector<std::pair<float, int> >,
            std::greater<std::pair<float, int> > >
            top_result_pq;

    const long count = prediction_size;
    for (int i = 0; i < count; ++i) {
        const float value = prediction[i];

        // Only add it if it beats the threshold and has a chance at being in
        // the top N.
        if (value < threshold) {
            continue;
        }

        top_result_pq.push(std::pair<float, int>(value, i));

        // If at capacity, kick the smallest value out.
        if (top_result_pq.size() > num_results) {
            top_result_pq.pop();
        }
    }

    // Copy to output vector and reverse into descending order.
    while (!top_result_pq.empty()) {
        top_results->push_back(top_result_pq.top());
        top_result_pq.pop();
    }
    std::reverse(top_results->begin(), top_results->end());
}

std::string RunInferenceOnImage(char * graph, char * labels, char * image_path) {
    const int num_threads = 1;

    std::unique_ptr<tflite::FlatBufferModel> model(
            tflite::FlatBufferModel::BuildFromFile(graph));
    if (!model) {
        LOG(FATAL) << "Failed to mmap model " << graph << std::endl;
    }
    LOG(INFO) << "Loaded model " << graph << std::endl;
    model->error_reporter();
    LOG(INFO) << "resolved reporter" << std::endl;

#ifdef TFLITE_CUSTOM_OPS_HEADER
    tflite::MutableOpResolver resolver;
  RegisterSelectedOps(&resolver);
#else
    tflite::ops::builtin::BuiltinOpResolver resolver;
#endif

    std::unique_ptr<tflite::Interpreter> interpreter;
    tflite::InterpreterBuilder(*model, resolver)(&interpreter);
    if (!interpreter) {
        LOG(FATAL) << "Failed to construct interpreter";
    }

    if (num_threads != -1) {
        interpreter->SetNumThreads(num_threads);
    }

    if (interpreter->AllocateTensors() != kTfLiteOk) {
        LOG(FATAL) << "Failed to allocate tensors!";
    }


    LOG(INFO) << "Getting the input tensor" << std::endl;
    int input = interpreter->inputs()[0];

    TfLiteIntArray* dims = interpreter->tensor(input)->dims;

    const int wanted_width = dims->data[2];
    const int wanted_height = dims->data[1];
    const int wanted_channels = dims->data[3];
    const int wanted_type = interpreter->tensor(input)->type;


    // Read the label list
    std::vector<std::string> label_strings;
    std::ifstream t;
    t.open(labels);
    std::string line;
    while (t) {
        std::getline(t, line);
        label_strings.push_back(line);
    }
    t.close();

    // Load the image
    auto img = imread(image_path);

    LOG(INFO) << "input image size: " << img.cols << "x" << img.rows << "x" << img.channels() << std::endl;

    Mat resized;
    resize(img, resized, Size(wanted_width, wanted_height), INTER_CUBIC);
    resized.convertTo(resized, CV_32FC3);
    resized /= 255.0;
    vector<float> array;

    if (resized.isContinuous()) {
        array.assign((float*)resized.datastart, (float*)resized.dataend);
    } else {
        for (int i = 0; i < resized.rows; ++i) {
            array.insert(array.end(), resized.ptr<float>(i), resized.ptr<float>(i)+resized.cols);
        }
    }

    LOG(INFO) << "output image size: " << resized.cols << "x" << resized.rows << "x" << resized.channels() << std::endl;

//    namedWindow("hello", WINDOW_KEEPRATIO);
//    imshow("hello", resized);
//    waitKey(0);

    const float input_std = 255.0f;

    LOG(INFO) << "Requesting output node" << std::endl;
    float *in = interpreter->typed_input_tensor<float>(input);
    if (in == nullptr)
    {
        LOG(INFO) << "output node is null" << std::endl;
    }

    in = array.data();

    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

    if (interpreter->Invoke() != kTfLiteOk) {
        LOG(FATAL) << "Failed to invoke!";
    }

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "TFLite took "
              << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()
              << "us.\n";

    cout << "3" <<endl;

    int output = interpreter->outputs()[0];
    TfLiteIntArray* output_dims = interpreter->tensor(output)->dims;
    auto output_size = output_dims->data[output_dims->size - 1];
    int output_type = interpreter->tensor(output)->type;

    std::vector<std::pair<float, int>> results;

    float *scores = interpreter->typed_output_tensor<float>(0);
    for (int i = 0; i < output_size; ++i) {
        float value = scores[i];
        results.push_back(std::pair<float, int>(value, i));
    }


    for (const auto& result : results) {
        cout << result.first << ": " << label_strings[result.second] << endl;
    }

    return "s";
}
#endif

int main(int argc, char * argv[])
{
    if (argc != 4)
    {
        std::cout << "usage: test graph.lite labels.txt image.jpg" << std::endl;
        return -1;
    }
    std::cout << "Using graph " << argv[1];
    std::cout << " with labels " << argv[2];
    std::cout << " and image " << argv[3] << std::endl;
    RunInferenceOnImage(argv[1], argv[2], argv[3]);
}