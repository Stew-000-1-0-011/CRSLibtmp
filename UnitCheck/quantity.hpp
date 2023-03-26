/**
 * @file quantity.hpp
 * @author stew (stew.*(I fogot.)@gmail.com)
 * @brief
 * 単位を扱うためのテンプレートなど。最適化のもとでは実行ファイルを重たくすることはないと思う
 * (ほぼ全てがinline展開されるはずだし、使われないconstexpr定数は消えるはず)
 * @version 0.1
 * @date 2023-02-06
 * @attention
 * C++23 Deducing Thisが使えるようになったら全面的に書き換えること。
 * 無いのがおかしい、書いてて苦痛。
 * C++26になったらsubscript operator([] <- これ)を実装すること。
 * meta classが使えるようになったら全てを検討し直すこと。
 * 
 * @copyright Copyright (c) 2023
 * 
 * @implements
 * これ、参照はどう伝播すればいいんだ...？とりあえずElementが参照型を許容するということにする。
 * ↑もうやめましょう...土台無理な話だったんだ。私が代わりに関数のシグネチャを固定しますから。
 * というのも、auto q2 = ++q;でq2はqと同じ型になる必要があり、qとq3が同じ型のとき++q = q3ができる必要がある。
 * いや、Elementのcvref修飾をそのままQuantityに伝播させればいけるな？
 */

#pragma once

#include <utility>

#include <CRSLibtmp/utility.hpp>


namespace CRSLib::UnitCheck
{
	template<non_cvref Element, auto unit>
	struct Quantity;

	template<>

	// 量
	template<non_cvref Element, auto unit>
	struct Quantity final
	{
		Element element;

		Quantity() = default;
		Quantity(const Quantity&) = default;
		Quantity(Quantity&&) = default;
		Quantity& operator=(const Quantity&) = default;
		Quantity& operator=(Quantity&&) = default;
		
		template<class Element2>
		Quantity(Quantity<Element2> quantity2)
		requires (std::is_reference_v<Element2>):
			element{std::forward<Element2>(quantity2.element)}
		{}

		// Define Quantity Unary Operator Inner
		#define stew_DQMOInner(operator_signature, cvref, element_expression, unit_expression) \
		constexpr auto operator operator_signature cvref -> PropagateCVRef<decltype(element_expression), Quantity<std::remove_cvref_t<decltype(element_expression)>, unit_expression>> /*noexcept(auto)*/\
		requires\
			requires\
			{\
				element_expression;\
				unit_expression;\
			}\
		{\
			if constexpr(std::is_reference_v<decltype(element_expression)>)\
			{\
				element_expression;\
				return *this;\
			}\
			else\
			{\
				return {element_expression};\
			}\
		}

		// Define Quantity Member Operator
		#define stew_DQMO(operator_signature, element_expression, unit_expression) \
		stew_DQMOInner(operator_signature, &, element_expression, unit_expression)\
		stew_DQMOInner(operator_signature, &&, element_expression, unit_expression)\
		stew_DQMOInner(operator_signature, const &, element_expression, unit_expression)\
		stew_DQMOInner(operator_signature, const &&, element_expression, unit_expression)\
		stew_DQMOInner(operator_signature, volatile &, element_expression, unit_expression)\
		stew_DQMOInner(operator_signature, volatile &&, element_expression, unit_expression)\
		stew_DQMOInner(operator_signature, const volatile &, element_expression, unit_expression)\
		stew_DQMOInner(operator_signature, const volatile &&, element_expression, unit_expression)

		stew_DQMO(++(int), element++, unit++)
		stew_DQMO(++(int), element--, unit--)

		stew_DQMO(++(), ++element, ++unit)
		stew_DQMO(++(), --element, --unit)
		stew_DQMO(~(), ~element, ~unit)
		stew_DQMO(!(), !element, !unit)
		stew_DQMO(+(), +element, +unit)
		stew_DQMO(-(), -element, -unit)

		stew_DQMO(()(auto&& ... args), element(std::forward<decltype(args)>(args)...), unit())

		// // Define Quantity Unary Operator
		// #define stew_DQUO(unary) \
		// constexpr auto operator unary() const -> Quantity<std::remove_cvref_t<decltype(unary element)>, unary unit>\
		// {\
		// 	return unary element;\
		// }

		// stew_DQUO(~)
		// stew_DQUO(!)
		// stew_DQUO(+)
		// stew_DQUO(-)

		constexpr auto operator()(auto&& ... args) ->
			Quantity
			<
				std::remove_cvref_t<decltype(element(std::forward<decltype(args)(args) ...))>,
				unit.template operator()<decltype(std::forward<decltype(args)>(args))...>()
			>
		{
			return element(std::forward<Args>(args)...);
		}

		template<class ... Args>
		constexpr auto operator()(Args&& ... args) const -> Quantity<std::remove_cvref_t<decltype(element(std::forward<Args>(args)...))>, unit(std::forward<Args>(args)...)>
		{
			return element(std::forward<Args>(args)...);
		}

		explicit constexpr operator const Element&() const&
		requires
			(unit.is_zero())
		{
			return element;
		}

		explicit constexpr operator Element&() &
		requires
			(unit.is_zero())
		{
			return element;
		}

		explicit constexpr operator const Element&&() const&& noexcept
		requires
			(unit.is_zero())
		{
			return std::move(element);
		}

		explicit constexpr operator Element&&() && noexcept
		requires
			(unit.is_zero())
		{
			return std::move(element);
		}

		// define Quantity Assignment Operator
		#define stew_DQAO(op) \
		template<class E, auto u>\
		constexpr auto operator op(CRSLib::cvref_same<Quantity<E, u>> auto&& quantity) & -> Quantity&\
		{\
			element op CRSLib::forward_like<decltype(quantity)>(quantity.element);\
			return *this;\
		}

		stew_DQAO(=)
		stew_DQAO(*=)
		stew_DQAO(/=)
		stew_DQAO(%=)
		stew_DQAO(+=)
		stew_DQAO(-=)
		stew_DQAO(<<=)
		stew_DQAO(>>=)
		stew_DQAO(&=)
		stew_DQAO(^=)
		stew_DQAO(|=)
	};

	namespace QuantityOperator
	{
		// Define Quantity Binary Operator
		#define stew_DUBO(op) \
		template<class EL, class ER, class UL, class UR>\
		constexpr auto operator op(CRSLib::cvref_same<Quantity<EL, UL>> auto&& l, CRSLib::cvref_same<Quantity<ER, UR>> auto&& r) -> Quantity<decltype(l.element op r.element), decltype(std::declval<UL>() op std::declval<UR>())>\
		{\
			return l.element op r.element;\
		}

		stew_DUBO(*)
		stew_DUBO(/)
		stew_DUBO(%)
		stew_DUBO(+)
		stew_DUBO(-)
		stew_DUBO(<<)
		stew_DUBO(>>)
		stew_DUBO(&)
		stew_DUBO(^)
		stew_DUBO(|)
		stew_DUBO(&&)
		stew_DUBO(||)

		template<class EL, class ER, class U>
		constexpr auto operator<=>(CRSLib::cvref_same<Quantity<EL, U>> auto&& l, CRSLib::cvref_same<Quantity<ER, U>> auto&& r)
		{
			return l.element <=> r.element;
		}

		template<class EL, class ER, class U>
		constexpr bool operator==(CRSLib::cvref_same<Quantity<EL, U>> auto&& l, CRSLib::cvref_same<Quantity<ER, U>> auto&& r)
		{
			return l.element == r.element;
		}
	}
}