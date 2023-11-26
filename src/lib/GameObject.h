#include <draco/mesh/mesh.h>


#include <vector>
#include <Transform.h>
#include <Model.h>

class GameObject {
//public:
//	GraphNode(Mesh* mesh)
//		: mesh_(mesh),
//		local_(Transform::origin())
//	{}
//
//private:
//	Transform local_;
//	Mesh* mesh_;
//
//	GraphNode* children_[MAX_CHILDREN];
//	int numChildren_;

private:
		Model* model_;
		Transform* local_;
		Transform* world_;
		std::vector<GameObject*> children_;
		GameObject* parent_;
		bool dirty = false;

public:
		GameObject(Model* model)
			: model_(model) {}

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

		void setParent(GameObject* parent) {
			if (parent_ != nullptr) {
				parent_->removeChild(this);
			}
			parent->addChild(this);
		}

		

};