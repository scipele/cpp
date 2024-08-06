// filename:  linked_list.cpp
// this is an example of how to implement a linked list class in c++
// 
// this simple example only stores an integer value and a pointer tonthe next node,
// however note that the struct could have other variables added as needed
//
// 
// by T. Sciple,  8/6/2024
//
#include <iostream>

class Node {
public:
	int value;
	Node* next;
};

void print_list(Node* n) {
		while (n != NULL) {
			std::cout	<< "Node Value: "
	           			<< n->value
							<< ", pointer memory addr of next node :"
							<< static_cast<void*> (n->next)
							<< "\n";
		n = n->next;
	}
}

void InsertAtFront(Node** head, int new_value) {
	// 1. Prepare new node by new keyword and class obj specification
	Node* new_node = new Node();
	new_node->value = new_value;
	// 2. Put in front of current head
	new_node->next = *head;
	// move head to point to the  newnode
	*head = new_node;
}


void InsertAtBack(Node** head, int new_value) {        	// 1. Prepare new Node
	// 1.  Prepare for addition of a new node
	Node* new_node = new Node();
	new_node->value = new_value;
	new_node->next = NULL;
	// 2. if linked list is empty then make this new element head
	if(*head == NULL) {
		*head = new_node;
		return;
	}
	// 3. Find the last node
	Node *chk_last = *head;
	while (chk_last->next != NULL) {
		chk_last = chk_last->next;
		}
	// 4. set the value of the last node
	chk_last->next = new_node;
}


void insertAfter(Node* prev_node, int new_value) {
     // 1. Make sure prev node is not NULL
    if (prev_val == NULL) {
	std::cout << "Previous value cant be NULL";
	return;
     }
     // 2.  Prepare for addition of new node
     Node* new_node = new Node();
     new_node->value = new_value;
     // 3.  insert new node after previou value referenced 
     new_node->next = prev_node->next;
     prev_node->next = new_node;
}


int main () {

   Node* head = new Node();
	Node* second = new Node();
	Node* third = new Node();
	
	head->value = 10;
	head->next = second;
	second->value = 20;
	second->next = third;
	third->value = 30;
	third->next = NULL;
	
	// Try out the Insert At Front Function
	InsertAtFront(&head, 5);
	InsertAtFront(&head, 3);
	InsertAtBack(&head, 35);
	InsertAfter(second, 25);
	print_list(head);
return 0;
}
