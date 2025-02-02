/**
 *  @file oglplus/error/outcome.hpp
 *  @brief Class combining a return value and deferred error handler
 *
 *  @author Matus Chochlik
 *
 *  Copyright 2010-2015 Matus Chochlik. Distributed under the Boost
 *  Software License, Version 1.0. (See accompanying file
 *  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#pragma once
#ifndef OGLPLUS_ERROR_OUTCOME_1405082311_HPP
#define OGLPLUS_ERROR_OUTCOME_1405082311_HPP

#include <oglplus/error/deferred_handler.hpp>
#include <cassert>

namespace oglplus {

template <typename T>
class PositiveOutcome;

template <typename T>
class NegativeOutcome;

template <typename T>
class Outcome
{
protected:
	DeferredHandler _error;
	T _value;
public:
#if !OGLPLUS_NO_DEFAULTED_FUNCTIONS
	Outcome(Outcome&&) = default;
#else
	Outcome(Outcome&& temp)
	OGLPLUS_NOEXCEPT(true)
	 : _error(std::move(temp._error))
	 , _value(std::move(temp._value))
	{ }
#endif

	Outcome(T&& value)
	OGLPLUS_NOEXCEPT(true)
	 : _value(std::move(value))
	{ }

	Outcome(DeferredHandler&& handler)
	 : _error(std::move(handler))
	{ }

	/// Returns stored value or cancels the error and returns the parameter
	const T& ValueOr(const T& value)
	{
		return _error.cancel()?value:_value;
	}

	/// Return true if there was no error, false otherwise
	bool Done(void) const
	{
		return !_error;
	}

	// Dismisses the error handler and returns true if there was no error
	bool DoneWithoutError(void)
	{
		return !_error.cancel();
	}
};

/// Stores a reference to T or a deferred error handler
/** If an instance of Outcome<T&> stores an error handler
 *  and it is not dismissed, then the deferred error handler
 *  is invoked in the dectructor.
 */
template <typename T>
class Outcome<T&>
{
protected:
	DeferredHandler _error;
	T* _ptr;
public:
#if !OGLPLUS_NO_DEFAULTED_FUNCTIONS
	Outcome(Outcome&&) = default;
#else
	Outcome(Outcome&& temp)
	OGLPLUS_NOEXCEPT(true)
	 : _error(std::move(temp._error))
	 , _ptr(std::move(temp._ptr))
	{ }
#endif

	Outcome(T& ref)
	OGLPLUS_NOEXCEPT(true)
	 : _ptr(&ref)
	{ }

	Outcome(DeferredHandler handler)
	 : _error(std::move(handler))
	 , _ptr(nullptr)
	{ }

	Outcome(DeferredHandler handler, T& ref)
	OGLPLUS_NOEXCEPT(true)
	 : _error(std::move(handler))
	 , _ptr(&ref)
	{ }

	DeferredHandler ReleaseHandler(void)
	OGLPLUS_NOEXCEPT(true)
	{
		return std::move(_error);
	}

	/// Trigger the error handler if any or return the stored reference
	T& Then(void)
	{
		_error.trigger();
		assert(_ptr != nullptr);
		return *_ptr;
	}

	/// Return true if there was no error, false otherwise
	bool Done(void) const
	OGLPLUS_NOEXCEPT(true)
	{
		return !_error;
	}

	// Dismisses the error handler and returns true if there was no error
	bool DoneWithoutError(void)
	{
		return !_error.cancel();
	}
};

template <typename T>
class PositiveOutcome
 : public Outcome<T>
{
public:
	PositiveOutcome(Outcome<T>&& base)
	OGLPLUS_NOEXCEPT(true)
	 : Outcome<T>(std::move(base))
	{ }

	OGLPLUS_EXPLICIT
	operator bool (void) const
	OGLPLUS_NOEXCEPT(true)
	{
		return !this->_error;
	}

	bool operator ! (void) const
	OGLPLUS_NOEXCEPT(true)
	{
		return bool(this->_error);
	}
};

template <typename T>
class NegativeOutcome
 : public Outcome<T>
{
public:
#if !OGLPLUS_NO_DEFAULTED_FUNCTIONS
	NegativeOutcome(NegativeOutcome&&) = default;
#else
	NegativeOutcome(NegativeOutcome&& temp)
	OGLPLUS_NOEXCEPT(true)
	 : Outcome<T>(static_cast<Outcome<T>&&>(temp))
	{ }
#endif

	NegativeOutcome(Outcome<T>&& base)
	OGLPLUS_NOEXCEPT(true)
	 : Outcome<T>(std::move(base))
	{ }

	OGLPLUS_EXPLICIT
	operator bool (void) const
	OGLPLUS_NOEXCEPT(true)
	{
		return bool(this->_error);
	}

	bool operator ! (void) const
	OGLPLUS_NOEXCEPT(true)
	{
		return !this->_error;
	}
};

template <typename T>
static inline
PositiveOutcome<T> Succeeded(Outcome<T>&& outcome)
OGLPLUS_NOEXCEPT(true)
{
	return std::move(outcome);
}

template <typename T>
static inline
NegativeOutcome<T> Failed(Outcome<T>&& outcome)
OGLPLUS_NOEXCEPT(true)
{
	return std::move(outcome);
}

} // namespace oglplus

#endif // include guard
