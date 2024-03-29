﻿/*
Copyright (c) 2016, oasi-adamay
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

* Neither the name of glsCV nor the names of its
contributors may be used to endorse or promote products derived from
this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*!
@file	convolutionalNeuralNetwork.cpp
@brief	convolutional neural network
@author	oasi-adamay	
 */

#include "convolutionalNeuralNetwork.hpp"

namespace CNN {
#if 0		// for single channels
void ReLU_accumlate_filter2D(
	cv::Mat &inputPlanes,				// input [planes][rows][cols]
	cv::Mat &outputPlane,				// output [rows][cols]
	cv::Mat &kernels,					// kernels [planes][ksize][ksize]
	const float bias					// bias
	)
{
	CV_Assert(inputPlanes.dims == 3);
	CV_Assert(kernels.dims == 3);
	CV_Assert(outputPlane.dims == 2);
	CV_Assert(inputPlanes.size[0] == kernels.size[0]);


	cv::Size ipSize = cv::Size(inputPlanes.size[2], inputPlanes.size[1]);	//input image size
	cv::Size kSize = cv::Size(kernels.size[2], kernels.size[1]);			//kernel size
	cv::Mat uIntermediatePlane = cv::Mat::zeros(ipSize, CV_32FC1);			//intermediate

	//2D-convolution and accumlate
	// output = Σ (input(k) * kernel(k))
	for (int ipIndex = 0; ipIndex < inputPlanes.size[0]; ipIndex++) {
		cv::Mat inputPlane = cv::Mat(ipSize, CV_32FC1, inputPlanes.ptr<float>(ipIndex));
		cv::Mat kernel = cv::Mat(kSize, CV_32FC1, kernels.ptr<float>(ipIndex));
		cv::Mat filterOutput;
//		cv::filter2D(inputPlane, filterOutput, -1, kernel, cv::Point(-1, -1), 0.0, cv::BORDER_REPLICATE);
		cv::filter2D(inputPlane, filterOutput, -1, kernel, cv::Point(-1, -1), 0.0, cv::BORDER_CONSTANT);	//compatibility for gls::convolutionalNeuralNetwork
		
		//		cv::add(uIntermediatePlane, filterOutput, uIntermediatePlane);
		cv::accumulate(filterOutput, uIntermediatePlane);

		std::cout << "i:" << ipIndex << std::endl;
		std::cout << kernel << std::endl;
	}

	//bias
	cv::add(uIntermediatePlane, bias, uIntermediatePlane);

#if 1
	//activation function (ReLU - Rectified Linear Unit)
	uIntermediatePlane = cv::max(uIntermediatePlane*0.1, uIntermediatePlane);
#else
	cv::Mat moreThanZero = cv::Mat(ipSize, CV_32FC1, 0.0);
	cv::Mat lessThanZero = cv::Mat(ipSize, CV_32FC1, 0.0);
	cv::max(uIntermediatePlane, 0.0, moreThanZero);
	cv::min(uIntermediatePlane, 0.0, lessThanZero);
	cv::scaleAdd(lessThanZero, 0.1, moreThanZero, uIntermediatePlane);
#endif

	uIntermediatePlane.copyTo(outputPlane);

}

void convolutionalNeuralNetwork(
	cv::Mat &inputPlanes,				///! input [planes][rows][cols]
	cv::Mat &outputPlanes,				///! output [planes][rows][cols]
	cv::Mat &weights,					///! kernels [inputPlanes*outputPlanes][ksize][ksize]
	std::vector<double>& biases			///! bias [outputPlanes]
	)
{
	CV_Assert(inputPlanes.dims == 3);
	CV_Assert(outputPlanes.dims == 3);
	CV_Assert(weights.dims == 3);
	CV_Assert(inputPlanes.size[0] * outputPlanes.size[0] == weights.size[0]);

	cv::Size ipSize = cv::Size(inputPlanes.size[2], inputPlanes.size[1]);
	cv::Size kSize = Size(weights.size[2], weights.size[1]);


#ifdef _OPENMP
#pragma omp parallel for
#endif
	for (int opIndex = 0; opIndex < outputPlanes.size[0]; opIndex++) {
		int _sz[3] = { inputPlanes.size[0], kSize.height, kSize.width };
		cv::Mat kernels = cv::Mat(3, _sz, CV_32FC1, weights.ptr<float>(inputPlanes.size[0] * opIndex));
		cv::Mat outputPlane = cv::Mat(ipSize, CV_32FC1, outputPlanes.ptr<float>(opIndex));
		ReLU_accumlate_filter2D(inputPlanes, outputPlane, kernels, (float)biases[opIndex]);
	}

}

#else
template <typename T>
static void pack(Mat& src, Mat& dst, int channels = 4){
	CV_Assert(src.dims == 3);
	CV_Assert(src.channels() == 1);

	const int srcPlanes = src.size[0];
	const Size srcSize(src.size[2], src.size[1]);
	const int srcChannels = src.channels();

	//		const int dstChannels = channels;
	const int dstChannels = srcPlanes / channels > 0 ? channels : srcChannels;
	const int dstPlanes = srcPlanes / dstChannels;
	const Size dstSize = srcSize;

	CV_Assert(srcPlanes % dstChannels == 0);

	const int sizes[3] = { dstPlanes, dstSize.height, dstSize.width };

	dst = Mat(3, sizes, CV_MAKETYPE(src.depth(), dstChannels));

	for (int i = 0; i < src.size[0]; i++){
		for (int j = 0; j < src.size[1]; j++){
			for (int k = 0; k < src.size[2]; k++){
				for (int cn = 0; cn < srcChannels; cn++){
					int _i = i / dstChannels;
					int _j = j;
					int _k = k;
					int _cn = i % dstChannels;
					*(dst.ptr<T>(_i, _j, _k) + _cn) = *(src.ptr<T>(i, j, k) + cn);
				}
			}
		}
	}
}


void ReLU_accumlate_filter2D(
	cv::Mat &inputPlanes,				// input [ip][rows][cols](ich)
	cv::Mat &outputPlane,				// output [rows][cols](och)
	cv::Mat &kernels,					// kernels [ip*och][ksize][ksize](ch)
	const double* bias					// bias
	)
{
	CV_Assert(inputPlanes.dims == 3);
	CV_Assert(kernels.dims == 3);
	CV_Assert(outputPlane.dims == 2);
	CV_Assert(inputPlanes.size[0] * outputPlane.channels() == kernels.size[0]);
	CV_Assert(inputPlanes.channels() == kernels.channels());

	int ipChannels = inputPlanes.channels();
	int opChannels = outputPlane.channels();
//	CV_Assert(ipChannels == opChannels);


	cv::Size ipSize = cv::Size(inputPlanes.size[2], inputPlanes.size[1]);	//input image size
	cv::Size opSize = ipSize;	//output image size
	cv::Size kSize = cv::Size(kernels.size[2], kernels.size[1]);			//kernel size
	std::vector<cv::Mat> outputSplit(opChannels, cv::Mat::zeros(opSize, outputPlane.depth()));

	//2D-convolution and accumlate
	// output = Σ (input(k) * kernel(k))
	for (int opCh = 0; opCh < opChannels; opCh++) {
		cv::Mat accm = Mat::zeros(opSize, outputPlane.depth());

		for (int ipIndex = 0; ipIndex < inputPlanes.size[0]; ipIndex++) {
			cv::Mat inputPack = cv::Mat(ipSize, inputPlanes.type(), inputPlanes.ptr<float>(ipIndex));
			cv::Mat kernelPack = cv::Mat(kSize, kernels.type(), kernels.ptr<float>(inputPlanes.size[0] * opCh + ipIndex));

			std::vector<cv::Mat> inputSplit(ipChannels);
			cv::split(inputPack, inputSplit);
			std::vector<cv::Mat> kernelSplit(ipChannels);
			cv::split(kernelPack, kernelSplit);

			for (int ipCh = 0; ipCh < ipChannels; ipCh++) {
				cv::Mat input = inputSplit[ipCh];
				cv::Mat kernel = kernelSplit[ipCh];
				cv::Mat filterOutput;
				cv::filter2D(input, filterOutput, -1, kernel, cv::Point(-1, -1), 0.0, cv::BORDER_CONSTANT);	//compatibility for gls::convolutionalNeuralNetwork
				accm += filterOutput;
				//std::cout << "och:" << opCh << "\ti:" << ipIndex << "\tich:" << ipCh << std::endl;
				//std::cout << kernel << std::endl;
			}
		}

		//bias
		accm += bias[opCh];
		outputSplit[opCh] = accm;
	}

	cv::merge(outputSplit, outputPlane);


	//activation function (ReLU - Rectified Linear Unit)
	outputPlane = cv::max(outputPlane*0.1, outputPlane);

}

void convolutionalNeuralNetwork(
	cv::Mat &inputPlanes,				///! input [planes/ch][rows][cols](ch)
	cv::Mat &outputPlanes,				///! output [planes/ch][rows][cols](ch)
	cv::Mat &_weights,					///! kernels [outputPlanes*inputPlanes/ch][ksize][ksize](ch)
	std::vector<double>& biases,		///! bias [outputPlanes]
	bool outputPacked					///! output plane to be packed.
	)
{
	CV_Assert(inputPlanes.dims == 3);
//	CV_Assert(outputPlanes.dims == 3);
	CV_Assert(_weights.dims == 3);

	cv::Size ipSize = cv::Size(inputPlanes.size[2], inputPlanes.size[1]);
	cv::Size kSize = Size(_weights.size[2], _weights.size[1]);

	const int n_inputPlanes = inputPlanes.size[0];
	const int n_inputChannels = inputPlanes.channels();
	const int n_outputChannels = outputPacked ?
		(biases.size() / 4 > 0 ? 4 : 1) :
		1;
	const int n_outputPlanes = (int)biases.size() / n_outputChannels;
	//const int n_outputPlanes = outputPlanes.size[0];
	//const int n_outputChannels = outputPlanes.channels();
	CV_Assert(n_inputPlanes * n_inputChannels *
		n_outputPlanes * n_outputChannels
		== _weights.size[0] * _weights.channels());

	Mat weights;
	if (_weights.channels() == inputPlanes.channels()){
		weights = _weights;
	}
	else{
		//if inputPlanes is packed, pack weights.  
		pack<float>(_weights, weights, inputPlanes.channels());
	}

	{	//allocate output planes
		int _size[3] = { n_outputPlanes, ipSize.height, ipSize.width };
		outputPlanes = cv::Mat(3, _size, CV_MAKETYPE(inputPlanes.depth(), n_outputChannels));
	}


	int _sz[3] = { n_inputPlanes * n_outputChannels, kSize.height, kSize.width };

#ifdef _OPENMP
#pragma omp parallel for
#endif 
	for (int opIndex = 0; opIndex < n_outputPlanes; opIndex++) {
		//std::cout << "----- op:" << opIndex <<  std::endl;

		cv::Mat kernels = cv::Mat(3, _sz, weights.type(), weights.ptr<float>(_sz[0] * opIndex));
		cv::Mat outputPlane = cv::Mat(ipSize, outputPlanes.type(), outputPlanes.ptr<float>(opIndex));
		ReLU_accumlate_filter2D(inputPlanes, outputPlane, kernels, &biases[n_outputChannels * opIndex]);
	}

	//std::cout << "==========================" << std::endl;

}
#endif



}
