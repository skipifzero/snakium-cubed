#pragma once
#ifndef SFZ_COMMON_GLOBAL_HPP
#define SFZ_COMMON_GLOBAL_HPP

// Visual Studio
#ifdef _MSC_VER
	// Visual Studio 2013 and earlier
	#if (_MSC_VER <= 1800)
		#ifndef noexcept
		#define noexcept
		#endif
		#ifndef constexpr
		#define constexpr
		#endif
	#endif
#endif

#endif