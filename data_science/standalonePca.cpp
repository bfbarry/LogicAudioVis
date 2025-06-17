#include <stdexcept>
#include <vector>
#include <cstring>
#include <Eigen/Dense>
#include <Eigen/SVD>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

namespace py = pybind11;

static Eigen::MatrixXf _pcaFitTransform(Eigen::MatrixXf& data) { 
    //center data
    Eigen::VectorXf mean = data.colwise().mean();
    Eigen::MatrixXf centered = data.rowwise() - mean.transpose();

    // perform SVD
    Eigen::BDCSVD<Eigen::MatrixXf> svd(centered, Eigen::ComputeThinU | Eigen::ComputeThinV);

    // right SVs == PCs
    Eigen::MatrixXf components = svd.matrixV();

    // SV^2 == eigenvals
    Eigen::VectorXf explained_variance = svd.singularValues().array().square() / (data.rows() - 1);

    // project
    Eigen::MatrixXf window_pca = centered * components;

    return window_pca;
}


static py::array_t<float> pcaFitTransform(py::array_t<float> input) {
    py::buffer_info buf = input.request();
    if (buf.ndim != 2) {
        throw std::runtime_error("should be a 2d array");
    }

    int rows = buf.shape[0];
    int cols = buf.shape[1];

    float* ptr = static_cast<float*>(buf.ptr);
    Eigen::Map<Eigen::MatrixXf> data_map(ptr, rows, cols);
    Eigen::MatrixXf data = data_map;

    Eigen::MatrixXf result = _pcaFitTransform(data);

    // back to np array
    auto output = py::array_t<float>(
        {result.rows(), result.cols()},
        {sizeof(float) * result.cols(), sizeof(float)}
    );

    //copy data into output's pointer
    py::buffer_info out_buf = output.request();
    float* output_ptr = static_cast<float*>(out_buf.ptr);
    std::memcpy(output_ptr, result.data(), result.size() * sizeof(float));

    return output;
}

PYBIND11_MODULE(standalonePca, m) {
    m.doc() = "PCA implementation using Eigen";
    m.def("fit_transform", &pcaFitTransform, "Perform PCA fit and transform");
}
