#ifndef _REGISTRY_H
#define _REGISTRY_H

struct RegistryElement
{
	char *value;
	RegistryElement *next;
};

struct RegistryNode
{
	char *name;
	RegistryNode *next;
	RegistryElement *first;
};

void clearRegistryNode(RegistryNode *);
void clearRegistryElement(RegistryElement *);
void clearRegistry(RegistryNode *);
void clearRegistry();
RegistryNode *createRegistryNode(char *);
RegistryNode *addRegistryNode(char *);
RegistryElement *createRegistryElement(char *);
RegistryNode *findRegistryNode(char *);
RegistryElement *addRegistryElement(char *,char *);
RegistryElement *findRegistryElement(char*,char*);
int regNodeSize(char*);
int regInt(char*,int=0);
double regDouble(char*);
char *regString(char*);
float *regFloat(char*);
int regListInt(char *,int  = 0);
double regListDouble(char *,int  = 0);
char *regListString(char *,int  = 0);
RegistryNode *getProgramRegistry();


#endif