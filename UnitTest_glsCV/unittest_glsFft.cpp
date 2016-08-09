/*
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

#include "stdafx.h"
#include "CppUnitTest.h"
#include "Timer.h"
#include "Unittest_common.h"

#include "glsCV.h"



using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest_glsCV
{	

	int test_glsFft(const int N, const int flags,const bool is2D = true){
		int ULPS = 64;
		float EPS = 1e-4f;
		Size size;
		if (is2D){
			size = Size(N, N);
		}
		else{
			size = Size(N, 1);
//			size = Size(1 , N);
		}
		
		Mat imgSrc = Mat(size, CV_32FC2);
		Mat imgFft = Mat::zeros(imgSrc.size(), imgSrc.type());
		Mat imgFftRef = Mat::zeros(imgSrc.size(), imgSrc.type());

		cout << "Size:" << imgSrc.size() << endl;

		//---------------------------------
		//init Src image
		FillRandU<float>(imgSrc);

		//reference select
#if 1	
		//---------------------------------
		//CPU FFT(cv::dft)
		{
			Timer tmr("cv:dft:   \t");
			cv::dft(imgSrc, imgFftRef, flags);
		}
#else
		//---------------------------------
		//CPU FFT(cv::dft)
		{
			Timer tmr("cv:dft:   \t");
			void fft_dit_Stockham_radix2_type0(const Mat& src, Mat &dst);
			fft_dit_Stockham_radix2_type0(imgSrc, imgFftRef);
		}
#endif

		//target select
#if 0
		//---------------------------------
		//CPU FFT(cv::dft)
		{
			Timer tmr("Stockham:   \t");
			void fft_dit_Stockham_radix2_type0(const Mat& src, Mat &dst);
			fft_dit_Stockham_radix2_type0(imgSrc, imgFft);
		}
#elif 0
		{
			Timer tmr("Stockham:   \t");
			//void fft_dif_Stockham_radix4_type0(const Mat& src, Mat &dst);
			//fft_dif_Stockham_radix4_type0(imgSrc, imgFft);
			void fft_dit_Stockham_radix4_type0(const Mat& src, Mat &dst);
			fft_dit_Stockham_radix4_type0(imgSrc, imgFft);
		}


#else
		//---------------------------------
		{
			int _flags = 0;
			if (flags & DFT_SCALE)	_flags |= GLS_FFT_SCALE;
			if (flags & DFT_INVERSE)_flags |= GLS_FFT_INVERSE;
			GlsMat _src(imgSrc);
			GlsMat _dst;
			{
				Timer tmr("glsFft:  \t");
				gls::fft(_src, _dst,_flags);
			}
			_dst.download(imgFft);
		}
#endif

		//verify
		int errNum = 0;
		if (!AreEqual<float>(imgFftRef, imgFft, ULPS, EPS)) errNum = -1;

		cout << "errNum:" << errNum << endl;

		return errNum;
	}


	TEST_CLASS(UnitTest_glsFft)
	{
	public:

		TEST_METHOD(FFT_256x256)
		{
			cout << __FUNCTION__ << endl;
			const int N = 256;
			const int flags = 0;
			int errNum = test_glsFft(N, flags);
			Assert::AreEqual(0, errNum);
		}
		BEGIN_TEST_METHOD_ATTRIBUTE(FFT_256x256)
			//TEST_OWNER(L"OwnerName")
			TEST_PRIORITY(1)
			TEST_MY_TRAIT(L"basic")
		END_TEST_METHOD_ATTRIBUTE()

		TEST_METHOD(FFT_32x32)
		{
			cout << __FUNCTION__ << endl;
			const int N = 32;
			const int flags = 0;
			int errNum = test_glsFft(N, flags);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(FFT_16x16)
		{
			cout << __FUNCTION__ << endl;
			const int N = 16;
			const int flags = 0;
			int errNum = test_glsFft(N, flags);
			Assert::AreEqual(0, errNum);
		}


		TEST_METHOD(FFT_32)
		{
			cout << __FUNCTION__ << endl;
			const int N = 32;
			const int flags = 0;
			int errNum = test_glsFft(N, flags, false);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(FFT_64)
		{
			cout << __FUNCTION__ << endl;
			const int N = 64;
			const int flags = 0;
			int errNum = test_glsFft(N, flags,false);
			Assert::AreEqual(0, errNum);
		}
		BEGIN_TEST_METHOD_ATTRIBUTE(FFT_64)
			//TEST_OWNER(L"OwnerName")
			TEST_PRIORITY(1)
			TEST_MY_TRAIT(L"basic")
		END_TEST_METHOD_ATTRIBUTE()



		TEST_METHOD(FFT_128x128_SCALE)
		{
			cout << __FUNCTION__ << endl;
			const int N = 128;
			const int flags = DFT_SCALE;
			int errNum = test_glsFft(N, flags);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(IFFT_64x64)
		{
			cout << __FUNCTION__ << endl;
			const int N = 64;
			const int flags = DFT_INVERSE;
			int errNum = test_glsFft(N, flags);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(IFFT_32x32_SCALE)
		{
			cout << __FUNCTION__ << endl;
			const int N = 32;
			const int flags = DFT_INVERSE + DFT_SCALE;
			int errNum = test_glsFft(N, flags);
			Assert::AreEqual(0, errNum);
		}


		TEST_METHOD(FFT_64x64_SCALE_benchmark)
		{
			cout << __FUNCTION__ << endl;
			const int N = 64;
			const int flags = DFT_SCALE;
			int errNum = 0;
			int loop = 10;
			for (int i = 0; i < loop; i++){
				errNum += test_glsFft(N, flags);
			}
			Assert::AreEqual(0, errNum);
		}
		BEGIN_TEST_METHOD_ATTRIBUTE(FFT_64x64_SCALE_benchmark)
			//TEST_OWNER(L"OwnerName")
			//TEST_PRIORITY(1)
			TEST_MY_TRAIT(L"benchmark")
		END_TEST_METHOD_ATTRIBUTE()

		TEST_METHOD(FFT_128x128_SCALE_benchmark)
		{
			cout << __FUNCTION__ << endl;
			const int N = 128;
			const int flags = DFT_SCALE;
			int errNum = 0;
			int loop = 10;
			for (int i = 0; i < loop; i++){
				errNum += test_glsFft(N, flags);
			}
			Assert::AreEqual(0, errNum);
		}
		BEGIN_TEST_METHOD_ATTRIBUTE(FFT_128x128_SCALE_benchmark)
			//TEST_OWNER(L"OwnerName")
			//TEST_PRIORITY(1)
			TEST_MY_TRAIT(L"benchmark")
		END_TEST_METHOD_ATTRIBUTE()

		TEST_METHOD(FFT_256x256_SCALE_benchmark)
		{
			cout << __FUNCTION__ << endl;
			const int N = 256;
			const int flags = DFT_SCALE;
			int errNum = 0;
			int loop = 10;
			for (int i = 0; i < loop; i++){
				errNum += test_glsFft(N, flags);
			}
			Assert::AreEqual(0, errNum);
		}
		BEGIN_TEST_METHOD_ATTRIBUTE(FFT_256x256_SCALE_benchmark)
			//TEST_OWNER(L"OwnerName")
			//TEST_PRIORITY(1)
			TEST_MY_TRAIT(L"benchmark")
		END_TEST_METHOD_ATTRIBUTE()

		TEST_METHOD(FFT_512x512_SCALE_benchmark)
		{
			cout << __FUNCTION__ << endl;
			const int N = 512;
			const int flags = DFT_SCALE;
			int errNum = 0;
			int loop = 10;
			for (int i = 0; i < loop; i++){
				errNum += test_glsFft(N, flags);
			}
			Assert::AreEqual(0, errNum);
		}
		BEGIN_TEST_METHOD_ATTRIBUTE(FFT_512x512_SCALE_benchmark)
			//TEST_OWNER(L"OwnerName")
			//TEST_PRIORITY(1)
			TEST_MY_TRAIT(L"benchmark")
		END_TEST_METHOD_ATTRIBUTE()


		TEST_METHOD(FFT_1024x1024_SCALE_benchmark)
		{
			cout << __FUNCTION__ << endl;
			const int N = 1024;
			const int flags = DFT_SCALE;
			int errNum = 0;
			int loop = 5;
			for (int i = 0; i < loop; i++){
				errNum += test_glsFft(N, flags);
			}
			Assert::AreEqual(0, errNum);
		}
		BEGIN_TEST_METHOD_ATTRIBUTE(FFT_1024x1024_SCALE_benchmark)
			//TEST_OWNER(L"OwnerName")
			//TEST_PRIORITY(1)
			TEST_MY_TRAIT(L"benchmark")
		END_TEST_METHOD_ATTRIBUTE()



		TEST_METHOD(FFT_2048x2048_SCALE_benchmark)
		{
			cout << __FUNCTION__ << endl;
			const int N = 2048;
			const int flags = DFT_SCALE;
			int errNum = 0;
			int loop = 5;
			for (int i = 0; i < loop; i++){
				errNum += test_glsFft(N, flags);
			}
			Assert::AreEqual(0, errNum);
		}
		BEGIN_TEST_METHOD_ATTRIBUTE(FFT_2048x2048_SCALE_benchmark)
			//TEST_OWNER(L"OwnerName")
			//TEST_PRIORITY(1)
			TEST_MY_TRAIT(L"benchmark")
		END_TEST_METHOD_ATTRIBUTE()




	};
}