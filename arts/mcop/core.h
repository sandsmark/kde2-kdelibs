/* this file was generated by the MCOP idl compiler - DO NOT EDIT */

#ifndef CORE_H
#define CORE_H

#include "common.h"

enum HeaderMagic {MCOP_MAGIC = 1347371853};
enum MessageType {mcopInvocation = 1, mcopReturn = 2, mcopServerHello = 3, mcopClientHello = 4, mcopAuthAccept = 5};
enum AttributeType {streamIn = 1, streamOut = 2, streamMulti = 4, attributeStream = 8, attributeAttribute = 16, streamAsync = 32};
class Header : public Type {
public:
	Header();
	Header(HeaderMagic magic, long messageLength, MessageType messageType);
	Header(Buffer& stream);
	Header(const Header& copyType);
	Header& operator=(const Header& assignType);
	virtual ~Header();

	HeaderMagic magic;
	long messageLength;
	MessageType messageType;

// marshalling functions
	void readType(Buffer& stream);
	void writeType(Buffer& stream) const;
};

class Invocation : public Type {
public:
	Invocation();
	Invocation(long requestID, long objectID, long methodID);
	Invocation(Buffer& stream);
	Invocation(const Invocation& copyType);
	Invocation& operator=(const Invocation& assignType);
	virtual ~Invocation();

	long requestID;
	long objectID;
	long methodID;

// marshalling functions
	void readType(Buffer& stream);
	void writeType(Buffer& stream) const;
};

class ServerHello : public Type {
public:
	ServerHello();
	ServerHello(const std::string& serverID, const std::vector<std::string>& authProtocols, const std::string& authSeed);
	ServerHello(Buffer& stream);
	ServerHello(const ServerHello& copyType);
	ServerHello& operator=(const ServerHello& assignType);
	virtual ~ServerHello();

	std::string serverID;
	std::vector<std::string> authProtocols;
	std::string authSeed;

// marshalling functions
	void readType(Buffer& stream);
	void writeType(Buffer& stream) const;
};

class ClientHello : public Type {
public:
	ClientHello();
	ClientHello(const std::string& serverID, const std::string& authProtocol, const std::string& authData);
	ClientHello(Buffer& stream);
	ClientHello(const ClientHello& copyType);
	ClientHello& operator=(const ClientHello& assignType);
	virtual ~ClientHello();

	std::string serverID;
	std::string authProtocol;
	std::string authData;

// marshalling functions
	void readType(Buffer& stream);
	void writeType(Buffer& stream) const;
};

class ObjectReference : public Type {
public:
	ObjectReference();
	ObjectReference(const std::string& serverID, long objectID, const std::vector<std::string>& urls);
	ObjectReference(Buffer& stream);
	ObjectReference(const ObjectReference& copyType);
	ObjectReference& operator=(const ObjectReference& assignType);
	virtual ~ObjectReference();

	std::string serverID;
	long objectID;
	std::vector<std::string> urls;

// marshalling functions
	void readType(Buffer& stream);
	void writeType(Buffer& stream) const;
};

class ParamDef : public Type {
public:
	ParamDef();
	ParamDef(const std::string& type, const std::string& name);
	ParamDef(Buffer& stream);
	ParamDef(const ParamDef& copyType);
	ParamDef& operator=(const ParamDef& assignType);
	virtual ~ParamDef();

	std::string type;
	std::string name;

// marshalling functions
	void readType(Buffer& stream);
	void writeType(Buffer& stream) const;
};

class MethodDef : public Type {
public:
	MethodDef();
	MethodDef(const std::string& name, const std::string& type, long flags, const std::vector<ParamDef *>& signature);
	MethodDef(Buffer& stream);
	MethodDef(const MethodDef& copyType);
	MethodDef& operator=(const MethodDef& assignType);
	virtual ~MethodDef();

	std::string name;
	std::string type;
	long flags;
	std::vector<ParamDef *> signature;

// marshalling functions
	void readType(Buffer& stream);
	void writeType(Buffer& stream) const;
};

class AttributeDef : public Type {
public:
	AttributeDef();
	AttributeDef(const std::string& name, const std::string& type, AttributeType flags);
	AttributeDef(Buffer& stream);
	AttributeDef(const AttributeDef& copyType);
	AttributeDef& operator=(const AttributeDef& assignType);
	virtual ~AttributeDef();

	std::string name;
	std::string type;
	AttributeType flags;

// marshalling functions
	void readType(Buffer& stream);
	void writeType(Buffer& stream) const;
};

class InterfaceDef : public Type {
public:
	InterfaceDef();
	InterfaceDef(const std::string& name, const std::vector<std::string>& inheritedInterfaces, const std::vector<MethodDef *>& methods, const std::vector<AttributeDef *>& attributes);
	InterfaceDef(Buffer& stream);
	InterfaceDef(const InterfaceDef& copyType);
	InterfaceDef& operator=(const InterfaceDef& assignType);
	virtual ~InterfaceDef();

	std::string name;
	std::vector<std::string> inheritedInterfaces;
	std::vector<MethodDef *> methods;
	std::vector<AttributeDef *> attributes;

// marshalling functions
	void readType(Buffer& stream);
	void writeType(Buffer& stream) const;
};

class TypeComponent : public Type {
public:
	TypeComponent();
	TypeComponent(const std::string& type, const std::string& name);
	TypeComponent(Buffer& stream);
	TypeComponent(const TypeComponent& copyType);
	TypeComponent& operator=(const TypeComponent& assignType);
	virtual ~TypeComponent();

	std::string type;
	std::string name;

// marshalling functions
	void readType(Buffer& stream);
	void writeType(Buffer& stream) const;
};

class TypeDef : public Type {
public:
	TypeDef();
	TypeDef(const std::string& name, const std::vector<TypeComponent *>& contents);
	TypeDef(Buffer& stream);
	TypeDef(const TypeDef& copyType);
	TypeDef& operator=(const TypeDef& assignType);
	virtual ~TypeDef();

	std::string name;
	std::vector<TypeComponent *> contents;

// marshalling functions
	void readType(Buffer& stream);
	void writeType(Buffer& stream) const;
};

class EnumComponent : public Type {
public:
	EnumComponent();
	EnumComponent(const std::string& name, long value);
	EnumComponent(Buffer& stream);
	EnumComponent(const EnumComponent& copyType);
	EnumComponent& operator=(const EnumComponent& assignType);
	virtual ~EnumComponent();

	std::string name;
	long value;

// marshalling functions
	void readType(Buffer& stream);
	void writeType(Buffer& stream) const;
};

class EnumDef : public Type {
public:
	EnumDef();
	EnumDef(const std::string& name, const std::vector<EnumComponent *>& contents);
	EnumDef(Buffer& stream);
	EnumDef(const EnumDef& copyType);
	EnumDef& operator=(const EnumDef& assignType);
	virtual ~EnumDef();

	std::string name;
	std::vector<EnumComponent *> contents;

// marshalling functions
	void readType(Buffer& stream);
	void writeType(Buffer& stream) const;
};

class ModuleDef : public Type {
public:
	ModuleDef();
	ModuleDef(const std::string& moduleName, const std::vector<ModuleDef *>& modules, const std::vector<EnumDef *>& enums, const std::vector<TypeDef *>& types, const std::vector<InterfaceDef *>& interfaces);
	ModuleDef(Buffer& stream);
	ModuleDef(const ModuleDef& copyType);
	ModuleDef& operator=(const ModuleDef& assignType);
	virtual ~ModuleDef();

	std::string moduleName;
	std::vector<ModuleDef *> modules;
	std::vector<EnumDef *> enums;
	std::vector<TypeDef *> types;
	std::vector<InterfaceDef *> interfaces;

// marshalling functions
	void readType(Buffer& stream);
	void writeType(Buffer& stream) const;
};

class InterfaceRepo : virtual public Object {
public:
	static InterfaceRepo *_fromString(std::string objectref);
	static InterfaceRepo *_fromReference(ObjectReference ref, bool needcopy);

	inline InterfaceRepo *_copy() {
		assert(_refCnt > 0);
		_refCnt++;
		return this;
	}

	virtual long insertModule(const ModuleDef& newModule) = 0;
	virtual void removeModule(long moduleID) = 0;
	virtual InterfaceDef* queryInterface(const std::string& name) = 0;
	virtual TypeDef* queryType(const std::string& name) = 0;
};

typedef ReferenceHelper<InterfaceRepo> InterfaceRepo_var;

class InterfaceRepo_stub : virtual public InterfaceRepo, virtual public Object_stub {
protected:
	InterfaceRepo_stub();

public:
	InterfaceRepo_stub(Connection *connection, long objectID);

	long insertModule(const ModuleDef& newModule);
	void removeModule(long moduleID);
	InterfaceDef* queryInterface(const std::string& name);
	TypeDef* queryType(const std::string& name);
};

class InterfaceRepo_skel : virtual public InterfaceRepo, virtual public Object_skel {
public:
	InterfaceRepo_skel();

	static std::string _interfaceNameSkel();
	std::string _interfaceName();
	void _buildMethodTable();
	void *_cast(std::string interface);
	void dispatch(Buffer *request, Buffer *result,long methodID);
};

#endif /* CORE_H */
