/**
* file GLFWEW.cpp
*/
#include "GLFWEW.h"
#include <iostream>

/// GLFWEWをラップするための名前空間
namespace GLFWEW{
	/**
	* GLFW からのエラー報告を処理する
	*
	* @param erroeエラー番号
	* @paaram desc エラー内容
	*/
	void ErrorCallback(int error, const char* desc) {
		std::cerr << "ERROR: " << desc << std::endl;
	}

	/**
	* シングルトンインスタンスを取得する
	*
	* @return Window のシングルトンスタンス
	*/
	Window& Window::Instance() {
		static Window instance;
		return instance;
	}

	/**
	* コンストラクタ
	*/
	Window::Window() :isGLFWInitialized(false), isInitialized(false), window(nullptr) {

	}

	/**
	* デストラクタ
	*/
	Window::~Window() {
		if (isGLFWInitialized) {
			glfwTerminate();
		}
	}
	/**
	* GLFW/GLEWの初期化
	*
	* @param w ウィンドウの描画範囲の幅（ピクセル）
	* @param h ウィンドウの描画範囲の高さ（ピクセル）
	* @param title ウィンドウタイトル(UTF-8の０終端文字列
	*
	* @retval true 初期化成功
	* @retval false 初期化失敗
	*/
	bool Window::Init(int w, int h, const char* title) {
		
		if (isInitialized) {
			std::cerr << "ERROR: GLFWEWは既に初期化されています。" << std::endl;
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
			std::cerr << "ERROR: GLEWの初期化に失敗しました。" << std::endl;
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
	*ウィンドウズを閉じるべきか調べる
	*
	* @retval true 閉じる
	* @retval false 閉じない
	*/
	bool Window::ShouldClose() const {
		return glfwWindowShouldClose(window) != 0;
	}

	/**
	* フロントバッファとバックバッファを切り替える
	*/
	void Window::SwapBuffers() const {
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

} //namespace GLFWEW










































