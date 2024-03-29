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

#ifndef _GLS_MAT_H_
#define _GLS_MAT_H_

#include <memory>	//shared_ptr

namespace gls
{

GLenum convCVtype2GLsizedFormat(int type);
GLenum convCVtype2GLformat(int type);
GLenum convCVtype2GLtype(int type);
int convFmtGL2CV(GLenum  format);

//! 参照カウンタを持つ，OpenGL texture用の基底ストレージクラス
class GlsMat
{
private:
	shared_ptr<GLuint> _texid;	//!< テクスチャidを保存する、shared_ptr
	int flag;					//!< フォーマット等を保存するフラグ

	//! テクスチャ生成
	void createTexture(const int _dims, const int* _sizes, int _type);

	//! テクスチャ生成(2D)
	void createTexture(const int _width, const int _height, int _type);
	//! テクスチャ生成(3D)
	void createTexture(const int _width, const int _height, const int _depth, int _type);

	//! テクスチャ削除
	void deleteTexture(void);

	//! 初期化
	void init(void){
		flag = 0;
		dims = 0;
		plns = 0;
		rows = 0;
		cols = 0;
	}

public:
	
	int dims;	//! 配列の次元 >= 2
	int plns;	//!< 面数(depth)
	int rows;	//!< 行数(height)
	int cols;	//!< 列数(width)

	struct MSize
	{
		MSize(int* _p);
		Size operator()(void) const;
		const int& operator[](int i) const;
		int& operator[](int i);
		operator const int*() const;
		//bool operator == (const MSize& sz) const;
		//bool operator != (const MSize& sz) const;

		int* p;
	};
	MSize size;



	//! cv::matと同じサイズ、同じtypeのテクスチャーを生成し、データをuploadします。
	explicit GlsMat(const Mat & cvmat);
	//! size, typeのテクスチャーを生成します。
	GlsMat(const Size size, const int type);
	//! N次元クスチャーを生成します。２次元または３次元のみサポートします。
	//! dims==2 の時　sizes[0]:rows / sizes[1]:cols
	//! dims==3 の時　sizes[0]:planes / sizes[1]:rows / sizes[2]:cols
	GlsMat(const int _dims, const int* sizes, const int type);

	//! 空の行列です。テクスチャーは生成されません。
	GlsMat(void);

	//! コピーコンストラクタ
	GlsMat(const GlsMat &);

	//! デストラクタ　参照カウントが最後のエントリを示すときに、テクスチャーを削除します。
	~GlsMat(void);

	//! 代入　ヘッダー情報のみのコピーで、データ自体のコピーは発生しません。
	GlsMat& operator=(const GlsMat& rhs);

	//! 行列サイズ(width,height)
	//Size size(void) const { return Size(cols, rows); }
	//! 行列要素の型を返します．OpenCVのMat.type()と同じです。
	int type(void) const { return CV_MAT_TYPE(flag); }
	//! チャンネル数　(1-4)
	int channels(void) const { return CV_MAT_CN(flag); }
	//! 行列要素のビット深度を返します．OpenCVのMat.depth()と同じです。
	int depth(void) const { return CV_MAT_DEPTH(flag); }

	//! 配列が要素を持たない（テクスチャーが生成されていない）場合に true を返します．
	bool empty(void) const { return _texid.use_count() == 0; }

	//! 行列が連続であるか否かを調べます． 
	bool isContinuous(void) const { return true; }

	size_t total(void) const;

	//! 行列を別の行列にコピーします．
	void copyTo(GlsMat& m) const;

	//! 行列とその内部データの完全なコピーを作成します．
	GlsMat clone(void) const;

	/*!
	OpenGL　wrapper
	*/

	//! テクスチャーのIDの取得
	GLuint texid(void)const { return _texid.use_count() == 0 ? 0 : *_texid; }

	//! OpenGL internal format (SizedFormatを使用します。GL_RGB32Fなど)
	GLenum glSizedFormat(void) const{ return convCVtype2GLsizedFormat(CV_MAT_TYPE(flag)); }
	//! OpenGL format (GL_RED,GL_RG,GL_RGB,GL_RED_INTEGER等)
	GLenum glFormat(void) const{ return convCVtype2GLformat(CV_MAT_TYPE(flag)); }
	//! OpenGL type (GL_FLOAT,GL_UNSIGED_BYTET等)
	GLenum glType(void) const{ return convCVtype2GLtype(CV_MAT_TYPE(flag)); }

	//! CPU->GPUへのupload
	void upload(const Mat&src);

	//! GPU->CPUへのdownload
	void download(Mat&dst) const;
	operator Mat() const;

	//! texture filter (GL_NEAREST/GL_LINEAR)
	void setInterpolation(GLint interpolation) const;

	//! texture filter (GL_CLAMP_TO_EDGE/GL_CLAMP_TO_BORDER/GL_MIRRORED_REPEAT/GL_REPEAT/GL_MIRROR_CLAMP_TO_EDGE)
	void setWrapMode(GLint mode) const;

};

/*!
出力GlsMatの取得

in-placeの時、dstのサイズ、タイプがsrcのサイズ、タイプと異なるときに、新しいGlsMatを生成(新しいtextureの生成)
それ以外はdstのテクスチャを利用する。
*/
static GlsMat getDstMat(const GlsMat& src, GlsMat& dst){
	GlsMat _dst;
	if (src.texid() == dst.texid() || dst.size() != src.size() || dst.type() != src.type()){
		_dst = GlsMat(src.size(), src.type());
	}
	else{
		_dst = dst;
	}
	return _dst;
}

static GlsMat getDstMat(const GlsMat& src0, const GlsMat& src1, GlsMat& dst){
//	Gls_Assert(src0.size()==src1.size() && src0.type()==src1.type());
	GlsMat _dst;
	if (src0.texid() == dst.texid() || src1.texid() == dst.texid() ||
		dst.size() != src0.size() || dst.type() != src0.type()){
		_dst = GlsMat(src0.size(), src0.type());
	}
	else{
		_dst = dst;
	}
	return _dst;
}


/*!
出力GlsMatの取得

dstのサイズ、タイプが要求のサイズ、タイプと異なるときに、新しいGlsMatを生成(新しいtextureの生成)
それ以外はdstのテクスチャを利用する。
*/
static GlsMat getDstMat(const Size size, const int type, GlsMat& dst){
	GlsMat _dst;
	if (dst.size() != size || dst.type() != type){
		_dst = GlsMat(size, type);
	}
	else{
		_dst = dst;
	}
	return _dst;
}



}//namespace gls

using gls::GlsMat;

#endif