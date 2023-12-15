#pragma once

////////////////////////////////////////////////////////////////////////////////////////
// Class Declration
////////////////////////////////////////////////////////////////////////////////////////
#define DECLARE_SCENE_CLASS(Class) \
	static const char* GetClassName() { return #Class; } \
public: \
	Class(); \
	explicit Class(InputArchive& inputArchive); \
	explicit Class(InputArchiveSwapBytes& inputArchive); \
	Class(const Class&) = delete; \
	Class& operator=(const Class&) = delete; \
	Class(Class&&); \
	Class& operator=(Class&&); \
	~Class(); \
	Class& operator<<(InputArchive& inputArchive); \
	Class& operator<<(InputArchiveSwapBytes& inputArchive); \
	const Class& operator>>(OutputArchive& outputArchive) const; \
	const Class& operator>>(OutputArchiveSwapBytes& outputArchive) const; \
private: \
	Class##Impl* m_p##Class##Impl = nullptr; \
public: \

#define PIMPL_SCENE_CLASS(Class) \
	Class::Class() { m_p##Class##Impl = new Class##Impl(); } \
	Class::Class(InputArchive& inputArchive) { m_p##Class##Impl = new Class##Impl(inputArchive); } \
	Class::Class(InputArchiveSwapBytes& inputArchive) { m_p##Class##Impl = new Class##Impl(inputArchive); } \
	Class::Class(Class&& rhs) { *this = cd::MoveTemp(rhs); } \
	Class& Class::operator=(Class&& rhs) { std::swap(m_p##Class##Impl, rhs.m_p##Class##Impl); return *this; } \
	Class::~Class() \
	{ \
		if (m_p##Class##Impl) \
		{ \
			delete m_p##Class##Impl; \
			m_p##Class##Impl = nullptr; \
		} \
	} \
	Class& Class::operator<<(InputArchive& inputArchive) { *m_p##Class##Impl << inputArchive; return *this; } \
	Class& Class::operator<<(InputArchiveSwapBytes& inputArchive) { *m_p##Class##Impl << inputArchive; return *this; } \
	const Class& Class::operator>>(OutputArchive& inputArchive) const { *m_p##Class##Impl >> inputArchive; return *this; } \
	const Class& Class::operator>>(OutputArchiveSwapBytes& inputArchive) const { *m_p##Class##Impl >> inputArchive; return *this; } \

#define DECLARE_SCENE_IMPL_CLASS(Class) \
	Class##Impl() = default; \
	template<bool SwapBytesOrder> \
	explicit Class##Impl(TInputArchive<SwapBytesOrder>& inputArchive) { *this << inputArchive; } \
	Class##Impl(const Class##Impl&) = default; \
	Class##Impl& operator=(const Class##Impl&) = default; \
	Class##Impl(Class##Impl&&) = default; \
	Class##Impl& operator=(Class##Impl&&) = default; \
	~Class##Impl() = default; \

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
public: \

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
	Class##TypeTraits::Type Class::Get##Type() const { return m_p##Class##Impl->Get##Type(); } \

#define IMPLEMENT_SIMPLE_TYPE_APIS(Class, Type) \
public: \
	void Set##Type(Class##TypeTraits::Type value) { m_##Type = MoveTemp(value); } \
	Class##TypeTraits::Type& Get##Type() { return m_##Type; } \
	Class##TypeTraits::Type Get##Type() const { return m_##Type; } \
private: \
	Class##TypeTraits::Type m_##Type; \
public: \

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
public: \

////////////////////////////////////////////////////////////////////////////////////////
// Vector Type
////////////////////////////////////////////////////////////////////////////////////////
#define EXPORT_VECTOR_TYPE_APIS_WITH_PLURAL(Class, Type, Plural) \
	void Set##Type##Capacity(uint32_t count); \
	void Set##Type##Count(uint32_t count); \
	uint32_t Get##Type##Count() const; \
	void Set##Type##Plural(std::vector<Class##TypeTraits::Type> elements); \
	void Set##Type(uint32_t index, Class##TypeTraits::Type element); \
	std::vector<Class##TypeTraits::Type>& Get##Type##Plural(); \
	const std::vector<Class##TypeTraits::Type>& Get##Type##Plural() const; \
	Class##TypeTraits::Type& Get##Type(uint32_t index); \
	const Class##TypeTraits::Type& Get##Type(uint32_t index) const; \
	void Add##Type(Class##TypeTraits::Type element); \

#define PIMPL_VECTOR_TYPE_APIS_WITH_PLURAL(Class, Type, Plural) \
	void Class::Set##Type##Capacity(uint32_t count) { m_p##Class##Impl->Set##Type##Capacity(count); } \
	void Class::Set##Type##Count(uint32_t count) { m_p##Class##Impl->Set##Type##Count(count); } \
	uint32_t Class::Get##Type##Count() const { return m_p##Class##Impl->Get##Type##Count(); } \
	void Class::Set##Type##Plural(std::vector<Class##TypeTraits::Type> elements) { m_p##Class##Impl->Set##Type##Plural(cd::MoveTemp(elements)); } \
	void Class::Set##Type(uint32_t index, Class##TypeTraits::Type element) { m_p##Class##Impl->Set##Type(index, cd::MoveTemp(element)); } \
	std::vector<Class##TypeTraits::Type>& Class::Get##Type##Plural() { return m_p##Class##Impl->Get##Type##Plural(); } \
	const std::vector<Class##TypeTraits::Type>& Class::Get##Type##Plural() const { return m_p##Class##Impl->Get##Type##Plural(); } \
	Class##TypeTraits::Type& Class::Get##Type(uint32_t index) { return m_p##Class##Impl->Get##Type(index); } \
	const Class##TypeTraits::Type& Class::Get##Type(uint32_t index) const { return m_p##Class##Impl->Get##Type(index); } \
	void Class::Add##Type(Class##TypeTraits::Type element) { m_p##Class##Impl->Add##Type(cd::MoveTemp(element)); } \

#define IMPLEMENT_VECTOR_TYPE_APIS_WITH_PLURAL(Class, Type, Plural) \
public: \
	void Set##Type##Capacity(uint32_t count) { m_##Type##Plural.reserve(count); } \
	void Set##Type##Count(uint32_t count) { m_##Type##Plural.resize(count); } \
	uint32_t Get##Type##Count() const { return static_cast<uint32_t>(m_##Type##Plural.size()); } \
	void Set##Type##Plural(std::vector<Class##TypeTraits::Type> elements) { m_##Type##Plural = cd::MoveTemp(elements); } \
	void Set##Type(uint32_t index, Class##TypeTraits::Type element) { m_##Type##Plural[index] = cd::MoveTemp(element); } \
	std::vector<Class##TypeTraits::Type>& Get##Type##Plural() { return m_##Type##Plural; } \
	const std::vector<Class##TypeTraits::Type>& Get##Type##Plural() const { return m_##Type##Plural; } \
	Class##TypeTraits::Type& Get##Type(uint32_t index) { return m_##Type##Plural[index]; } \
	const Class##TypeTraits::Type& Get##Type(uint32_t index) const { return m_##Type##Plural[index]; } \
	void Add##Type(Class##TypeTraits::Type element) { m_##Type##Plural.push_back(cd::MoveTemp(element)); } \
private: \
	std::vector<Class##TypeTraits::Type> m_##Type##Plural; \
public: \

// Default plural is using "s". But sometimes you will need to use "es" such as Meshes.
#define EXPORT_VECTOR_TYPE_APIS(Class, Type) EXPORT_VECTOR_TYPE_APIS_WITH_PLURAL(Class, Type, s)
#define PIMPL_VECTOR_TYPE_APIS(Class, Type) PIMPL_VECTOR_TYPE_APIS_WITH_PLURAL(Class, Type, s)
#define IMPLEMENT_VECTOR_TYPE_APIS(Class, Type) IMPLEMENT_VECTOR_TYPE_APIS_WITH_PLURAL(Class, Type, s)