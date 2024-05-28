#include "esp_camera.h"
#include <WiFi.h>
#include "detect_model_data.h"  // Your converted TFLite model
#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"

const char* ssid = "HUAWEI-8NBn";
const char* password = "PakistaN123";

#define TENSOR_ARENA_SIZE 70 * 1024
uint8_t tensor_arena[TENSOR_ARENA_SIZE];

void startCameraServer();

#define CAMERA_MODEL_AI_THINKER
#include "camera_pins.h"

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); // Disable brownout detector

  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_QVGA;
  config.jpeg_quality = 12;
  config.fb_count = 1;

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
}

void runInference(camera_fb_t* fb) {
  const tflite::Model* model = tflite::GetModel(detect_model_data);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    Serial.println("Model schema version mismatch");
    return;
  }

  static tflite::AllOpsResolver resolver;
  static tflite::MicroInterpreter interpreter(model, resolver, tensor_arena, TENSOR_ARENA_SIZE);
  interpreter.AllocateTensors();

  TfLiteTensor* input = interpreter.input(0);
  TfLiteTensor* output = interpreter.output(0);

  // Preprocess image to fit model input
  // Assuming model input is 300x300, resize and normalize the image

  // Run inference
  if (interpreter.Invoke() != kTfLiteOk) {
    Serial.println("Error during inference");
    return;
  }

  // Process output
  for (int i = 0; i < output->dims->data[1]; ++i) {
    float confidence = output->data.f[i * 6 + 1];
    if (confidence > 0.5) {
      int class_id = static_cast<int>(output->data.f[i * 6 + 0]);
      if (class_id == 0) {
        float x_center = output->data.f[i * 6 + 2];
        float y_center = output->data.f[i * 6 + 3];
        Serial.printf("Human detected at center: (%f, %f)\n", x_center, y_center);
      }
    }
  }
}

void loop() {
  camera_fb_t* fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }

  runInference(fb);

  esp_camera_fb_return(fb);
  delay(2000);  // Adjust delay as necessary
}