struct Node {
	int value;
	Node * left;
	Node * right;
};
void inorder_traverse(Node *node)
{
	if (node->left != NULL) {
		inorder_traverse(node->left);
	}
	cout<<" "<<node->value;
	if (node->right != NULL) {
		inorder_traverse(node->right);
	}
}

void inorder_traverse_nonrecursive(Node *node)
{
	Stack stack;
	do {
		while(node) {
			stack.push(node);
			node = node->left;
		}
		do {
			Node *top = stack.top();
			stack.pop();
			if(top->right) {
				node = top->right;
				break;
			}
		}while(!stack.empty());
	}while(!stack.empty());
}

class InorderIterator : public Iterator {
	public:
		InorderIterator(Node *node) {
			Node *cur = node;
			while(cur) {
				mStack.push(cur);
				cur = cur->left;
			}
		}

		virtual Node*next() {
			if (mStack.empty()) {
				return NULL;
			}
			Node *top = mStack.top();
			mStack.pop();
			if(top->right) {
				Node *cur = top->right;
				while(cur) {
					mStack.push(cur);
					cur = cur->left;
				}
			}
			return top;
		}

	private:
		std::stack<Node*> mStack;
};
