#pragma once
#include <iostream>

namespace sakura {
	namespace ser {
		template<typename Visitor> struct StructVisitor
		{
			const char* name;
			Visitor& visitor;
			template<typename T> StructVisitor& field(const char* label, T& value)
			{
				visit(visitor, value);
				return *this;
			}
		};

		template<typename Visitor>
		StructVisitor<Visitor> visit_struct(const char* name, Visitor& visitor)
		{
			return StructVisitor<Visitor> { name, visitor };
		}

		template<typename Visitor> struct EnumVisitor
		{
			const char* name;
			Visitor& visitor;
			template<typename T> EnumVisitor& elem(const char* label, T value) { return *this; }
		};

		template<typename Visitor> EnumVisitor<Visitor> visit_enum(const char* name, Visitor& visitor)
		{
			return EnumVisitor<Visitor> { name, visitor };
		}

		struct OStreamWriter;
		template<typename T> void print_struct(std::ostream& out, const T& s)
		{
			OStreamWriter writer { out };
			visit(writer, s);
		}
		template<typename T> void print_struct(const T& s) { print_struct(std::cout, s); }
	} // namespace ser
} // namespace sakura

#define SAKURA_STRUCT(TYPE, FIELDS)                                            \
	namespace sakura::ser {                                                     \
		template<typename Visitor> void visit(Visitor& visitor, TYPE& obj)       \
		{                                                                        \
			visit_struct(#TYPE, visitor) FIELDS;                                  \
		}                                                                        \
		template<typename Visitor> void visit(Visitor& visitor, const TYPE& obj) \
		{                                                                        \
			visit_struct(#TYPE, visitor) FIELDS;                                  \
		}                                                                        \
	}
#define SAKURA_FIELD(NAME) .field(#NAME, obj.NAME)
#define SAKURA_IS_FRIEND(TYPE)                                \
	template<typename V> friend void SAKURA::visit(V&, TYPE&); \
	template<typename V> friend void SAKURA::visit(V&, const TYPE&);

#define SAKURA_ENUM(TYPE, ELEMS)                                          \
	namespace sakura::ser {                                                \
		template<typename Visitor> void visit(Visitor& visitor, TYPE, TYPE) \
		{                                                                   \
			using EnumTypeName = TYPE;                                       \
			visit_enum(#TYPE, visitor) ELEMS;                                \
		}                                                                   \
	}
#define SAKURA_ENUM_ELEM(NAME) .elem(#NAME, EnumTypeName::NAME)

namespace sakura::ser {
	struct OStreamWriter
	{
		std::ostream& out;
		OStreamWriter(std::ostream& out_ = std::cout) : out(out_) {}
	};

	template<typename T>
	inline typename std::enable_if<std::is_arithmetic<T>::value, void>::type
	visit(OStreamWriter& writer, const T& value)
	{
		writer.out << value;
	}

	template<typename T>
	inline typename std::enable_if<std::is_enum<T>::value, void>::type
	visit(OStreamWriter& writer, const T& value)
	{
		writer.out << (long long)(value);
	}

	inline void visit(OStreamWriter& writer, const std::string& string)
	{
#if __cplusplus >= 201400L
		writer.out << std::quoted(string);
#else
		writer.out << '"' << string << '"';
#endif
	}

	template<typename Element> void visit(OStreamWriter& writer, const std::vector<Element>& vector)
	{
		writer.out << '[';
		for (size_t i = 0; i < vector.size(); ++i) {
			if (i != 0)
				writer.out << ", ";
			visit(writer, vector[i]);
		}
		writer.out << ']';
	}

	template<typename T, size_t N> void visit(OStreamWriter& writer, T (&arr)[N])
	{
		writer.out << '[';
		for (size_t i = 0; i < N; ++i) {
			if (i != 0)
				writer.out << ", ";
			visit(writer, arr[i]);
		}
		writer.out << ']';
	}

	template<> struct StructVisitor<OStreamWriter>
	{
		const char* name;
		OStreamWriter& writer;
		bool first;

		StructVisitor(const char* name_, OStreamWriter& writer_)
		: name(name_), writer(writer_), first(true)
		{
			writer.out << name << '{';
		}

		~StructVisitor() { writer.out << '}'; }

		template<typename T> StructVisitor& field(const char* label, const T& value)
		{
			if (!first)
				writer.out << ", ";
			first = false;
			writer.out << label << ':';
			visit(writer, value);
			return *this;
		}
	};
} // namespace sakura::ser
