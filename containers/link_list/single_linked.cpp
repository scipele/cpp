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
			Node* next;		// pointer to the next node in the list, 8 bytes in size on a 64-bit system
			int value;
			// Constructor to initialize the node with a value and set next to nullptr
			Node(int val) : value(val), next(nullptr) {}	
		};

	Node* head;

public:
	// Constructor to initialize the linked list with head set to nullptr
	LinkedList() : head(nullptr) {}

	// Destructor to clean up the linked list and free memory
	~LinkedList() {
		while (head != nullptr) {
			Node* temp = head;
			head = head->next;
			delete temp;
		}
	}

	// PrintList function to display the contents of the linked list
	// Why are the memory addresses 32 bytes apart?  Because the Node struct is 32 bytes in size, and each node is allocated
	//  in memory with that size. The memory addresses of consecutive nodes will be separated by the size of the Node struct,
	//  which is 32 bytes in this case.
	void PrintList() const {
		std::cout << "Printing Linked List:\n\n";
		std::cout << "value | this_mem_adr   | next_mem_adr   | delta_bytes \n";
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
							<< " | "
							<< std::setw(12)
							<< (n->next ? reinterpret_cast<char*>(n->next) - reinterpret_cast<char*>(n) : 0)	
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


    // Print the list
    list.PrintList();


return 0;
}

