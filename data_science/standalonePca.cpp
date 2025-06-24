#include <stdexcept>
#include <vector>
#include <cstring>
#include <Eigen/Dense>
#include <Eigen/SVD>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

namespace py = pybind11;

static Eigen::MatrixXf _pcaFitTransform(Eigen::MatrixXf& data) {
    // Center the data
    Eigen::VectorXf mean = data.colwise().mean();
    Eigen::MatrixXf centered = data.rowwise() - mean.transpose();

    // Compute full SVD
    Eigen::BDCSVD<Eigen::MatrixXf> svd(centered, Eigen::ComputeThinU | Eigen::ComputeThinV);

    // Determine how many components to keep
    int k = std::min(data.rows(), data.cols());

    // Get the top-k principal components
    Eigen::MatrixXf components = svd.matrixV().leftCols(k);

    // Optional: flip signs for consistency with sklearn
    for (int i = 0; i < components.cols(); ++i) {
        if (components(0, i) < 0)
            components.col(i) *= -1;
    }

    // Project data
    Eigen::MatrixXf transformed = centered * components;

    return transformed;
}


static py::array_t<float> pcaFitTransform(py::array_t<float> input) {
    py::buffer_info buf = input.request();
    if (buf.ndim != 2) {
        throw std::runtime_error("should be a 2d array");
    }

    int rows = buf.shape[0];
    int cols = buf.shape[1];

    float* ptr = static_cast<float*>(buf.ptr);
    Eigen::Map<Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>> data_map(ptr, rows, cols);
    Eigen::MatrixXf data = data_map;

    Eigen::MatrixXf result = _pcaFitTransform(data);

    // back to np array
    auto output = py::array_t<float>(
        {result.rows(), result.cols()},
        {sizeof(float), sizeof(float) * result.rows()}
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
