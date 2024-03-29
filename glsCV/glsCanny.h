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

#ifndef _GLS_CANNY_H_
#define _GLS_CANNY_H_

namespace gls
{

/*!
Cannyアルゴリズムを用いて，画像のエッジを検出します．

@param image – 8ビット，シングルチャンネルの入力画像．
@param edges – 出力されるエッジのマップ． image と同じサイズ，同じ型です．
@param threshold1 – ヒステリシスが存在する処理の，1番目の閾値．
@param threshold2 – ヒステリシスが存在する処理の，2番目の閾値．
@param apertureSize – Sobel() オペレータのアパーチャサイズ．
@param L2gradient – 画像勾配の強度を求めるために，より精度の高い  L_2 ノルムを使用します。
*/
void Canny(const GlsMat& image, GlsMat& edges, double threshold1, double threshold2, int apertureSize = 3, bool L2gradient = false);

}//namespace gls

#endif