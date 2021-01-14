#pragma once
#include <GL\glew.h>
#include <glm/vec3.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "Light.h"

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



	void SetLights(std::vector<Light>& lights, std::vector<Light_Point>& pointLights) {
		Shader* shader = this;
		shader->Use();

		shader->SetAttribute("nLights", int(lights.size()));
		for (int i = 0; i < lights.size(); i++)
		{
			std::string number = std::to_string(i);
			shader->SetAttribute("lights[" + number + "].color", lights[i].color);
			shader->SetAttribute("lights[" + number + "].direction", lights[i].direction);
			shader->SetAttribute("lights[" + number + "].intensity", lights[i].intensity);
		}

		shader->SetAttribute("nPointLights", int(pointLights.size()));

		for (int i = 0; i < pointLights.size(); i++) {
			std::string number = std::to_string(i);
			shader->SetAttribute("pointLights[" + number + "].color", pointLights[i].color);
			shader->SetAttribute("pointLights[" + number + "].falloffLinear", pointLights[i].falloffLinear);
			shader->SetAttribute("pointLights[" + number + "].falloffQuadratic", pointLights[i].falloffQuadratic);
			shader->SetAttribute("pointLights[" + number + "].intensity", pointLights[i].intensity);
			shader->SetAttribute("pointLights[" + number + "].position", pointLights[i].position);

		}
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
             
			
			struct Light_Point {
				vec3 position;
				vec3 color;
				float falloffLinear;
				float falloffQuadratic;
				float intensity;
			};
			uniform Light_Point pointLights[32];
			uniform int nPointLights;

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
			  for(int i=0; i< nPointLights; i++){
				float distance = length(pointLights[i].position - vertexPos);
				vec3 direction = (vertexPos - pointLights[i].position)/distance ;
				float falloff = 1/(1 + pointLights[i].falloffLinear * distance + pointLights[i].falloffQuadratic * distance * distance );
				diffuse +=  falloff* vec4( pointLights[i].intensity * pointLights[i].color *  max(dot(Normal, -direction), 0.0) , 1.0f);
			    specular += falloff*vec4( pointLights[i].intensity * pointLights[i].color * pow(	max( dot(viewDir, reflect(direction, Normal)), 0.0 ), 32), 1.0f);

			  }
		
			diffuse =  texture(ourTexture, TexCoord) * diffuse;
			//if(nPointLights ==0 ){diffuse =  texture(ourTexture, TexCoord) * vec4(1.0,1.0,1.0,1.0);}

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
		//yfghj

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
		ID = shaderProgram;
	}
	
};