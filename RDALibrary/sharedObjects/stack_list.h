#pragma once
#include <cstdlib>
#include <iostream>

namespace RDA {
	// Not finished
	// Its a meant to encapsulate a variable and give it a header that points to other elements in a chain
	// Its meant to be "allocated on stack", or better said, to reduce the number of allocations, by attaching it to a variable
	class stack_element {
		stack_element* mForward = nullptr;
		stack_element* mBackward = nullptr;
	protected:
		stack_element() = default;
		~stack_element() = default;

		void insert(stack_element& other) {
			if (mForward) {
				mForward->mBackward = mBackward;
			}
			if (mBackward) {
				mBackward->mForward = mForward;
			}
			stack_element* helper = &other;
			while (*helper < *this){
				helper = helper->mForward;
			}
			while (*helper > *this){
				helper = helper->mBackward;
			}
			mForward = helper;
			if (helper){
				helper->mBackward = this;
			}

		}

		void pop() {
			if (mForward){
				mForward->mBackward = mBackward;
			}
			if (mBackward){
				mBackward->mForward = mForward;
			}
			mForward = nullptr;
			mBackward = nullptr;
		}

		friend bool operator==(stack_element& a, stack_element& b) { return &a == &b; };
		friend bool operator!=(stack_element& a, stack_element& b) { return &a != &b; };
		friend bool operator>=(stack_element& a, stack_element& b) { return &a >= &b; };
		friend bool operator<=(stack_element& a, stack_element& b) { return &a <= &b; };
		friend bool operator<(stack_element& a, stack_element& b) { return &a < &b; };
		friend bool operator>(stack_element& a, stack_element& b) { return &a > &b; };
	};
	template<typename T>
	class stack_list {
		stack_list() = default;

		void addElement(stack_element& pEntity) {
			stack_element* current = &_dummy;
			while (current->_forward) {
				if (current->_forward > pEntity) {
					break;
				}
				current = current->_forward;
			}
			pEntity._forward = current->_forward;
			if (current->_forward) {
				current->_forward->_backward = &pEntity;
			}
			pEntity._backward = current;
			current->_forward = &pEntity;
		};
		stack_element* getNext(stack_element* pointer) {
			return pointer->_forward;
		}

		stack_element* getNext() {
			return _dummy._forward;
		}

	protected:
		meshEntity _dummy;
	};

}