#pragma once

#include "type_aliases.hpp"

namespace sakura {
	namespace time {
		using Cycles = u64;
		using Seconds = f32;
		using SecondsDoublePrecision = f64;

		constexpr f32 DEFAULT_IDEAL_FRAME_RATE = 30.0f;

		Cycles seconds_to_cycles(time::SecondsDoublePrecision time_in_seconds, f32 cycles_per_second);
		Seconds cycles_to_seconds(time::Cycles time_in_cycles, f32 cycles_per_second);

		Seconds calc_delta_time(Cycles end, Cycles begin, f32 cycles_per_second);
	} // namespace time

	class Clock
	{
	public:
		/// <summary>
		/// Reads the current time cycles value from the high frequency timer, rather than letting the user set it.
		/// </summary>
		explicit Clock(f32 frame_rate = time::DEFAULT_IDEAL_FRAME_RATE);

		/// <summary>
		/// Start the clock with the current CPU time in cycle, as returned by the high resolution
		/// timer. Sets is_paused_ to false.
		/// </summary>
		void start_with_time_now();

		/// <summary>
		/// Start the clock with a given time offset in seconds.
		/// Sets is_paused_ to false.
		/// </summary>
		/// <param name="start_time_in_seconds"></param>
		void start_from(time::Seconds start_time_in_seconds);

		/// <summary>
		/// Returns the difference between this clock absolute time and that of another clock, in seconds.
		/// </summary>
		time::Seconds calc_delta_seconds(const Clock& other);

		/// <summary>
		/// Should be called once per frame, passing the real measured frame delta time in seconds
		/// </summary>
		void update(time::SecondsDoublePrecision dt_real_seconds);

		/// <summary>
		/// Add one ideal frame interval, scaled by the time scale.
		/// </summary>
		void single_step();

		f32 ideal_frame_rate() const { return ideal_frame_rate_; }
		void set_ideal_frame_rate(f32 value) { ideal_frame_rate_ = value; }

		time::Cycles delta_time_cycles() const { return delta_time_cycles_; }
		time::Seconds delta_time_seconds() const
		{
			return time::cycles_to_seconds(delta_time_cycles(), cycles_per_second_);
		}

		time::Cycles time_cycles() const { return time_cycles_; }

		bool is_paused() const { return is_paused_; }
		void set_is_paused(bool value) { is_paused_ = value; }

		f32 time_scale() const { return time_scale_; }
		void set_time_scale(f32 value) { time_scale_ = value; }

	private:
		f32 ideal_frame_rate_;
		f32 cycles_per_second_;
		time::Cycles delta_time_cycles_ = 0;
		time::Cycles time_cycles_ = 0;
		bool is_paused_ = false;
		f32 time_scale_ = 1.0f;
	};

	/// <summary>
	/// Class that allows to query a time duration from the time the object gets created until you call get()
	/// </summary>
	class Duration
	{
	public:
		Duration();

		time::Seconds get() const;

	private:
		f32 cycles_per_second_;
		time::Cycles time_cycles_;
	};
} // namespace sakura