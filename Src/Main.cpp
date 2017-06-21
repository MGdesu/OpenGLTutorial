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

//3D�x�N�^�[�^
struct  Vector3
{
	float x, y, z;
};

//RGBA�J���[�^
struct Color
{
	float r, g, b, a;
};

//���_�f�[�^�^
struct Vertex
{
	glm::vec3 posision;//<���W

	glm::vec4 color;//<�F

	glm::vec2 texCoord;//<�e�N�X�`�����W

};

//���_�f�[�^
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
//�C���f�b�N�X�f�[�^
const GLuint indices[] = {
	0,1,2,2,3,0,
	4,5,6,7,8,9,
	10,11,12,12,13,10,
	
};

//���_�V�F�_�̃p�����[�^�^
struct VertexData
{
	glm::mat4 matMVP;
	glm::vec4 lightPosition;
	glm::vec4 lightColor;
	glm::vec4 ambientColor;
};

/**
* ���C�g�f�[�^�i�_�����j
*/
struct PointLight
{
	glm::vec4 position;
	glm::vec4 color;
};

const int maxLightCount = 4;

/**
* ���C�e�B���O�p�����[�^
*/
struct LightData
{
	glm::vec4 ambientColor;
	PointLight light[maxLightCount];
};

/**
* �|�X�g�G�t�F�N�g�f�[�^
*/
struct PostEffectData
{
	glm::mat4x4 matColor;
};

/**
* �����`��f�[�^
*/
struct  RenderingPart
{
	GLsizei size;
	GLvoid*offset;
};

/**
* RenderingPart���쐬����
* 
* @param size �`�悷��C���f�b�N�X��
* @param offset �`��J�n�C���f�b�N�X�̃I�t�Z�b�g�i�C���f�b�N�X�P�ʁj
*
* @return �쐬���������`��I�u�W�F�N�g
*/
constexpr RenderingPart MakeRenderingPart(GLsizei size, GLsizei offset) {
	return{ size,reinterpret_cast<GLvoid*>(offset * sizeof(GLuint)) };
}

/**
* �����`��f�[�^
*/
static const RenderingPart renderingParts[] = {
	MakeRenderingPart(12,0),
	MakeRenderingPart(6,12),
};


/**
* Vertex Buffer object �𐶐�
*
* @param size ���_�f�[�^�̃T�C�Y
* @param data ���_�f�[�^�ւ̃|�C���^
*
* @return �쐬���� VBO.
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
* Index Buffer object �𐶐�
*
* @param size �C���f�b�N�X�f�[�^�̃T�C�Y
* @param data �C���f�b�N�X�f�[�^�̃f�[�^�ւ̃|�C���^
*
* @return �쐬���� IBO.
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
* ���_�A�g���r���[�g��ݒ肷��
*
* @param index ���_�A�g���r���[�Ƃ̃C���f�b�N�X
* @param cls ���_�f�[�^�^��
* @param mbr ���_�A�h���r���[�g�ɐݒ肷�邃���������o�ϐ���
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
* Vertex Array object �𐶐�
*
* @param vbo VAO �Ɋ֘A�t����ꂽVBO
* @param ibo VAO�Ɋ֘A�t����ꂽIBO
*
* @return �쐬���� VAO.
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
* Uniform Block object �𐶐�
*
* @param size Uniform Block�̃T�C�Y
* @param data Uniform Block�ɓ]������f�[�^�ւ̃|�C���^
*
* @return �쐬���� UBO.
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
* �V�F�_�[�R���p�C������
*
* @param type �V�F�_�[�̎��
* @param string �V�F�_�[�{�[�h�ւ̃|�C���^
*
* @return �쐬�����V�F�_�[�I�u�W�F�N�g

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
				std::cerr << "ERROR:�V�F�_�[�̃R���p�C���Ɏ��s\n" << buf.data() << std::endl;
			}
		}
		glDeleteShader(shader);
		return 0;
	}
	return shader;
}
*/
/**
* �v���O�����I�u�W�F�N�g�𐶐�
*
* @param vsCode ���_�V�F�_�[�R�[�h�ւ̃|�C���^
* @param fsCode �t���O�����g�V�F�_�[�R�[�h�ւ̃|�C���^
*
* @return �쐬�����v���O�����I�u�W�F�N�g

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
				std::cerr << "ERROR:�V�F�_�[�̃����N�Ɏ��s\n" << buf.data() << std::endl;
			}
		}
		glDeleteProgram(program);
		return 0;
	}
	return program;

}
*/
///�G���g���[�|�C���g
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
	
	//�e�N�X�`���f�[�^
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

































