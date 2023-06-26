#include <iostream>
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "Shader.h"
#include "Model.h"
#include "Camera.h"
#include "Texture.h"

//��������Ӧ����
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

float screen_width = 1280.0f;          //���ڿ��
float screen_height = 720.0f;          //���ڸ߶�
									   //�������ز���

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

	// ��ʼ��GLFW
	glfwInit();                                                     // ��ʼ��GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);                  // OpenGL�汾Ϊ3.3�����ΰ汾�ž���Ϊ3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // ʹ�ú���ģʽ(������������)
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // ���ʹ�õ���Mac OS Xϵͳ�����������
	glfwWindowHint(GLFW_RESIZABLE, FALSE);						    // ���ɸı䴰�ڴ�С
																	// ��������(���ߡ���������)
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

	// ��ʼ��GLAD������OpenGL����ָ���ַ�ĺ���
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// ָ����ǰ�ӿڳߴ�(ǰ��������Ϊ���½�λ�ã���������������Ⱦ���ڿ���)
	glViewport(0, 0, screen_width, screen_height);
	Shader shader("res/shader/Model.vs", "res/shader/Model.fs");//������ɫ��
	Shader normalShader("res/shader/normal_visualization.vs", "res/shader/normal_visualization.fs", "res/shader/normal_visualization.gs");
	Model ourModel(path);







	glEnable(GL_DEPTH_TEST);	

	if (PolygonMode == 1)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //��������ģʽ
	if (PolygonMode == 2)
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); //���õ�ģʽ

	glPointSize(point_size);//��Ĵ�С����
	glLineWidth(line_width);//�߿�����
	













	// ��Ⱦѭ��
	while (!glfwWindowShouldClose(window)) {

		//����ÿ֡��ʱ���
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		processInput(window);

		glClearColor(0.0f, 0.34f, 0.57f, 1.0f); //������ɫ�趨
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//������ɫ�������Ȼ���

		shader.Use();

		// Transform����任����
		glm::mat4 model(1);//model���󣬾ֲ�����任����������
		model = glm::translate(model, glm::vec3(0.0f, -0.7f, 0.0f)); 
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	
		glm::mat4 view(1);//view������������任���۲�����ϵ
		view = camera.GetViewMatrix();
		glm::mat4 projection(1);//projection����ͶӰ����
		projection = glm::perspective(glm::radians(camera.Zoom), screen_width / screen_height, 0.1f, 100.0f);

		// ����ɫ���д������
		shader.SetMat4("model", model);
		shader.SetMat4("view", view);
		shader.SetMat4("projection", projection);
		shader.SetVec3("lightPos", lightPos);
		shader.SetVec3("viewPos", camera.Position);

		//����
		ourModel.Draw(shader);

		if (display_normal_input == '1') {

			//���Ʒ�����
			normalShader.Use();
			normalShader.SetMat4("projection", projection);
			normalShader.SetMat4("view", view);
			normalShader.SetMat4("model", model);

			ourModel.Draw(normalShader);
		}


		glfwSwapBuffers(window);
		glfwPollEvents();


	}


	// �������е���Դ����ȷ�˳�����
	glfwTerminate();
	return 0;
}

//��������
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

//���ڷֱ�������Ӧ
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

//����ƶ���Ӧ
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
//��������Ӧ
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}
