#pragma once

////////////////////////////////////////////////////////////////////////////////////////
// String Type
////////////////////////////////////////////////////////////////////////////////////////
#define EXPORT_STRING_TYPE_APIS(Class, Type) \
	void Set##Type(Class##TypeTraits::Type id); \
	const char* Get##Type() const;

#define PIMPL_STRING_TYPE_APIS(Class, Type) \
	void Class::Set##Type(Class##TypeTraits::Type value) { return m_p##Class##Impl->Set##Type(cd::MoveTemp(value)); } \
	const char* Class::Get##Type() const { return m_p##Class##Impl->Get##Type().c_str(); } \

#define IMPLEMENT_STRING_TYPE_APIS(Class, Type) \
public: \
	void Set##Type(Class##TypeTraits::Type value) { m_##Type = MoveTemp(value); } \
	Class##TypeTraits::Type& Get##Type() { return m_##Type; } \
	const Class##TypeTraits::Type& Get##Type() const { return m_##Type; } \
private: \
	Class##TypeTraits::Type m_##Type; \
public:

////////////////////////////////////////////////////////////////////////////////////////
// Simple Type : bool, char, int, uint, float, double, ... in small size
////////////////////////////////////////////////////////////////////////////////////////
#define EXPORT_SIMPLE_TYPE_APIS(Class, Type) \
	void Set##Type(Class##TypeTraits::Type id); \
	Class##TypeTraits::Type& Get##Type(); \
	Class##TypeTraits::Type Get##Type() const;

#define PIMPL_SIMPLE_TYPE_APIS(Class, Type) \
	void Class::Set##Type(Class##TypeTraits::Type value) { return m_p##Class##Impl->Set##Type(cd::MoveTemp(value)); } \
	Class##TypeTraits::Type& Class::Get##Type() { return m_p##Class##Impl->Get##Type(); } \
	Class##TypeTraits::Type Class::Get##Type() const { return m_p##Class##Impl->Get##Type(); }

#define IMPLEMENT_SIMPLE_TYPE_APIS(Class, Type) \
public: \
	void Set##Type(Class##TypeTraits::Type value) { m_##Type = MoveTemp(value); } \
	Class##TypeTraits::Type& Get##Type() { return m_##Type; } \
	Class##TypeTraits::Type Get##Type() const { return m_##Type; } \
private: \
	Class##TypeTraits::Type m_##Type; \
public:

////////////////////////////////////////////////////////////////////////////////////////
// Complex Type : customized class, ... in large size
////////////////////////////////////////////////////////////////////////////////////////
#define EXPORT_COMPLEX_TYPE_APIS(Class, Type) \
	void Set##Type(Class##TypeTraits::Type id); \
	Class##TypeTraits::Type& Get##Type(); \
	const Class##TypeTraits::Type& Get##Type() const;

#define PIMPL_COMPLEX_TYPE_APIS(Class, Type) \
	void Class::Set##Type(Class##TypeTraits::Type value) { return m_p##Class##Impl->Set##Type(cd::MoveTemp(value)); } \
	Class##TypeTraits::Type& Class::Get##Type() { return m_p##Class##Impl->Get##Type(); } \
	const Class##TypeTraits::Type& Class::Get##Type() const { return m_p##Class##Impl->Get##Type(); }

#define IMPLEMENT_COMPLEX_TYPE_APIS(Class, Type) \
public: \
	void Set##Type(Class##TypeTraits::Type value) { m_##Type = MoveTemp(value); } \
	Class##TypeTraits::Type& Get##Type() { return m_##Type; } \
	const Class##TypeTraits::Type& Get##Type() const { return m_##Type; } \
private: \
	Class##TypeTraits::Type m_##Type;  \
public:

////////////////////////////////////////////////////////////////////////////////////////
// Vector Type
////////////////////////////////////////////////////////////////////////////////////////
#define EXPORT_VECTOR_TYPE_APIS(Class, Type) \
	void Set##Type##Capacity(uint32_t count); \
	void Set##Type##Count(uint32_t count); \
	uint32_t Get##Type##Count() const; \
	void Set##Type##s(std::vector<Class##TypeTraits::Type> elements); \
	void Set##Type(uint32_t index, Class##TypeTraits::Type element); \
	std::vector<Class##TypeTraits::Type>& Get##Type##s(); \
	const std::vector<Class##TypeTraits::Type>& Get##Type##s() const; \
	Class##TypeTraits::Type& Get##Type(uint32_t index); \
	const Class##TypeTraits::Type& Get##Type(uint32_t index) const; \
	void Add##Type(Class##TypeTraits::Type element);

#define PIMPL_VECTOR_TYPE_APIS(Class, Type) \
	void Class::Set##Type##Capacity(uint32_t count) { m_p##Class##Impl->Set##Type##Capacity(count); } \
	void Class::Set##Type##Count(uint32_t count) { m_p##Class##Impl->Set##Type##Count(count); } \
	uint32_t Class::Get##Type##Count() const { return m_p##Class##Impl->Get##Type##Count(); } \
	void Class::Set##Type##s(std::vector<Class##TypeTraits::Type> elements) { m_p##Class##Impl->Set##Type##s(cd::MoveTemp(elements)); } \
	void Class::Set##Type(uint32_t index, Class##TypeTraits::Type element) { m_p##Class##Impl->Set##Type(index, cd::MoveTemp(element)); } \
	std::vector<Class##TypeTraits::Type>& Class::Get##Type##s() { return m_p##Class##Impl->Get##Type##s(); } \
	const std::vector<Class##TypeTraits::Type>& Class::Get##Type##s() const { return m_p##Class##Impl->Get##Type##s(); } \
	Class##TypeTraits::Type& Class::Get##Type(uint32_t index) { return m_p##Class##Impl->Get##Type(index); } \
	const Class##TypeTraits::Type& Class::Get##Type(uint32_t index) const { return m_p##Class##Impl->Get##Type(index); } \
	void Class::Add##Type(Class##TypeTraits::Type element) { m_p##Class##Impl->Add##Type(cd::MoveTemp(element)); }

#define IMPLEMENT_VECTOR_TYPE_APIS(Class, Type) \
public: \
	void Set##Type##Capacity(uint32_t count) { m_##Type##s.reserve(count); } \
	void Set##Type##Count(uint32_t count) { m_##Type##s.resize(count); } \
	uint32_t Get##Type##Count() const { return static_cast<uint32_t>(m_##Type##s.size()); } \
	void Set##Type##s(std::vector<Class##TypeTraits::Type> elements) { m_##Type##s = cd::MoveTemp(elements); } \
	void Set##Type(uint32_t index, Class##TypeTraits::Type element) { m_##Type##s[index] = cd::MoveTemp(element); } \
	std::vector<Class##TypeTraits::Type>& Get##Type##s() { return m_##Type##s; } \
	const std::vector<Class##TypeTraits::Type>& Get##Type##s() const { return m_##Type##s; } \
	Class##TypeTraits::Type& Get##Type(uint32_t index) { return m_##Type##s[index]; } \
	const Class##TypeTraits::Type& Get##Type(uint32_t index) const { return m_##Type##s[index]; } \
	void Add##Type(Class##TypeTraits::Type element) { m_##Type##s.push_back(cd::MoveTemp(element)); } \
private: \
	std::vector<Class##TypeTraits::Type> m_##Type##s; \
public: