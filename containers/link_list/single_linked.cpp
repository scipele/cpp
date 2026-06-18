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
#include <iomanip>

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

	// Why is this printing in order and not in the order in which the nodes were added? 
	// Because the InsertAtFront method adds new nodes to the front of the list, so the most recently
	// added node will be at the front. When we print the list, we start from the head and follow the
	// next pointers, which means we will see the nodes in the order they were added, with the most 
	//recent node first. If we want to print in the order they were added, we would need to insert 
	//at the back of the list instead of the front.
	void PrintList() const {
		std::cout << "Printing Linked List:\n\n";
		std::cout << "value | this_mem_adr | next_mem_adr\n";
		Node* n = head;
			while (n != NULL) {
				std::cout	<< std::setw(5)
							<< n->value
							<< " | "
							<< std::setw(12)
							<< static_cast<void*> (n)
							<< " | "
							<< std::setw(12)	
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

	// It traverses the list until it finds the last node (where next is NULL) and then appends the new node there.
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

    list.InsertAtBack(30);
    list.InsertAtBack(5);
    list.InsertAtBack(10);
    list.InsertAtBack(15);
    list.InsertAtBack(2);

	list.InsertAtFront(100);
	list.InsertAtFront(105);
	list.InsertAtFront(4);


    // Print the list
    list.PrintList();


return 0;
}

