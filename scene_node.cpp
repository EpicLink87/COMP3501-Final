#include <stdexcept>
#define GLM_FORCE_RADIANS
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <time.h>

#include "scene_node.h"

namespace game {

SceneNode::SceneNode(const std::string name, const Resource *geometry, const Resource *material, const Resource *texture, const Resource *envmap){

    // Set name of scene node
    name_ = name;

    // Set geometry
    if (geometry->GetType() == PointSet){
        mode_ = GL_POINTS;
    } else if (geometry->GetType() == Mesh){
        mode_ = GL_TRIANGLES;
    } else {
        throw(std::invalid_argument(std::string("Invalid type of geometry: " + name)));
    }

    array_buffer_ = geometry->GetArrayBuffer();
    element_array_buffer_ = geometry->GetElementArrayBuffer();
    size_ = geometry->GetSize();

    // Set material (shader program)
    if (material->GetType() != Material){
        throw(std::invalid_argument(std::string("Invalid type of material")));
    }

	// Set texture
	if (texture) {
		texture_ = texture->GetResource();
	}
	else {
		texture_ = 0;
	}

	if (envmap) {
		envmap_ = envmap->GetResource();
	}
	else {
		envmap_ = 0;
	}

    material_ = material->GetResource();

    // Other attributes
    scale_ = glm::vec3(1.0, 1.0, 1.0);
	reflectivity_ = 0.0f;
	roughness_ = 0.5f;
	blending_ = false;
	pleaseKill_ = false;
}


SceneNode::~SceneNode(){
}


const std::string SceneNode::GetName(void) const {

    return name_;
}


glm::vec3 SceneNode::GetPosition(void) const {

    return position_;
}


glm::quat SceneNode::GetOrientation(void) const {

    return orientation_;
}

glm::vec3 SceneNode::GetJointPos(void) const {

	return jointPos_;
}


glm::vec3 SceneNode::GetScale(void) const {

    return scale_;
}

bool SceneNode::GetBlending(void) const {

	return blending_;
}


void SceneNode::SetPosition(glm::vec3 position){

    position_ = position;
}


void SceneNode::SetOrientation(glm::quat orientation){

    orientation_ = orientation;
}

void SceneNode::SetJointPos(glm::vec3 joint) {

	jointPos_ = joint;
}


void SceneNode::SetScale(glm::vec3 scale){

    scale_ = scale;
}

void SceneNode::SetReflectivity(float reflectivity)
{
	reflectivity_ = reflectivity;
}

void SceneNode::SetRoughness(float roughness)
{
	roughness_ = roughness;
}


void SceneNode::Translate(glm::vec3 trans){

    position_ += trans;
}


void SceneNode::Rotate(glm::quat rot){

    orientation_ *= rot;
    orientation_ = glm::normalize(orientation_);
}


void SceneNode::Scale(glm::vec3 scale){

    scale_ *= scale;
}

void SceneNode::SetBlending(bool blending) {

	blending_ = blending;
}

GLenum SceneNode::GetMode(void) const {

    return mode_;
}


GLuint SceneNode::GetArrayBuffer(void) const {

    return array_buffer_;
}


GLuint SceneNode::GetElementArrayBuffer(void) const {

    return element_array_buffer_;
}


GLsizei SceneNode::GetSize(void) const {

    return size_;
}


GLuint SceneNode::GetMaterial(void) const {

    return material_;
}

float SceneNode::GetReflectivity()
{
	return reflectivity_;
}

float SceneNode::GetRoughness()
{
	return roughness_;
}

void SceneNode::AddChild(SceneNode *node) {
	children_.push_back(node);
	node->SetParent(this);
}


SceneNode *SceneNode::GetChild(std::string node_name) {

	// Find node with the specified name
	for (int i = 0; i < children_.size(); i++) {
		if (children_[i]->GetName() == node_name) {
			return children_[i];
		}
	}
	return NULL;

}

void SceneNode::RemoveChild(SceneNode *node) {
	int rmIdx = -1;
	for (int i = 0; i < children_.size(); i++) {
		if (children_[i] == node) {
			rmIdx = i;
		}
	}
	if (rmIdx != -1)
		children_.erase(children_.begin() + rmIdx);
}

void SceneNode::SetParent(SceneNode *node) {
	parent_ = node;
}


SceneNode *SceneNode::GetParent() {
	return parent_;
}


void SceneNode::Draw(Camera *camera){

	// Select blending or not
	if (blending_) {
		// Disable z-buffer
		glEnable(GL_DEPTH_TEST);

		// Enable blending
		glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Simpler form
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBlendEquationSeparate(GL_FUNC_ADD, GL_MAX);
	}
	else {
		// Enable z-buffer

		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
	}

    // Select proper material (shader program)
    glUseProgram(material_);

    // Set geometry to draw
    glBindBuffer(GL_ARRAY_BUFFER, array_buffer_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_array_buffer_);

    // Set globals for camera
    camera->SetupShader(material_);

    // Set world matrix and other shader input variables
    SetupShader(material_);

    // Draw geometry
    if (mode_ == GL_POINTS){
        glDrawArrays(mode_, 0, size_);
    } else {
        glDrawElements(mode_, size_, GL_UNSIGNED_INT, 0);
    }

	glDisable(GL_BLEND);

	// Draw all scene nodes
	for (int i = 0; i < children_.size(); i++) {
		children_[i]->Draw(camera);
	}
}


void SceneNode::Update(void){
	for (int i = 0; i < children_.size(); i++) {
		children_[i]->Update();
	}
}

glm::mat4 SceneNode::GetTransformations() {
	// World transformation
	glm::mat4 scaling = glm::scale(glm::mat4(1.0), scale_);
	glm::mat4 rotation = glm::mat4_cast(orientation_);
	glm::mat4 jointTans = glm::translate(glm::mat4(1.0), jointPos_);
	glm::mat4 invJointTans = glm::translate(glm::mat4(1.0), glm::vec3(-jointPos_.x, -jointPos_.y, -jointPos_.z));
	glm::mat4 translation = glm::translate(glm::mat4(1.0), position_);
	glm::mat4 transf = translation * jointTans * rotation * invJointTans * scaling;
	if (parent_ != NULL)
		transf = parent_->GetTransformations() * transf;
	return transf;
}

bool SceneNode::GetPleaseKill()
{
	return pleaseKill_;
}

void SceneNode::SetupShader(GLuint program){

    // Set attributes for shaders
    GLint vertex_att = glGetAttribLocation(program, "vertex");
    glVertexAttribPointer(vertex_att, 3, GL_FLOAT, GL_FALSE, 11*sizeof(GLfloat), 0);
    glEnableVertexAttribArray(vertex_att);

    GLint normal_att = glGetAttribLocation(program, "normal");
    glVertexAttribPointer(normal_att, 3, GL_FLOAT, GL_FALSE, 11*sizeof(GLfloat), (void *) (3*sizeof(GLfloat)));
    glEnableVertexAttribArray(normal_att);

    GLint color_att = glGetAttribLocation(program, "color");
    glVertexAttribPointer(color_att, 3, GL_FLOAT, GL_FALSE, 11*sizeof(GLfloat), (void *) (6*sizeof(GLfloat)));
    glEnableVertexAttribArray(color_att);

    GLint tex_att = glGetAttribLocation(program, "uv");
    glVertexAttribPointer(tex_att, 2, GL_FLOAT, GL_FALSE, 11*sizeof(GLfloat), (void *) (9*sizeof(GLfloat)));
    glEnableVertexAttribArray(tex_att);

    GLint world_mat = glGetUniformLocation(program, "world_mat");
    glUniformMatrix4fv(world_mat, 1, GL_FALSE, glm::value_ptr(GetTransformations()));

	glm::mat4 normal_matrix = glm::transpose(glm::inverse(GetTransformations()));
	GLint normal_mat = glGetUniformLocation(program, "normal_mat");
	glUniformMatrix4fv(normal_mat, 1, GL_FALSE, glm::value_ptr(normal_matrix));



	// Texture
	if (texture_) {
		GLint tex = glGetUniformLocation(program, "texture_map");
		glUniform1i(tex, 0); // Assign the first texture to the map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_); // First texture we bind
		// Define texture interpolation
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	GLint reflect_var = glGetUniformLocation(program, "reflectivity");
	// Environment map
	if (envmap_) {
		GLint tex = glGetUniformLocation(program, "env_map");
		glUniform1i(tex, 1); // Assign the first texture to the map
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, envmap_); // First texture we bind
		// Define texture interpolation
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glUniform1f(reflect_var, reflectivity_);
	}
	else {
		//If we're not using the environment map, we set the reflectivity to zero.
		glUniform1f(reflect_var, 0.0f);
	}

    // Timer
    GLint timer_var = glGetUniformLocation(program, "timer");
    double current_time = glfwGetTime();
    glUniform1f(timer_var, (float) current_time);

	//Roughness
	GLint roughnessVar = glGetUniformLocation(program, "roughness");
	glUniform1f(roughnessVar, roughness_);
}

} // namespace game;
