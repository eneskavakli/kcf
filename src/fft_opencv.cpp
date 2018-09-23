#include "fft_opencv.h"

void FftOpencv::init(unsigned width, unsigned height, unsigned num_of_feats, unsigned num_of_scales)
{
    Fft::init(width, height, num_of_feats, num_of_scales);
    std::cout << "FFT: OpenCV" << std::endl;
}

void FftOpencv::set_window(const MatDynMem &window)
{
    m_window = window;
}

void FftOpencv::forward(const MatScales &real_input, ComplexMat &complex_result)
{
    Fft::forward(real_input, complex_result);

    cv::Mat tmp;
    cv::dft(real_input.plane(0), tmp, cv::DFT_COMPLEX_OUTPUT);
    complex_result = ComplexMat(tmp);
}

void FftOpencv::forward_window(MatScaleFeats &feat, ComplexMat &complex_result, MatScaleFeats &temp)
{
    Fft::forward_window(feat, complex_result, temp);

    for (uint i = 0; i < uint(feat.size[0]); ++i) {
        for (uint j = 0; j < uint(feat.size[1]); ++j) {
            cv::Mat complex_res;
            cv::Mat channel = feat.plane(i, j);
            cv::dft(channel.mul(m_window), complex_res, cv::DFT_COMPLEX_OUTPUT);
            complex_result.set_channel(int(j), complex_res);
        }
    }
}

void FftOpencv::inverse(ComplexMat &  complex_input, MatScales & real_result)
{
    Fft::inverse(complex_input, real_result);

    if (complex_input.n_channels == 1) {
        cv::dft(complex_input.to_cv_mat(), real_result.plane(0), cv::DFT_INVERSE | cv::DFT_REAL_OUTPUT | cv::DFT_SCALE);
    } else {
        std::vector<cv::Mat> mat_channels = complex_input.to_cv_mat_vector();
        std::vector<cv::Mat> ifft_mats(ulong(complex_input.n_channels));
        for (uint i = 0; i < uint(complex_input.n_channels); ++i) {
            cv::dft(mat_channels[i], ifft_mats[i], cv::DFT_INVERSE | cv::DFT_REAL_OUTPUT | cv::DFT_SCALE);
        }
        cv::merge(ifft_mats, real_result);
    }
}

FftOpencv::~FftOpencv() {}
