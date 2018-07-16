#pragma once

#include <queue>

template<class T>
struct HuffmanTreeNode{
	T _val;
	HuffmanTreeNode<T>* _left;
	HuffmanTreeNode<T>* _right;
	HuffmanTreeNode<T>* _parent;
public:
	HuffmanTreeNode(const T& val):
		_val(val),
		_left(NULL),
		_right(NULL),
		_parent(NULL)
	{}
};

template<class T>
class HuffmanTree{
	typedef HuffmanTreeNode<T> Node;

	struct PtrNodeCompare
	{
		bool operator()(Node* n1, Node* n2)
		{
			return n1->_val > n2->_val;
		}
	};

public:
	HuffmanTree(T* a, int size, const T& invalid)
	{
		std::priority_queue< Node*, std::vector<Node*>, PtrNodeCompare > q;
		for (int i = 0; i < size; i++)
		{
			if (a[i] != invalid)
				q.push(new Node(a[i] ) );
		}
	
		Node* p1;
		Node* p2;
		while ( q.size() > 1)
		{
			p1 = q.top();
			q.pop();
			p2 = q.top();
			q.pop();
			Node* cur = new Node((p1->_val + p2->_val));
			if ( p1->_val > p2->_val )
			{
				cur->_left = p1;
				cur->_right = p2;
			}
			else
			{
				cur->_left = p2;
				cur->_right = p1;
			}
			p1->_parent = cur;
			p2->_parent = cur;
			q.push(cur);
			_pRoot = cur;
		}
	}
	~HuffmanTree()
	{

	}

	Node* GetRoot()
	{
		return _pRoot;
	}

private:
	HuffmanTree(const HuffmanTree<T>& t);
	HuffmanTree<T>& operator=(const HuffmanTree<T>& t);

protected:
	Node* _pRoot;
};
