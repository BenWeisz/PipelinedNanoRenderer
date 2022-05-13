#include <iostream>
#include <Eigen/Dense>

#include "timage.h"
#include "model.h"

int main() {
    TImage image(500, 500);
    image.write("./out/test.tga");

    Model model("../models/african_head.obj");

    return 0;
}