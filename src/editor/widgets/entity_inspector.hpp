#pragma once
#include "../scene/scene.hpp"
#include "log/log.hpp"
#include <string>
#include <sstream>
#include "serialization/serialization.hpp"

namespace sakura::ser {
	struct ImGuiEntityInspectorWalker
	{
		sakura::ecs::Entity entity = sakura::ecs::Entity::Invalid;
		const char* name_;
		using VisitComponentsFn = void (*)(ImGuiEntityInspectorWalker&, sakura::ecs::ECS& ecs, sakura::ecs::Entity entity);
		VisitComponentsFn visit_components_callback = nullptr;
	};

	template<typename T>
	inline typename std::enable_if<std::is_arithmetic<T>::value, void>::type
	visit(ImGuiEntityInspectorWalker& writer, const T& value)
	{
		sakura::logging::log_info("%d", (long long)(value));
	}

	template<typename T>
	inline typename std::enable_if<std::is_enum<T>::value, void>::type
	visit(ImGuiEntityInspectorWalker& writer, const T& value)
	{
		sakura::logging::log_info("%d", (long long)(value));
	}

	inline void visit(ImGuiEntityInspectorWalker& writer, const std::string& string)
	{
		sakura::logging::log_info("%s", string.c_str());
	}

	template<typename Element>
	void visit(ImGuiEntityInspectorWalker& writer, const std::vector<Element>& vector)
	{
		sakura::logging::log_info("vector of %d elements", N);
	}

	template<typename T, size_t N> void visit(ImGuiEntityInspectorWalker& walker, T (&arr)[N])
	{
		sakura::logging::log_info("array of %d elements", N);
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
		{}
		~StructVisitor() {}

		template<typename T> StructVisitor& field(const char* label, T& value)
		{
			std::ostringstream stream;
			stream << label << ": ";
			sakura::ser::print_struct(stream, value);
			sakura::logging::log_info("%s", stream.str().c_str());
			return *this;
		}

		StructVisitor& field(const char* label, sakura::editor::Scene& value)
		{
			visit(imgui_walker_, value);
			return *this;
		}

	private:
		const char* name_ = nullptr;
		ImGuiEntityInspectorWalker& imgui_walker_;
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