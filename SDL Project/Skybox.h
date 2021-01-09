#pragma once
#include <SDL\SDL.h>
#include <GL\glew.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <iostream>
#include "stb_image.h"
#include <filesystem>
#include "Camera.h"



class Skybox
{

public:
	unsigned int VAO;
	unsigned int VBO;
	unsigned int skyboxTexture;
	unsigned int shaderProgram;


	void Render(Camera& camera) {
		glm::mat4 viewProjectionMatrix = camera.projectionMatrix * glm::mat4(glm::mat3(camera.viewMatrix));
		glUseProgram(shaderProgram);
		glDepthMask(GL_FALSE);

		unsigned int loc_viewProjectionMatrix = glGetUniformLocation(shaderProgram, "viewProjectionMatrix");
		glUniformMatrix4fv(loc_viewProjectionMatrix, 1, GL_FALSE, glm::value_ptr(viewProjectionMatrix));

		glBindVertexArray(VAO);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glDepthMask(GL_TRUE);
	}

	void Load() {
		float skyboxVertices[] = {
			// positions          
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f
		};
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		glGenVertexArrays(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
		

		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(2);

		
		
		const char* vertexShaderSource = R"(
			#version 330 core
			layout (location = 2) in vec3 aPos;
			
			out vec3 TexCoords;
			
			uniform mat4 viewProjectionMatrix;
			
			void main()
			{
			    TexCoords = vec3(-aPos.x, aPos.y, -aPos.z);
			    gl_Position = viewProjectionMatrix * vec4(aPos, 1.0);
			}  
		)";
			
		const char* fragmentShaderSource = R"(
			#version 330 core
			out vec4 FragColor;

			in vec3 TexCoords;

			uniform samplerCube skyboxTexture;

			void main()
			{    
				FragColor = texture(skyboxTexture, TexCoords);
				//FragColor = vec4(TexCoords,1);
			}
)";
			
			


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

		glActiveTexture(GL_TEXTURE10);
		glGenTextures(1, &skyboxTexture);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);

		int width, height, nrChannels;
		stbi_set_flip_vertically_on_load(false);

			for (unsigned int i = 0; i < 6; i++)
			{
				std::string skyboxPath = std::filesystem::current_path().parent_path().string() + "\\Assets\\skybox\\" + std::to_string(i) + ".jpg";
				unsigned char* data = stbi_load(skyboxPath.c_str(), &width, &height, &nrChannels, 0);
				
				if (data)
				{
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
						0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
					);
					stbi_image_free(data);
				}
				else
				{
					std::cout << "Cubemap tex failed to load at path: " << skyboxPath << std::endl;
					stbi_image_free(data);
				}
			}
		
		
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
		glUniform1i(glGetUniformLocation(shaderProgram, "skyboxTexture"), 10);

	}


};

