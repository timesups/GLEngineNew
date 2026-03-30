#pragma once
#include <variant>

#include "Shader.hpp"
#include "Texture.hpp"

enum class MaterialPropertyType
{
	Float,
	Int,
	Bool,
	Vec2,
	Vec3,
	Vec4,
	Mat3,
	Mat4,
	Texture2D,
	TextureCube
};

using MaterialPropertyValue = std::variant<
	float,
	int,
	bool,
	glm::vec2,
	glm::vec3,
	glm::vec4,
	glm::mat3,
	glm::mat4,
	std::shared_ptr<Texture>
>;


struct MaterialProperty
{
	std::string name;
	MaterialPropertyType type;
	MaterialPropertyValue value;
	bool isDirty = true;
};


class Material 
{
public:
	Material(std::shared_ptr<Shader> shader):m_shader(shader) {}
private:
	std::string m_name;
	std::shared_ptr<Shader> m_shader;
	std::unordered_map<std::string, MaterialProperty> m_properties;

	int m_nextTextureUnit = 0;
	std::unordered_map<std::string, int>m_textureUnits;

	bool m_isApplied = false;
};