#pragma once
#error "alignmentを考慮できない。struct S{i32, i8, i8}が8バイト、struct S{i8, i32, i8}が12バイトなのは実装依存じゃないか?"

// #pragma once

// #include <concepts>
// #include <iostream>
// #include <utility>
// #include <cstdint>
// #include <algorithm>

// namespace CRSLib
// {
// 	struct DisplayMarker{};

// 	template<class ... Ts>
// 	requires ((std::derived_from<Ts, DisplayMarker> || requires(const Ts imut)
// 	{
// 		std::cout << imut;
// 	}) && ...)
// 	struct InfoSpecialization
// 	{
// 		alignof(std::max({1, sizeof(Ts) ...})) std::byte[std::max(1, (0 + ... + sizeof(Ts)))];

// 		void display() const
// 		{
// 			size_t head = 0;
// 			reinterpret_cast
// 		}
// 	};

// 	template<class T>
// 	concept info_specialization = requires(const T imut)
// 	{
// 		[]<class ... Args>(const InfoSpecialization<Args ...>&)
// 		{}(imut);
// 	};

// 	template<class Target, info_specialization Spec>
// 	struct StructInfo final
// 	{
// 		static void display(const Target& self) const
// 		{
// 			reinterpret_cast<const Spec&>(self).display();
// 		}
// 	};

	
// }