#include <iostream>
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "Shader.h"
#include "Model.h"
#include "Camera.h"
#include "Texture.h"

//鼠标键盘响应函数
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

float screen_width = 1280.0f;          //窗口宽度
float screen_height = 720.0f;          //窗口高度
									   //摄像机相关参数

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = screen_width / 2.0f;
float lastY = screen_height / 2.0f;
bool firstMouse = true;

glm::vec3 lightPos(8.0f,8.0f,5.0f);
float deltaTime = 0.0f;
float lastFrame = 0.0f;


int main() {
	string path;
	char is_default = '1';
	char display_normal_input = '0';
	int line_width = 1;
	int PolygonMode = 0;
	int point_size = 1;
	glm::vec3 colortest(1.0f, 0.5f, 0.2f);
	std::cout << "Please input path"<<endl;
	std::cin >> path;

	std::cout << "Whether run with default configs" << endl;
	std::cout << "0 is no , 1 is yes" << endl;
	std::cin >> is_default;

	if (is_default == '1') {

	}
	else {
		std::cout << "Choose the PolygonMode" << endl;
		std::cout << "0 is NO , 1 is LINE , 2 is POINT" << endl;
		std::cin >> PolygonMode;

		if(PolygonMode == 1){
			std::cout << "Set the width of the line " << endl;
			std::cin >> line_width;
		}

		if (PolygonMode == 2) {
			std::cout << "Set the size of point " << endl;
			std::cin >> point_size;
		}

		std::cout << "Whether display the normal" << endl;
		std::cout << "0 is non-display , 1 is display" << endl;
		std::cin >> display_normal_input;

	}

	// 初始化GLFW
	glfwInit();                                                     // 初始化GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);                  // OpenGL版本为3.3，主次版本号均设为3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 使用核心模式(无需向后兼容性)
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 如果使用的是Mac OS X系统，需加上这行
	glfwWindowHint(GLFW_RESIZABLE, FALSE);						    // 不可改变窗口大小
																	// 创建窗口(宽、高、窗口名称)
	auto window = glfwCreateWindow(screen_width, screen_height, "3D_MODEL_DISPLAY", nullptr, nullptr);
	if (window == nullptr) {
		std::cout << "Failed to Create OpenGL Context" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// 初始化GLAD，加载OpenGL函数指针地址的函数
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// 指定当前视口尺寸(前两个参数为左下角位置，后两个参数是渲染窗口宽、高)
	glViewport(0, 0, screen_width, screen_height);
	Shader shader("res/shader/Model.vs", "res/shader/Model.fs");//加载着色器
	Shader normalShader("res/shader/normal_visualization.vs", "res/shader/normal_visualization.fs", "res/shader/normal_visualization.gs");
	Model ourModel(path);







	glEnable(GL_DEPTH_TEST);	

	if (PolygonMode == 1)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //采用网格模式
	if (PolygonMode == 2)
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); //采用点模式

	glPointSize(point_size);//点的大小设置
	glLineWidth(line_width);//线宽设置
	













	// 渲染循环
	while (!glfwWindowShouldClose(window)) {

		//计算每帧的时间差
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		processInput(window);

		glClearColor(0.0f, 0.34f, 0.57f, 1.0f); //背景颜色设定
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//清理颜色缓冲和深度缓冲

		shader.Use();

		// Transform坐标变换矩阵
		glm::mat4 model(1);//model矩阵，局部坐标变换至世界坐标
		model = glm::translate(model, glm::vec3(0.0f, -0.7f, 0.0f)); 
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	
		glm::mat4 view(1);//view矩阵，世界坐标变换至观察坐标系
		view = camera.GetViewMatrix();
		glm::mat4 projection(1);//projection矩阵，投影矩阵
		projection = glm::perspective(glm::radians(camera.Zoom), screen_width / screen_height, 0.1f, 100.0f);

		// 向着色器中传入参数
		shader.SetMat4("model", model);
		shader.SetMat4("view", view);
		shader.SetMat4("projection", projection);
		shader.SetVec3("lightPos", lightPos);
		shader.SetVec3("viewPos", camera.Position);

		//绘制
		ourModel.Draw(shader);

		if (display_normal_input == '1') {

			//绘制法向量
			normalShader.Use();
			normalShader.SetMat4("projection", projection);
			normalShader.SetMat4("view", view);
			normalShader.SetMat4("model", model);

			ourModel.Draw(normalShader);
		}


		glfwSwapBuffers(window);
		glfwPollEvents();


	}


	// 清理所有的资源并正确退出程序
	glfwTerminate();
	return 0;
}

//键盘输入
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		camera.ProcessKeyboard(TOP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		camera.ProcessKeyboard(SIDE, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
		camera.ProcessKeyboard(FRONT, deltaTime);
}

//窗口分辨率自适应
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

//鼠标移动响应
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}
//鼠标滚轮响应
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}
