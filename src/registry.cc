#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <ctype.h>
#include "registry.h"
#include "memory.h"

RegistryNode *programRegistry=NULL;


/*

	Name:						createRegistryNode
	Description:		creates single Node wiith given name
	Arguments:			char *name -	the name of node
	Returned Value:	pointer to new allocated memory
									NULL - if allocation failed
	
*/

RegistryNode *createRegistryNode(char *name)
{
	if(!name)
		return NULL;
	RegistryNode *_new = (RegistryNode *) _malloc(sizeof(RegistryNode));
	assert(_new != NULL);
	if(_new)
	{
		_new -> first = NULL;
		_new -> next = NULL;
		_new->name = _strdup(name);
		assert(_new -> name != NULL);
		if(!_new->name)
		{
			_free(_new);
			return NULL;
		}
	}
	return _new;
}

/*

	Name:						addRegistryNode
	Description:		adds single node to registry structure
	Arguments:			char *name - the name of new node
	Returned Value:	pointer to new allocated memory
									NULL - if allocation fails
	
*/

RegistryNode *addRegistryNode(char *name)
{
	RegistryNode *_new = createRegistryNode(name);
	assert(_new != NULL);
	if(_new)
	{
		if(programRegistry)
		{
			RegistryNode *tmp = programRegistry;
			while(tmp -> next)
				tmp = tmp -> next;
			tmp -> next = _new;					
		}
		else
		{
			clearRegistryNode(_new);
			return NULL;			
		}
	}
	return _new;	
}


/*

	Name:						createRegistryElement
	Description:		creates single value element of registry
	Arguments:			char *val - string representing a value of
									element
	Returned Value:	pointer to new element
									NULL - if allocation fails
	
*/

RegistryElement *createRegistryElement(char *val)
{
	if(!val)
		return NULL;
	RegistryElement *_new = (RegistryElement*) _malloc(sizeof(RegistryElement));
	assert(_new != NULL);
	if(_new)
	{
		_new -> next = NULL;
		_new -> value = _strdup(val);
		assert(_new ->  value != NULL);
		if(!_new -> value)
		{
			_free(_new);
			return NULL;
		}
	}
	return _new;
}

/*

	Name:						findRegistryNode
	Description:		finds a node which name is == given name
	Arguments:			char *name	- name of node
	Returned Value:	pointer to node
									NULL - if there is no node in registry
	
*/

RegistryNode *findRegistryNode(char *name)
{
	RegistryNode *tmp = programRegistry;
	while(tmp)
	{
		if(!strcmp(tmp->name,name))
			return tmp;
		tmp=tmp->next;
	}
	return tmp;
}


/*

	Name:						findRegistryElement
	Description:		finds specified element in registry node
	Arguments:			RegistryNode *node - node
									int p	- the number of element
	Returned Value:	pointer to element
									NULL - if there is no such a element in a node
	
*/

RegistryElement *findRegistryElement(RegistryNode *node,int p=0)
{
	RegistryElement *tmp = NULL;
	assert(node != NULL);
	if(node)
	{
		tmp = node -> first;
		int id = 0;
		while(tmp && id != p)
		{
			tmp = tmp -> next;
			id ++;
		}
		if(id != p)
			return NULL;
	}
	return tmp;
}

/*

	Name:						addRegistryElement
	Description:		adds specified element to specified node
	Arguments:			char *node - the node name
									char *element - value of element
	Returned Value:	pointer to newly created element
									NULL - if allocation fails
	
*/

RegistryElement *addRegistryElement(char *nodeName,char *elemName)
{
	RegistryNode *tmp = findRegistryNode(nodeName);
	RegistryElement *tmpTmp = NULL;
	RegistryElement *tmpElem = NULL;
	if(tmp)
	{
		tmpElem = createRegistryElement(elemName);
		assert(tmpElem != NULL);
		if(tmpElem)
		{
			if(tmp -> first)
			{
				tmpTmp = tmp -> first;
				while(tmpTmp -> next)
					tmpTmp = tmpTmp -> next;
				tmpTmp -> next = tmpElem;
			}
			else
				tmp -> first = tmpElem;
		}
	}
	return tmpElem;
}


/*

	Name:						clearRegistryElement
	Description:		clears memory used by element
	Arguments:			RegistryElement *elem - pointer to element to be
									freed
	Returned Value:	void
	
*/

void clearRegistryElement(RegistryElement *elem)
{
	if(elem)
	{
		_free(elem -> value);
		_free(elem);
	}
}

/*

	Name:						clearRegistryNode
	Description:		clears memory used by registry node
	Arguments:			RegistryNode *node - node to be freed
	Returned Value:	void
	
*/

void clearRegistryNode(RegistryNode *node)
{
	if(node)
	{
		RegistryElement *tmp = node -> first;
		while(tmp)
		{
			RegistryElement *toRemove = tmp;
			tmp = tmp -> next;
			clearRegistryElement(toRemove);
		}
		node -> first = NULL;
		_free(node -> name);
		_free(node);
	}
}

/*

	Name:						clearRegistry
	Description:		clears whole registry
	Arguments:			nothing
	Returned Value:	void
	
*/

void clearRegistry(RegistryNode *registry)
{
	RegistryNode *tmp = registry;
	while(tmp)
	{
		RegistryNode *toRemove = tmp;
		tmp = tmp -> next;
		clearRegistryNode(toRemove);
	}
	programRegistry = NULL;
}


void clearRegistry()
{
	RegistryNode *tmp = programRegistry;
	while(tmp)
	{
		RegistryNode *toRemove = tmp;
		tmp = tmp -> next;
		clearRegistryNode(toRemove);
	}
	programRegistry = NULL;
}

/*

	Name:						regInt
	Description:		finds node 'name' and return it's value as int
	Arguments:			char *name - name of node
									int val - default value if node can not be
									found or decoded in proper way
	Returned Value:	int
	
*/


int regInt(char *name,int val)
{
	assert(programRegistry != NULL);
	if(programRegistry)
	{
		RegistryNode *tmp = findRegistryNode(name);
		if(tmp && tmp -> first)
		{
			double dVal = strtod(tmp -> first -> value,NULL);
			if(dVal == HUGE_VAL || dVal == -HUGE_VAL)
				return 0xffff;
			return (int)dVal;
			
		}
	}
	return 0xffff;
}

/*

	Name:						regDouble
	Description:		finds node 'name' and return it's value as double
	Arguments:			char *name - name of node
	Returned Value:	double
	
*/


double regDouble(char *name)
{
	assert(programRegistry != NULL);
	if(programRegistry)
	{
		RegistryNode *tmp = findRegistryNode(name);
		if(tmp && tmp -> first)
		{
			double dVal = strtod(tmp -> first -> value,NULL);
			if(dVal == HUGE_VAL || dVal == -HUGE_VAL)
				return 0xffff;
			return dVal;
		}
	}
	return 0xffff;
}

/*

	Name:						regString
	Description:		finds node 'name' and return it's value as string
	Arguments:			char *name - name of node
	Returned Value:	ptr to char (string)
	
*/

char *regString(char *name)
{
	assert(programRegistry != NULL);
	if(programRegistry)
	{
		RegistryNode *tmp = findRegistryNode(name);
		if(tmp && tmp -> first)
		{
			return tmp -> first -> value;
		}
	}
	return NULL;
}

/*

	Name:						regNodeSize
	Description:		returns the count of elements in node
	Arguments:			char *name - node name
	Returned Value:	int - the count of elements in a node
	
*/


int regNodeSize(char *name)
{
	int retVal = 0;
	assert(programRegistry != NULL);
	if(programRegistry)
	{
		RegistryNode *tmp = findRegistryNode(name);
		if(tmp)
		{
			RegistryElement *elem = tmp -> first;
			while(elem)
			{
				retVal++;
				elem = elem -> next;
			}
		}
	}
	return retVal;
}


/*

	Name:						regListInt
	Description:		returns ideks'nth element of node
	Arguments:			char *name - name of node
									int index - the number of element
	Returned Value:	value of this element
	
*/

int regListInt(char *name,int index)
{
	double rVal = 0xffff;
	assert(programRegistry != NULL);
	if(!programRegistry)
		return 0;
	RegistryNode *node = findRegistryNode(name);
	RegistryElement *element = findRegistryElement(node,index);
	if(element)
	{
		rVal = strtod(element -> value,NULL);
		if(rVal == HUGE_VAL || rVal == -HUGE_VAL)
			rVal = 0xffff;
		else
			rVal = (int) rVal;
	}
	return (int)rVal;
}

/*

	Name:						regListDouble
	Description:		returns ideks'nth element of node
	Arguments:			char *name - name of node
									int index - the number of element
	Returned Value:	value of this element
	
*/


double regListDouble(char *name,int indeks)
{
	double rVal = 0xffff;
	assert(programRegistry != NULL);
	if(!programRegistry)
		return 0;
	RegistryNode *node = findRegistryNode(name);
	RegistryElement *element = findRegistryElement(node,indeks);
	if(element)
	{
		rVal = strtod(element -> value,NULL);
		if(rVal == HUGE_VAL || rVal == -HUGE_VAL)
			rVal = 0xffff;	
	}
	return rVal;
}

/*

	Name:						regListString
	Description:		returns ideks'nth element of node
	Arguments:			char *name - name of node
									int index - the number of element
	Returned Value:	value of this element
	
*/

char *regListString(char *name,int indeks)
{
	assert(programRegistry != NULL);
	if(!programRegistry)
		return NULL;
	RegistryNode *node = findRegistryNode(name);
	RegistryElement *element = findRegistryElement(node,indeks);
	if(element)
		return element -> value;
	return NULL;
}

RegistryNode *getProgramRegistry()
{
	return programRegistry;
}
