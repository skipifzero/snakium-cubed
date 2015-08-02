#if !defined(SFZ_NO_DEBUG) && !defined(SFZ_NO_ASSERTIONS)

#define sfz_assert_debug_impl(condition) assert(condition)

#define sfz_assert_debug_m_impl(condition, message) \
{ \
	if (!condition) { \
		std::cerr << message << std::endl; \
		assert(condition); \
	} \
}

#else
#define sfz_assert_debug_impl(condition) ((void)0)
#define sfz_assert_debug_m_impl(condition, message) ((void)0)
#endif



#if !defined(SFZ_NO_ASSERTIONS)

#define sfz_assert_release_impl(condition) \
{ \
	if (!condition) { \
		assert(condition); \
		std::terminate(); \
	} \
}

#define sfz_assert_release_m_impl(condition, message) \
{ \
	if (!condition) { \
		std::cerr << message << std::endl; \
		assert(condition); \
		std::terminate(); \
	} \
}

#else
#define sfz_assert_release_impl(condition) ((void)0)
#define sfz_assert_release_m_impl(condition, message) ((void)0)
#endif



#if !defined(SFZ_DISABLE_ERRORS)

#define sfz_error_impl(message) \
{ \
	std::cerr << message << std::endl; \
	std::terminate(); \
}

#else
#define sfz_error_impl(message) ((void)0)
#endif
