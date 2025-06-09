// #include "shader.hpp"
// #include "texture.hpp"
// #include "glm.hpp"

// #include <glad/glad.h>
// #include <GLFW/glfw3.h>
// #include <stb_image.h>

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <iostream>

template <class T>
struct Tree {
	bool hollow=false;
	T value;
	Tree* left=nullptr;
	Tree* right=nullptr;
	Tree(T&& _value, Tree&& _left, Tree&& _right)
	:	value{_value}{
		left = new Tree{(Tree&&)_left};
		right = new Tree{(Tree&&)_right};
	}
	Tree(const Tree& tree) : value{tree.value}, left{nullptr}, right{nullptr}{
		std::cout << "Tree(const&)\n";
		if (tree.left) {
			assert (tree.left != nullptr);
			left = new Tree(*(tree.left));
		} if (tree.right) {
			right = new Tree(*(tree.right));
		}
	}
	Tree(Tree& tree) : this{&tree}{
		std::cout << "Tree(&)\n";
	}
	Tree(Tree&& tree) : value{tree.value}, left{tree.left}, right{tree.right}{
		tree.hollow = true;
		tree.left = nullptr;
		tree.right = nullptr;
	}
	Tree() : value{}, left{nullptr}, right{nullptr}{
	}
	~Tree() {
		if (hollow) return;
		if (left){
			delete left;
			left = nullptr;
		} if (right) {
			delete right;
		}
	}
	Tree& get_left() {
		return *left;
	}
	Tree& get_right() {
		return *right;
	}
	Tree& insert(T&& value) {
		for (auto i=this;i != nullptr;) {
			if (value >= i->value)
		}
	}
};

int main() {
	auto& tree = Tree<int>(12, {}, {})
		.insert(21)
		.insert(34)
		.insert(32);
}
