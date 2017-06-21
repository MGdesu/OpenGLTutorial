/**
* @file Main.cpp
*/
#include "GLFWEW.h"
#include "glm/gtc/matrix_transform.hpp"
#include <iostream>
#include <vector>
#include "Texture.h"
#include "Shader.h"
#include "OffscreenBuffer.h"
#include"UniformBuffer.h"

//3Dベクター型
struct  Vector3
{
	float x, y, z;
};

//RGBAカラー型
struct Color
{
	float r, g, b, a;
};

//頂点データ型
struct Vertex
{
	glm::vec3 posision;//<座標

	glm::vec4 color;//<色

	glm::vec2 texCoord;//<テクスチャ座標

};

//頂点データ
const Vertex vertices[] = {
	
	{ { -0.5f,-0.3f, 0.5f},{ 0.0f,0.0f,1.0f,1.0f },{0.0f,0.0f} },
	{ { 0.3f,-0.3f, 0.5f },{ 0.0f,1.0f,0.0f,1.0f },{ 1.0f,0.0f } },
	{ { 0.3f, 0.5f, 0.5f },{ 0.0f,0.0f,1.0f,1.0f },{ 1.0f,1.0f } },
	{ { -0.5f, 0.5f, 0.5f},{ 1.0f,0.0f,0.0f,1.0f },{ 0.0f,1.0f } },
	
	{ { -0.3f,0.3f,0.1f },{ 0.0f,0.0f,1.0f,1.0f },{ 0.0f,1.0f } },
	{ { -0.3f,-0.5f,0.1f },{ 0.0f,1.0f,1.0f,1.0f },{ 0.0f,0.0f } },
	{ { 0.5f,-0.5f,0.1f },{ 0.0f,0.0f,1.0f,1.0f } ,{ 1.0f,0.0f } },

	{ { 0.5f,-0.5f,0.1f },{ 1.0f,0.0f,0.0f,1.0f },{ 1.0f,0.0f } },
	{ { 0.5f,0.3f,0.1f },{ 1.0f,1.0f,1.0f,1.0f },{ 1.0f,1.0f } },
	{ { -0.3f,0.3f,0.1f },{ 1.0f,0.0f,0.0f,1.0f } ,{ 0.0f,1.0f } },

	{ { -1.0f,-1.0f, 0.5f },{ 1.0f,1.0f,1.0f,1.0f },{ 1.0f,0.0f } },
	{ { 1.0f,-1.0f, 0.5f },{ 1.0f,1.0f,1.0f,1.0f },{ 0.0f,0.0f } },
	{ { 1.0f, 1.0f, 0.5f },{ 1.0f,1.0f,1.0f,1.0f },{ 0.0f,1.0f } },
	{ { -1.0f, 1.0f, 0.5f },{ 1.0f,1.0f,1.0f,1.0f },{ 1.0f,1.0f } },

};
//インデックスデータ
const GLuint indices[] = {
	0,1,2,2,3,0,
	4,5,6,7,8,9,
	10,11,12,12,13,10,
	
};

//頂点シェダのパラメータ型
struct VertexData
{
	glm::mat4 matMVP;
	glm::vec4 lightPosition;
	glm::vec4 lightColor;
	glm::vec4 ambientColor;
};

/**
* ライトデータ（点光源）
*/
struct PointLight
{
	glm::vec4 position;
	glm::vec4 color;
};

const int maxLightCount = 4;

/**
* ライティングパラメータ
*/
struct LightData
{
	glm::vec4 ambientColor;
	PointLight light[maxLightCount];
};

/**
* ポストエフェクトデータ
*/
struct PostEffectData
{
	glm::mat4x4 matColor;
};

/**
* 部分描画データ
*/
struct  RenderingPart
{
	GLsizei size;
	GLvoid*offset;
};

/**
* RenderingPartを作成する
* 
* @param size 描画するインデックス数
* @param offset 描画開始インデックスのオフセット（インデックス単位）
*
* @return 作成した部分描画オブジェクト
*/
constexpr RenderingPart MakeRenderingPart(GLsizei size, GLsizei offset) {
	return{ size,reinterpret_cast<GLvoid*>(offset * sizeof(GLuint)) };
}

/**
* 部分描画データ
*/
static const RenderingPart renderingParts[] = {
	MakeRenderingPart(12,0),
	MakeRenderingPart(6,12),
};


/**
* Vertex Buffer object を生成
*
* @param size 頂点データのサイズ
* @param data 頂点データへのポインタ
*
* @return 作成した VBO.
*/
GLuint CreateVBO(GLsizeiptr size, const GLvoid*data)
{
	GLuint vbo = 0;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	return vbo;
}

/**
* Index Buffer object を生成
*
* @param size インデックスデータのサイズ
* @param data インデックスデータのデータへのポインタ
*
* @return 作成した IBO.
*/
GLuint CreateIBO(GLsizeiptr size, const GLvoid* data)
{
	GLuint ibo = 0;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,size,data,GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
	return ibo;
}

/**
* 頂点アトリビュートを設定する
*
* @param index 頂点アトリビューとのインデックス
* @param cls 頂点データ型名
* @param mbr 頂点アドリビュートに設定するｃｌｓメンバ変数名
*/
#define SetVertexAttribPointer(index,cls,mbr)SetVertexArribPointerI(\
	index,\
	sizeof(cls::mbr)/sizeof(float),\
	sizeof(cls),\
	reinterpret_cast<GLvoid*>(offsetof(cls,mbr)))

void SetVertexArribPointerI(
	GLuint index, GLint size, GLsizei stride, const GLvoid*pointer)
{
	glEnableVertexAttribArray(index);
	glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, stride, pointer);
}

/**
* Vertex Array object を生成
*
* @param vbo VAO に関連付けられたVBO
* @param ibo VAOに関連付けられたIBO
*
* @return 作成した VAO.
*/
GLuint CreateVao(GLuint vbo,GLuint ibo)
{
	GLuint vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	SetVertexAttribPointer(0, Vertex, posision);
	SetVertexAttribPointer(1, Vertex, color);
	SetVertexAttribPointer(2, Vertex, texCoord);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBindVertexArray(0);
	return vao;
}

/**
* Uniform Block object を生成
*
* @param size Uniform Blockのサイズ
* @param data Uniform Blockに転送するデータへのポインタ
*
* @return 作成した UBO.
*/
GLuint CreateUBO(GLsizeiptr size, const GLvoid* data = nullptr)
{
	GLuint ubo;
	glGenBuffers(1, &ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferData(GL_UNIFORM_BUFFER, size, data, GL_STREAM_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	return ubo;
}

/**
* シェダーコンパイルする
*
* @param type シェダーの種類
* @param string シェダーボードへのポインタ
*
* @return 作成したシェダーオブジェクト

GLuint CompileShader(GLenum type, const GLchar* string)
{
	GLuint shader= glCreateShader(type);
	glShaderSource(shader, 1, &string, nullptr);
	glCompileShader(shader);
	GLint compiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		GLint infoLen = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
		if (infoLen) {
			std::vector<char>buf;
			buf.resize(infoLen);
			if (static_cast<int>(buf.size()) >= infoLen) {
				glGetShaderInfoLog(shader, infoLen, NULL, buf.data());
				std::cerr << "ERROR:シェダーのコンパイルに失敗\n" << buf.data() << std::endl;
			}
		}
		glDeleteShader(shader);
		return 0;
	}
	return shader;
}
*/
/**
* プログラムオブジェクトを生成
*
* @param vsCode 頂点シェダーコードへのポインタ
* @param fsCode フラグメントシェダーコードへのポインタ
*
* @return 作成したプログラムオブジェクト

GLuint CreateShaderProgram(const GLchar*vsCode, const GLchar* fsCode)
{
	GLuint vs = CompileShader(GL_VERTEX_SHADER, vsCode);
	GLuint fs = CompileShader(GL_FRAGMENT_SHADER, fsCode);
	if (!vs || !fs) {
		return 0;
	}

	GLuint program = glCreateProgram();
	glAttachShader(program, fs);
	glDeleteShader(fs);
	glAttachShader(program, vs);
	glDeleteShader(vs);
	glLinkProgram(program);
	GLint linkStatus = GL_FALSE;
	glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
	if (linkStatus!=GL_TRUE) {
		GLint infoLen = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLen);
		if (infoLen) {
			std::vector<char>buf;
			buf.resize(infoLen);
			if (static_cast<int>(buf.size()) >= infoLen) {
				glGetProgramInfoLog(program, infoLen, NULL, buf.data());
				std::cerr << "ERROR:シェダーのリンクに失敗\n" << buf.data() << std::endl;
			}
		}
		glDeleteProgram(program);
		return 0;
	}
	return program;

}
*/
///エントリーポイント
int main() {

	GLFWEW::Window&window = GLFWEW::Window::Instance();
	if (!window.Init(800, 600, "OpenGl Tutorial")) {
		return 1;
	}

	const GLuint vbo = CreateVBO(sizeof(vertices), vertices);
	const GLuint ibo = CreateIBO(sizeof(indices), indices);
	const GLuint vao = CreateVao(vbo,ibo);
	const UniformBufferPtr uboVertex = UniformBuffer::Create(
		sizeof(VertexData), 0, "VertexData");
	const UniformBufferPtr uboLight = UniformBuffer::Create(
		sizeof(LightData), 1, "LightData");
	const UniformBufferPtr uboPostEffect =UniformBuffer::Create(
		sizeof(PostEffectData), 2, "PostEffectData");
	const Shader::ProgramPtr progTutorial=
		Shader::Program::Create("Res/Tutoria.vert", "Res/Tutorial.frag");
	const Shader::ProgramPtr progColorFiler =
		Shader::Program::Create("Res/Posterization.vert", "Res/Posterization.frag");
	if (!vbo || !ibo || !vao || !uboVertex || !uboLight || !progTutorial || !progColorFiler) {
		return 1;
	}
	progTutorial->UniformBlockBinding("VertexData", 0);
	progTutorial->UniformBlockBinding("LightData", 1);
	//progColorFiler->UniformBlockBinding("PostEffectData", 2);

	//const GLuint uboIndex = glGetUniformBlockIndex(shaderProgram, "VertexData");
	//if (uboIndex == GL_INVALID_INDEX) {
		///return 1;
	//}
	//glUniformBlockBinding(shaderProgram, uboIndex, 0);
	
	//テクスチャデータ
	static const uint32_t textureData[] = {
		0xffffffff,0xffcccccc,0xffffffff,0xffcccccc,0xffffffff,
		0xff888888,0xffffffff,0xff888888,0xffffffff,0xff888888,
		0xffffffff,0xff444444,0xffffffff,0xff444444,0xffffffff,
		0xff000000,0xffffffff,0xff000000,0xffffffff,0xff000000,
		0xffffffff,0xff000000,0xffffffff,0xff000000,0xffffffff,
	};

	

	//TexturePtr tex = Texture::Create(5, 5, GL_RGBA8, GL_RGBA, textureData);
	TexturePtr tex = Texture::LoadFromFile("Res/kuribo.bmp");

	if (!tex) {
		return 1;
	}

	glEnable(GL_DEPTH_TEST);
	
	float i =0.0f;

	const OffscreenBufferPtr offscreen = OffscreenBuffer::Create(800, 600);

	while (!window.ShouldClose()) {
		
		glBindFramebuffer(GL_FRAMEBUFFER, offscreen->GetFramebuffer());

		glClearColor(0.1f, 0.3f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		static float degree = 0.0f;
		degree +=0.0f;
		if (degree >= 360.0f) { degree -= 360.0f; }
		const glm::vec3 viewPos = glm::rotate(
			glm::mat4(), glm::radians(degree), glm::vec3(0, 1, 0))*glm::vec4(2, 3, 3, 1);

		//glUseProgram(shaderProgram);
		progTutorial->UseProgram();

		//const GLint matMVPLoc = glGetUniformLocation(shaderProgram, "matMVP");
		const glm::mat4x4 matProj =
			glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
		const glm::mat4x4 matView =
			glm::lookAt(viewPos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

		VertexData vertexData;
		vertexData.matMVP = matProj *matView;
		uboVertex->BufferSubData(&vertexData);

		LightData lightData;
		lightData.ambientColor = glm::vec4(0.05f, 0.1f,0.2f, 1);
		lightData.light[0].position = glm::vec4(1, 1, 1, 1);
		lightData.light[0].color = glm::vec4(2, 2, 2, 1);
		lightData.light[1].position = glm::vec4(-0.2f, 0, 0.6f ,1);
		lightData.light[1].color = glm::vec4(0.125f, 0.125f, 0.05f, 1);
		uboLight->BufferSubData(&lightData);
      
		progTutorial->BindTexture(GL_TEXTURE0, GL_TEXTURE_2D, tex->Id());
		glBindVertexArray(vao);

		glDrawElements(
			GL_TRIANGLES, renderingParts[0].size,GL_UNSIGNED_INT, renderingParts[0].offset
		);


		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.5f, 0.3f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		progColorFiler->UseProgram();
		progTutorial->BindTexture(GL_TEXTURE0, GL_TEXTURE_2D, offscreen->GetTexuter());

		PostEffectData postEffect;
		postEffect.matColor[0] = glm::vec4(0.393f, 0.349f, 0.272f, 0);
		postEffect.matColor[1] = glm::vec4(0.769f, 0.686f, 0.534f, 0);
		postEffect.matColor[2] = glm::vec4(0.189f, 0.168f, 0.131f, 0);
		postEffect.matColor[3] = glm::vec4(0, 0, 0, 1);
		uboPostEffect->BufferSubData(&postEffect);
		glDrawElements(
			GL_TRIANGLES, renderingParts[1].size,GL_UNSIGNED_INT, renderingParts[1].offset
		);
		window.SwapBuffers();
		
	}

	//glDeleteProgram(shaderProgram);
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);

	return 0;
}

































