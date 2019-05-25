/* Copyright 2018 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/
#include <cstdio>
#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/model.h"
#include "tensorflow/lite/optional_debug_tools.h"
#include <opencv2/opencv.hpp>

// This is an example that is minimal to read a model
// from disk and perform inference. There is no data being loaded
// that is up to you to add as a user.
//
// NOTE: Do not add any dependencies to this that cannot be built with
// the minimal makefile. This example must remain trivial to build with
// the minimal build tool.
//
// Usage: minimal <tflite model>

using namespace tflite;
using namespace std;
using namespace cv;

#define TFLITE_MINIMAL_CHECK(x)                              \
  if (!(x)) {                                                \
    fprintf(stderr, "Error at %s:%d\n", __FILE__, __LINE__); \
    exit(1);                                                 \
  }

int main(int argc, char *argv[]) {

    if (argc != 2) {
        fprintf(stderr, "minimal <tflite model>\n");
        return 1;
    }

    const char *filename = argv[1];
    Mat img = imread("files/stop.jpg", IMREAD_COLOR);
    Mat resized;
    resize(img, resized, Size(55, 100));
    Mat flot;
    resized.convertTo(flot, CV_32F);
    flot = flot / 255.0;
    int h = resized.rows;
    int w = resized.cols;
    cout << "height: " << h << endl;
    cout << "weight: " << w << endl;
    cout << "channels: " << resized.channels() << endl;
    cout << "type: " << resized.type() << endl;
    float input_image[100][55][3];
//    cout << "vector_size: " << input_image << endl;
//    cout << input_image[0] << " " << input_image[25] << " " << input_image[55] << endl;
//    namedWindow("hello", WINDOW_GUI_NORMAL);
//    imshow("hello", img);
//    waitKey(0);

    // Load model
    std::unique_ptr<tflite::FlatBufferModel> model =
            tflite::FlatBufferModel::BuildFromFile(filename);
    TFLITE_MINIMAL_CHECK(model != nullptr);

    // Build the interpreter
    tflite::ops::builtin::BuiltinOpResolver resolver;
    InterpreterBuilder builder(*model, resolver);
    std::unique_ptr<Interpreter> interpreter;
    builder(&interpreter);
    TFLITE_MINIMAL_CHECK(interpreter != nullptr)
    vector<int> dimensions = {1, 100, 55, 3};
    interpreter->ResizeInputTensor(interpreter->inputs()[0], dimensions);
    // Allocate tensor buffers.
    TFLITE_MINIMAL_CHECK(interpreter->AllocateTensors() == kTfLiteOk)

    // Fill input buffers
    auto ***input = interpreter->typed_input_tensor<float**>(0);
    for (int j = 0; j < 100; j++) {
        for (int i = 0; i < 55; i++) {
            for (int k = 0; k < 3; k++) {
                input[j][i][k] = flot.at<float>(j*i*k);
            }
        }
    }

    // Run inference
    TFLITE_MINIMAL_CHECK(interpreter->Invoke() == kTfLiteOk)

    // Read output buffers
    auto *output = interpreter->typed_output_tensor<float>(0);
    cout << "answers:"<<endl;
    for (int i = 0; i < 8; ++i) {
        cout << output[i] << endl;
    }
    return 0;
}