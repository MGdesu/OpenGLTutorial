/**
* file GLFWEW.cpp
*/
#include "GLFWEW.h"
#include <iostream>

/// GLFWEW�����b�v���邽�߂̖��O���
namespace GLFWEW{
	/**
	* GLFW ����̃G���[�񍐂���������
	*
	* @param erroe�G���[�ԍ�
	* @paaram desc �G���[���e
	*/
	void ErrorCallback(int error, const char* desc) {
		std::cerr << "ERROR: " << desc << std::endl;
	}

	/**
	* �V���O���g���C���X�^���X���擾����
	*
	* @return Window �̃V���O���g���X�^���X
	*/
	Window& Window::Instance() {
		static Window instance;
		return instance;
	}

	/**
	* �R���X�g���N�^
	*/
	Window::Window() :isGLFWInitialized(false), isInitialized(false), window(nullptr) {

	}

	/**
	* �f�X�g���N�^
	*/
	Window::~Window() {
		if (isGLFWInitialized) {
			glfwTerminate();
		}
	}
	/**
	* GLFW/GLEW�̏�����
	*
	* @param w �E�B���h�E�̕`��͈͂̕��i�s�N�Z���j
	* @param h �E�B���h�E�̕`��͈͂̍����i�s�N�Z���j
	* @param title �E�B���h�E�^�C�g��(UTF-8�̂O�I�[������
	*
	* @retval true ����������
	* @retval false ���������s
	*/
	bool Window::Init(int w, int h, const char* title) {
		
		if (isInitialized) {
			std::cerr << "ERROR: GLFWEW�͊��ɏ���������Ă��܂��B" << std::endl;
			return false;
		}
		if (!isGLFWInitialized) {
			glfwSetErrorCallback(ErrorCallback);
			if (glfwInit() != GL_TRUE) {
				return false;
			}
			isGLFWInitialized = true;

		}
		
		if (!window) {
			 window = glfwCreateWindow(w, h, title, nullptr, nullptr);
			 if (!window) {
				 return false;
			}
			 glfwMakeContextCurrent(window);
		}

		if (glewInit() != GLEW_OK) {
			std::cerr << "ERROR: GLEW�̏������Ɏ��s���܂����B" << std::endl;
			return false;
		}

		const GLubyte*renderer = glGetString(GL_RENDERER);
		std::cout << "Renderer: " << renderer << std::endl;
		const GLubyte*version = glGetString(GL_VERSION);
		std::cout << "Version: " << version << std::endl;
		const GLubyte*vendor = glGetString(GL_VENDOR);
		std::cout << "Vendor: " << vendor << std::endl;

		isInitialized = true;

		return true;
	}

	/**
	*�E�B���h�E�Y�����ׂ������ׂ�
	*
	* @retval true ����
	* @retval false ���Ȃ�
	*/
	bool Window::ShouldClose() const {
		return glfwWindowShouldClose(window) != 0;
	}

	/**
	* �t�����g�o�b�t�@�ƃo�b�N�o�b�t�@��؂�ւ���
	*/
	void Window::SwapBuffers() const {
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

} //namespace GLFWEW









































