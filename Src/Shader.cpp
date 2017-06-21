/**
* @file Shader.cpp
*/
#include "Shader.h"
#include <vector>
#include <iostream>
#include <cstdint>
#include <stdio.h>
#include <sys/stat.h>

namespace Shader {

	/**
	* シェーダープログラムを作成する
	* 
	* @param vsCode 頂点シェダーコードへのポインタ
	* @param fsCode フラグメントシェーダーコードへのポイント
	*
	* @return 作成したプログラムオブジェクト
	*/
	ProgramPtr Program::Create(const char*vsFilename, const char*fsFilename)
	{
		struct Impl :Program { Impl() {} ~Impl() {} };
		ProgramPtr p = std::make_shared<Impl>();
		if (!p) {
			std::cerr << "ERROR:うろグラム'" << vsFilename << "'の作成に失敗" << std::endl;
			return{};
		}
		p->program = CreateProgramFromFile(vsFilename, fsFilename);
		if (!p->program) {
			return{};
		}

		//サンプラーの数と位置を取得する
		GLint activeUniforms;
		glGetProgramiv(p->program, GL_ACTIVE_UNIFORMS, &activeUniforms);
		for (int i = 0;i < activeUniforms;i++) {
			GLint size;
			GLenum type;
			GLchar name[128];
			glGetActiveUniform(p->program, i, sizeof(name), nullptr, &size, &type, name);
			if (type == GL_SAMPLER_2D) {
				p->samplerCount = size;
				p->samplerLocation = glGetUniformLocation(p->program, name);
				if (p->samplerLocation < 0) {
					std::cerr << "ERRROR:プログラム'" << vsFilename << "'の作成に失敗" << std::endl;
					return{};
				}
				break;
			}
		}

		//頂点シェーダーファイル名の末尾から".vert"を取り除いたものをプログラム名とする
		p->name = vsFilename;
		p->name.resize(p->name.size() - 4);

		return p;
	}
	
	/**
	* デストラクタ
	*/
	Program::~Program()
	{
		if (program) {
			glDeleteProgram(program);
		}
	}

	/**
	*
	* @param blockName 割り当てるUniformブロックの名前
	* @param bindingPoint 割り当て先のバインディングポイント
	*
	* @return true 割り当て成功
	* @return false 割り当て失敗
	*/
	bool Program::UniformBlockBinding(const char*blockName, GLuint bindingPoint)
	{
		const GLuint blockIndex = glGetUniformBlockIndex(program, blockName);
		if (blockIndex == GL_INVALID_INDEX) {
			std::cerr << "ERROR(" << name << "):Uniformブロック'" << blockName << "'が見つかりません" << std::endl;
			return false;
		}
		glUniformBlockBinding(program, blockIndex, bindingPoint);
		const GLenum result = glGetError();
		if (result != GL_NO_ERROR) {
			std::cerr << "ERROR(" << name << "):Uniformブロック'" << blockName << "'のバインドに失敗" << std::endl;
			return false;
		}
		return true;
	}

	/**
	* 描画用プログラムに設定
	*/
	void Program::UseProgram()
	{
		glUseProgram(program);
		for (GLint i = 0;i < samplerCount;i++) {
			glUniform1i(samplerLocation + i, i);
		}
	}
	/**
	* テクスチャをテクスチャ・イメージ・ユニットに割り当てる
	* 
	* @param unit 割り当て先のテクスチャ・イメージ・ユニット番号(GL_TEXTURE0~).
	* @param type w利宛るテクスチャの種類(GL_TEXTURE_1D,GL_TEXTURE_2D,etc).
	* @param texture 割り当てるテクスチャオブジェクト
	*/
	void Program::BindTexture(GLenum unit, GLenum type, GLuint texture)
	{
		if (unit >= GL_TEXTURE0&&unit < static_cast<GLenum>(GL_TEXTURE0 + samplerCount)) {
			glActiveTexture(unit);
			glBindTexture(type, texture);
		}
	}

	/**
	* シェダーコンパイルする
	*
	* @param type シェダーの種類
	* @param string シェダーボードへのポインタ
	*
	* @return 作成したシェダーオブジェクト
	*/
	GLuint CompileShader(GLenum type, const GLchar* string)
	{
		GLuint shader = glCreateShader(type);
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
	
	/**
	* プログラムオブジェクトを生成
	*
	* @param vsCode 頂点シェダーコードへのポインタ
	* @param fsCode フラグメントシェダーコードへのポインタ
	*
	* @return 作成したプログラムオブジェクト
	*/
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
		if (linkStatus != GL_TRUE) {
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
	/**
	* シェダーコンパイルする
	*
	* @param type シェダーの種類
	* @param string シェダーボードへのポインタ
	*
	* @return 作成したシェダーオブジェクト
	*/
	bool ReadFile(const char*  filename, std::vector<char>&buf)
	{
		struct  stat st;
		if (stat(filename, &st)) {
			return false;
		}
		FILE*fp = fopen(filename, "rb");
		if (!fp) {
			return false;
		}
		buf.resize(st.st_size + 1);
		const size_t readSize = fread(buf.data(), 1, st.st_size, fp);
		fclose(fp);
		if (readSize != st.st_size) {
			return false;
		}
		buf.back() = '\0';
		return true;
	}


	/**
	* ファイルからシェダープログラムを作成
	*
	* @param vsCode 頂点シェダーファイル名
	* @param fsCode フラグメントシェダーファイル名
	*
	* @return 作成したプログラムオブジェクト
	*/
	GLuint CreateProgramFromFile(const char* vsFilename, const char* fsFilename)
	{
		std::vector<char>vsBuf;
		if (!ReadFile(vsFilename, vsBuf)) {
			std::cerr << "ERROR in Shader::CreateProgramFromFile:\n" <<
				vsFilename << "を読み込めません" << std::endl;
			return 0;
		}
		std::vector<char>fsBuf;
		if (!ReadFile(fsFilename, fsBuf)) {
			std::cerr << "ERROR in Shader::CreateProgramFromFile:\n" <<
				fsFilename << "を読み込めません" << std::endl;
			return 0;
		}
		return CreateShaderProgram(vsBuf.data(), fsBuf.data());

	}
}






























