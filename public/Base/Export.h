#pragma once

#include "Base/Template.h"

#include <vector>

#ifdef CORE_BUILD_SHARED
#define CORE_API API_EXPORT
#else
#define CORE_API API_IMPORT
#endif

#ifdef TOOL_BUILD_SHARED
#define TOOL_API API_EXPORT
#else
#define TOOL_API API_IMPORT
#endif

#define EXPORT_OBJECT_ID_APIS(IDType) \
	void SetID(IDType id); \
	IDType GetID() const;

#define PIMPL_ID_APIS(Type) \
	void Type::SetID(Type##ID id) { m_p##Type##Impl->SetID(id); } \
	Type##ID Type::GetID() const { return m_p##Type##Impl->GetID(); }

#define IMPLEMENT_ID_APIS(TypeID, MemberVariable) \
	void SetID(TypeID id) { MemberVariable = id; } \
	TypeID GetID() const { return MemberVariable; }

#define EXPORT_NAME_APIS() \
	void SetName(const char* pName); \
	void SetName(std::string name); \
	const char* GetName() const;

#define PIMPL_NAME_APIS(Type) \
	void Type::SetName(const char* pName) { return m_p##Type##Impl->SetName(pName); } \
	void Type::SetName(std::string name) { return m_p##Type##Impl->SetName(cd::MoveTemp(name)); } \
	const char* Type::GetName() const { return m_p##Type##Impl->GetName().c_str(); }

#define IMPLEMENT_NAME_APIS(MemberVariable) \
	void SetName(std::string name) { MemberVariable = cd::MoveTemp(name); } \
	std::string& GetName() { return MemberVariable; } \
	const std::string& GetName() const { return MemberVariable; }

#define EXPORT_VECTOR_DATA_APIS(ElementType) \
	void Set##ElementType##Capacity(uint32_t count); \
	void Set##ElementType##Count(uint32_t count); \
	uint32_t Get##ElementType##Count() const; \
	void Set##ElementType##s(std::vector<ElementType> elements); \
	void Set##ElementType(uint32_t index, ElementType element); \
	std::vector<ElementType>& Get##ElementType##s(); \
	const std::vector<ElementType>& Get##ElementType##s() const; \
	ElementType& Get##ElementType(uint32_t index); \
	const ElementType& Get##ElementType(uint32_t index) const;

#define PIMPL_VECTOR_DATA_APIS(Type, ElementType) \
	void Type::Set##ElementType##Capacity(uint32_t count) { m_p##Type##Impl->Set##ElementType##Capacity(count); } \
	void Type::Set##ElementType##Count(uint32_t count) { m_p##Type##Impl->Set##ElementType##Count(count); } \
	uint32_t Type::Get##ElementType##Count() const { return m_p##Type##Impl->Get##ElementType##Count(); } \
	void Type::Set##ElementType##s(std::vector<ElementType> elements) { m_p##Type##Impl->Set##ElementType##s(cd::MoveTemp(elements)); } \
	void Type::Set##ElementType(uint32_t index, ElementType element) { m_p##Type##Impl->Set##ElementType(index, cd::MoveTemp(element)); } \
	std::vector<ElementType>& Type::Get##ElementType##s() { return m_p##Type##Impl->Get##ElementType##s(); } \
	const std::vector<ElementType>& Type::Get##ElementType##s() const { return m_p##Type##Impl->Get##ElementType##s(); } \
	ElementType& Type::Get##ElementType(uint32_t index) { return m_p##Type##Impl->Get##ElementType(index); } \
	const ElementType& Type::Get##ElementType(uint32_t index) const { return m_p##Type##Impl->Get##ElementType(index); }

#define IMPLEMENT_VECTOR_DATA_APIS(ElementType, MemberVariable) \
	void Set##ElementType##Capacity(uint32_t count) { MemberVariable.reserve(count); } \
	void Set##ElementType##Count(uint32_t count) { MemberVariable.resize(count); } \
	uint32_t Get##ElementType##Count() const { return static_cast<uint32_t>(MemberVariable.size()); } \
	void Set##ElementType##s(std::vector<ElementType> elements) { MemberVariable = cd::MoveTemp(elements); } \
	void Set##ElementType(uint32_t index, ElementType element) { MemberVariable[index] = cd::MoveTemp(element); } \
	std::vector<ElementType>& Get##ElementType##s() { return MemberVariable; } \
	const std::vector<ElementType>& Get##ElementType##s() const { return MemberVariable; } \
	ElementType& Get##ElementType(uint32_t index) { return MemberVariable[index]; } \
	const ElementType& Get##ElementType(uint32_t index) const { return MemberVariable[index]; }
