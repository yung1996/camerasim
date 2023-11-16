#include <iostream>
#include <cmath>
#include <opencv2/opencv.hpp>

int main() {
    cv::Mat image = cv::imread("../test_image.png");
    int height = image.rows / 3;
    int width = image.cols / 4;
    float c_x_p = width / 2.0f;
    float c_y_p = height / 2.0f;
    cv::Mat fisheye = cv::Mat::zeros(cv::Size{width, height}, CV_8UC3);
    std::vector<cv::Mat> cube_maps;
    cube_maps.emplace_back(image(cv::Rect(0*width, 1*height, width, height))); // left
    cube_maps.emplace_back(image(cv::Rect(1*width, 1*height, width, height))); // front
    cube_maps.emplace_back(image(cv::Rect(2*width, 1*height, width, height))); // right
    cube_maps.emplace_back(image(cv::Rect(3*width, 1*height, width, height))); // back
    cube_maps.emplace_back(image(cv::Rect(1*width, 0*height, width, height))); // up
    cube_maps.emplace_back(image(cv::Rect(1*width, 2*height, width, height))); // bottom

    // for (int i = 0; i < cube_maps.size(); ++i) {
    //     cv::imshow("Image", cube_maps[i]);
    //     cv::waitKey(0);
    // }

    for (int iy = 0; iy < fisheye.rows; iy++) {
        for (int ix = 0; ix < fisheye.cols; ix ++) {
            // step 1: pixel -> world coord
            float x = ((ix - c_x_p)/static_cast<float>(width))*2.0f;
            float y = ((iy - c_y_p)/static_cast<float>(height))*2.0f;
            float r = std::sqrt((x*x) + (y*y));
            if (r > 1.0f) {
                continue;
            }
            // step 2: get incident ray
            float z = std::sqrt(1 - (x*x) - (y*y));

            // step 3: find max axis
            std::vector<float> axis{x, y, z};
            float max_value = -1.0;
            float max_idx = -1;
            for (int i = 0; i < axis.size(); ++i) {
                if (std::abs(axis[i]) > max_value) {
                    max_value = std::abs(axis[i]);
                    max_idx = i;
                }
            }
            float ma = -1.0;
            float sc = -1.0;
            float tc = -1.0;
            int cube_idx = -1;
            // step 4: find ma
            if (max_idx == 0 && axis[max_idx] >= 0) { // +rx
                sc = -z;
                tc = -y;
                ma = std::abs(x);
                cube_idx = 2; // right         
            } else if (max_idx == 0 && axis[max_idx] < 0) { // -rx
                sc = z;
                tc = -y;
                ma = std::abs(x);
                cube_idx = 0; // left
            } else if (max_idx == 1 && axis[max_idx] >= 0) { // +ry
                sc = x;
                tc = z;
                ma = std::abs(y); 
                cube_idx = 5; // bottom 
            } else if (max_idx == 1 && axis[max_idx] < 0) { // -ry
                sc = x;
                tc = -z;
                ma = std::abs(y);
                cube_idx = 4; // up
            } else if (max_idx == 2 && axis[max_idx] >= 0) { // +rz
                sc = x;
                tc = -y;
                ma = std::abs(z);
                cube_idx = 1; // front
            } else if (max_idx == 2 && axis[max_idx] < 0) { // -rz
                sc = -x;
                tc = -y;
                ma = std::abs(z);
                cube_idx = 3; // back
            }

            // step 5: find pixel
            float s = (sc/ma + 1)/2.0f;
            float t = (tc/ma + 1)/2.0f;

            int u = static_cast<int>(s*static_cast<float>(width));
            int v = static_cast<int>((1.0f-t)*static_cast<float>(height));

            fisheye.at<cv::Vec3b>(iy, ix)[0] = cube_maps[cube_idx].at<cv::Vec3b>(v, u)[0];
            fisheye.at<cv::Vec3b>(iy, ix)[1] = cube_maps[cube_idx].at<cv::Vec3b>(v, u)[1];
            fisheye.at<cv::Vec3b>(iy, ix)[2] = cube_maps[cube_idx].at<cv::Vec3b>(v, u)[2];
        }
    }
    cv::imshow("Fisheye Image", fisheye);
    cv::imwrite("./fisheye.png", fisheye);
    cv::waitKey(0);

    return 0;
}