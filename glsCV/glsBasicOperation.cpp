#include "stdafx.h"


#include "glsBasicOperation.h"


class glsShaderScalarOperation : public glsShaderBase
{
public:
	glsShaderScalarOperation(void);

	//attribute location
	GLuint position;
	//uniform  location
	GLuint texSrc;
	GLuint v4_scalar;
	GLuint i_flags;
};



class glsShaderBinaryOperation : public glsShaderBase
{
public:
	glsShaderBinaryOperation(void);

	//attribute location
	GLuint position;
	//uniform  location
	GLuint texSrc0;
	GLuint texSrc1;
	GLuint i_flags;
};


//-----------------------------------------------------------------------------
//global 
glsShaderScalarOperation* shaderScalarOperation = 0;
glsShaderBinaryOperation* shaderBinaryOperation = 0;

void glsBasicOperationInit(void){
	shaderScalarOperation = new glsShaderScalarOperation();
	shaderBinaryOperation = new glsShaderBinaryOperation();
}

void glsBasicOperationTerminate(void){
	delete shaderScalarOperation;
	delete shaderBinaryOperation;
}

#define OPCODE_ADD 0
#define OPCODE_SUB 1
#define OPCODE_MUL 2
#define OPCODE_DIV 3
#define OPCODE_MIN 4
#define OPCODE_MAX 5
#define OPCODE_MUL_SPCETRUM		 6
#define OPCODE_MUL_SPCETRUM_CONJ 7
#define OPCODE_MUL_SPCETRUM_POC  8



static const char vertexShaderCode[] =
"#version 330 core\n"
"layout (location = 0)in  vec2 position;\n"
"void main(void)\n"
"{\n"
"   gl_Position  = vec4(position,0.0,1.0);\n"
"}\n"
;

//-----------------------------------------------------------------------------
glsShaderScalarOperation::glsShaderScalarOperation(void){
	const char fragmentShaderCode[] =
		"#version 330 core\n"
		"precision highp float;\n"
		"uniform sampler2DRect	texSrc;\n"
		"uniform vec4 scalar;\n"
		"uniform int	flags;\n"
		"layout (location = 0) out vec4 dst;\n"
		"\n"
		"void main(void)\n"
		"{\n"
		"	vec4 src = texture(texSrc, gl_FragCoord.xy);\n"
		"	switch(flags){\n"
		"   case(0): dst = scalar + src; break;\n"
		"	case(1): dst = scalar - src; break;\n"
		"	case(2): dst = scalar * src; break;\n"
		"	case(3): dst = scalar / src; break;\n"
		"	case(4)://OPCODE_MIN \n"
		"		dst.r = scalar.r < src.r? scalar.r : src.r;\n"
		"		dst.g = scalar.g < src.g? scalar.g : src.g;\n"
		"		dst.b = scalar.b < src.b? scalar.b : src.b;\n"
		"		dst.a = scalar.a < src.a? scalar.a : src.a;\n"
		"		break;\n"
		"	case(5)://OPCODE_MAX \n"
		"		dst.r = scalar.r > src.r? scalar.r : src.r;\n"
		"		dst.g = scalar.g > src.g? scalar.g : src.g;\n"
		"		dst.b = scalar.b > src.b? scalar.b : src.b;\n"
		"		dst.a = scalar.a > src.a? scalar.a : src.a;\n"
		"		break;\n"
		"	default: dst = src;\n"
		"   }\n"
		"}\n"
		;


	LoadShadersCode(vertexShaderCode, fragmentShaderCode);

	// Attribute & Uniform location
	position = glGetAttribLocation(program, "position");

	texSrc = glGetUniformLocation(program, "texSrc");
	v4_scalar = glGetUniformLocation(program, "scalar");
	i_flags = glGetUniformLocation(program, "flags");

}


//-------------------------------------------------------------
// Scalar operation
static
void glslScalarOperation(
const glsShaderScalarOperation* shader,	//progmra ID
const vec4 scalar,				//scalar
const vector<GLuint>& texSrc,	//src texture IDs
const vector<GLuint>& texDst,	//dst texture IDs
const int flags					//flag (opcode)
)
{
	int width;
	int height;

	glBindTexture(GL_TEXTURE_RECTANGLE, texSrc[0]);
	glGetTexLevelParameteriv(
		GL_TEXTURE_RECTANGLE, 0,
		GL_TEXTURE_WIDTH, &width
		);

	glGetTexLevelParameteriv(
		GL_TEXTURE_RECTANGLE, 0,
		GL_TEXTURE_HEIGHT, &height
		);

	glBindTexture(GL_TEXTURE_RECTANGLE, 0);


	//FBO 
	GLuint fbo = 0;

	//---------------------------------
	// FBO
	// create FBO (off-screen framebuffer)
	glGenFramebuffers(1, &fbo);

	// bind offscreen framebuffer (that is, skip the window-specific render target)
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	//---------------------------------
	// vbo
	GLuint vao = 0;
	GLuint vbo = 0;

	// [-1, 1] の正方形
	static GLfloat position[][2] = {
		{ -1.0f, -1.0f },
		{ 1.0f, -1.0f },
		{ 1.0f, 1.0f },
		{ -1.0f, 1.0f }
	};

	// create vao&vbo
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	// bind vao & vbo
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	// upload vbo data
	glBufferData(GL_ARRAY_BUFFER, (int)sizeof(position), position, GL_STATIC_DRAW);

	// Set VertexAttribute
	glEnableVertexAttribArray(shaderBinaryOperation->position);	//enable attribute Location
	glVertexAttribPointer(
		shaderBinaryOperation->position,	// attribute location.
		2,					// size	(Specifies the number of components) x,y
		GL_FLOAT,			// type
		GL_FALSE,			// normalized?
		0,					// stride (Specifies the byte offset between consecutive generic vertex attributes)
		(void*)0			// array buffer offset (Specifies a pointer to the first generic vertex attribute in the array)
		);



	//program
	{
		glUseProgram(shader->program);
	}

	//uniform
	{
		glUniform1i(shader->i_flags, flags);
		glUniform4fv(shader->v4_scalar, 1,&scalar[0]);

	}

	for (int i = 0; i < texSrc.size(); i++){
		//Bind Texture
		{
			int id = 0;
			glActiveTexture(GL_TEXTURE0 + id);
			glBindTexture(GL_TEXTURE_RECTANGLE, texSrc[i]);
			glUniform1i(shader->texSrc, id);
		}
		//dst texture
		{
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, texDst[i], 0);
			assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
		}

		GLenum bufs[] =
		{
			GL_COLOR_ATTACHMENT0,
		};
		glDrawBuffers(1, bufs);

		//Viewport
		{
			glViewport(0, 0, width, height);
		}

		//Render!!
		{
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
			glFlush();
		}
	}

	//clean up
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);

	glDeleteFramebuffers(1, &fbo);

}






//-----------------------------------------------------------------------------
glsShaderBinaryOperation::glsShaderBinaryOperation(void){

	const char fragmentShaderCode[] =
		"#version 330 core\n"
		"precision highp float;\n"
		"uniform sampler2DRect	texSrc0;\n"
		"uniform sampler2DRect	texSrc1;\n"
		"uniform int			flags;\n"
		"layout (location = 0) out vec4 dst;\n"
		"\n"
		"void main(void)\n"
		"{\n"
		"	vec4 src0 = texture(texSrc0, gl_FragCoord.xy);\n"
		"	vec4 src1 = texture(texSrc1, gl_FragCoord.xy);\n"
		"	switch(flags){\n"
		"   case(0): dst = src0 + src1; break;\n"
		"	case(1): dst = src0 - src1; break;\n"
		"	case(2): dst = src0 * src1; break;\n"
		"	case(3): dst = src0 / src1; break;\n"
		"	case(4)://OPCODE_MIN \n"
		"		dst.r = src0.r < src1.r? src0.r : src1.r;\n"
		"		dst.g = src0.g < src1.g? src0.g : src1.g;\n"
		"		dst.b = src0.b < src1.b? src0.b : src1.b;\n"
		"		dst.a = src0.a < src1.a? src0.a : src1.a;\n"
		"		break;\n"
		"	case(5)://OPCODE_MAX \n"
		"		dst.r = src0.r > src1.r? src0.r : src1.r;\n"
		"		dst.g = src0.g > src1.g? src0.g : src1.g;\n"
		"		dst.b = src0.b > src1.b? src0.b : src1.b;\n"
		"		dst.a = src0.a > src1.a? src0.a : src1.a;\n"
		"		break;\n"
		"	case(6)://OPCODE_MUL_SPCETRUM \n"
		"		dst.r = src0.r*src1.r - src0.g*src1.g;\n"
		"		dst.g = src0.r*src1.g + src0.g*src1.r;\n"
		"		dst.b = src0.b*src1.b - src0.a*src1.a;\n"
		"		dst.a = src0.b*src1.a + src0.a*src1.b;\n"
		"		break;\n"
		"	case(7)://OPCODE_MUL_SPCETRUM_CONJ \n"
		"		dst.r = src0.r*src1.r + src0.g*src1.g;\n"
		"		dst.g = -src0.r*src1.g + src0.g*src1.r;\n"
		"		dst.b = src0.b*src1.b + src0.a*src1.a;\n"
		"		dst.a = -src0.b*src1.a + src0.a*src1.b;\n"
		"		break;\n"
		"	case(8)://OPCODE_MUL_SPCETRUM_POC \n"
		"       vec2 tmp0;\n"
		"       vec2 tmp1;\n"
		"		tmp0.r = src0.r*src1.r + src0.g*src1.g;\n"
		"		tmp0.g = -src0.r*src1.g + src0.g*src1.r;\n"
		"		tmp1.r = src0.b*src1.b + src0.a*src1.a;\n"
		"		tmp1.g = -src0.b*src1.a + src0.a*src1.b;\n"
		"		dst.r = tmp0.r / sqrt(tmp0.r*tmp0.r+tmp0.g*tmp0.g);\n"
		"		dst.g = tmp0.g / sqrt(tmp0.r*tmp0.r+tmp0.g*tmp0.g);\n"
		"		dst.b = tmp1.r / sqrt(tmp1.r*tmp1.r+tmp1.g*tmp1.g);\n"
		"		dst.a = tmp1.g / sqrt(tmp1.r*tmp1.r+tmp1.g*tmp1.g);\n"
		"		break;\n"
		"	default: dst = src0;\n"
		"   }\n"
		"}\n"
		;


	LoadShadersCode(vertexShaderCode, fragmentShaderCode);

	// Attribute & Uniform location
	position = glGetAttribLocation(program, "position");

	texSrc0  = glGetUniformLocation(program, "texSrc0");
	texSrc1 = glGetUniformLocation(program, "texSrc1");
	i_flags = glGetUniformLocation(program, "flags");

}

//-------------------------------------------------------------
// Binary operation
static 
void glslBinaryOperation(
	const glsShaderBinaryOperation* shader,	//progmra ID
	const vector<GLuint>& texSrc0,	//src texture IDs
	const vector<GLuint>& texSrc1,	//src texture IDs
	const vector<GLuint>& texDst,	//dst texture IDs
	const int flags					//flag (opcode)
	)
{
	int width;
	int height;

	glBindTexture(GL_TEXTURE_RECTANGLE, texSrc0[0]);
	glGetTexLevelParameteriv(
		GL_TEXTURE_RECTANGLE, 0,
		GL_TEXTURE_WIDTH, &width
		);

	glGetTexLevelParameteriv(
		GL_TEXTURE_RECTANGLE, 0,
		GL_TEXTURE_HEIGHT, &height
		);

	glBindTexture(GL_TEXTURE_RECTANGLE, 0);


	//FBO 
	GLuint fbo = 0;

	//---------------------------------
	// FBO
	// create FBO (off-screen framebuffer)
	glGenFramebuffers(1, &fbo);

	// bind offscreen framebuffer (that is, skip the window-specific render target)
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	//---------------------------------
	// vbo
	GLuint vao = 0;
	GLuint vbo = 0;

	// [-1, 1] の正方形
	static GLfloat position[][2] = {
		{ -1.0f, -1.0f },
		{ 1.0f, -1.0f },
		{ 1.0f, 1.0f },
		{ -1.0f, 1.0f }
	};

	// create vao&vbo
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	// bind vao & vbo
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	// upload vbo data
	glBufferData(GL_ARRAY_BUFFER, (int)sizeof(position), position, GL_STATIC_DRAW);

	// Set VertexAttribute
	glEnableVertexAttribArray(shaderBinaryOperation->position);	//enable attribute Location
	glVertexAttribPointer(
		shaderBinaryOperation->position,	// attribute location.
		2,					// size	(Specifies the number of components) x,y
		GL_FLOAT,			// type
		GL_FALSE,			// normalized?
		0,					// stride (Specifies the byte offset between consecutive generic vertex attributes)
		(void*)0			// array buffer offset (Specifies a pointer to the first generic vertex attribute in the array)
		);



	//program
	{
		glUseProgram(shader->program);
	}

	//uniform
	{
		glUniform1i(shader->i_flags, flags);
	}

	for (int i = 0; i < texSrc0.size(); i++){
		//Bind Texture
		{
			int id = 0;
			glActiveTexture(GL_TEXTURE0 + id);
			glBindTexture(GL_TEXTURE_RECTANGLE, texSrc0[i]);
			glUniform1i(shader->texSrc0, id);
			id++;
			glActiveTexture(GL_TEXTURE0 + id);
			glBindTexture(GL_TEXTURE_RECTANGLE, texSrc1[i]);
			glUniform1i(shader->texSrc1, id);

		}
		//dst texture
		{
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, texDst[i], 0);
			assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
		}

		GLenum bufs[] =
		{
			GL_COLOR_ATTACHMENT0,
		};
		glDrawBuffers(1, bufs);

		//Viewport
		{
			glViewport(0, 0, width, height);
		}

		//Render!!
		{
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
			glFlush();
		}
	}

	//clean up
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);

	glDeleteFramebuffers(1, &fbo);

}

//スカラと配列の 要素毎の和を求めます．
void glsAdd(const vec4& scalar, const glsMat& src, glsMat& dst){
	assert(src.type == GL_FLOAT);
	glslScalarOperation(shaderScalarOperation, scalar, src.texArray, dst.texArray, OPCODE_ADD);
}

//2 つの配列同士の 要素毎の和を求めます
void glsAdd(const glsMat& src0, const glsMat& src1, glsMat& dst)
{
	assert(src0.type == GL_FLOAT);
	assert(src1.type == GL_FLOAT);
	glslBinaryOperation(shaderBinaryOperation, src0.texArray, src1.texArray, dst.texArray, OPCODE_ADD);
}


//スカラと配列の 要素毎の差を求めます．
void glsSubtract(const vec4& scalar, const glsMat& src, glsMat& dst){
	assert(src.type == GL_FLOAT);
	glslScalarOperation(shaderScalarOperation, scalar, src.texArray, dst.texArray, OPCODE_SUB);
}

//2 つの配列同士の 要素毎の差を求めます
void glsSubtract(const glsMat& src0, const glsMat& src1, glsMat& dst)
{
	assert(src0.type == GL_FLOAT);
	assert(src1.type == GL_FLOAT);
	glslBinaryOperation(shaderBinaryOperation, src0.texArray, src1.texArray, dst.texArray, OPCODE_SUB);
}


//スカラと配列の 要素毎の積を求めます．
void glsMultiply(const vec4& scalar, const glsMat& src, glsMat& dst){
	assert(src.type == GL_FLOAT);
	glslScalarOperation(shaderScalarOperation, scalar, src.texArray, dst.texArray, OPCODE_MUL);
}

//2 つの配列同士の 要素毎の積を求めます
void glsMultiply(const glsMat& src0, const glsMat& src1, glsMat& dst)
{
	assert(src0.type == GL_FLOAT);
	assert(src1.type == GL_FLOAT);
	glslBinaryOperation(shaderBinaryOperation, src0.texArray, src1.texArray, dst.texArray, OPCODE_MUL);
}

//スカラと配列の 要素毎の商を求めます．
void glsDivide(const vec4& scalar, const glsMat& src, glsMat& dst){
	assert(src.type == GL_FLOAT);
	glslScalarOperation(shaderScalarOperation, scalar, src.texArray, dst.texArray, OPCODE_DIV);
}


//2 つの配列同士の  要素毎の商を求めます．
void glsDivide(const glsMat& src0, const glsMat& src1, glsMat& dst){
	assert(src0.type == GL_FLOAT);
	assert(src1.type == GL_FLOAT);
	glslBinaryOperation(shaderBinaryOperation, src0.texArray, src1.texArray, dst.texArray, OPCODE_DIV);
}

//スカラと配列の 要素毎の最小値を求めます．
void glsMin(const vec4& scalar, const glsMat& src, glsMat& dst){
	assert(src.type == GL_FLOAT);
	glslScalarOperation(shaderScalarOperation, scalar, src.texArray, dst.texArray, OPCODE_MIN);
}

//2 つの配列同士の  要素毎の最小値を求めます．
void glsMin(const glsMat& src0, const glsMat& src1, glsMat& dst){
	assert(src0.type == GL_FLOAT);
	assert(src1.type == GL_FLOAT);
	glslBinaryOperation(shaderBinaryOperation, src0.texArray, src1.texArray, dst.texArray, OPCODE_MIN);
}

//スカラと配列の 要素毎の最大値を求めます．
void glsMax(const vec4& scalar, const glsMat& src, glsMat& dst){
	assert(src.type == GL_FLOAT);
	glslScalarOperation(shaderScalarOperation, scalar, src.texArray, dst.texArray, OPCODE_MAX);
}

//2 つの配列同士の  要素毎の最大値を求めます．
void glsMax(const glsMat& src0, const glsMat& src1, glsMat& dst){
	assert(src0.type == GL_FLOAT);
	assert(src1.type == GL_FLOAT);
	glslBinaryOperation(shaderBinaryOperation, src0.texArray, src1.texArray, dst.texArray, OPCODE_MAX);
}

//2 つのフーリエスペクトル同士の要素毎の乗算を行います．
//conj = ture の場合、2 番目の配列を複素共役に変更できます．
void glsMulSpectrums(const glsMat& src0, const glsMat& src1, glsMat& dst, bool conj){
	assert(src0.internalFormat == GL_RG32F);
	assert(src1.internalFormat == GL_RG32F);
	if (conj)glslBinaryOperation(shaderBinaryOperation, src0.texArray, src1.texArray, dst.texArray, OPCODE_MUL_SPCETRUM_CONJ);
	else glslBinaryOperation(shaderBinaryOperation, src0.texArray, src1.texArray, dst.texArray, OPCODE_MUL_SPCETRUM);
}

//2 つのフーリエスペクトル同士の  要素毎の位相限定相関を求めます．
void glsMulSpectrumsPhaseOnly(const glsMat& src0, const glsMat& src1, glsMat& dst){
	assert(src0.internalFormat == GL_RG32F);
	assert(src1.internalFormat == GL_RG32F);
	glslBinaryOperation(shaderBinaryOperation, src0.texArray, src1.texArray, dst.texArray, OPCODE_MUL_SPCETRUM_POC);
}







void glsAdd(const Vec4f& scalar, const Mat& src, Mat& dst)
{
	glsMat _src(src, false);
	glsMat _dst(src, false);
	vec4 _scalar = { scalar[0], scalar[1], scalar[2], scalar[3] };
	_src.CopyFrom(src);					//upload
	glsAdd(_scalar, _src, _dst);			//execute
	_dst.CopyTo(dst);					//download
}


void glsAdd(const Mat& src0, const Mat& src1, Mat& dst)
{
	CV_Assert(src0.size() == src1.size());
	CV_Assert(src0.type() == src1.type());

	glsMat _src0(src0, false);
	glsMat _src1(src0, false);
	glsMat _dst(src0, false);

	//---------------------------------
	//upload
	_src0.CopyFrom(src0);
	_src1.CopyFrom(src1);

	//---------------------------------
	//execute
	glsAdd(_src0, _src1, _dst);

	//---------------------------------
	//download
	_dst.CopyTo(dst);

}


void glsSubtract(const Vec4f& scalar, const Mat& src, Mat& dst)
{
	glsMat _src(src, false);
	glsMat _dst(src, false);
	vec4 _scalar = { scalar[0], scalar[1], scalar[2], scalar[3] };
	_src.CopyFrom(src);					//upload
	glsSubtract(_scalar, _src, _dst);	//execute
	_dst.CopyTo(dst);					//download
}


void glsSubtract(const Mat& src0, const Mat& src1, Mat& dst)
{
	CV_Assert(src0.size() == src1.size());
	CV_Assert(src0.type() == src1.type());

	glsMat _src0(src0, false);
	glsMat _src1(src0, false);
	glsMat _dst(src0, false);

	//---------------------------------
	//upload
	_src0.CopyFrom(src0);
	_src1.CopyFrom(src1);

	//---------------------------------
	//execute
	glsSubtract(_src0, _src1, _dst);

	//---------------------------------
	//download
	_dst.CopyTo(dst);

}


void glsMultiply(const Vec4f& scalar, const Mat& src, Mat& dst)
{
	glsMat _src(src, false);
	glsMat _dst(src, false);
	vec4 _scalar = { scalar[0], scalar[1], scalar[2], scalar[3] };
	_src.CopyFrom(src);					//upload
	glsMultiply(_scalar, _src, _dst);	//execute
	_dst.CopyTo(dst);					//download
}


void glsMultiply(const Mat& src0, const Mat& src1, Mat& dst)
{
	CV_Assert(src0.size() == src1.size());
	CV_Assert(src0.type() == src1.type());

	glsMat _src0(src0, false);
	glsMat _src1(src0, false);
	glsMat _dst(src0, false);

	//---------------------------------
	//upload
	_src0.CopyFrom(src0);
	_src1.CopyFrom(src1);

	//---------------------------------
	//execute
	glsMultiply(_src0, _src1, _dst);

	//---------------------------------
	//download
	_dst.CopyTo(dst);

}


void glsDivide(const Vec4f& scalar, const Mat& src, Mat& dst)
{
	glsMat _src(src, false);
	glsMat _dst(src, false);
	vec4 _scalar = { scalar[0], scalar[1], scalar[2], scalar[3] };
	_src.CopyFrom(src);					//upload
	glsDivide(_scalar, _src, _dst);	//execute
	_dst.CopyTo(dst);					//download
}


void glsDivide(const Mat& src0, const Mat& src1, Mat& dst)
{
	CV_Assert(src0.size() == src1.size());
	CV_Assert(src0.type() == src1.type());

	glsMat _src0(src0, false);
	glsMat _src1(src0, false);
	glsMat _dst(src0, false);

	//---------------------------------
	//upload
	_src0.CopyFrom(src0);
	_src1.CopyFrom(src1);

	//---------------------------------
	//execute
	glsDivide(_src0, _src1, _dst);

	//---------------------------------
	//download
	_dst.CopyTo(dst);

}

void glsMin(const Vec4f& scalar, const Mat& src, Mat& dst)
{
	glsMat _src(src, false);
	glsMat _dst(src, false);
	vec4 _scalar = { scalar[0], scalar[1], scalar[2], scalar[3] };
	_src.CopyFrom(src);					//upload
	glsMin(_scalar, _src, _dst);	//execute
	_dst.CopyTo(dst);					//download
}

void glsMin(const Mat& src0, const Mat& src1, Mat& dst)
{
	CV_Assert(src0.size() == src1.size());
	CV_Assert(src0.type() == src1.type());

	glsMat _src0(src0, false);
	glsMat _src1(src0, false);
	glsMat _dst(src0, false);

	_src0.CopyFrom(src0);	//upload
	_src1.CopyFrom(src1);	//upload
	glsMin(_src0, _src1, _dst); //execute
	_dst.CopyTo(dst);	//download

}

void glsMax(const Vec4f& scalar, const Mat& src, Mat& dst)
{
	glsMat _src(src, false);
	glsMat _dst(src, false);
	vec4 _scalar = { scalar[0], scalar[1], scalar[2], scalar[3] };
	_src.CopyFrom(src);					//upload
	glsMax(_scalar, _src, _dst);	//execute
	_dst.CopyTo(dst);					//download
}

void glsMax(const Mat& src0, const Mat& src1, Mat& dst)
{
	CV_Assert(src0.size() == src1.size());
	CV_Assert(src0.type() == src1.type());

	glsMat _src0(src0, false);
	glsMat _src1(src0, false);
	glsMat _dst(src0, false);

	_src0.CopyFrom(src0);	//upload
	_src1.CopyFrom(src1);	//upload
	glsMax(_src0, _src1, _dst); //execute
	_dst.CopyTo(dst);	//download

}

void glsMulSpectrums(const Mat& src0, const Mat& src1, Mat& dst, bool conj){
	CV_Assert(src0.size() == src1.size());
	CV_Assert(src0.type() == src1.type());
	CV_Assert(src0.type() == CV_32FC2);

	glsMat _src0(src0, false);
	glsMat _src1(src0, false);
	glsMat _dst(src0, false);

	_src0.CopyFrom(src0);	//upload
	_src1.CopyFrom(src1);	//upload
	glsMulSpectrums(_src0, _src1, _dst,conj); //execute
	_dst.CopyTo(dst);	//download

}

void glsMulSpectrumsPhaseOnly(const Mat& src0, const Mat& src1, Mat& dst){
	CV_Assert(src0.size() == src1.size());
	CV_Assert(src0.type() == src1.type());
	CV_Assert(src0.type() == CV_32FC2);

	glsMat _src0(src0, false);
	glsMat _src1(src0, false);
	glsMat _dst(src0, false);

	_src0.CopyFrom(src0);	//upload
	_src1.CopyFrom(src1);	//upload
	glsMulSpectrumsPhaseOnly(_src0, _src1, _dst); //execute
	_dst.CopyTo(dst);	//download

}


