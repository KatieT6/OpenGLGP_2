#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <draco/mesh/mesh.h>

#include <vector>

#include <Model.h>

class GameObject {
protected:
		std::list<std::unique_ptr<GameObject>> children_;
		GameObject* parent_ = nullptr;
		bool dirty_ = true;

public:
		std::string name;
		glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		Shader* shader_ = nullptr;
		Model* model_ = nullptr;
		Transform transform;

		GameObject(Model* model, Shader* shader)
			: model_(model), shader_(shader) {}

		GameObject() {
			model_ = NULL;
		}
		GameObject(Model& model, std::string name)
			: model_(&model), name(name) {}

		GameObject(std::string nameofObj){
			name = nameofObj;
		}

		/*void addChild(GameObject* child) {
			children_.emplace_back(child);
			child->parent_ = this;
		}*/

		template<typename... TArgs>
		void addChild(TArgs&... args)
		{
			children_.emplace_back(std::make_unique<GameObject>(args...));
			children_.back()->parent_ = this;
		}

		//void draw(Transform parent, glm::mat4 projection, glm::mat4 view, bool dirty) {
		//	dirty_ |= dirty  ;
		//	if (dirty_) {
		//		transform.getLocalModelMatrix(parent);
		//		//world_ = local_;
		//	}

		//	if (model_) {
		//		model_->Draw(parent, &transform, projection, view, this->dirty_);

		//	}

		//	for (unsigned int i = 0; i < children_.size(); i++)
		//	{
		//		children_[i]->draw(transform, projection, view, dirty_);
		//	}
		//		dirty_ = false;
		//}

		void draw(Shader& ourShader, unsigned int& display, unsigned int& total)
		{
			printName();

			if (model_)
			{
				ourShader.setVec4("dynamicColor", color);
				ourShader.setMat4("model", transform.getModelMatrix());
				model_->Draw(ourShader);
				display++;
			}
			total++;

			for (auto&& child : children_)
			{
				child->draw(ourShader, display, total);
			}

			ourShader.setVec4("dynamicColor", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		}

		void updateSelfAndChild()
		{
			if (transform.isDirty()) {
				forceUpdateSelfAndChild();
				return;
			}

			for (auto&& child : children_)
			{
				child->updateSelfAndChild();
			}
		}

		void forceUpdateSelfAndChild()
		{
			if (parent_)
				transform.computeModelMatrix(parent_->transform.getModelMatrix());
			else
				transform.computeModelMatrix();

			for (auto&& child : children_)
			{
				child->forceUpdateSelfAndChild();
			}
		}

		/*void setParent(GameObject* parent) {
			if (parent_ != nullptr) {
				parent_->removeChild(this);
			}
			parent->addChild(this);
		}*/

		/*void setLocalPosition(const glm::vec3& position) {
			transform.position = position;
			dirty_ = true;
		}

		void setLocalScale(const glm::vec3& scale) {
			transform.scale = scale;
			dirty_ = true;
		}

		void setLocalRotation(const glm::vec3& rotation) {
			transform.eulerRot = rotation;
			dirty_ = true;
		}*/

		void setTransform(Transform local)
		{
			transform = local;
			dirty_ = true;
		}

		void setColor(glm::vec4 color) {
			this->color = color;
		}

		//znajdz dziecko po nazwie
		GameObject* getChildByName(const std::string& name) {
			for (auto& child : children_) {
				if (child->name == name) {
					return child.get();
				}

				GameObject* foundChild = child->getChildByName(name);
				if (foundChild != nullptr) {
					return foundChild;
				}
			}
			return nullptr;
		}

		std::string getName() {
			return name;
		}

		void printName() {
			std::cout << name << std::endl;
		}

		Transform getLocalTransform() {
			return transform;
		}
};

#endif // !GAMEOBJECT_H