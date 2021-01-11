#pragma once
#include <GL\glew.h>
#include <glm/vec3.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>


class Shader {
	unsigned int ID;

	GLint GetUniformLocation(std::string uniformName) {
		GLint location = glGetUniformLocation(ID, uniformName.c_str());
		if (location == -1) {
			std::cout << R"(Uniform variable ")" << uniformName << '"' << " not found in shader." << std::endl;
		}
		return location;
	}
public:
	void Use(){ glUseProgram(ID); }

	unsigned int GetID() { return ID; }

	void SetAttribute(std::string attributeName, glm::vec3 vector) {
		glUseProgram(ID);
		glUniform3f(GetUniformLocation(attributeName), vector.x, vector.y, vector.z);
	}
	void SetAttribute(std::string attributeName, float floatNum) {
		glUseProgram(ID);
		glUniform1f(GetUniformLocation(attributeName), floatNum);
	}
	void SetAttribute(std::string attributeName, int intNum) {
		glUseProgram(ID);
		glUniform1i(GetUniformLocation(attributeName), intNum);
	}
	void SetAttribute(std::string attributeName, glm::mat4 matrix) {
		glUseProgram(ID);
		glUniformMatrix4fv(GetUniformLocation(attributeName), 1, GL_FALSE, glm::value_ptr(matrix));
	}


	Shader() {

		const char* vertexShaderSource = R"(
			#version 330 core
			layout (location = 0) in vec3 aPos;
			layout (location = 1) in vec3 aNorm;
			layout (location = 2) in vec2 aTexCoord;
			out vec3 Normal;
			out vec2 TexCoord;
			out vec3 vertexPos;
			uniform mat4 viewMatrix;
			uniform mat4 normalMatrix;
			uniform mat4 modelMatrix;


			void main()
			{
			   gl_Position =viewMatrix * vec4(aPos, 1.0);
			   TexCoord = aTexCoord;
			   Normal = normalize(vec3(normalMatrix*vec4(aNorm , 0.0)));
			   vertexPos = vec3( modelMatrix * vec4(aPos, 1.0) );
			}
		)";

		const char* fragmentShaderSource = R"(
			#version 330 core
			out vec4 FragColor;
			in vec3 Normal;
			in vec2 TexCoord;
			in vec3 vertexPos;
			uniform vec3 cameraPosition;

			uniform sampler2D ourTexture;
			uniform float specularAmount;
			uniform float diffuseAmount;
		
			struct Light{
				vec3 color;
				vec3 direction;
				float intensity;
			};
			uniform Light lights[32];
			uniform int nLights;

	
			void main()
			{

			  vec3 viewDir = normalize( cameraPosition - vertexPos);

			  vec4 diffuse;
			  vec4 specular;

			  for(int i=0; i< nLights; i++){
				diffuse +=  vec4( lights[i].intensity * lights[i].color *  max(dot(Normal, -lights[i].direction), 0.0) , 1.0f);
			    specular += vec4( lights[i].intensity * lights[i].color * pow(	max( dot(viewDir, reflect(-lights[i].direction, Normal)), 0.0 ), 8), 1.0f);
				
			  }
		
			diffuse =  texture(ourTexture, TexCoord) * diffuse;
			//if(nLights ==0 ){diffuse =  texture(ourTexture, TexCoord) * vec4(1.0,1.0,1.0,1.0);}

			vec4 ambient = texture(ourTexture, TexCoord) * vec4(0.3,0.3,0.3, 1.0);
			

			  FragColor = ambient + diffuseAmount* diffuse + specularAmount*specular;
		
			}
		)";

		unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
		glCompileShader(vertexShader);

		unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
		glCompileShader(fragmentShader);

		unsigned int shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);
		glLinkProgram(shaderProgram);
		ID = shaderProgram;

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
	
};