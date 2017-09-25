#ifndef MyList_H
#define MyList_H
#include<assert.h>

template<class T,bool isCreatNode>
class MyList
{
public:
	MyList()
		:beginNode(NULL),
		endNode(NULL)
	{
	}
	~MyList()
	{
		T* node = beginNode ;
		T* t = NULL ;
		while(node)
		{
			t = node ;
			node = node->next;
			delete t ;
		}
	}
	bool isEmpty()
	{
		return (beginNode==NULL);
	}
	void pushBackNode(T* node)
	{
#if Life_Tag
		assert(node->lifeTag==true);
		node->lifeTag = false ;
#endif
		node->next = NULL ;

		if (beginNode==NULL)
		{
			beginNode = node ;
		}
		else
		{
			endNode->next = node ;
		}
		endNode = node ;
	}

	T* frontNode()
	{
		return beginNode;
	}
	T* backNode()
	{
		if (NULL==beginNode)
		{
			endNode = NULL;
		}
		return endNode;
	}
	void setBackNode(T* node)
	{
		endNode = node;
	}

	T* popFrontNode()
	{
		T* node = beginNode ;
		if (beginNode)
		{
			beginNode = beginNode->next;
#if Life_Tag
			assert(node->lifeTag==false);
			node->lifeTag = true ;
#endif
			node->next = NULL;

		}
		else
		{
			if (isCreatNode)
			{
				node = new T;
#if Life_Tag
				node->lifeTag = true ;
#endif
				node->next = NULL;
			}
			else
			{
				assert("情况不太好"==NULL);
			}
		}
		return node;
	}
private:
	T* beginNode;
	T* endNode;
};

template<class T>
class TypeClass
{
public:
	typedef MyList<T,true> Collector;
	typedef MyList<T,false> List;
};



#endif