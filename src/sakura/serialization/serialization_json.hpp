#pragma once
#include <iostream>

#include "serialization_base.hpp"

#define RAPIDJSON_HAS_STDSTRING 1

#include "../../ext/rapidjson/include/rapidjson/document.h"
#include "../../ext/rapidjson/include/rapidjson/stringbuffer.h"
#include "../../ext/rapidjson/include/rapidjson/writer.h"

namespace sakura::ser {
	/// <summary>
	/// The JsonWriter takes a value from C++
	/// and writes it to json format using rapidjson
	/// </summary>
	struct JsonWriter
	{
		rapidjson::StringBuffer out;
		rapidjson::Writer<rapidjson::StringBuffer> writer;
		/*JsonWriter(rapidjson::StringBuffer& out_) : out(std::move(out_)), writer(out) {}*/
		JsonWriter() : writer(out) {}

		const char* get_string() const { return out.GetString(); }
	};

	template<typename T>
	inline typename std::enable_if_t<std::is_arithmetic_v<T> && !std::is_signed_v<T>, void>
	visit(JsonWriter& writer, const T& value)
	{
		writer.writer.Uint64(static_cast<sakura::u64>(value));
	}

	template<typename T>
	inline typename std::enable_if_t<std::is_arithmetic_v<T> && std::is_signed_v<T>, void>
	visit(JsonWriter& writer, const T& value)
	{
		writer.writer.Int64(static_cast<sakura::i64>(value));
	}

	template<> inline void visit(JsonWriter& writer, const bool& value)
	{
		writer.writer.Bool(value);
	}

	template<> inline void visit(JsonWriter& writer, const double& value)
	{
		writer.writer.Double(value);
	}

	template<typename T>
	inline typename std::enable_if<std::is_enum<T>::value, void>::type visit(JsonWriter& writer, const T& value)
	{
		visit(writer, std::underlying_type_t<T>(value));
	}

	inline void visit(JsonWriter& writer, const std::string& string)
	{
		writer.writer.String(string);
	}

	template<typename Element> void visit(JsonWriter& writer, const std::vector<Element>& vector)
	{
		writer.writer.StartArray();
		for (const auto& element : vector) {
			visit(writer, element);
		}
		writer.writer.EndArray();
	}

	template<typename Element, size_t N>
	void visit(JsonWriter& writer, const std::array<Element, N>& arr)
	{
		writer.writer.StartArray();
		for (const auto& element : arr) {
			visit(writer, element);
		}
		writer.writer.EndArray();
	}

	template<typename T, size_t N> void visit(JsonWriter& writer, T (&arr)[N])
	{
		writer.writer.StartArray();
		for (size_t i = 0; i < N; ++i) {
			visit(writer, arr[i]);
		}
		writer.writer.EndArray();
	}

	template<> struct StructVisitor<JsonWriter>
	{
		JsonWriter& writer;

		StructVisitor(const char*, JsonWriter& writer_) : writer(writer_)
		{
			writer.writer.StartObject();
		}

		~StructVisitor() { writer.writer.EndObject(); }

		template<typename T> StructVisitor& field(const char* label, const T& value)
		{
			writer.writer.Key(label);
			visit(writer, value);
			return *this;
		}
	};

	/// <summary>
	/// The JsonReader takes a stream of json format
	/// and reads it back to a C++ object using rapidjson
	/// </summary>
	struct JsonReader
	{
		rapidjson::Document document;
		static inline bool from_string(const char* json, JsonReader& outReader);
	};

	inline bool JsonReader::from_string(const char* json, JsonReader& outReader)
	{
		rapidjson::Document doc;
		doc.Parse(json);
		if (doc.HasParseError()) {
			return false;
		}

		outReader.document = std::move(doc);
	}

	template<typename T>
	inline typename std::enable_if_t<std::is_arithmetic_v<T> && !std::is_signed_v<T>, void>
	visit(JsonReader& reader, T& value)
	{
		if (!reader.document.IsUint64()) {
			return;
		}
		value = static_cast<T>(reader.in.GetUint64());
	}

	template<typename T>
	inline typename std::enable_if_t<std::is_arithmetic_v<T> && std::is_signed_v<T>, void>
	visit(JsonReader& reader, T& value)
	{
		if (!reader.document.IsInt64()) {
			return;
		}
		value = static_cast<T>(reader.in.GetInt64());
	}

	template<> inline void visit(JsonReader& reader, bool& value)
	{
		if (!reader.document.IsBool()) {
			return;
		}
		// #SK_TODO: check for IsNumber maybe and check if it's != 0
		value = reader.document.GetBool();
	}

	template<> inline void visit(JsonReader& reader, double& value)
	{
		if (!reader.document.IsNumber()) {
			return;
		}
		value = reader.document.GetDouble();
	}

	template<> inline void visit(JsonReader& reader, float& value)
	{
		if (!reader.document.IsNumber()) {
			return;
		}
		value = reader.document.GetFloat();
	}

	template<typename T>
	inline typename std::enable_if<std::is_enum<T>::value, void>::type visit(JsonReader& reader, T& value)
	{
		typename std::underlying_type_t<T> v;
		visit(reader, v);
		value = T(v);
	}

	inline void visit(JsonReader& reader, std::string& string)
	{
		if (!reader.document.IsString()) {
			return;
		}
		string = reader.document.GetString();
	}

	template<typename Element> void visit(JsonReader& reader, std::vector<Element>& vector)
	{
		if (!reader.document.IsArray()) {
			return;
		}

		vector.clear();
		for (auto& json_element : reader.document.GetArray()) {
			vector.push_back(Element());
			JsonReader element_reader { json_element };
			visit(element_reader, vector.back());
		}
	}

	template<typename Element, size_t N> void visit(JsonReader& reader, std::array<Element, N>& arr)
	{
		if (!reader.document.IsArray()) {
			return;
		}

		for (size_t i = 0; i < N; ++i) {
			Element& json_element = reader.document.GetArray()[i];
			JsonReader element_reader { json_element };
			visit(element_reader, arr[i]);
		}
	}

	template<typename T, size_t N> void visit(JsonReader& reader, T (&arr)[N])
	{
		if (!reader.document.IsArray()) {
			return;
		}

		for (size_t i = 0; i < N; ++i) {
			Element& json_element = reader.document.GetArray()[i];
			JsonReader element_reader { json_element };
			visit(element_reader, arr[i]);
		}
	}

	template<> struct StructVisitor<JsonReader>
	{
		const char* name;
		JsonReader& reader;
		const rapidjson::Value& input;

		StructVisitor(const char* name_, JsonReader& reader_)
		: name(name_), reader(reader_), input(reader.document)
		{}
		~StructVisitor() {}

		template<typename T> StructVisitor& field(const char* label, T& value)
		{
			auto i = input.FindMember(label);
			if (i != input.MemberEnd()) {
				JsonReader field_reader { i->value };
				visit(field_reader, value);
			}
			return *this;
		}
	};

} // namespace sakura::ser
