#pragma once
#include "Mesh.h"
#include <GL\glew.h>
#include "stb_image.h"


class Model {
public:
	Mesh mesh;
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO; //element buffer object -> indices
	unsigned int shaderProgram;
	unsigned int texture;

	void Render() {


	}

	void Load(std::string modelName) {
		mesh.Import(modelName + ".obj");
	

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		//prepare raw data
		std::vector<float> vertexData;
		

		vertexData.reserve(mesh.vertices.size() * 3);
		for (int i = 0; i < mesh.vertices.size(); i++) {
			vertexData.push_back(mesh.vertices[i].x);
			vertexData.push_back(mesh.vertices[i].y);
			vertexData.push_back(mesh.vertices[i].z);
			vertexData.push_back(mesh.normals[i].x);
			vertexData.push_back(mesh.normals[i].y);
			vertexData.push_back(mesh.normals[i].z);
			vertexData.push_back(mesh.textureCoordinates[i].x);
			vertexData.push_back(mesh.textureCoordinates[i].y);
		}


		glGenVertexArrays(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		//prepare raw data
		std::vector<unsigned int> indexData;

		indexData.reserve(mesh.indices.size());
		for (auto index : mesh.indices) {
			indexData.push_back(index);
		}


		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexData.size() * sizeof(unsigned int), indexData.data(), GL_STATIC_DRAW);

		LoadShaders();
		LoadTexture(modelName + ".png");
	}

private:

	void LoadShaders() {


		//shaders
		const char* vertexShaderSource = "#version 330 core\n"
			"layout (location = 0) in vec3 aPos;\n"
			"layout (location = 1) in vec3 aNorm;\n"
			"layout (location = 2) in vec2 aTexCoord;\n"
			"out vec3 Normal;\n"
			"out vec2 TexCoord;\n"
			"out vec3 vertexPos;\n"
			"uniform mat4 viewMatrix;\n"
			"uniform mat4 normalMatrix;\n"
			"uniform mat4 modelMatrix;\n"


			"void main()\n"
			"{\n"
			"   gl_Position =viewMatrix * vec4(aPos, 1.0);\n"
			"   TexCoord = aTexCoord;\n"
			"   Normal = normalize(vec3(normalMatrix*vec4(aNorm , 0.0)));\n"
			"   vertexPos = vec3( modelMatrix * vec4(aPos, 1.0) );\n"
			"}\0";

		const char* fragmentShaderSource = "#version 330 core\n"
			"out vec4 FragColor;\n"
			"in vec3 Normal;\n"
			"in vec2 TexCoord;\n"
			"in vec3 vertexPos;\n"
			"uniform vec3 cameraPosition;\n"

			"uniform sampler2D ourTexture;\n"
			"uniform float specularAmount;\n"
			"uniform float diffuseAmount;\n"
			"void main()\n"
			"{\n"
			"  vec3 L1Dir = normalize(vec3(1, 0, 0.0));\n"
			"  vec3 L2Dir = normalize(vec3(-1, -0.7, -1));\n"
			"  vec3 L3Dir = vec3(0, 0, -1);\n"

			"  vec4 L1Color = vec4(0.0, 0.2, 0.4, 1);\n"
			"  vec4 L2Color = vec4(0.7, 0.8, 1, 1);\n"
			"  vec4 L3Color = vec4(0, 0, 0, 1);\n"

			"  vec4 L1Diffuse = L1Color *  max(dot(Normal, -L1Dir), 0.0);\n"
			"  vec4 L2Diffuse = L2Color *  max(dot(Normal, -L2Dir), 0.0);\n"
			"  vec4 L3Diffuse = L3Color *  max(dot(Normal, -L3Dir), 0.0);\n"

			//"  vec3 viewDir = normalize(cameraPosition - vertexPos);\n"
			"  vec3 viewDir = normalize( cameraPosition - vertexPos);\n"

			"  vec4 L1Specular = L1Color * pow(	max( dot(viewDir, reflect(L1Dir, Normal)), 0.0 ), 8);\n"
			"  vec4 L2Specular = L2Color * pow( max( dot(viewDir, reflect(L2Dir, Normal)), 0.0 ), 8);\n"
			"  vec4 L3Specular = L3Color * pow( max( dot(viewDir, reflect(L3Dir, Normal)), 0.0 ), 8);\n"
			" \n"
			//"  vec4 amb = 0.2f * texture(ourTexture, TexCoord);\n"

			"vec4 diffuse = texture(ourTexture, TexCoord) * (L1Diffuse + L2Diffuse + L3Diffuse);\n"
			"vec4 specular = L1Specular + L2Specular + L3Specular;\n"

			"  FragColor = diffuseAmount* diffuse + specularAmount*specular;\n"
			//"  FragColor =texture(ourTexture, TexCoord);\n "
			"}\0";

		unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
		glCompileShader(vertexShader);

		unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
		glCompileShader(fragmentShader);

		shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);
		glLinkProgram(shaderProgram);

		int  success;
		char infoLog[512];

		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		}

		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM\n" << infoLog << std::endl;
		}
	}

	bool LoadTexture(std::string textureFileName) {
		int width, height, nrChannels;
		stbi_set_flip_vertically_on_load(true);
		std::string texturePath = std::filesystem::current_path().parent_path().string() + "\\Assets\\"+textureFileName;
		unsigned char* data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);
		if (!data) { 
			std::cout << "Failed to load Player texture" << std::endl; 
			return false;
		}
		else {
			glGenTextures(1, &texture);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glUseProgram(shaderProgram);
			glUniform1i(glGetUniformLocation(shaderProgram, "texture"), 0);

			stbi_image_free(data);
			return true;
		}
	}
};