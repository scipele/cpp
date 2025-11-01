//// filename:  linked_list.cpp
// this is an example of how to implement a linked list class in c++
// 
// this simple example only stores an integer value and a pointer tonthe next node,
// however note that the struct could have other variables added as needed
//
// 
// by T. Sciple,  8/6/2024
//
#include <iostream>

class LinkedList {
private:

	struct Node {
		int value;
		Node* next;

		Node(int val) : value(val), next(nullptr) {}	
	};

	Node* head;

public:
	LinkedList() : head(nullptr) {}

	~LinkedList() {
		while (head != nullptr) {
			Node* temp = head;
			head = head->next;
			delete temp;
		}
	}


	void PrintList() const {
			Node* n = head;
			while (n != NULL) {
				std::cout	<< "Node Value: "
							<< n->value
								<< ", pointer memory addr of next node :"
								<< static_cast<void*> (n->next)
								<< "\n";
			n = n->next;
		}
	}


	void InsertAtFront(int new_value) {
		Node* new_node = new Node(new_value);
		new_node->next = head;
		head = new_node;
	}


	void InsertAtBack(int new_value) {        	// 1. Prepare new Node
		Node* new_node = new Node(new_value);
		new_node->value = new_value;
		new_node->next = NULL;
		if(head == nullptr) {
			head = new_node;
			return;
		}
		Node* chk_last = head;
		while (chk_last->next != nullptr) {
			chk_last = chk_last->next;
			}
		chk_last->next = new_node;
	}


	void insertAfter(Node* prev_node, int new_value) {
		if (prev_node == nullptr) {
		std::cout << "Previous value cant be NULL";
		return;
		}
		Node* new_node = new Node(new_value);
		new_node->next = prev_node->next;
		prev_node->next = new_node;
	}

	
	Node* getHead() const {
    	return head;
    }
};


int main () {

	LinkedList list;

    list.InsertAtBack(10);
    list.InsertAtBack(20);
//    Node* second = list.getHead()->next; // Accessing the second node directly
    list.InsertAtBack(30);

    // Insert at the front
    list.InsertAtFront(5);
    list.InsertAtFront(3);

    // Insert after the second node
//    list.insertAfter(second, 25);

    // Print the list
    list.PrintList();


return 0;
}

