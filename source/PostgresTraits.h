#pragma once


namespace Postgres
{
	enum class TypeCategory : uint32_t
	{
		TypeUnknown = 0u,
		TypeEnum
	};

	//-------------------------------------------------------------------------------------------------
	/// Traits
	//-------------------------------------------------------------------------------------------------
	template <typename Type, typename = void>
	class Traits
	{
	public:
	};
/*
	template <typename Type>
	class Traits<typename Type, typename std::enable_if<std::is_enum<Type>::value>::type>
	{
	public:
		typedef typename Traits<typename std::underlying_type<Type>::type>::Type Type;

		static constexpr size_t		PackSize = sizeof(Type);
		static constexpr int		Format = 1;
	};
*/
	template <>
	class Traits<int16_t>
	{
	public:
		typedef int16_t Type;

		static constexpr size_t		PackSize = sizeof(Type);
		static constexpr int		Format = 1;
	};

	template <>
	class Traits<int32_t>
	{
	public:
		typedef int32_t Type;

		static constexpr size_t		PackSize = sizeof(Type);
		static constexpr int		Format = 1;
	};

	template <>
	class Traits<int64_t>
	{
	public:
		typedef int64_t Type;

		static constexpr size_t		PackSize = sizeof(Type);
		static constexpr int		Format = 1;
	};

	template <>
	class Traits<uint16_t>
	{
	public:
		typedef int16_t Type;

		static constexpr size_t		PackSize = sizeof(Type);
		static constexpr int		Format = 1;
	};

	template <>
	class Traits<uint32_t>
	{
	public:
		typedef int32_t Type;

		static constexpr size_t		PackSize = sizeof(Type);
		static constexpr int		Format = 1;
	};

	template <>
	class Traits<uint64_t>
	{
	public:
		typedef int64_t Type;

		static constexpr size_t		PackSize = sizeof(Type);
		static constexpr int		Format = 1;
	};

	template <>
	class Traits<bool>
	{
	public:
		typedef int8_t Type;

		static constexpr size_t		PackSize = sizeof(Type);
		static constexpr int		Format = 1;
	};

	//-------------------------------------------------------------------------------------------------
	/// EndianSwap
	//-------------------------------------------------------------------------------------------------
	forceinline int16_t& EndianSwap(int16_t& x)
	{
		x = (x << 8) | ((x >> 8) & 0xFF);
		return x;
	}

	forceinline int32_t& EndianSwap(int32_t& x)
	{
		x = ((x << 8) & 0xFF00FF00) | ((x >> 8) & 0xFF00FF);
		x = (x << 16) | ((x >> 16) & 0xFFFF);
		return x;
	}

	forceinline int64_t& EndianSwap(int64_t& x)
	{
		x = ((x << 8) & 0xFF00FF00FF00FF00ULL) | ((x >> 8) & 0x00FF00FF00FF00FFULL);
		x = ((x << 16) & 0xFFFF0000FFFF0000ULL) | ((x >> 16) & 0x0000FFFF0000FFFFULL);
		x = (x << 32) | ((x >> 32) & 0xFFFFFFFFULL);
		return x;
	}

	forceinline uint16_t& EndianSwap(uint16_t& x)
	{
		x = (x << 8) | (x >> 8);
		return x;
	}

	forceinline uint32_t& EndianSwap(uint32_t& x)
	{
		x = ((x << 8) & 0xFF00FF00) | ((x >> 8) & 0xFF00FF);
		x = (x << 16) | (x >> 16);
		return x;
	}

	forceinline uint64_t& EndianSwap(uint64_t& x)
	{
		x = ((x << 8) & 0xFF00FF00FF00FF00ULL) | ((x >> 8) & 0x00FF00FF00FF00FFULL);
		x = ((x << 16) & 0xFFFF0000FFFF0000ULL) | ((x >> 16) & 0x0000FFFF0000FFFFULL);
		x = (x << 32) | (x >> 32);
		return x;
	}
}
