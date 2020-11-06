#pragma once
#include "../scene/scene.hpp"
#include "log/log.hpp"
#include "../ext/imgui/imgui.h"
#include "../ext/imgui/misc/cpp/imgui_stdlib.h"

#include <string>
#include "../game_lib/components/components.hpp"

namespace sakura::ser {
	struct ImGuiEntityInspectorWalker
	{
		sakura::ecs::Entity entity = sakura::ecs::Entity::Invalid;
		const char* name_;
		using VisitComponentsFn = void (*)(ImGuiEntityInspectorWalker&, sakura::ecs::ECS& ecs, sakura::ecs::Entity entity);
		VisitComponentsFn visit_components_callback = nullptr;
	};

	template<typename T> struct ImguiEnumWriter
	{
		const char* name = "";
		typename std::underlying_type<T>::type accepted_value;
		typename std::underlying_type<T>::type final_value = -1; // READ THIS from imgui to select new value
		const char* initial_name = "";
	};

	template<typename EnumType> struct EnumVisitor<ImguiEnumWriter<EnumType>>
	{
		const char* name;
		ImguiEnumWriter<EnumType>& visitor;
		bool begun_combo = false;

		EnumVisitor(const char* name_, ImguiEnumWriter<EnumType>& visitor_)
		: name(name_), visitor(visitor_)
		{
			if (ImGui::BeginCombo(visitor.name, visitor.initial_name)) {
				begun_combo = true;
			}
		}
		~EnumVisitor()
		{
			if (begun_combo)
				ImGui::EndCombo();
		}

		template<typename T> EnumVisitor& elem(const char* label, T value)
		{
			if (begun_combo) {
				const bool is_selected = ((typename std::underlying_type<T>::type)value == visitor.accepted_value);
				if (ImGui::Selectable(label, is_selected))
					visitor.final_value = (typename std::underlying_type<T>::type)value;

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			return *this;
		}
	};

	template<typename T> struct ImguiEnumGatherer
	{
		typename std::underlying_type<T>::type accepted_value;
		const char* gathered_name = "";
	};

	template<typename EnumType> struct EnumVisitor<ImguiEnumGatherer<EnumType>>
	{
		const char* name;
		ImguiEnumGatherer<EnumType>& visitor;
		template<typename T> EnumVisitor& elem(const char* label, T value)
		{
			if (value == (T)visitor.accepted_value) {
				visitor.gathered_name = label;
			}
			return *this;
		}
	};

	/// <summary>
	/// Write to json stream
	/// </summary>
	struct ImguiWriter
	{
		const char* name;
		ImguiWriter(const char* name_) : name(name_) {}
		ImguiWriter() {}
	};

	template<typename T>
	inline typename std::enable_if_t<std::is_integral<T>::value, void> visit(ImguiWriter& writer, T& value)
	{
		ImGui::InputInt(writer.name, &value);
	}
	inline void visit(ImguiWriter& writer, bool& value) { ImGui::Checkbox(writer.name, &value); }

	inline void visit(ImguiWriter& writer, float& value) { ImGui::InputFloat(writer.name, &value); }
	inline void visit(ImguiWriter& writer, double& value)
	{
		ImGui::InputDouble(writer.name, &value);
	}

	template<typename T>
	inline typename std::enable_if<std::is_enum<T>::value, void>::type visit(ImguiWriter& writer, T& value)
	{
		ImguiEnumGatherer<T> enum_gatherer;
		enum_gatherer.accepted_value = typename std::underlying_type<T>::type(value);
		visit(enum_gatherer, T {}, T {});

		ImguiEnumWriter<T> enum_writer;
		enum_writer.name = writer.name;
		enum_writer.initial_name = enum_gatherer.gathered_name;
		enum_writer.accepted_value = enum_writer.final_value = typename std::underlying_type<T>::type(value);
		visit(enum_writer, T {}, T {});

		value = (T)enum_writer.final_value;
	}

	inline void visit(ImguiWriter& writer, std::string& string)
	{
		ImGui::InputText(writer.name, &string);
	}

	template<typename Element> void visit(ImguiWriter& writer, std::vector<Element>& vector)
	{
		if (ImGui::CollapsingHeader(writer.name)) {
			int count = 0;
			for (auto element : vector) {
				std::string count_as_string = std::to_string(count);
				ImguiWriter element_writer(count_as_string.c_str());
				visit(element_writer, element);
			}
		}
	}

	template<typename T, size_t N> void visit(ImguiWriter& writer, T (&arr)[N])
	{
		if (ImGui::CollapsingHeader(writer.name)) {
			for (size_t i = 0; i < N; ++i) {
				auto& element = arr[i];
				std::string i_as_string = std::to_string(i);
				ImguiWriter element_writer(i_as_string.c_str());
				visit(element_writer, element);
			}
		}
	}

	inline void visit(ImGuiEntityInspectorWalker& walker, sakura::editor::Scene& scene)
	{
		auto& ecs = scene.ecs;
		auto& entity = walker.entity;
		if (!ecs.is_entity_handle_active(entity))
			return;

		walker.visit_components_callback(walker, scene.ecs, walker.entity);
	}

	template<> struct StructVisitor<ImGuiEntityInspectorWalker>
	{
		StructVisitor(const char* name, ImGuiEntityInspectorWalker& walker)
		: name_(name), imgui_walker_(walker)
		{
			header_open_ = ImGui::CollapsingHeader(name_);
		}
		~StructVisitor() {}

		template<typename T> StructVisitor& field(const char* label, T& value)
		{
			if (header_open_) {
				ImguiWriter writer { label };
				visit(writer, value);
			}
			return *this;
		}

		StructVisitor& field(const char* label, sakura::editor::Scene& value)
		{
			if (header_open_) {
				visit(imgui_walker_, value);
			}
			return *this;
		}

		StructVisitor& field(const char* label, float2& value)
		{
			if (header_open_) {
				ImGui::DragFloat2(label, &value.x, 0.1, -FLT_MAX, FLT_MAX);
			}
			return *this;
		}

	private:
		const char* name_ = nullptr;
		ImGuiEntityInspectorWalker& imgui_walker_;
		bool header_open_ = true;
	};
} // namespace sakura::ser

namespace sakura::editor::widgets {
	class EntityInspector
	{
	public:
		void draw(sakura::editor::Scene& scene,
					 const sakura::ecs::Entity& entity,
					 sakura::ser::ImGuiEntityInspectorWalker::VisitComponentsFn visit_components_callback);

		void set_name(const char* name) { name_ = name; }
		const char* get_name() const { return name_; }

	private:
		bool window_open_ = true;
		const char* name_ = "Inspector";
	};
} // namespace sakura::editor::widgets