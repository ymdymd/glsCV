#include "stdafx.h"


#include "glsMerge.h"





//-----------------------------------------------------------------------------
// glsShaderMerge
class glsShaderMerge : public glsShaderBase
{
public:
	glsShaderMerge(void);

};

//-----------------------------------------------------------------------------
// glsShaderMergeU unsigned
class glsShaderMergeU : public glsShaderBase
{
public:
	glsShaderMergeU(void);
};

//-----------------------------------------------------------------------------
// glsShaderMergeS unsigned
class glsShaderMergeS : public glsShaderBase
{
public:
	glsShaderMergeS(void);
};


//-----------------------------------------------------------------------------
//global 
glsShaderMerge* shaderMerge = 0;
glsShaderMergeU* shaderMergeU = 0;
glsShaderMergeS* shaderMergeS = 0;

void glsMergeInit(void){
	shaderMerge = new glsShaderMerge();
	shaderMergeU = new glsShaderMergeU();
	shaderMergeS = new glsShaderMergeS();
}

void glsMergeTerminate(void){
	delete shaderMerge;
	delete shaderMergeU;
	delete shaderMergeS;
}


static const char vertexShaderCode[] =
"#version 330 core\n"
"layout (location = 0)in  vec2 position;\n"
"void main(void)\n"
"{\n"
"   gl_Position  = vec4(position,0.0,1.0);\n"
"}\n"
;




//-----------------------------------------------------------------------------
//glsShaderMerge
glsShaderMerge::glsShaderMerge(void)
	:glsShaderBase()
{
	const char fragmentShaderCode[] = 
"#version 330 core\n"
"precision highp float;\n"
"uniform ivec2	offset;\n"
"uniform sampler2D	texSrc0;\n"
"uniform sampler2D	texSrc1;\n"
"uniform sampler2D	texSrc2;\n"
"uniform sampler2D	texSrc3;\n"
"layout (location = 0) out vec4 dst;\n"
"void main(void)\n"
"{\n"
"	dst.r = texelFetch(texSrc0, ivec2(gl_FragCoord.xy)+offset,0).r;\n"
"	dst.g = texelFetch(texSrc1, ivec2(gl_FragCoord.xy)+offset,0).r;\n"
"	dst.b = texelFetch(texSrc2, ivec2(gl_FragCoord.xy)+offset,0).r;\n"
"	dst.a = texelFetch(texSrc3, ivec2(gl_FragCoord.xy)+offset,0).r;\n"
"\n"
"}\n"
;
	// Create and compile our GLSL program from the shaders
	LoadShadersCode(vertexShaderCode, fragmentShaderCode);
}


//-----------------------------------------------------------------------------
//glsShaderMergeU
glsShaderMergeU::glsShaderMergeU(void)
	:glsShaderBase()
{
	const char fragmentShaderCode[] =
"#version 330 core\n"
"precision highp float;\n"
"uniform ivec2	offset;\n"
"uniform usampler2D	texSrc0;\n"
"uniform usampler2D	texSrc1;\n"
"uniform usampler2D	texSrc2;\n"
"uniform usampler2D	texSrc3;\n"
"layout (location = 0) out uvec4 dst;\n"
"void main(void)\n"
"{\n"
"	dst.r = texelFetch(texSrc0, ivec2(gl_FragCoord.xy)+offset,0).r;\n"
"	dst.g = texelFetch(texSrc1, ivec2(gl_FragCoord.xy)+offset,0).r;\n"
"	dst.b = texelFetch(texSrc2, ivec2(gl_FragCoord.xy)+offset,0).r;\n"
"	dst.a = texelFetch(texSrc3, ivec2(gl_FragCoord.xy)+offset,0).r;\n"
"}\n"
;
	// Create and compile our GLSL program from the shaders
	LoadShadersCode(vertexShaderCode, fragmentShaderCode);
}

//-----------------------------------------------------------------------------
//glsShaderMergeS
glsShaderMergeS::glsShaderMergeS(void)
	:glsShaderBase()
{
	const char fragmentShaderCode[] =
"#version 330 core\n"
"precision highp float;\n"
"uniform ivec2	offset;\n"
"uniform isampler2D	texSrc0;\n"
"uniform isampler2D	texSrc1;\n"
"uniform isampler2D	texSrc2;\n"
"uniform isampler2D	texSrc3;\n"
"layout (location = 0) out ivec4 dst;\n"
"void main(void)\n"
"{\n"
"	dst.r = texelFetch(texSrc0, ivec2(gl_FragCoord.xy)+offset,0).r;\n"
"	dst.g = texelFetch(texSrc1, ivec2(gl_FragCoord.xy)+offset,0).r;\n"
"	dst.b = texelFetch(texSrc2, ivec2(gl_FragCoord.xy)+offset,0).r;\n"
"	dst.a = texelFetch(texSrc3, ivec2(gl_FragCoord.xy)+offset,0).r;\n"
"}\n"
;
	// Create and compile our GLSL program from the shaders
	LoadShadersCode(vertexShaderCode, fragmentShaderCode);
}




//---------------------------------------------------------------------------
//
static void glsMergeProcess(
	const glsShaderBase* shader,	//progmra ID
	const vector<GLuint>& texSrc,	//src texture IDs
	const Rect& rectSrc,			// Merge src rectangel
	const Rect& rectDst 			// Merge dst rectangel
	)
{
	const int offset[2] = { rectSrc.x, rectSrc.y };

	//program
	{
		glUseProgram(shader->program);
	}

	//uniform
	{
		glUniform2iv(glGetUniformLocation(shader->program, "offset"), 1,&offset[0]);
	}


	//Bind Texture
	{
		for (int id = 0; id < texSrc.size(); id++){
			glActiveTexture(GL_TEXTURE0 + id);
			glBindTexture(GL_TEXTURE_2D, texSrc[id]);
			string name = "texSrc" + to_string(id);
			glUniform1i(glGetUniformLocation(shader->program, name.c_str()), id);
		}
	}

	glsVAO vao(glGetAttribLocation(shader->program, "position"));
	//Viewport
	{
		glViewport(rectDst.x, rectDst.y, rectDst.width, rectDst.height);
	}

	//Render!!
	{
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glFlush();
	}

	GL_CHECK_ERROR();

	//	glFinish();

}

static 
glsShaderBase* selectShader(int type){
	glsShaderBase* shader = 0;
	switch (CV_MAT_DEPTH(type)){
	case(CV_32F) : shader = shaderMerge; break;
	case(CV_8U) :
	case(CV_16U) : shader = shaderMergeU; break;
	case(CV_8S) :
	case(CV_16S) :
	case(CV_32S) : shader = shaderMergeS; break;
	default: GLS_Assert(0);		//not implement
	}
	return shader;
}

//Merge texture
void glsMerge(const vector<glsMat>& plnSrc, glsMat& dst){
	int cn = (int)plnSrc.size();
	GLS_Assert(1 <= cn);
	GLS_Assert(cn <= 4);

	glsMat _dst = glsMat(plnSrc[0].size(), CV_MAKE_TYPE(plnSrc[0].depth(), cn), plnSrc[0].blkNum());

	glsShaderBase* shader = selectShader(plnSrc[0].type());

	Rect rectSrc(0, 0, plnSrc[0].texWidth(), plnSrc[0].texHeight());
	Rect rectDst = rectSrc;

	glsFBO fbo(1);

	for (int i = 0; i < plnSrc[0].texArray.size(); i++){

		//dst texture
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _dst.texArray[i], 0);
		GLS_Assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

		vector<GLuint> texSrc(cn);
		for (int c = 0; c < cn; c++){
			texSrc[c] = plnSrc[c].texArray[i];
		}

		glsMergeProcess(shader, texSrc, rectSrc, rectDst);

	}
	dst = _dst;
}


