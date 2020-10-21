#include "time.hpp"

#include "platform/platform.hpp"

//////////////////////////////////////////////////////////////////////////
/// Utilities
//////////////////////////////////////////////////////////////////////////
sakura::time::Cycles sakura::time::seconds_to_cycles(time::SecondsDoublePrecision time_in_seconds, f32 cycles_per_second)
{
	return static_cast<time::Cycles>(time_in_seconds * cycles_per_second);
}

sakura::time::Seconds sakura::time::cycles_to_seconds(time::Cycles time_in_cycles, f32 cycles_per_second)
{
	return static_cast<time::Seconds>(time_in_cycles) / cycles_per_second;
}

sakura::time::Seconds sakura::time::calc_delta_time(Cycles end, Cycles begin, f32 cycles_per_second)
{
	auto dt_cycles = end - begin;
	return cycles_to_seconds(dt_cycles, cycles_per_second);
}

//////////////////////////////////////////////////////////////////////////
/// Clock 
//////////////////////////////////////////////////////////////////////////
sakura::Clock::Clock(f32 frame_rate /*= time::DEFAULT_IDEAL_FRAME_RATE*/) :
	ideal_frame_rate_(frame_rate),
	cycles_per_second_(static_cast<f32>(platform::get_high_resolution_timer_frequency()))
{ 
}

void sakura::Clock::start_with_time_now()
{
	set_is_paused(false);
	time_cycles_ = platform::get_high_resolution_timer_cycles();
}

void sakura::Clock::start_from(time::Seconds start_time_in_seconds)
{
	set_is_paused(false);
	time_cycles_ = time::seconds_to_cycles(start_time_in_seconds, cycles_per_second_);
}

sakura::time::Seconds sakura::Clock::calc_delta_seconds(const Clock& other)
{
	return time::calc_delta_time(time_cycles(), other.time_cycles(), cycles_per_second_);
}

void sakura::Clock::update(time::SecondsDoublePrecision dt_real_seconds)
{
	if (!is_paused())
	{
		delta_time_cycles_ = time::seconds_to_cycles(dt_real_seconds * time_scale(), cycles_per_second_);
		time_cycles_ += delta_time_cycles_;
	}
}

void sakura::Clock::single_step()
{
	if (is_paused())
	{
		time::SecondsDoublePrecision inv_frame_rate = 1.0 / ideal_frame_rate(); // #SK_TODO: cache this when setting the ideal frame rate
		delta_time_cycles_ = time::seconds_to_cycles(inv_frame_rate * time_scale(), cycles_per_second_);
		time_cycles_ += delta_time_cycles_;
	}
}

//////////////////////////////////////////////////////////////////////////
/// Duration
//////////////////////////////////////////////////////////////////////////
sakura::Duration::Duration() :
	cycles_per_second_(static_cast<f32>(platform::get_high_resolution_timer_frequency())),
	time_cycles_(platform::get_high_resolution_timer_cycles())
{
}

sakura::time::Seconds sakura::Duration::get() const
{
	return time::calc_delta_time(platform::get_high_resolution_timer_cycles(), time_cycles_, cycles_per_second_);
}
