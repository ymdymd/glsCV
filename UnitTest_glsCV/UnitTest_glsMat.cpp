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

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "HookCoutCerr.h"
#include "Timer.h"

#include "glsCV.h"
#include "UnitTest_Common.h"

//Lib 
#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "glew32.lib")
#pragma comment (lib, "glfw3dll.lib")

#pragma comment (lib, "glsCV.lib")


namespace UnitTest_glsCV
{

	template <typename T>
	int test_GlsMat_CopyTo(const int cvtype, const Size size = Size(32, 24)){
		Mat imgSrc = Mat(size, cvtype);
		Mat imgDst = Mat::zeros(imgSrc.size(), imgSrc.type());
//		Mat imgDst;

		cout << "Size:" << imgSrc.size() << endl;

		//---------------------------------
		//init Src image
		FillRandU<T>(imgSrc);

		GlsMat glsSrc(imgSrc);		//create GlsMat and  upload
		glsSrc.CopyTo(imgDst);		// download


		//verify
		int errNum = 0;
		if (!AreEqual<T>(imgSrc, imgDst)) errNum = -1;

		//if (errNum){
		//	cout << imgSrc << endl;
		//	cout << imgDst << endl;
		//}

		return errNum;
	}

	TEST_CLASS(UnitTest_GlsMat)
	{
	public:
		TEST_METHOD(GlsMat_Copy_CV_8UC1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_GlsMat_CopyTo<uchar>(CV_8UC1);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(GlsMat_Copy_CV_8UC2)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_GlsMat_CopyTo<uchar>(CV_8UC2);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(GlsMat_Copy_CV_8UC3)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_GlsMat_CopyTo<uchar>(CV_8UC3);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(GlsMat_Copy_CV_8UC4)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_GlsMat_CopyTo<uchar>(CV_8UC4);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(GlsMat_Copy_CV_8SC1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_GlsMat_CopyTo<schar>(CV_8SC1);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(GlsMat_Copy_CV_8SC2)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_GlsMat_CopyTo<schar>(CV_8SC2);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(GlsMat_Copy_CV_8SC3)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_GlsMat_CopyTo<schar>(CV_8SC3);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(GlsMat_Copy_CV_8SC4)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_GlsMat_CopyTo<schar>(CV_8SC4);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(GlsMat_Copy_CV_16SC1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_GlsMat_CopyTo<short>(CV_16SC1);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(GlsMat_Copy_CV_16SC2)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_GlsMat_CopyTo<short>(CV_16SC2);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(GlsMat_Copy_CV_16SC3)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 24;
			int errNum = test_GlsMat_CopyTo<short>(CV_16SC3);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(GlsMat_Copy_CV_16SC4)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 24;
			int errNum = test_GlsMat_CopyTo<short>(CV_16SC4);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(GlsMat_Copy_CV_16UC1)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 24;
			int errNum = test_GlsMat_CopyTo<ushort>(CV_16UC1);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(GlsMat_Copy_CV_16UC2)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 24;
			int errNum = test_GlsMat_CopyTo<ushort>(CV_16UC2);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(GlsMat_Copy_CV_16UC3)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 24;
			int errNum = test_GlsMat_CopyTo<ushort>(CV_16UC3);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(GlsMat_Copy_CV_16UC4)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 24;
			int errNum = test_GlsMat_CopyTo<ushort>(CV_16UC4);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(GlsMat_Copy_CV_32SC1)
		{
			cout << __FUNCTION__ << endl;
			const int width  = 32;
			const int height = 24;
			int errNum = test_GlsMat_CopyTo<int>(CV_32SC1);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(GlsMat_Copy_CV_32SC2)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 24;
			int errNum = test_GlsMat_CopyTo<int>(CV_32SC2);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(GlsMat_Copy_CV_32SC3)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 24;
			int errNum = test_GlsMat_CopyTo<int>(CV_32SC3);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(GlsMat_Copy_CV_32SC4)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 24;
			int errNum = test_GlsMat_CopyTo<int>(CV_32SC4);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(GlsMat_Copy_CV_32FC1)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 24;
			int errNum = test_GlsMat_CopyTo<float>(CV_32FC1);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(GlsMat_Copy_CV_32FC2)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 24;
			int errNum = test_GlsMat_CopyTo<float>(CV_32FC2);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(GlsMat_Copy_CV_32FC3)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 24;
			int errNum = test_GlsMat_CopyTo<float>(CV_32FC3);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(GlsMat_Copy_CV_32FC4)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 24;
			int errNum = test_GlsMat_CopyTo<float>(CV_32FC4);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(GlsMat_Copy_CV_8UC1_4x8)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_GlsMat_CopyTo<uchar>(CV_8UC1, Size(4, 8));
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(GlsMat_Copy_CV_8UC1_4x3)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_GlsMat_CopyTo<uchar>(CV_8UC1, Size(4, 3));
			Assert::AreEqual(0, errNum);
		}



		TEST_METHOD(GlsMat_Copy_CV_32FC1_5x5)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_GlsMat_CopyTo<float>(CV_32FC1, Size(5, 5));
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(GlsMat_Copy_CV_32FC1_1024x1024)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_GlsMat_CopyTo<float>(CV_32FC1, Size(1024, 1024));
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(GlsMat_Copy_CV_32FC1_1x1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_GlsMat_CopyTo<float>(CV_32FC1, Size(1, 1));
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(GlsMat_Copy_CV_32FC1_3x4)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_GlsMat_CopyTo<float>(CV_32FC1, Size(3, 4));
			Assert::AreEqual(0, errNum);
		}

		//@@@ TODO issue #6
#if 0
		TEST_METHOD(GlsMat_Copy_CV_8UC1_3x4)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_GlsMat_CopyTo<uchar>(CV_8UC1, Size(3, 4));
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(GlsMat_Copy_CV_16UC1_3x4)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_GlsMat_CopyTo<ushort>(CV_16UC1, Size(3, 4));
			Assert::AreEqual(0, errNum);
		}
#endif



	};


}