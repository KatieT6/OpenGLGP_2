#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <draco/mesh/mesh.h>

#include <vector>

#include <Model.h>

class GameObject {
private:
		Transform* local_ = new Transform();
		Transform* world_;
		std::vector<GameObject*> children_;
		GameObject* parent_;
		bool dirty_ = true;

public:
		Shader* shader_ = nullptr;
		Model* model_;
		GameObject(Model* model, Shader* shader)
			: model_(model), shader_(shader) {}

		GameObject() {
			model_ = NULL;
		}

		void addChild(GameObject* child) {
			children_.push_back(child);
			child->parent_ = this;
		}

		void removeChild(GameObject* child) {
			for (int i = 0; i < children_.size(); i++) {
				if (children_[i] == child) {
					children_.erase(children_.begin() + i);
					child->parent_ = nullptr;
					return;
				}
			}
		}

		void draw(Transform parent, glm::mat4 projection, glm::mat4 view, bool dirty) {
			dirty_ |= dirty  ;
			if (dirty_) {
				local_->getLocalModelMatrix(parent);
				//world_ = local_;
			}

			if (model_) {
				model_->Draw(parent, local_, projection, view, this->dirty_);
			}

			for (unsigned int i = 0; i < children_.size(); i++)
			{
				children_[i]->draw(*local_, projection, view, dirty_);
			}
				dirty_ = false;
		}

		void setParent(GameObject* parent) {
			if (parent_ != nullptr) {
				parent_->removeChild(this);
			}
			parent->addChild(this);
		}

		void setLocalPosition(const glm::vec3& position) {
			local_->position = position;
			dirty_ = true;
		}

		void setLocalScale(const glm::vec3& scale) {
			local_->scale = scale;
			dirty_ = true;
		}

		void setLocalRotation(const glm::vec3& rotation) {
			local_->eulerRot = rotation;
			dirty_ = true;
		}

		void setTransform(Transform* local)
		{
			local_ = local;
			dirty_ = true;
		}

		Transform* getLocalTransform() {
			return local_;
		}
};

#endif // !GAMEOBJECT_H